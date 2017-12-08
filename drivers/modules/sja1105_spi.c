/*
* AVB switch driver module for SJA1105
* Copyright (C) 2016-2017 NXP Semiconductors
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
* \file  sja1105_spi.c
*
* \author Philippe Guasch, Laurent Brando, Stoica Cosmin
*
* \date 2017-02-24
*
* \brief SJA1105 SPI driver
*
*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <linux/of_mdio.h>

#include <linux/netdevice.h>

#include "sja1105_ll.h"
#include "sja1105_cfg_file.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_spi.h"
#include "NXP_SJA1105P_portConfig.h"

/*
 * Local constants and macros
 *
 */
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#ifdef EVB
#define LAST_SWITCH_ID	1
#define SWITCHES_TO_CONFIGURE	2
#elif defined(BBMINI)
#define LAST_SWITCH_ID	2
#define SWITCHES_TO_CONFIGURE	3
#else
#error "Neither EVB nor BBMINI are defined!"
#endif

/*
 * Local structure definitions
 *
 */
struct port_desc {
	/**< whether port is connected to Host - Switch management */
	bool is_host;
	/**< port speed: 10, 100, 1000 Mbps */
	SJA1105P_speed_t speed;
	/**< port's xmii mode 0: MII, 1: RMII, 2: RGMII, 3: GMII */
	SJA1105P_xmiiMode_t xmii_mode;
	/**< if false connected to real phy true if Null Phy direct connection */
	bool phy_not_mac;
};

struct sja1105_platform_data {
	/**< Name of SJA1105 configuration binary file */
	const char * fw_name;
	/**< Number of port to which Host is tied */
	int host_port_id;
	struct port_desc ports[SJA1105_PORT_NB];
};

struct sja1105_context_data {
	/**< passsed at spi probing*/
	struct spi_device	*spi_dev;
	/**< Open Firmware node describing device */
	struct device_node	*of_node;
	/**< Constructed filename depends on number of SJA1105
	 *   chips in configuration and their revision MRA1 or MRA2
	 */
	char fw_name[64];
	/**< Used by request firmware framework to expect completion */
	struct completion conf_loaded;

	int state;
	int sja1105_chip_revision;
	struct sja1105_platform_data *pdata;

	/**< Block list resulting from call to configuration file parsing function */
	struct list_head cfg_block_list;

	int switch_id;
	struct list_head list;


	struct notifier_block notifier_block;
	struct net_device *ndev;
	struct phy_device *phy_dev;
	int last_link;
};

/*
 * Static variables
 *
 */
static LIST_HEAD(switches_list);
static int max_hz = 25000000; /* 25 MHz is the default but might have to be reduced */
module_param(max_hz, int, S_IRUGO);
MODULE_PARM_DESC(max_hz, "SPI bus speed may be limited for the remote SJA1105 application board, 25MHz is the maximum");

static int spimode = 1;
module_param(spimode, int, S_IRUGO);
MODULE_PARM_DESC(spimode, "SPI mode 0 or 1, defaults to 1");

static char *ifname =  "eth0";
module_param(ifname, charp, S_IRUGO);
MODULE_PARM_DESC(ifname, "Network interface name for SJA1105 Host port: default to 'eth0'");

static int verbosity =  0;
module_param(verbosity, int, S_IRUGO);
MODULE_PARM_DESC(verbosity, "Trace level'");

static int rloop = 1;
module_param(rloop, int, S_IRUGO);
MODULE_PARM_DESC(rloop, "number of attempts to get device id. default 1");

