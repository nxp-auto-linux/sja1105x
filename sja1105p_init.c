/*
* AVB switch driver module for SJA1105
* Copyright (C) 2016 - 2018 NXP Semiconductors
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
/**
*
* \file  sja1105p_spi.c
*
* \author Philippe guasch, Laurent Brando
*
* \date 2016-02-22
*
* \brief SJA1105P SPI driver
*
*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>

#include "NXP_SJA1105P_clockGenerationUnit.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_spi.h"
#include "NXP_SJA1105P_portConfig.h"
#include "NXP_SJA1105P_cbs.h"

#include "sja1105p_spi_linux.h"
#include "sja1105p_cfg_file.h"
#include "sja1105p_general_status.h"
#include "sja1105p_debugfs.h"
#include "sja1105p_switchdev.h"
#include "sja1105p_init.h"

/*
 * Local constants and macros
 *
 */
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/*
 * Static variables
 *
 */
static int max_hz = SPI_FREQUENCY; /* 25 MHz is the default but might have to be reduced */
module_param(max_hz, int, S_IRUGO);
MODULE_PARM_DESC(max_hz, "SPI bus speed may be limited for the remote SJA1105P application board, 25MHz is the maximum");

static char *ifname =  "eth0";
module_param(ifname, charp, S_IRUGO);
MODULE_PARM_DESC(ifname, "Network interface name for SJA1105P Host port: default to 'eth0'");

int verbosity =  0;
module_param(verbosity, int, S_IRUGO);
MODULE_PARM_DESC(verbosity, "Trace level'");

static int enable_switchdev = 1;
module_param(enable_switchdev, int, S_IRUGO);
MODULE_PARM_DESC(enable_switchdev, "Enable the switchdev driver");


/* This is not very elegant. Linux linux treats every switch as a separate device,
 * whereas the HAL functions treat multiswitch configurations as one big logical device.
 * This means eg SJA1105P_initManualPortMapping() must only be called once, once the mapping for ALL switches is known.
 * We accumulate information during subsequent probe calls, once all switches are probed, we do one final call to sja1105p_probe_final
 */
static DEFINE_RWLOCK(rwlock);
static int switches_active;
bool do_auto_mapping = false;
SJA1105P_port_t portMapping[SJA1105P_N_LOGICAL_PORTS];
struct sja1105p_context_data *sja1105p_context_arr[SJA1105P_N_SWITCHES];


#define NUM_DT_ENTRIES 1
static const struct of_device_id sja1105p_dt_ids[] = {
	{  .compatible = "nxp,sja1105p-switch",  .data = (const void*)0 },   /* SJA1105P */
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sja1105p_dt_ids);


/* Filenames to be located in /lib/firmware for instance */
const char * product_names[] = {
	"sja1105p"
};

/* chip version is determined in sja1105p_check_device_id(), suffix chosen accordingly */
const char * fw_name_suffix[SJA1105P_NB_REV] = {
	""
	""
};

struct _SJA_CONF sja_cfg;



static int sja1105p_check_device_id(struct spi_device *spi, unsigned int device_select, bool dump_id)
{
	int val = -1;
	int i;
	u32 devid;

	if (!sja_get_device_id(&devid, device_select)) {
		for (i = 0; i < SJA1105P_NB_REV; i++) {
			if (devid == device_id_list[i]) {
				val = i;
				if (dump_id) dev_info(&spi->dev, "Detected Device ID %08x (%s)\n", devid, fw_name_suffix[i]);
				break;
			}
		}
	}

	if (val == -1)
		dev_err(&spi->dev, "SJA1105P invalid Device Id, is %08x\n", devid);

	return val;
}

