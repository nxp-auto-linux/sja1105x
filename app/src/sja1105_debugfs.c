/*
* AVB switch driver module for SJA1105
* Copyright (C) 2016 NXP Semiconductors
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
* \file  sja1105_debugfs.c
*
* \author Philippe guasch, Laurent Brando
*
* \date 2017-07-31
*
* \brief Exposes switch information via debugfs
*
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

#include "sja1105_spi_linux.h"
#include "sja1105_cfg_file.h"
#include "sja1105_general_status.h"
#include "sja1105_debugfs.h"
#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_diagnostics.h"

#define BUFSIZE 20

extern int verbosity;
static long pending_register_read = -1;
static long pending_reg_write_addr = -1;
static unsigned long pending_reg_write_cont;
static struct dentry *sja_dentry;

static int sja1105_general_id_show(struct seq_file *s, void *data)
{
	struct sja1105_context_data *ctx_data = s->private;
	int device_select = ctx_data->device_select;
	u32 val32;

	seq_printf(s, "\nDevice ID of switch %d\n", device_select);

	SJA1105P_getDeviceId(&val32, device_select);
	seq_printf(s, "%08x\n", val32);

	return 0;
}

static int sja1105_general_configuration_show(struct seq_file *s, void *data)
{
	int err;
	struct sja1105_context_data *ctx_data = s->private;
	struct spi_device *spi = ctx_data->spi_dev;
	int device_select = ctx_data->device_select;
	SJA1105P_configurationFlagsArgument_t configFlags;

	err = SJA1105P_getConfigurationFlags(&configFlags, device_select);
	if (err) {
		dev_err(&spi->dev, "Error: Could not get configuration flags for switch id %d (err=%d)\n", device_select, err);
		return err;
	}

	seq_printf(s, "\nConfiguration Status Information of switch %d\n", device_select);
	seq_printf(s, "Configuration %s: LocalCRCfail=%s - DevID unmatched=%s - GlobalCRCfail=%s\n",
		(configFlags.configs ? "valid" : "invalid"),
		(configFlags.crcchkl ? "yes" : "no"),
		(configFlags.ids     ? "yes" : "no"),
		(configFlags.crcchkg ? "yes" : "no"));

	return 0;
}

static int sja1105_general_registers_show(struct seq_file *s, void *data)
{
	int err = 0;
	struct sja1105_context_data *ctx_data = s->private;
	struct spi_device *spi = ctx_data->spi_dev;
	int device_select = ctx_data->device_select;

	SJA1105P_generalStatusL2Argument_t p_generalStatusL2;
	SJA1105P_generalStatusHashconfsArgument_t p_generalStatusHashconfs;
	SJA1105P_generalStatusVlanArgument_t p_generalStatusVlan;
	SJA1105P_generalStatusMem0Argument_t p_generalStatusMem0;
	SJA1105P_generalStatusDropArgument_t p_generalStatusDrop;
	uint32_t p_ramparerr;

	err += SJA1105P_getGeneralStatusL2(&p_generalStatusL2, device_select);
	err += SJA1105P_getGeneralStatusHashconfs(&p_generalStatusHashconfs, device_select);
	err += SJA1105P_getGeneralStatusVlan(&p_generalStatusVlan, device_select);
	err += SJA1105P_getGeneralStatusMem0(&p_generalStatusMem0, device_select);
	err += SJA1105P_getGeneralStatusDrop(&p_generalStatusDrop, device_select);
	err += SJA1105P_getGeneralStatusRamError(&p_ramparerr, device_select);
	if (err) {
		dev_err(&spi->dev, "Error: Could not get GeneralStatus for switch id %d (err=%d)\n", device_select, err);
		return err;
	}

	seq_printf(s, "\nGeneral Status Registers of switch %d\n", device_select);

	seq_printf(s, "L2:        portenf=%u, fwds=%u, macfds=%u, enffds=%u, l2busyfds=%u, l2busys=%u, macadd=%llu\n",
		   p_generalStatusL2.portenf,
		   p_generalStatusL2.fwds,
		   p_generalStatusL2.macfds,
		   p_generalStatusL2.enffds,
		   p_generalStatusL2.l2busyfds,
		   p_generalStatusL2.l2busys,
		   p_generalStatusL2.macadd);

	seq_printf(s, "Hashconfs: vlanidhc=%u, hashconfs=%u, macaddhc=%llu\n",
		   p_generalStatusHashconfs.vlanidhc,
		   p_generalStatusHashconfs.hashconfs,
		   p_generalStatusHashconfs.macaddhc);

	seq_printf(s, "Vlan:      wpvlanid=%u, port=%u, vlanbusys=%u, wrongports=%u, vnotfounds=%u\n",
		   p_generalStatusVlan.wpvlanid,
		   p_generalStatusVlan.port,
		   p_generalStatusVlan.vlanbusys,
		   p_generalStatusVlan.wrongports,
		   p_generalStatusVlan.vnotfounds);


	seq_printf(s, "Mem0:      emptys=%u, buffers=%u\n",
		   p_generalStatusMem0.emptys,
		   p_generalStatusMem0.buffers);


	seq_printf(s, "Drop:      port=%u, fwds=%u, parts=%u\n",
		   p_generalStatusDrop.port,
		   p_generalStatusDrop.fwds,
		   p_generalStatusDrop.parts);


	seq_printf(s, "RamError: %u\n", p_ramparerr);

	return 0;
}

static int sja1105_ethernet_mac_level_show(struct seq_file *s, void *data)
{
	struct sja1105_context_data *ctx_data = s->private;
	struct spi_device *spi = ctx_data->spi_dev;
	int device_select = ctx_data->device_select;
	SJA1105P_macLevelErrors_t p_macLevelErrors;
	int err, port;

	for (port=0; port<SJA1105P_N_LOGICAL_PORTS; port++) {
		err = SJA1105P_getMacErrors(&p_macLevelErrors, port);
		if (err) {
			dev_err(&spi->dev, "Error: Could not get ethernet mac level status for switch id %d, port %d (err=%d)\n", device_select, port, err);
			return err;
		}

		seq_printf(s, "\nEthernet MAC-level status switch %d port %d\n", device_select, port);

		seq_printf(s, "nRunt      = %u\n", p_macLevelErrors.nRunt);
		seq_printf(s, "nSoferr    = %u\n", p_macLevelErrors.nSoferr);
		seq_printf(s, "nAlignerr  = %u\n", p_macLevelErrors.nAlignerr);
		seq_printf(s, "nMiierr    = %u\n", p_macLevelErrors.nMiierr);
	}

	return 0;
}

static int sja1105_ethernet_high_level_show(struct seq_file *s, void *data)
{
	struct sja1105_context_data *ctx_data = s->private;
	struct spi_device *spi = ctx_data->spi_dev;
	int device_select = ctx_data->device_select;
	uint64_t tx_bytes, tx_packets, rx_bytes, rx_packets;
	uint32_t rx_crc_errors, rx_length_errors, polerr, vlanerr, n664err;
	uint32_t not_reach, egr_disabled, part_drop, qfull;
	int err, port;



	for (port=0; port<SJA1105P_N_LOGICAL_PORTS; port++) {
		err = SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_OCTETS,
							&tx_bytes, port,
							SJA1105P_e_etherStatDirection_EGRESS);
		err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_PKTS,
							&tx_packets, port,
							SJA1105P_e_etherStatDirection_EGRESS);
		err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_OCTETS,
							&rx_bytes, port,
							SJA1105P_e_etherStatDirection_INGRESS);
		err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_PKTS,
							&rx_packets, port,
							SJA1105P_e_etherStatDirection_INGRESS);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_CRCERR,
							&rx_crc_errors, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_SIZEERR,
							&rx_length_errors, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_NOT_REACH,
							&not_reach, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_EGR_DISABLED,
							&egr_disabled, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_PART_DROP,
							&part_drop, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_QFULL,
							&qfull, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_POLERR,
							&polerr, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_VLANERR,
							&vlanerr, port,
							SJA1105P_e_etherStatDirection_BOTH);
		err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_N664ERR,
							&n664err, port,
							SJA1105P_e_etherStatDirection_BOTH);
		if (err) {
			dev_err(&spi->dev, "Error: Could not get ethernet high level status for switch id %d, port %d (err=%d)\n", device_select, port, err);
			return err;
		}

		seq_printf(s, "\nEthernet High-level status switch %d port %d\n", device_select, port);

		seq_printf(s, "tx_bytes         = %llu\n", tx_bytes);
		seq_printf(s, "tx_packets       = %llu\n", tx_packets);
		seq_printf(s, "rx_bytes         = %llu\n", rx_bytes);
		seq_printf(s, "rx_packets       = %llu\n", rx_packets);

		seq_printf(s, "rx_crc_errors    = %u\n", rx_crc_errors);
		seq_printf(s, "rx_length_errors = %u\n", rx_length_errors);
		seq_printf(s, "polerr           = %u\n", polerr);
		seq_printf(s, "vlanerr          = %u\n", vlanerr);
		seq_printf(s, "n664err          = %u\n", n664err);
		seq_printf(s, "not_reach        = %u\n", not_reach);
		seq_printf(s, "egr_disabled     = %u\n", egr_disabled);
		seq_printf(s, "part_drop        = %u\n", part_drop);
		seq_printf(s, "qfull            = %u\n", qfull);
	}

	return 0;
}

static int sja1105_register_rw_write(struct file* file, const char __user* user_buf, size_t size, loff_t* pos)
{
	int ret;
	long reg_addr;
	unsigned long reg_content;
	char buf[BUFSIZE];
	char operation;

	/* get buffer from userspace */
	if (copy_from_user(buf, user_buf, size < BUFSIZE ? size : BUFSIZE))
		goto error;

	ret = sscanf(buf, "%c:%lx:%lx", &operation, &reg_addr, &reg_content);

	if (ret < 2)
		goto error;

	if (operation == 'w' && ret != 3) {
		pr_err("Error: Missing register content!\n");
		goto error;
	}

	switch (operation) {
	case 'r':
		pending_register_read = reg_addr;
		if (verbosity > 1)
			pr_alert("Marking register %lx as pending, read this file to get its content\n", reg_addr);

		break;
	case 'w':
		pending_reg_write_addr = reg_addr;
		pending_reg_write_cont = reg_content;
		if (verbosity > 1)
			pr_alert("Marking write of %08lx to register %lx as pending, read this file to execute write\n", pending_reg_write_cont, reg_addr);

		break;
	default:
		break;
	}