static int switches_active = 0;
static DEFINE_MUTEX(sw_lock);
static DEFINE_RWLOCK(rwlock);
static const struct of_device_id sja1105_dt_ids[] = {
  /* single SJA1105 chip configuration */
  {  .compatible = "nxp,sja1105-switch-1-1",  .data = (const void*)0 },
  /* two SJA1105 chips configuration : first chip */
  {  .compatible = "nxp,sja1105-switch-1-2",  .data = (const void*)1 },
  /* two SJA1105 chips configuration : second chip */
  {  .compatible = "nxp,sja1105-switch-2-2",  .data = (const void*)2 },
  /* three SJA1105 chips configuration : first chip */
  {  .compatible = "nxp,sja1105-switch-1-3",  .data = (const void*)3 },
  /* three SJA1105 chips configuration : second chip */
  {  .compatible = "nxp,sja1105-switch-2-3",  .data = (const void*)4 },
  /* three SJA1105 chips configuration : third chip */
  {  .compatible = "nxp,sja1105-switch-3-3",  .data = (const void*)5 },
  { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, sja1105_dt_ids);

struct SJA1105_spi_ops_t {
	uint8_t (*read32)(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);
	uint8_t (*write32)(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);
};

extern uint8_t SJA1105_spiRead32(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);
extern uint8_t SJA1105_spiWrite32(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);

/*
 * Operations to be registered to HAL
 */
const struct SJA1105_spi_ops_t ops = {
	.read32 = SJA1105_spiRead32,
	.write32 = SJA1105_spiWrite32
};

/* Filenames to be located in /lib/firmware for instance */
const char * fw_name_array[] = {
	"sja1105_cfg%s.bin",
	"sja1105_cfg%s_1-2.bin",
	"sja1105_cfg%s_2-2.bin",
	"sja1105_cfg%s_1-3.bin",
	"sja1105_cfg%s_2-3.bin",
	"sja1105_cfg%s_3-3.bin"
};

const char * fw_name_suffix[SJA1105_NB_REV] = {
	"_mra1",
	"_mra2lt",
	"_mra2",
	"_mra2lt",
	"_mra2lt"
};
const int * generic_switch_ids[] = {
	0,
	0,
	1,
	0,
	1,
	2
};
/*
 * Static functions
 *
 */
static void sja1105_registerSpiCB(const struct SJA1105_spi_ops_t *ops)
{
	if (ops) {
		SJA1105P_registerSpiRead32CB(ops->read32);
		SJA1105P_registerSpiWrite32CB(ops->write32);
	} else {
		SJA1105P_registerSpiRead32CB(NULL);
		SJA1105P_registerSpiWrite32CB(NULL);
	}
}


/**
 * sja1105_write - write N words to chip
 * @spi: The spi device
 * @cmd: The command buffer including address and data
 * @nb_words: Number of words to write
 *
 */
static int sja1105_write(struct spi_device *spi, u32 *cmd, u8 nb_words)
{
	struct spi_message m;
	struct spi_transfer t;
	int rc;

	spi_message_init(&m);
	memset(&t, 0, sizeof(t));
	t.tx_buf = cmd;
	t.rx_buf = NULL;
	t.len = (nb_words << 2);
	t.bits_per_word = 16;
	spi_message_add_tail(&t, &m);
	rc = spi_sync(spi, &m);

	if (rc) dev_err(&spi->dev, "spi_sync rc %d\n", rc);

	return rc;
}

/**
 * sja1105_read  - read 32bit by 32 bit from slave
 * @dev: The chip state (device)
 * @reg_addr: The register address to start from
 *
 * @nb_words: number of words to read
 * @read_buffer: buffer where to store read value
 *
 * @return: error status
 */
static int sja1105_read(struct spi_device *spi, u32 *cmd, int nb_words, u32 *read_buffer)
{
	struct spi_message m;
	struct spi_transfer t;
	int rc;

	spi_message_init(&m);
	memset(&t, 0, sizeof(t));
	t.tx_buf = cmd;
	t.rx_buf = read_buffer;
	t.len = (nb_words << 2);
	t.bits_per_word = 16;
	spi_message_add_tail(&t, &m);
	rc = spi_sync(spi, &m);

	if (rc)
		if (verbosity > 3)
			dev_info(&spi->dev, "spi_sync rc %d\n", rc);

	return rc;
}



/**
 * sja1105_cfg_block_write  - write multiple 32bits words to SJA1105
 * @dev: The chip state (device)
 * @reg_addr: The register address to write to
 * @data: The pointer to buffer of 32bits words
 * @nb_words: number of 32bits words to write
 *
 * @return: value read
 */
static int sja1105_cfg_block_write(struct spi_device *spi, u32 reg_addr, u32 *data, int nb_words)
{
	u32 cmd[SJA1105_CONFIG_WORDS_PER_BLOCK+1], upper, down;
	int i = 0, j=0;

	cmd[0] = cpu_to_le32 (CMD_ENCODE_RWOP(CMD_WR_OP) | CMD_ENCODE_ADDR(reg_addr));
	upper = (cmd[0] & 0x0000FFFF) << 16;
	down = (cmd[0] & 0xFFFF0000) >> 16;
	cmd[0] = upper | down;

	while (i < nb_words) {
		cmd[i+1] = *data++;
		upper = (cmd[i+1] & 0x0000FFFF) << 16;
		down = (cmd[i+1] & 0xFFFF0000) >> 16;
		cmd[i+1] = upper | down;
		udelay(10000);
		if (verbosity > 1) dev_info(&spi->dev, "0x%08x\n", cmd[i+1]);
		i++;
	}

	return sja1105_write(spi, cmd, nb_words + 1);
}



/**
 * sja1105_read_reg32  - read 32bit register from slave
 * @dev: The chip state (device)
 * @reg_addr: The register address to start from
 *
 * @return: value read
 */
static u32 sja1105_read_reg32(struct spi_device *spi, u32 reg_addr)
{
	u32 cmd[2], resp[2], upper, down;
	struct spi_message m;
	struct spi_transfer t;

	int rc;

	if (verbosity > 3) dev_info(&spi->dev, "reading 4bytes @%08x tlen %d t.bits_per_word %d\n", reg_addr, 8, 64);

	cmd[0] = cpu_to_le32 (CMD_ENCODE_RWOP(CMD_RD_OP) | CMD_ENCODE_ADDR(reg_addr) | CMD_ENCODE_WRD_CNT(1));
	cmd[1] = 0;

	upper = (cmd[0] & 0x0000FFFF) << 16;
	down = (cmd[0] & 0xFFFF0000) >> 16;
	cmd[0] = upper | down;

	spi_message_init(&m);
	memset(&t, 0, sizeof(t));
	t.tx_buf = cmd;
	t.rx_buf = resp;
	t.len = sizeof(cmd);
	t.bits_per_word = 16;
	spi_message_add_tail(&t, &m);
	rc = spi_sync(spi, &m);

	if (rc)
		if (verbosity > 3)
			dev_info(&spi->dev, "spi_sync rc %d\n", rc);

	upper = (resp[1] & 0x0000FFFF) << 16;
	down = (resp[1] & 0xFFFF0000) >> 16;
	resp[1] = upper | down;

	return le32_to_cpu(resp[1]);
}


/**
 * sja1105_write_reg32  - write 32bit register
 * @spi: The spi device
 * @reg_addr: The register address to be written
 * @val: The value written to register
 *
 * @return: error status
 */
static int sja1105_write_reg32(struct spi_device *spi, u32 reg_addr, u32 val)
{
	u32 cmd[2], resp[2], upper, down;
	struct spi_message m;
	struct spi_transfer t;
	int rc;

	if (verbosity > 3) dev_info(&spi->dev, "writing 4bytes @%08x tlen %d t.bits_per_word %d\n", reg_addr, 8, 64);

	cmd[0] = cpu_to_le32 (CMD_ENCODE_RWOP(CMD_WR_OP) | CMD_ENCODE_ADDR(reg_addr) | CMD_ENCODE_WRD_CNT(1));
	upper = (cmd[0] & 0x0000FFFF) << 16;
	down = (cmd[0] & 0xFFFF0000) >> 16;
	cmd[0] = upper | down;

	cmd[1] = val;
	upper = (cmd[1] & 0x0000FFFF) << 16;
	down = (cmd[1] & 0xFFFF0000) >> 16;
	cmd[1] = upper | down;

	spi_message_init(&m);
	memset(&t, 0, sizeof(t));
	t.tx_buf = cmd;
	t.rx_buf = resp;
	t.len = sizeof(cmd);
	t.bits_per_word = 16;
	spi_message_add_tail(&t, &m);
	rc = spi_sync(spi, &m);

	if (rc)
		if (verbosity > 3)
			dev_info(&spi->dev, "spi_sync rc %d\n", rc);

	return rc;
}


/*
 * Public functions
 *
 */
uint8_t SJA1105_spiRead32(uint8_t deviceSelect, uint8_t wordCount,
			  uint32_t registerAddress,
			  uint32_t *p_registerValue)
{
	struct spi_device *spi = NULL;
	struct sja1105_context_data *data;
	int i = 0;

	read_lock(&rwlock);
	list_for_each_entry(data, &switches_list, list) {
		if(data->switch_id == deviceSelect){
		    spi = data->spi_dev;
		    break;
		}
		i++;
	}
	read_unlock(&rwlock);

	if( !spi ) {
		read_lock(&rwlock);
		if(i <= switches_active) {
			read_unlock(&rwlock);
			return 0;
		}
		else {
			printk("No device found for %d \n", deviceSelect);
			read_unlock(&rwlock);
			return -EINVAL;
		}
	}

	i=0;

	if (verbosity > 3) dev_info(&spi->dev, "%s: device %d wordCount=%d, registerAddress=%08x\n", __func__, deviceSelect, wordCount, registerAddress);

	while (i < wordCount) {
		*p_registerValue = sja1105_read_reg32(&spi->dev, registerAddress);
		if (verbosity > 3) dev_info(&spi->dev, "%s: wordCount %d i %d registerAddress=%08x registerValue=%08x\n", __func__, wordCount, i, registerAddress, *p_registerValue);
		p_registerValue++;
		registerAddress++;
		i++;
	}


	return 0;
}

uint8_t SJA1105_spiWrite32(uint8_t deviceSelect, uint8_t wordCount,
			   uint32_t registerAddress,
			   uint32_t *p_registerValue)
{
	struct spi_device *spi = NULL;
	struct sja1105_context_data *data;
	int i = 0;


	read_lock(&rwlock);
	list_for_each_entry(data, &switches_list, list) {
		if( data->switch_id == deviceSelect) {
		    spi = data->spi_dev;
		    break;
		}
		i++;
	}
	read_unlock(&rwlock);

	if( !spi ) {
		read_lock(&rwlock);
		if( i <= switches_active) {
		    read_unlock(&rwlock);
		    return 0;
		}
		else {
		    printk("No device found for %d \n", deviceSelect);
		    read_unlock(&rwlock);
		    return -EINVAL;
		}
	}

	i = 0;

	if (verbosity > 3) dev_info(&spi->dev, "%s: device %d wordCount=%d, registerAddress=%08x registerValue=%08x\n", __func__, deviceSelect, wordCount, registerAddress, *p_registerValue);

	sja1105_cfg_block_write(spi, registerAddress, p_registerValue, wordCount);

	return 0;
}

#define NUM_MAC_LVL_COUNTERS1 4
static char * mac_lvl_counters1 [NUM_MAC_LVL_COUNTERS1] = {
	"N_RUNT",
	"N_SOFERR",
	"N_ALIGNERR",
	"N_MIIERR",
};

#define NUM_MAC_LVL_COUNTERS2 10
static char * mac_lvl_counters2 [NUM_MAC_LVL_COUNTERS2] = {
	"RSVD",
	"SPCERRS",
	"DRN664ERRS",
	"RSVD",
	"BAGDROP",
	"LENDROPS",
	"PORTDROPS",
	"RSVD",
	"SPCPRIOR",
	"RSVD",
};

#define NUM_ETH_HIGH_LVL_COUNTERS1 16
static char * eth_high_lvl_counters1 [NUM_ETH_HIGH_LVL_COUNTERS1] = {
	"N_TXBYTE",
	"N_TXBYTESH",
	"N_TXFRM",
	"N_TXFRMSH",
	"N_RXBYTE",
	"N_RXBYTESH",
	"N_RXFRM",
	"N_RXFRMSH",
	"N_POLERR",
	"RSVD",
	"RSVD",
	"N_CRCERR",
	"N_SIZERR",
	"RSVD",
	"N_VLANERR",
	"N_N664ERR",
};

#define NUM_ETH_HIGH_LVL_COUNTERS2 4
static char * eth_high_lvl_counters2 [NUM_ETH_HIGH_LVL_COUNTERS2] = {
	"N_NOT_REACH",
	"N_ERG_DISABLED",
	"N_PART_DROP",
	"N_QFULL",
};


static void sja1105_cgu_dump(struct spi_device *spi)
{
	u32 val[50];

	dev_info(&spi->dev, "\nCGU settings\n");
	val[0] = sja1105_read_reg32(spi, 0x00100007);
	val[1] = sja1105_read_reg32(spi, 0x00100009);
	val[2] = sja1105_read_reg32(spi, 0x0010000A);

	val[3] = sja1105_read_reg32(spi, 0x0010000B);
	val[4] = sja1105_read_reg32(spi, 0x0010000C);
	val[5] = sja1105_read_reg32(spi, 0x0010000D);
	val[6] = sja1105_read_reg32(spi, 0x0010000E);
	val[7] = sja1105_read_reg32(spi, 0x0010000F);

	val[8] = sja1105_read_reg32(spi, 0x00100013);
	val[9] = sja1105_read_reg32(spi, 0x00100014);
	val[10] = sja1105_read_reg32(spi, 0x00100015);
	val[11] = sja1105_read_reg32(spi, 0x00100016);
	val[12] = sja1105_read_reg32(spi, 0x00100018);
	val[13] = sja1105_read_reg32(spi, 0x00100019);

	val[14] = sja1105_read_reg32(spi, 0x0010001A);
	val[15] = sja1105_read_reg32(spi, 0x0010001B);
	val[16] = sja1105_read_reg32(spi, 0x0010001C);
	val[17] = sja1105_read_reg32(spi, 0x0010001D);
	val[18] = sja1105_read_reg32(spi, 0x0010001F);
	val[19] = sja1105_read_reg32(spi, 0x00100020);

	val[20] = sja1105_read_reg32(spi, 0x00100021);
	val[21] = sja1105_read_reg32(spi, 0x00100022);
	val[22] = sja1105_read_reg32(spi, 0x00100023);
	val[23] = sja1105_read_reg32(spi, 0x00100024);
	val[24] = sja1105_read_reg32(spi, 0x00100026);
	val[25] = sja1105_read_reg32(spi, 0x00100027);

	val[26] = sja1105_read_reg32(spi, 0x00100028);
	val[27] = sja1105_read_reg32(spi, 0x00100029);
	val[28] = sja1105_read_reg32(spi, 0x0010002A);
	val[29] = sja1105_read_reg32(spi, 0x0010002B);
	val[30] = sja1105_read_reg32(spi, 0x0010002D);
	val[31] = sja1105_read_reg32(spi, 0x0010002E);

	val[32] = sja1105_read_reg32(spi, 0x0010002F);
	val[33] = sja1105_read_reg32(spi, 0x00100030);
	val[34] = sja1105_read_reg32(spi, 0x00100031);
	val[35] = sja1105_read_reg32(spi, 0x00100032);
	val[36] = sja1105_read_reg32(spi, 0x00100034);
	val[37] = sja1105_read_reg32(spi, 0x00100035);

	val[38] = sja1105_read_reg32(spi, 0x00100810);
	val[39] = sja1105_read_reg32(spi, 0x00100811);
	val[40] = sja1105_read_reg32(spi, 0x00100812);
	val[41] = sja1105_read_reg32(spi, 0x00100813);
	val[42] = sja1105_read_reg32(spi, 0x00100814);

	dev_info(&spi->dev, "\nPLL_0_S 0x%x PLL_1_S 0x%x PLL_2_S 0x%x\n", val[0], val[1], val[2]);

	dev_info(&spi->dev, "\n IDIV_0_C 0x%x IDIV_1_C 0x%x IDIV_2_C 0x%x IDIV_3_C 0x%x IDIV_4_C 0x%x \n", val[3], val[4], val[5], val[6], val[7]);

	dev_info(&spi->dev, "\nPORT0\n");

	dev_info(&spi->dev, "\nMII0_MII_TX_CLK 0x%x MII0_MII_RX_CLK 0x%x MII0_RMII_REF_CLK 0x%x MII0_RGMII_TX_CLK 0x%x MII0_EXT_TX_CLK 0x%x MII0_EXT_RX_CLK 0x%x\n", val[8], val[9], val[10], val[11], val[12], val[13] );

	dev_info(&spi->dev, "\nPORT1\n");

	dev_info(&spi->dev, "\nMII1_MII_TX_CLK 0x%x MII1_MII_RX_CLK 0x%x MII1_RMII_REF_CLK 0x%x MII1_RGMII_TX_CLK 0x%x MII1_EXT_TX_CLK 0x%x MII1_EXT_RX_CLK 0x%x\n", val[14], val[15], val[16], val[17], val[18], val[19] );

	dev_info(&spi->dev, "\nPORT2\n");

	dev_info(&spi->dev, "\nMII2_MII_TX_CLK 0x%x MII2_MII_RX_CLK 0x%x MII2_RMII_REF_CLK 0x%x MII2_RGMII_TX_CLK 0x%x MII2_EXT_TX_CLK 0x%x MII2_EXT_RX_CLK 0x%x\n", val[20], val[21], val[22], val[23], val[24], val[25] );

	dev_info(&spi->dev, "\nPORT3\n");

	dev_info(&spi->dev, "\nMII3_MII_TX_CLK 0x%x MII3_MII_RX_CLK 0x%x MII3_RMII_REF_CLK 0x%x MII3_RGMII_TX_CLK 0x%x MII3_EXT_TX_CLK 0x%x MII3_EXT_RX_CLK 0x%x\n", val[26], val[27], val[28], val[29], val[30], val[31] );

	dev_info(&spi->dev, "\nPORT4\n");

	dev_info(&spi->dev, "\nMII4_MII_TX_CLK 0x%x MII4_MII_RX_CLK 0x%x MII4_RMII_REF_CLK 0x%x MII4_RGMII_TX_CLK 0x%x MII4_EXT_TX_CLK 0x%x MII4_EXT_RX_CLK 0x%x\n", val[32], val[33], val[34], val[35], val[36], val[37] );

	dev_info(&spi->dev, "\nCFG_PAD_MII0_ID 0x%x CFG_PAD_MII1_ID 0x%x CFG_PAD_MII2_ID 0x%x CFG_PAD_MII3_ID 0x%x CFG_PAD_MII4_ID 0x%x \n", val[38], val[39], val[40], val[41], val[42] );
}

static int sja1105_stats_show(struct seq_file *s, void *data)
{
	struct spi_device *spi = s->private;
	u32 val32;
	int i, j;

	if (verbosity > 1) {
		seq_printf(s, "\nGeneral Status\n");
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS1);
		seq_printf(s, "general_status_1    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS2);
		seq_printf(s, "general_status_2    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS3);
		seq_printf(s, "general_status_3    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS4);
		seq_printf(s, "general_status_4    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS5);
		seq_printf(s, "general_status_5    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS6);
		seq_printf(s, "general_status_6    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS7);
		seq_printf(s, "general_status_7    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS8);
		seq_printf(s, "general_status_8    = %08x\n", val32);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_GENERAL_STATUS9);
		seq_printf(s, "general_status_9    = %08x\n", val32);
	}

	for (i=0; i<SJA1105_PORT_NB; i++) {
		seq_printf(s, "\nEthernet MAC-level status port%d\n", i);
		val32 = sja1105_read_reg32(spi, SJA1105_REG_PORT_MAC_STATUS(i));
		for (j=0; j<NUM_MAC_LVL_COUNTERS1; j++)
			seq_printf(s, "port%d %s    = %u\n", i, mac_lvl_counters1[j], val32 & (0xFF << (j*8)));

		val32 = sja1105_read_reg32(spi, SJA1105_REG_PORT_MAC_STATUS(i) + 1);
		for (j=0; j<NUM_MAC_LVL_COUNTERS2; j++)
			seq_printf(s, "port%d %s    = %u\n", i, mac_lvl_counters2[j], val32 & (1 << j));
	}

	for (i=0; i<SJA1105_PORT_NB; i++) {
		seq_printf(s, "\nEthernet High-level status port%d\n", i);
		for (j=0; j<NUM_ETH_HIGH_LVL_COUNTERS1; j++) {
			val32 = sja1105_read_reg32(spi, SJA1105_REG_PORT_HIGH_STATUS1(i) + j);
			seq_printf(s, "port%d %s    = %u\n", i, eth_high_lvl_counters1[j], val32);
		}
		for (j=0; j<NUM_ETH_HIGH_LVL_COUNTERS2; j++) {
			val32 = sja1105_read_reg32(spi, SJA1105_REG_PORT_HIGH_STATUS2(i) + j);
			seq_printf(s, "port%d %s    = %u\n", i, eth_high_lvl_counters2[j], val32);
		}
	}
	if (verbosity > 1)
		sja1105_cgu_dump(spi);


	return 0;
}

static int sja1105_stats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_stats_show, inode->i_private);
}



static const struct file_operations sja1105_fops = {
	.open		= sja1105_stats_seq_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static void sja1105_debugfs_init(struct spi_device *spi)
{
	struct dentry *sja_dentry;
	char name[64];
	struct sja1105_context_data *data = spi_get_drvdata(spi);

	snprintf(name, sizeof(name), "sja1105_%d", data->switch_id);
	sja_dentry = debugfs_create_dir(name, NULL);
	if (!sja_dentry)
		return;

	debugfs_create_file("stats", S_IRUSR, sja_dentry, spi, &sja1105_fops);
}


static int sja1105_check_device_id(struct spi_device *spi)
{
	int ret = -1;
	int i;
	u32 devid = sja1105_read_reg32(spi, SJA1105_REG_DEVICEID);

	for (i = 0; i < SJA1105_NB_REV; i++) {
		if (devid == device_id_list[i]) {
			ret = i;
			if (verbosity > 3)
				dev_info(&spi->dev, "Detected Device ID %08x (%s)\n", devid, fw_name_suffix[i]);
			break;
		}
	}

	return ret;
}


static bool sja1105_check_device_status(struct spi_device *spi, bool expected_status, bool *pstatus)
{
	u32 status;
	u32 expected_val = expected_status ? SJA1105_BIT_STATUS_CONFIG_DONE : 0;
	bool ret = true;

	status = sja1105_read_reg32(spi, SJA1105_REG_STATUS);

	*pstatus = (expected_val == (status & SJA1105_BIT_STATUS_CONFIG_DONE));

	if (expected_status && !*pstatus) {
		if (verbosity > 3)
			dev_info(&spi->dev, "Error: SJA1105_REG_STATUS=%08x - LocalCRCfail=%d - DevID unmatched=%d, GlobalCRCfail=%d\n",
				status,
				(int)(status & SJA1105_BIT_STATUS_CRCCHKL),
				(int)(status & SJA1105_BIT_STATUS_DEVID_MATCH),
				(int)(status & SJA1105_BIT_STATUS_CRCCHKG));
		ret = false;
	}

	return ret;
}


static void sja1105_reset(struct spi_device *spi)
{
	u32 rst = sja1105_read_reg32(spi, SJA1105_REG_RESET_CTRL);
        u32 cmd[2], upper, down;

	rst |= SJA1105_BIT_RESET_CTRL_COLDRESET;

        cmd[0] = cpu_to_le32 (CMD_ENCODE_RWOP(CMD_WR_OP) | CMD_ENCODE_ADDR(SJA1105_REG_RESET_CTRL));
        upper = (cmd[0] & 0x0000FFFF) << 16;
        down = (cmd[0] & 0xFFFF0000) >> 16;
        cmd[0] = upper | down;

        upper = (rst & 0x0000FFFF) << 16;
        down = (rst & 0xFFFF0000) >> 16;
        cmd[1] = upper | down;

        return sja1105_write(spi, cmd, 2);
}


static int sja1105_config_avb_param_check(struct spi_device *spi, struct sja1105_context_data *sw_ctx)
{
	struct sja1105_cfg_block * avb_param_block;
	u8 * p;
	u8 metaframe_sa[6];
	u8 metaframe_da[6];
	int i;
	struct net_device *ndev;

	if (verbosity > 2) dev_info(&spi->dev, "Looking for AVB parameters\n");

	/* Seek for AVB parameters section of config file if any */
	avb_param_block = sja1105_config_block_seek(&sw_ctx->cfg_block_list, SJA1105_BLOCK_AVB_PARAMETERS);
	if (avb_param_block /*&& avb_param_block->block_sz == 3 already checked */) {
		p = (u8*)avb_param_block->block_data;
		for (i = 0; i<6; i++) {
			metaframe_da[i] = p[i+6];
			metaframe_sa[i] = p[i];
		}

		ndev = dev_get_by_name(&init_net, ifname);

		if (verbosity > 2) dev_info(&spi->dev, "Got dev=%p name= %s\n", ndev, ifname);

		if (ndev) {
			/* Let 's register the address that is programmed as Metaframe DA as a Unicast entry.
			An alternative would have been to read the address and patch the AVB parameters block
			but would require CRC32 recomputation over the block and the whole file */
			//dev_uc_add(ndev, (const unsigned char *)metaframe_da);

			if (verbosity > 2) dev_info(&spi->dev, "Registered management port DST MAC address=%02x:%02x:%02x:%02x:%02x:%02x\n",
				metaframe_da[0],
				metaframe_da[1],
				metaframe_da[2],
				metaframe_da[3],
				metaframe_da[4],
				metaframe_da[5]);
			if (verbosity > 2) dev_info(&spi->dev, "Registered management port SRC MAC address=%02x:%02x:%02x:%02x:%02x:%02x\n",
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

static int sja1105_config_xmii_param_check(struct spi_device *spi, struct sja1105_context_data *sw_ctx)
{
	struct sja1105_cfg_block * xmii_param_block;
	u8 * p;
	u32  xmii_parameters;
	int xmii_shift_offset;
	u32 xmii_from_cfg;
	int i;
	
	if (verbosity > 2) dev_info(&spi->dev, "Looking for xMII parameters\n");

	xmii_param_block = sja1105_config_block_seek(&sw_ctx->cfg_block_list, SJA1105_BLOCK_xMII_PARAMETERS);
	if (xmii_param_block /*&& xmii_param_block->block_sz == 1*/) {
		p = (u8*)xmii_param_block->block_data;
		xmii_parameters = 0;
		xmii_shift_offset = 32 - 3*SJA1105_PORT_NB;

		/* Apply port configuration as discovered in DTS could be done */
		/* For now just check consistency */
		for (i = 0; i < SJA1105_PORT_NB; i++) {
			struct port_desc * port = &sw_ctx->pdata->ports[i];
			xmii_parameters |= ((port->xmii_mode & 0x3) + ((port->phy_not_mac&1) << 2)) << (xmii_shift_offset + 3*i);
			if (verbosity > 3) dev_info(&sw_ctx->spi_dev->dev, "XMII(port%d) xmii_mode %d phy_not_mac %d xmii_shift_offset %d\n", i, port->xmii_mode & 0x3, port->phy_not_mac & 1, xmii_shift_offset + 3*i); 
		}

		xmii_from_cfg = (p[3] << 24)| (p[2]<<16) | (p[1] << 8) | p[0];

		if (verbosity > 3) dev_info(&sw_ctx->spi_dev->dev, "XMII check Got %x from file expected %x\n", xmii_from_cfg,xmii_parameters); 
/*
		if (xmii_from_cfg != xmii_parameters)
			return -1;
*/
	}
	return 0;
}

bool sja1105_post_cfg_load_check(struct spi_device *spi,  struct sja1105_context_data *sw_ctx)
{
	int chip_id;
	bool status;

	/* Trying to read back the SJA1105 status via SPI... */
	chip_id  = sja1105_check_device_id(spi);
	if (chip_id != sw_ctx->sja1105_chip_revision)
		return false;

	if (!sja1105_check_device_status(spi, true, &status))
		return false;

	return status;
}


static void sja1105_configuration_load(const struct firmware *config_file, void *spi_context)
{
	struct spi_device *spi = (struct spi_device *)spi_context;
	struct sja1105_context_data *data = spi_get_drvdata(spi);
	int nb_words;
	u32 * cfg_data;
	int remaining_words;
	u32 dev_addr;
	u32 val;
	bool swap_required;
	int i;
	SJA1105P_portStatusMiixArgument_t portStatusMiix;

	mutex_lock(&sw_lock);
	/* read back ports configuration */
	for (i=0; i<SJA1105_PORT_NB; i++) {
		SJA1105P_getPortStatusMiix(&portStatusMiix, i, data->switch_id);
		if (verbosity > 1)
			dev_info(&spi->dev, "REG_PORT_STATUS(%d): speed=%d phyMode=%d xmiiMode=%d\n", i, portStatusMiix.speed, portStatusMiix.phyMode, portStatusMiix.xmiiMode);
	}

	if (verbosity > 2) sja1105_cgu_dump(spi);

	if (config_file->size == 0) {
		dev_err(&spi->dev, "Error: SJA1105 Switch configuration is empty\n");
		goto err_cfg;
	}

	nb_words = (config_file->size >> 2);
	cfg_data = (u32*)config_file->data;

	/* Check that file is actually compatible with chip revision */
	if (data->sja1105_chip_revision < 0 || data->sja1105_chip_revision >= SJA1105_NB_REV) {
		dev_err(&spi->dev, "Error: SJA1105 unhandled revision\n");
		goto err_cfg;
	}

	val = cfg_data[0];
	if (val ==__builtin_bswap32(device_id_list[data->sja1105_chip_revision]) ) {
		dev_err(&spi->dev, "Config file requires swap, incorrect endianness\n");
		swap_required = true;
	}
	else if (val == device_id_list[data->sja1105_chip_revision])
		swap_required = false;
	else {
		dev_err(&spi->dev, "Error: SJA1105 unhandled revision Switch incompatible configuration file (%x - %x)\n", val, device_id_list[data->sja1105_chip_revision]);
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
	sja1105_config_file_split_blocks(spi, &data->cfg_block_list, cfg_data, nb_words);
	if (list_empty(&data->cfg_block_list)) {
		dev_err(&spi->dev, "SJA1105 error parsing configuration file\n");
		goto err_cfg;
	}

	if (sja1105_config_avb_param_check(spi, data) < 0) {
		dev_err(&spi->dev, "SJA1105 failed to get net device!\n");
		goto err_cfg;
	}
	if (sja1105_config_xmii_param_check(spi, data) < 0) {
		dev_err(&spi->dev, "SJA1105 XMII parameters mismatch\n");
		goto err_cfg;
	}

	if (verbosity > 1) dev_info(&spi->dev, "swap_required %d nb_words %d dev_addr %08x\n", swap_required, nb_words, (u32)SJA1105_CONFIG_START_ADDRESS);

	remaining_words = nb_words;
	dev_addr = SJA1105_CONFIG_START_ADDRESS;

	i = 0;
	while (remaining_words > 0) {
		int block_size_words = MIN(1/*SJA1105_CONFIG_WORDS_PER_BLOCK*/, remaining_words);

		if (verbosity > 2) dev_info(&spi->dev, "block_size_words %d remaining_words %d\n", block_size_words, remaining_words);

		if (sja1105_cfg_block_write(spi, dev_addr, cfg_data, block_size_words) < 0)
			goto err_cfg;

		if (verbosity > 1) dev_info(&spi->dev, "Loaded block %d @%08x\n", i, dev_addr);

		dev_addr += block_size_words;
		cfg_data += block_size_words;
		remaining_words -= block_size_words;
		i++;
		if(i%10==0)sja1105_post_cfg_load_check(spi, data);
	}

	if (!sja1105_post_cfg_load_check(spi, data))  {
		dev_err(&spi->dev, "SJA1105 configuration failed\n");
		goto err_cfg;
	}	

	if (data->switch_id == LAST_SWITCH_ID) {
		int k = 0;
		for (k = 0; k < SWITCHES_TO_CONFIGURE; k++) {
			/* perform config sync for each ports */
			if (SJA1105P_synchSwitchConfiguration_extended(k/*data->switch_id*/) != 0) {
				dev_err(&spi->dev, "SJA1105 port autoconfiguration failed\n");
				goto err_cfg;
			}

			/* perform autoconfiguration for each ports */
			if (SJA1105P_autoConfigPorts_extended(k /*data->switch_id*/) != 0) {
				dev_err(&spi->dev, "SJA1105 port autoconfiguration failed\n");
				goto err_cfg;
			}
		}
	}
	dev_info(&spi->dev, "SJA1105 successfully configured!\n");

	/* read back ports configuration */
	for (i=0; i<SJA1105_PORT_NB; i++) {
		SJA1105P_getPortStatusMiix(&portStatusMiix, i, data->switch_id);
		if (verbosity > 3)
			dev_info(&spi->dev, "REG_PORT_STATUS(%d): speed=%d phyMode=%d xmiiMode=%d\n", i, portStatusMiix.speed, portStatusMiix.phyMode, portStatusMiix.xmiiMode);
	}

	if (verbosity > 2) sja1105_cgu_dump(spi);

	/*
	* Don't forget to release the firmware again
	*/
	release_firmware(config_file);

	complete(&data->conf_loaded);
	mutex_unlock(&sw_lock);
	return;

err_cfg:
	mutex_unlock(&sw_lock);
	release_firmware(config_file);
}


static void sja1105_port_reset(struct sja1105_context_data *switch_ctx)
{
	struct spi_device *spi = switch_ctx->spi_dev;
	u32 cfg_pad_mii_reg;
	int i, j, max_retries = 5;

	/*
	 * Reset only ports 0 and 4 because the other ones are cascaded with
	 * other switches
	 */
	for (i = 0; i < SJA1105_PORT_NB; i+=4) {
		cfg_pad_mii_reg = sja1105_read_reg32(spi,
					 SJA1105_CFG_PAD_MIIX_ID_PORT(i));

		/* Toggle RX Clock PullDown and Bypass */
		cfg_pad_mii_reg |= SJA1105_CFG_PAD_MIIX_ID_RXC_PD;
		cfg_pad_mii_reg |= SJA1105_CFG_PAD_MIIX_ID_RXC_BYPASS;

		for (j = 0; j < max_retries; j++) {
			sja1105_write_reg32(spi, SJA1105_CFG_PAD_MIIX_ID_PORT(i), cfg_pad_mii_reg);

			if (cfg_pad_mii_reg != sja1105_read_reg32(spi,
								  SJA1105_CFG_PAD_MIIX_ID_PORT(i))) {
				dev_err(&spi->dev, "Failed to reset delay\n");
				continue;
			}
			else {
				break;
			}
		}

		cfg_pad_mii_reg &= ~SJA1105_CFG_PAD_MIIX_ID_RXC_PD;
		cfg_pad_mii_reg &= ~SJA1105_CFG_PAD_MIIX_ID_RXC_BYPASS;

		for (j = 0; j < max_retries; j++) {
			sja1105_write_reg32(spi, SJA1105_CFG_PAD_MIIX_ID_PORT(i), cfg_pad_mii_reg);

			if (cfg_pad_mii_reg != sja1105_read_reg32(spi,
								  SJA1105_CFG_PAD_MIIX_ID_PORT(i))) {
				dev_err(&spi->dev, "Failed to reset delay\n");
				continue;
			}
			else {
				break;
			}
		}
	}
}


/* netdev event handler */
static int sja1105_netdev_event(struct notifier_block *this, unsigned long event,
							 void *ptr)
{
	struct sja1105_context_data *switch_ctx = container_of(this, struct sja1105_context_data, notifier_block);
	struct spi_device *spi = switch_ctx->spi_dev;

	spi_set_drvdata(spi, switch_ctx);

	if (event == NETDEV_CHANGE) {
		sja1105_port_reset(switch_ctx);
	}
	return NOTIFY_STOP;
}


static void sja1105_enet_adjust_link(struct net_device *ndev)
{
	struct phy_device *phy_dev = ndev->phydev;
	struct sja1105_context_data *data, *sw_context;
	int deviceSelect = 0;

	read_lock(&rwlock);
	list_for_each_entry(data, &switches_list, list) {
		if(data->switch_id == deviceSelect){
		    sw_context = data;
		    break;
		}
	}
	read_unlock(&rwlock);

	if (sw_context->last_link == 0 &&
	    phy_dev->link == 1) {
		/* reset delay lines */
		sja1105_port_reset(sw_context);
	}

	sw_context->last_link = phy_dev->link;
}


int sja1105_init_dt(struct sja1105_context_data *switch_ctx)
{
	struct sja1105_platform_data *pdata;
	struct device_node *np = switch_ctx->of_node;
	u32 val;
	struct device_node * port_node;
	int i;
	int rc = 0;
	const char * xmii_mode_str;
	struct device_node  *phy_node;

	/* Register netdev notifier */
	switch_ctx->notifier_block.notifier_call = sja1105_netdev_event;
	register_netdevice_notifier(&switch_ctx->notifier_block);

	if (!np) {
		if (verbosity > 2) dev_info(&switch_ctx->spi_dev->dev, "No OF node\n");
		return 0;
	}
	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	pdata->host_port_id = SJA1105_PORT_NB;
	for (i = 0; i < SJA1105_PORT_NB; i++) {
		char str_to_find[32];
		snprintf(str_to_find, 32, "port-%d", i);
		port_node = of_find_node_by_name(np, str_to_find);
		if (!port_node) {
			if (verbosity > 2) dev_err(&switch_ctx->spi_dev->dev, "Failed to find node port-%d\n", i);
			goto err_dt;
		}
		if (verbosity > 1) dev_info(&switch_ctx->spi_dev->dev, "Properties of port %d found\n", i);

		if (!of_property_read_u32(port_node, "speed", &val)) {
			if (verbosity > 3)
				dev_info(&switch_ctx->spi_dev->dev, "port-%d speed=%d\n", i, val);
			switch (val) {
			case 10:
				pdata->ports[i].speed =  SJA1105P_e_speed_10_MBPS;
				break;
			case 100:
				pdata->ports[i].speed =  SJA1105P_e_speed_100_MBPS;
				break;
			case 1000:
				pdata->ports[i].speed =  SJA1105P_e_speed_1_GBPS;
				break;
			default:
				dev_err(&switch_ctx->spi_dev->dev, "Bad speed found val=%d\n", val);
				goto err_dt;
			}
		} else {
			pdata->ports[i].speed =  SJA1105P_e_speed_100_MBPS; /*  default */
		}

		pdata->ports[i].is_host = false;
		if (!of_property_read_u32(port_node, "is-host", &val)) {
			if (verbosity > 3)
				dev_info(&switch_ctx->spi_dev->dev, "port-%d is-host=%d\n", i, val);
			if (val != 0) {
				pdata->ports[i].is_host = true;
				if (pdata->host_port_id == SJA1105_PORT_NB)
					pdata->host_port_id = i;
				else {
					dev_err(&switch_ctx->spi_dev->dev, "Only one port can be Host Port (management)" "port-%d current is port-%d\n", i, pdata->host_port_id);
					goto err_dt;
				}
			}
		}
		if (of_property_read_string(port_node, "xmii-mode", &xmii_mode_str)) {
			dev_err(&switch_ctx->spi_dev->dev, "Failed to find xmii-mode\n");
			goto err_dt;
		}

		/* connect PHY to Switch */
		phy_node = of_parse_phandle(port_node, "phy-handle", 0);
		if (phy_node) {
			switch_ctx->phy_dev = of_phy_connect(switch_ctx->ndev, phy_node,
							     &sja1105_enet_adjust_link, 0,
							     PHY_INTERFACE_MODE_RGMII);
			if (!switch_ctx->phy_dev)
				return -ENODEV;
			phy_start_aneg(switch_ctx->phy_dev);
		}

		if (strcmp (xmii_mode_str, "MII") == 0)
			pdata->ports[i].xmii_mode = SJA1105P_e_xmiiMode_MII;
		else if (strcmp (xmii_mode_str, "RMII") == 0)
			pdata->ports[i].xmii_mode = SJA1105P_e_xmiiMode_RMII;
		else if (strcmp (xmii_mode_str, "RGMII") == 0)
			pdata->ports[i].xmii_mode = SJA1105P_e_xmiiMode_RGMII;
		else
			pdata->ports[i].xmii_mode = SJA1105P_e_xmiiMode_RMII; /*  default */

		if (verbosity > 1) dev_info(&switch_ctx->spi_dev->dev, "port-%d xmii-mode=%s (xmii_mode=%d)\n", i, xmii_mode_str, pdata->ports[i].xmii_mode);

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


static ssize_t sja1105_read_sja_reg(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf,
				    size_t count)
{
	int items;
	struct spi_device *spidev = to_spi_device(dev);
	u32 regnum;
	int val;

	items = sscanf(buf, "%x", &regnum);
	if (items != 1) {
		pr_info("Expected \"regnum\" argument");
		return -EINVAL;
	}

	val = sja1105_read_reg32(spidev, regnum);

	pr_info("Read sja reg %#x: %#x\n", regnum, val);

	return count;
}

static ssize_t sja1105_write_sja_reg(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf,
				     size_t count)
{
	int err, items;
	struct spi_device *spidev = to_spi_device(dev);
	u32 regnum;
	u16 val;

	items = sscanf(buf, "%x %hx", &regnum, &val);
	if (items != 2) {
		pr_info("Expected arguments: \"0x<regnum> 0x<val>\"");
		return -EINVAL;
	}
	pr_info("Writing sja reg %#x with value %#x\n", regnum, val);

	err = sja1105_cfg_block_write(spidev, regnum, &val, 1);
	if (err) {
		pr_info("sja1105_cfg_block_write() error %d\n", err);
		return -EPERM;
	}

	return count;
}

static struct device_attribute sja1105_attrs[] = {
	__ATTR(read_sja_reg,
	       S_IWUSR,
	       NULL,
	       sja1105_read_sja_reg),

	__ATTR(write_sja_reg,
	       S_IWUSR,
	       NULL,
	       sja1105_write_sja_reg),
};

static void sja1105_sysfs_init(struct device *dev)
{
	int i, err;

	for (i = 0; i < ARRAY_SIZE(sja1105_attrs); i++) {
		err = device_create_file(dev, &sja1105_attrs[i]);
		if (err) {
			dev_err(dev, "ERROR creating sysfs file\n");
			goto undo;
		}
	}
	return;

undo:
	while (i > 0)
		device_remove_file(dev, &sja1105_attrs[--i]);
}

static void sja1105_sysfs_remove(struct device *dev)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sja1105_attrs); i++)
		device_remove_file(dev, &sja1105_attrs[i]);
}

static int sja1105_probe(struct spi_device *spi)
{
	struct sja1105_context_data *switch_ctx;
	int err;
	const struct of_device_id *match;
	int fw_idx;

	dev_info(&spi->dev, "Loading SJA1105 SPI driver\n");

	switch_ctx = devm_kzalloc(&spi->dev, sizeof(struct sja1105_context_data), GFP_KERNEL);
	if (!switch_ctx) {
		dev_err(&spi->dev, "Memory allocation for sja1105_context_data failed\n");
		return -ENOMEM;
	}

	switch_ctx->state = 0;
	switch_ctx->spi_dev = spi;
	/* SCLK held low between frames Data latched on second clock edge 
	i.e: CPOL=0-CPHA=1 alias MODE 1*/
	spi->mode = SPI_MODE_1;
	spi->bits_per_word = 16;
	spi->max_speed_hz = max_hz;

	write_lock(&rwlock);
	switches_active++;
	list_add_tail(&switch_ctx->list, &switches_list);
	write_unlock(&rwlock);

	if (verbosity > 1) dev_info(&spi->dev, "SJA1105 SPI Clock set to %dHz\n", max_hz);

	err = spi_setup(spi);
	if (err < 0) {
		dev_err(&spi->dev, "SJA1105 SPI Setup failed err=%d\n", err); 
		return err;
	}

	/* starting from now, the HAL can issue SPI r/w access to the device */
	sja1105_registerSpiCB(&ops);

	switch_ctx->state = 1;
	long k = 0;

	switch_ctx->sja1105_chip_revision = sja1105_check_device_id(spi);
/*
	while( k < 100 )
	{
 		sja1105_cgu_dump(spi);

		k++;
	}
*/
	if (switch_ctx->sja1105_chip_revision < 0 ) {
		dev_err(&spi->dev, "SJA1105 SPI Failed to read Device Id\n");
		return -ENODEV;
	} else if (switch_ctx->sja1105_chip_revision >  SJA1105_NB_REV) {
		dev_err(&spi->dev, "SJA1105 SPI unsupported version\n"); 
		return -ENODEV;
	}

	switch_ctx->last_link = 0;
	switch_ctx->ndev = dev_get_by_name(&init_net, ifname);

	switch_ctx->state = 2;
	switch_ctx->of_node = spi->dev.of_node;
	if ( switch_ctx->of_node ) {
		match = of_match_device(sja1105_dt_ids, &spi->dev);
		if (match) {
			fw_idx = (int) match->data;
			if (fw_idx < 0 || fw_idx > 5) {
				dev_err(&spi->dev, "SJA1105 Bad device table contents\n");
				return -EINVAL;
			}
			switch_ctx->switch_id = generic_switch_ids[fw_idx];
			snprintf(switch_ctx->fw_name, 63, fw_name_array[fw_idx], fw_name_suffix[switch_ctx->sja1105_chip_revision]); 
		}
		switch_ctx->state = 3;

		if (verbosity > 0) dev_info(&spi->dev, "SJA1105 switch preparing to load %s\n", switch_ctx->fw_name);

		sja1105_reset(spi);

		err = sja1105_init_dt(switch_ctx);
		if (err) {
			dev_err(&spi->dev, "SJA1105 error initializing device table\n");
			return err;
		}
	} else {
		dev_err(&spi->dev, "SJA1105 error DTS OF mode mandatory\n");
		return -ENODEV;
	}

	switch_ctx->state = 4;

	spi_set_drvdata(spi, switch_ctx);

	switch_ctx->state = 5;

	/* First of all need to check for SJA1105 init read */

	init_completion(&switch_ctx->conf_loaded);

	err = request_firmware_nowait(THIS_MODULE,
		FW_ACTION_NOHOTPLUG,
		switch_ctx->fw_name,
		&spi->dev,
		GFP_KERNEL,
		spi,
		sja1105_configuration_load);

	if (err)
		dev_err(&spi->dev, "Firmware loading failed with %d!\n", err);
	else {
		if (verbosity > 2) dev_info(&spi->dev, "SJA1105 switch firmware request ongoing\n");
		sja1105_debugfs_init(spi);
		sja1105_sysfs_init(&spi->dev);
	}

	return err;
}

static int sja1105_remove(struct spi_device *spi)
{
	struct sja1105_context_data *data = spi_get_drvdata(spi);

	if (data->state >= 5)
		wait_for_completion(&data->conf_loaded);

	sja1105_registerSpiCB(NULL);
	unregister_netdevice_notifier(&data->notifier_block);
	sja1105_sysfs_remove(&spi->dev);

	return 0;
}



static struct spi_driver sja1105_driver = {
	.driver = {
		.name = "sja1105",
		.owner = THIS_MODULE,
		.of_match_table = sja1105_dt_ids,
		.pm = NULL,
	},
	.probe = sja1105_probe,
	.remove = sja1105_remove,
};
module_spi_driver(sja1105_driver);

MODULE_DESCRIPTION("SJA1105 SPI driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:sja1105");