static bool sja1105p_check_device_status(struct spi_device *spi)
{
	struct sja1105p_context_data *sw_ctx = spi_get_drvdata(spi);
	SJA1105P_configurationFlagsArgument_t configFlags;
	uint8_t switchId;
	uint8_t err;
	bool ret = true;

	switchId = sw_ctx->device_select;
	err = SJA1105P_getConfigurationFlags(&configFlags, switchId);
	if (err) {
		dev_err(&spi->dev, "Error: Could not get configuration flags for switch id %d (err=%d)\n", switchId, err);
		ret = false;
	}

	/* configs is 1 if cfg is valid, other members are 1 on error */
	if (!configFlags.configs) {
		dev_err(&spi->dev, "Error: (switch id %d) LocalCRCfail=%d - DevID unmatched=%d, GlobalCRCfail=%d\n",
			switchId,
			configFlags.crcchkl,
			configFlags.ids,
			configFlags.crcchkg);
		ret = false;
	}

	return ret;
}

static int sja1105p_config_avb_param_check(struct spi_device *spi, struct sja1105p_context_data *sw_ctx)
{
	struct sja1105p_cfg_block * avb_param_block;
	u8 * p;
	u8 metaframe_sa[6];
	u8 metaframe_da[6];
	int i;
	struct net_device *ndev;

	if (verbosity) dev_info(&spi->dev, "Looking for AVB parameters\n");

	/* Seek for AVB parameters section of config file if any */
	avb_param_block = sja1105p_config_block_seek(&sw_ctx->cfg_block_list, SJA1105P_BLOCK_AVB_PARAMETERS);
	if (avb_param_block /*&& avb_param_block->block_sz == 3 already checked */) {
		p = (u8*)avb_param_block->block_data;
		for (i = 0; i<6; i++) {
			metaframe_da[i] = p[i+6];
			metaframe_sa[i] = p[i];
		}

		ndev = dev_get_by_name(&init_net, ifname);

		if (verbosity) dev_info(&spi->dev, "Got dev=%p name= %s\n", ndev, ifname);

		if (ndev) {
			/* Let 's register the address that is programmed as Metaframe DA as a Unicast entry.
			An alternative would have been to read the address and patch the AVB parameters block
			but would require CRC32 recomputation over the block and the whole file */
			//dev_uc_add(ndev, (const unsigned char *)metaframe_da);

			if (verbosity) dev_info(&spi->dev, "Registered management port DST MAC address=%02x:%02x:%02x:%02x:%02x:%02x\n",
				metaframe_da[0],
				metaframe_da[1],
				metaframe_da[2],
				metaframe_da[3],
				metaframe_da[4],
				metaframe_da[5]);
			if (verbosity) dev_info(&spi->dev, "Registered management port SRC MAC address=%02x:%02x:%02x:%02x:%02x:%02x\n",
				metaframe_sa[0],
				metaframe_sa[1],
				metaframe_sa[2],
				metaframe_sa[3],
				metaframe_sa[4],
				metaframe_sa[5]);
		} else {
			dev_err(&spi->dev, "Failed to retrieve net_device from %s\n", ifname);
			return -1;
		}
	}

	/* the absence of AVB parameters block is not necessarily an error
	since non AVB configurations may exist */
	return 0;
}

static bool sja1105p_post_cfg_load_check(struct spi_device *spi,  struct sja1105p_context_data *sw_ctx)
{
	int chip_id;

	/* Trying to read back the SJA1105P status via SPI... */
	chip_id  = sja1105p_check_device_id(spi, sw_ctx->device_select, 0);
	if (chip_id != sw_ctx->sja1105p_chip_revision)
		return false;

	return sja1105p_check_device_status(spi);
}