error:
	return size;
}

static int sja1105_register_rw_show(struct seq_file *s, void *data)
{
	struct sja1105_context_data *ctx_data = s->private;
	struct spi_device *spi = ctx_data->spi_dev;
	long reg_content;

	if (pending_register_read != -1) {
		/* get content of register, if there is a read pending */
		reg_content = sja1105_read_reg32(spi, pending_register_read);
		seq_printf(s, "Content of register %lx: [%08lx]\n", pending_register_read, reg_content);

		/* no read pending anymore */
		pending_register_read = -1;
	} else if (pending_reg_write_addr != -1) {
		/* write to register, if there is a write pending */
		if (sja1105_cfg_block_write(spi, pending_reg_write_addr, (u32*)&pending_reg_write_cont, 1) == 0)
			seq_printf(s, "Wrote %08lx to register %lx\n", pending_reg_write_cont, pending_reg_write_addr);
		else
			seq_printf(s, "Error writing to register %lx\n", pending_reg_write_addr);

		/* no write pending anymore */
		pending_reg_write_addr = -1;
	} else {
		seq_printf(s, "READ A REGISTER: write \"r:REG_ADDR\" to this file, where REG_ADDR is hexadecimal\n");
		seq_printf(s, "WRITE A REGISTER: write \"w:REG_ADDR:REG_CONTENT\" to this file, where REG_ADDR and REG_CONTENT are hexadecimal\n");
		seq_printf(s, "Afterwards read this file again to execute desired operation\n");
	}

	return 0;
}