char *sja1105p_clksrc_to_str(SJA1105P_clksrc_t clksrc)
{
	switch (clksrc)
	{
		case SJA1105P_e_clksrc_IDIV4: return "IDIV4";
		case SJA1105P_e_clksrc_IDIV3: return "IDIV3";
		case SJA1105P_e_clksrc_IDIV2: return "IDIV2";
		case SJA1105P_e_clksrc_IDIV1: return "IDIV1";
		case SJA1105P_e_clksrc_IDIV0: return "IDIV0";
		case SJA1105P_e_clksrc_PLL1_240: return "PLL1_240";
		case SJA1105P_e_clksrc_PLL1_120: return "PLL1_120";
		case SJA1105P_e_clksrc_PLL1: return "PLL1";
		case SJA1105P_e_clksrc_PLL0_240: return "PLL0_240";
		case SJA1105P_e_clksrc_PLL0_120: return "PLL0_120";
		case SJA1105P_e_clksrc_PLL0: return "PLL0";
		case SJA1105P_e_clksrc_MII4_RX_CLK: return "MII4_RX_CLK";
		case SJA1105P_e_clksrc_MII4_TX_CLK: return "MII4_TX_CLK";
		case SJA1105P_e_clksrc_MII3_RX_CLK: return "MII3_RX_CLK";
		case SJA1105P_e_clksrc_MII3_TX_CLK: return "MII3_TX_CLK";
		case SJA1105P_e_clksrc_MII2_RX_CLK: return "MII2_RX_CLK";
		case SJA1105P_e_clksrc_MII2_TX_CLK: return "MII2_TX_CLK";
		case SJA1105P_e_clksrc_MII1_RX_CLK: return "MII1_RX_CLK";
		case SJA1105P_e_clksrc_MII1_TX_CLK: return "MII1_TX_CLK";
		case SJA1105P_e_clksrc_MII0_RX_CLK: return "MII0_RX_CLK";
		case SJA1105P_e_clksrc_MII0_TX_CLK: return "MII0_TX_CLK";
		default: break;
	}
	return "unknown";
}

static void sja1105p_cgu_dump(struct spi_device *spi)
{
	int err, port;
	SJA1105P_port_t pport;
	SJA1105P_idivCControlRegisterArgument_t idivCtrl;
	SJA1105P_miixClockControlRegisterArgument_t miixClkCtrl, rgmiixClkCtrl;

	for (port=0; port<SJA1105P_PORT_NB; port++) {
		if (SJA1105P_getPhysicalPort(port, &pport))
			continue;

		err  = SJA1105P_getIdivCControlRegister(&idivCtrl, pport.physicalPort, pport.switchId);
		err += SJA1105P_getMiixClockControlRegister(&miixClkCtrl, pport.physicalPort, SJA1105P_e_miixInternalClk_MII_TX_CLK, pport.switchId);
		err += SJA1105P_getMiixClockControlRegister(&rgmiixClkCtrl, pport.physicalPort, SJA1105P_e_miixInternalClk_RGMII_TX_CLK, pport.switchId);


		dev_info(&spi->dev, "CGU-PORT%d:\n", port);
		dev_info(&spi->dev, "IDIV:      Autoblock=%x, Idiv=by %s, pd=%x\n", idivCtrl.autoblock, (idivCtrl.idiv==SJA1105P_e_idiv_BY_1)?"1":"10", idivCtrl.pd);
		dev_info(&spi->dev, "MII TXC:   Autoblock=%x, clksrc=%s, pd=%x\n", miixClkCtrl.autoblock,  sja1105p_clksrc_to_str(miixClkCtrl.clksrc), miixClkCtrl.pd);
		dev_info(&spi->dev, "RGMII TXC: Autoblock=%x, clksrc=%s, pd=%x\n", rgmiixClkCtrl.autoblock,  sja1105p_clksrc_to_str(rgmiixClkCtrl.clksrc), rgmiixClkCtrl.pd);
	}
}

static int sja1105p_configuration_load(const struct firmware *config_file, struct spi_device *spi)
{
	struct sja1105p_context_data *data = spi_get_drvdata(spi);
	int nb_words;
	u32 * cfg_data;
	int remaining_words;
	u32 dev_addr;
	u32 val;
	bool swap_required;
	int i;
	SJA1105P_portStatusMiixArgument_t portStatusMiix;

	/* read back ports configuration */
	for (i=0; i<SJA1105P_PORT_NB; i++) {
		SJA1105P_getPortStatusMiix(&portStatusMiix, i, data->device_select);
		if (verbosity > 0) dev_info(&spi->dev, "REG_PORT_STATUS(%d): speed=%d phyMode=%d xmiiMode=%d\n", i, portStatusMiix.speed, portStatusMiix.phyMode, portStatusMiix.xmiiMode);
	}

	if (verbosity > 3) sja1105p_cgu_dump(spi);

	if (config_file->size == 0) {
		dev_err(&spi->dev, "Error: SJA1105P Switch configuration is empty\n");
		goto err_cfg;
	}

	nb_words = (config_file->size >> 2);
	cfg_data = (u32*)config_file->data;

	/* Check that file is actually compatible with chip revision */
	if (data->sja1105p_chip_revision < 0 || data->sja1105p_chip_revision >= SJA1105P_NB_REV) {
		dev_err(&spi->dev, "Error: SJA1105P unhandled revision\n");
		goto err_cfg;
	}

	val = cfg_data[0];
	if (val ==__builtin_bswap32(device_id_list[data->sja1105p_chip_revision]) ) {
		dev_err(&spi->dev, "Config file requires swap, incorrect endianness\n");
		swap_required = true;
	}
	else if (val == device_id_list[data->sja1105p_chip_revision])
		swap_required = false;
	else {
		dev_err(&spi->dev, "Error: SJA1105P unhandled revision Switch incompatible configuration file (%x - %x)\n", val, device_id_list[data->sja1105p_chip_revision]);
		goto err_cfg;
	}

	if (swap_required) {
		int val;
		for (i = 0; i < nb_words; i++) {
			val = cfg_data[i];
			cfg_data[i] = __builtin_bswap32(val);
		}
	}

	INIT_LIST_HEAD(&data->cfg_block_list);
	sja1105p_config_file_split_blocks(spi, &data->cfg_block_list, cfg_data, nb_words, verbosity);
	if (list_empty(&data->cfg_block_list)) {
		dev_err(&spi->dev, "SJA1105P error parsing configuration file\n");
		goto err_cfg;
	}

	if (sja1105p_config_avb_param_check(spi, data) < 0) {
		dev_err(&spi->dev, "SJA1105P failed to get net device!\n");
		goto err_cfg;
	}

	if (verbosity > 1) dev_info(&spi->dev, "swap_required %d nb_words %d dev_addr %08x\n", swap_required, nb_words, (u32)SJA1105P_CONFIG_START_ADDRESS);

	remaining_words = nb_words;
	dev_addr = SJA1105P_CONFIG_START_ADDRESS;

	i = 0;
	while (remaining_words > 0) {
		//TODO: should use SJA1105P_CONFIG_WORDS_PER_BLOCK many words per transfer
		int block_size_words = MIN(SPI_CFG_BLOCKS, remaining_words);

		if (verbosity > 2) dev_info(&spi->dev, "block_size_words %d remaining_words %d\n", block_size_words, remaining_words);

		if (sja1105p_cfg_block_write(spi, dev_addr, cfg_data, block_size_words) < 0)
			goto err_cfg;

		if (verbosity > 1) dev_info(&spi->dev, "Loaded block %d @%08x\n", i, dev_addr);

		dev_addr += block_size_words;
		cfg_data += block_size_words;
		remaining_words -= block_size_words;
		i++;
	}

	if (!sja1105p_post_cfg_load_check(spi, data))  {
		dev_err(&spi->dev, "SJA1105P configuration failed\n");
		goto err_cfg;
	}

	/* read back ports configuration */
	for (i=0; i<SJA1105P_PORT_NB; i++) {
		SJA1105P_getPortStatusMiix(&portStatusMiix, i, data->device_select);
		if (verbosity > 0) dev_info(&spi->dev, "REG_PORT_STATUS(%d): speed=%d phyMode=%d xmiiMode=%d\n", i, portStatusMiix.speed, portStatusMiix.phyMode, portStatusMiix.xmiiMode);
	}

	if (verbosity > 3) sja1105p_cgu_dump(spi);

	if (verbosity > 0)  {
		read_lock(&rwlock);
		dev_info(&spi->dev, "SJA1105P (number %d) successfully configured!\n", switches_active);
		read_unlock(&rwlock);
	}

	/*
	* Don't forget to release the firmware again
	*/
	release_firmware(config_file);

	return 0;

err_cfg:
	release_firmware(config_file);
	return -EINVAL;
}