static int sja1105_general_id_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_general_id_show, inode->i_private);
}

static int sja1105_general_configuration_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_general_configuration_show, inode->i_private);
}

static int sja1105_general_registers_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_general_registers_show, inode->i_private);
}

static int sja1105_ethernet_mac_level_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_ethernet_mac_level_show, inode->i_private);
}

static int sja1105_ethernet_high_level_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_ethernet_high_level_show, inode->i_private);
}

static int sja1105_register_rw_open(struct inode *inode, struct file *file)
{
	return single_open(file, sja1105_register_rw_show, inode->i_private);
}

static const struct file_operations sja1105_general_id_fops = {
	.open		= sja1105_general_id_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static const struct file_operations sja1105_general_configuration_fops = {
	.open		= sja1105_general_configuration_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static const struct file_operations sja1105_general_registers_fops = {
	.open		= sja1105_general_registers_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static const struct file_operations sja1105_ethernet_mac_level_fops = {
	.open		= sja1105_ethernet_mac_level_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static const struct file_operations sja1105_ethernet_high_level_fops = {
	.open		= sja1105_ethernet_high_level_open,
	.release	= single_release,
	.read		= seq_read,
	.llseek		= seq_lseek,
};

static const struct file_operations sja1105_register_rw = {
	.open		= sja1105_register_rw_open,
	.release	= single_release,
	.read		= seq_read,
	.write		= sja1105_register_rw_write,
	.llseek		= seq_lseek,
};

void sja1105_debugfs_init(struct sja1105_context_data *ctx_data)
{
	struct dentry *general_dentry, *ethernet_dentry;

	sja_dentry = debugfs_create_dir("sja1105", NULL);
	if (!sja_dentry)
		return;

	general_dentry = debugfs_create_dir("general", sja_dentry);
	if (!general_dentry)
		return;

	debugfs_create_file("id", S_IRUSR, general_dentry, ctx_data, &sja1105_general_id_fops);
	debugfs_create_file("configuration", S_IRUSR, general_dentry, ctx_data, &sja1105_general_configuration_fops);
	debugfs_create_file("registers", S_IRUSR, general_dentry, ctx_data, &sja1105_general_registers_fops);
	debugfs_create_file("register_rw", S_IRUSR, general_dentry, ctx_data, &sja1105_register_rw);

	ethernet_dentry = debugfs_create_dir("ethernet", sja_dentry);
	if (!ethernet_dentry)
		return;

	debugfs_create_file("mac-level", S_IRUSR, ethernet_dentry, ctx_data, &sja1105_ethernet_mac_level_fops);
	debugfs_create_file("high-level", S_IRUSR, ethernet_dentry, ctx_data, &sja1105_ethernet_high_level_fops);
}

void sja1105_debugfs_remove()
{
	debugfs_remove_recursive(sja_dentry);
}