static int sja1105p_init_dt(struct sja1105p_context_data *switch_ctx)
{
	struct sja1105p_platform_data *pdata;
	struct device_node *np = switch_ctx->of_node;
	u32 val;
	struct device_node * port_node;
	int i;
	const char *firmware_name;

	if (!np) {
		if (verbosity) dev_info(&switch_ctx->spi_dev->dev, "No OF node\n");
		return 0;
	}

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	if (!of_property_read_string(np, "firmware_name", &firmware_name)) {
		if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "Firmware name \"%s\" found\n", firmware_name);
		snprintf(switch_ctx->fw_name, 63, firmware_name);
	} else {
		if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "No firmware_name property was found\n");
		switch_ctx->fw_name[0] = '\0';
	}

	pdata->host_port_id = SJA1105P_PORT_NB;
	for (i = 0; i < SJA1105P_PORT_NB; i++) {
		char str_to_find[32];
		snprintf(str_to_find, 32, "port-%d", i);
		port_node = of_find_node_by_name(np, str_to_find);
		if (!port_node) {
			if (verbosity) dev_err(&switch_ctx->spi_dev->dev, "Failed to find node port-%d\n", i);
			goto err_dt;
		}
		if (verbosity > 1) dev_info(&switch_ctx->spi_dev->dev, "Properties of port %d found\n", i);

		if (!of_property_read_u32(port_node, "logical-port-num", &val)) {
			if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "port-%d logical-port-num=%d\n", i, val);
			pdata->ports[i].logical_port_num = val;

		} else {
			pdata->ports[i].logical_port_num =  -1; /*  invalid */
		}

		pdata->ports[i].is_host = false;
		if (!of_property_read_u32(port_node, "is-host", &val)) {
			if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "port-%d is-host=%d\n", i, val);
			if (val != 0) {
				pdata->ports[i].is_host = true;
				if (pdata->host_port_id == SJA1105P_PORT_NB)
					pdata->host_port_id = i;
				else {
					dev_err(&switch_ctx->spi_dev->dev, "Only one port can be Host Port (management)" "port-%d current is port-%d\n", i, pdata->host_port_id);
					goto err_dt;
				}
			}
		}

		if (!of_property_read_u32(port_node, "null-phy", &val))
			pdata->ports[i].phy_not_mac = (val != 0);

		if (verbosity > 1) dev_info(&switch_ctx->spi_dev->dev, "port-%d null-phy=%d (phy_not_mac=%d)\n", i, val, pdata->ports[i].phy_not_mac);

		of_node_put(port_node);
	} /* for */

	switch_ctx->pdata = pdata;
	return 0;

err_dt:
	if (pdata) kzfree(pdata);
	switch_ctx->pdata = NULL;
	return -EINVAL;
}

void sja1105p_port_mapping(struct sja1105p_context_data *switch_ctx)
{
	int pport;
	struct sja1105p_platform_data *pdata = switch_ctx->pdata;

	for (pport = 0; pport < SJA1105P_PORT_NB; pport++) {
		int lport = pdata->ports[pport].logical_port_num;
		if (lport == -1) {
			/* if value is is invalid, ie no DT node was found, fall back to auto mapping */
			do_auto_mapping = true;
			break;
		} else if (lport >= SJA1105P_N_LOGICAL_PORTS) {
			/* 0xff indicates that port is cascaded,
			 * (does not have a logical port number).
			 * All other values are invalid
			 */
			if (lport != 0xff)
				dev_err(&switch_ctx->spi_dev->dev, "logical port number must be less than %d, or 0xff to indicate cascaded port (is %d)\n", SJA1105P_N_LOGICAL_PORTS, lport);
			continue;
		}

		if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "physical-port-%d: logical-port-%d, switch-%d\n", pport, lport, switch_ctx->device_select);
		/* fill out the port mapping structure */
		portMapping[lport].physicalPort = pport;
		portMapping[lport].switchId = switch_ctx->device_select;
	}
}

int sja1105p_probe_final(struct sja1105p_context_data *switch_ctx)
{
	int err;

	if (verbosity > 0) {
		read_lock(&rwlock);
		dev_info(&switch_ctx->spi_dev->dev, "%d of %d devices probed, final initialization\n", switches_active, SJA1105P_N_SWITCHES);
		read_unlock(&rwlock);
	}

	err = SJA1105P_synchSwitchConfiguration();
	if (err) {
		dev_err(&switch_ctx->spi_dev->dev, "SJA1105P config sync failed\n");
		return err;
	}

	if (do_auto_mapping) {
		if (verbosity > 0) dev_info(&switch_ctx->spi_dev->dev, "DTS does not contain complete port mapping, falling back to AutoPortMapping\n");
		SJA1105P_initAutoPortMapping();
	} else {
		err = SJA1105P_initManualPortMapping(portMapping);
		if (err)
			dev_err(&switch_ctx->spi_dev->dev, "SJA1105P_initManualPortMapping failed (err=%d)\n", err);
	}

	/* perform autoconfiguration for each port */
	err = SJA1105P_autoConfigPorts();
	if (err) {
		dev_err(&switch_ctx->spi_dev->dev, "SJA1105P port autoconfiguration failed\n");
		return err;
	}

	read_lock(&rwlock);
	dev_info(&switch_ctx->spi_dev->dev, "%d switch%s initialized successfully!\n", switches_active, (switches_active > 1)?"es":"");
	read_unlock(&rwlock);

	/* only init switchdev, if all switches were detected and initialized correctly */
	if (enable_switchdev)
		nxp_swdev_init(sja1105p_context_arr);

	return 0;
}

static int sja1105p_probe(struct spi_device *spi)
{
	struct sja1105p_context_data *switch_ctx;
	int err;
	const struct of_device_id *match;
	const struct firmware *sja1105p_firmware;
	int fw_idx;

	dev_info(&spi->dev, "Loading SJA1105P SPI driver\n");

	switch_ctx = devm_kzalloc(&spi->dev, sizeof(struct sja1105p_context_data), GFP_KERNEL);
	if (!switch_ctx) {
		dev_err(&spi->dev, "Memory allocation for sja1105p_context_data failed\n");
		return -ENOMEM;
	}

	read_lock(&rwlock);
	switch_ctx->device_select = switches_active;
	if (verbosity > 0) dev_info(&spi->dev, "Probing switch number %d\n", switch_ctx->device_select);
	read_unlock(&rwlock);

	sja1105p_context_arr[switch_ctx->device_select] = switch_ctx;

	switch_ctx->spi_dev = spi;
	/* SCLK held low between frames Data latched on second clock edge
	i.e: CPOL=0-CPHA=1 alias MODE 1*/
	spi->mode = SPI_MODE_1;
	spi->bits_per_word = SPI_BITS_PER_WORD;
	spi->max_speed_hz = max_hz;

	if (verbosity > 1) dev_info(&spi->dev, "SJA1105P SPI Clock set to %dHz, bits per word set to %d\n", max_hz, SPI_BITS_PER_WORD);

	err = spi_setup(spi);
	if (err < 0) {
		dev_err(&spi->dev, "SJA1105P SPI Setup failed err=%d\n", err);
		return err;
	}

	switch_ctx->of_node = spi->dev.of_node;
	if (!switch_ctx->of_node) {
		dev_err(&spi->dev, "SJA1105P error DTS OF mode mandatory\n");
		return -ENODEV;
	}

	match = of_match_device(sja1105p_dt_ids, &spi->dev);
	if (!match) {
		dev_err(&spi->dev, "SJA1105P error no matching DTS node was found\n");
		return -ENODEV;
	}
	if (verbosity > 1) dev_info(&spi->dev, "Found a matching DT entry (type %lu)\n", (long unsigned)match->data);

	fw_idx = (long unsigned)match->data;
	if (fw_idx < 0 || fw_idx >= NUM_DT_ENTRIES) {
		dev_err(&spi->dev, "SJA1105P Bad device table contents\n");
		return -EINVAL;
	}

	err = sja1105p_init_dt(switch_ctx);
	if (err) {
		dev_err(&spi->dev, "SJA1105P error initializing device table\n");
		return err;
	}

	/* starting from now, the HAL can issue SPI r/w access to the device */
	read_lock(&rwlock);
	register_spi_callback(spi, switch_ctx->device_select, switches_active);
	read_unlock(&rwlock);

	switch_ctx->sja1105p_chip_revision = sja1105p_check_device_id(spi, switch_ctx->device_select, 1);
	if (switch_ctx->sja1105p_chip_revision < 0 ) {
		dev_err(&spi->dev, "SJA1105P SPI Failed to read Device Id\n");
		return -ENODEV;
	} else if (switch_ctx->sja1105p_chip_revision >  SJA1105P_NB_REV) {
		dev_err(&spi->dev, "SJA1105P SPI unsupported version\n");
		return -ENODEV;
	}

	/* If Firmware name was not found in DT, compile a generic name */
	if (strcmp (switch_ctx->fw_name, "") == 0) {
		snprintf(switch_ctx->fw_name, 63, "%s_%d-%d_cfg%s.bin",
		product_names[fw_idx], switch_ctx->device_select+1, SJA1105P_N_SWITCHES, fw_name_suffix[switch_ctx->sja1105p_chip_revision]);
	}

	spi_set_drvdata(spi, switch_ctx);

	/* First of all need to check for SJA1105P init read */
	if (verbosity > 0) dev_info(&spi->dev, "SJA1105P switch preparing to load %s\n", switch_ctx->fw_name);

	err = request_firmware(&sja1105p_firmware, switch_ctx->fw_name, &spi->dev);
	if (err || !sja1105p_firmware) {
		dev_err(&spi->dev, "Firmware request failed with %d!\n", err);
		return err;
	} else {
		if (verbosity) dev_info(&spi->dev, "SJA1105P switch firmware request succeeded\n");
	}

	err = sja1105p_configuration_load(sja1105p_firmware, spi);
	if (err) {
		dev_err(&spi->dev, "Firmware loading failed with %d!\n", err);
		return err;
	}

	sja1105p_port_mapping(switch_ctx);

	sja1105p_debugfs_init(switch_ctx);

	/* Keep track of the total number of switches that were probed */
	write_lock(&rwlock);
	switches_active++;
	write_unlock(&rwlock);

	read_lock(&rwlock);
	if(switches_active == SJA1105P_N_SWITCHES) {
		read_unlock(&rwlock);
		sja1105p_probe_final(switch_ctx);
	} else {
		read_unlock(&rwlock);
	}

	return err;
}

static int sja1105p_remove(struct spi_device *spi)
{
	/* Keep track of the total number of switches that were probed */
	write_lock(&rwlock);
	switches_active--;
	write_unlock(&rwlock);

	read_lock(&rwlock);
	unregister_spi_callback(switches_active);
	read_unlock(&rwlock);

	sja1105p_debugfs_remove(spi_get_drvdata(spi));

	return 0;
}



static struct spi_driver sja1105p_driver = {
	.driver = {
		.name = "sja1105pqrs",
		.owner = THIS_MODULE,
		.of_match_table = sja1105p_dt_ids,
		.pm = NULL,
	},
	.probe = sja1105p_probe,
	.remove = sja1105p_remove,
};

static int __init sja1105p_driver_init(void)
{
	return spi_register_driver( &sja1105p_driver );
}
module_init(sja1105p_driver_init);

static void __exit sja1105p_driver_exit(void)
{
	if (enable_switchdev)
		nxp_swdev_exit();

	spi_unregister_driver( &sja1105p_driver );
}
module_exit(sja1105p_driver_exit);

MODULE_DESCRIPTION("SJA1105PQRS SPI driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:sja1105pqrs");
