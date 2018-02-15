/*
* AVB switch driver module
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
* \file sja1105p_cfg_file.c
*
* \author Philippe guasch
*
* \date 2016-02-22
*
* \brief Utilities to handle contents of SJA1105P configuration binary file
*
*****************************************************************************/
#include <linux/spi/spi.h>
#include <linux/platform_device.h>

#include "sja1105p_cfg_file.h"




struct sja1105p_block_attributes {
	u8 block_id;
	bool mandatory;
	u32 expected_length; /* 0 is no fixed length */
};


/******************************************************************************
* Exported variables
*****************************************************************************/
const u32 device_id_list[SJA1105P_NB_REV] = { SJA1105QS_VAL_DEVICEID,
                                              SJA1105PR_VAL_DEVICEID };



/******************************************************************************
* Local variables
*****************************************************************************/

const struct sja1105p_block_attributes sja1105p_block_attr_tb[] = {
  [0] = { .block_id = SJA1105P_BLOCK_L2_ADDR_LOOKUP_TB,      .mandatory = false, .expected_length = 0                 },
  [1] = { .block_id = SJA1105P_BLOCK_L2_POLICING_TB,         .mandatory = true,  .expected_length = 0                 },
  [2] = { .block_id = SJA1105P_BLOCK_VLAN_LOOKUP_TB,         .mandatory = false, .expected_length = 0                 },
  [3] = { .block_id = SJA1105P_BLOCK_L2_FORWARDING_TB,       .mandatory = true,  .expected_length = 0                 },
  [4] = { .block_id = SJA1105P_BLOCK_MAC_CONFIG_TB,          .mandatory = true,  .expected_length = SJA1105P_PORT_NB*8 },
  [5] = { .block_id = SJA1105P_BLOCK_L2_ADDR_LOOKUP_PARAMS,  .mandatory = false, .expected_length = 4                 },
  [6] = { .block_id = SJA1105P_BLOCK_L2_FORWARDING_PARAMS,   .mandatory = true,  .expected_length = 3                 },
  [7] = { .block_id = SJA1105P_BLOCK_AVB_PARAMETERS,         .mandatory = false, .expected_length = 4                 },
  [8] = { .block_id = SJA1105P_BLOCK_GENERAL_PARAMETERS,     .mandatory = true,  .expected_length = 11                },
  [9] = { .block_id = SJA1105P_BLOCK_RETAGGING_TB,           .mandatory = false, .expected_length = 0                 },
 [10] = { .block_id = SJA1105P_BLOCK_xMII_PARAMETERS,        .mandatory = true,  .expected_length = 1                 },
 [11] = { .block_id = SJA1105P_CREDIT_BASED_SHAPING_TB,      .mandatory = false, .expected_length = 20                },
 [12] = { .block_id = SJA1105P_BLOCK_CGU_CONFIG_PARAMETERS,  .mandatory = false, .expected_length = 0                 },
 [13] = { .block_id = SJA1105P_BLOCK_RGU_CONFIG_PARAMETERS,  .mandatory = false, .expected_length = 0                 },
 [14] = { .block_id = SJA1105P_BLOCK_ACU_CONFIG_PARAMETERS,  .mandatory = false, .expected_length = 0                 },
  {     /*sentinel*/   }
};
#define NB_BLOCK_TYPES 15
#define MANDATORY_BLOCK_MASK  (BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(6) | BIT(8) | BIT(10))

/******************************************************************************
* Exported functions
*****************************************************************************/
/**
* \brief Parses the Configuration file and splits it into a block list (double link)
*
*
* \param[in]  spi struct spi_device (handle to access dev
* \param[in/out]  block_list linked list of struct sja1105p_cfg_block
* \param[in]  cfg_data pointer on configuration file content
* \param[in]  cfg_word_size number of words to be parsed
* \param[in]  verbosity log level
*
* \return int  Returns 0 upon successful parsing , < 0 if failed.
*/
int sja1105p_config_file_split_blocks(struct spi_device *spi, struct list_head * block_list, u32* cfg_data, u32 cfg_word_size, int verbosity)
{
	u32 * p;
	struct sja1105p_cfg_block * cur;
	int remaining_sz;
	bool format_ok = false;
	int i;
	u32 block_mask = MANDATORY_BLOCK_MASK;
	int file_offset;
	int total_block_size = 0;

	for ( i = 0; i < SJA1105P_NB_REV; i++) {
		if (cfg_data[0] == device_id_list[i]) {
			format_ok = true;
			break;
		}
	}
	if (!format_ok  || cfg_word_size < 4)
		return -1;

	if (verbosity > 0)
		dev_info(&spi->dev, "Config 1st word=%08x\n", cfg_data[0]);

	p = &cfg_data[1];
	remaining_sz = cfg_word_size-1;
	while (remaining_sz >= 3) {
		u32 * block_start;
		cur = devm_kzalloc(&spi->dev, sizeof(struct sja1105p_cfg_block), GFP_KERNEL);
		if (!cur) {
			if (verbosity > 0) dev_err(&spi->dev, "Config Block allocation failed\n");
			goto err;
		}
		block_start = p;
		cur->block_id = *p++ >> 24;
		cur->block_sz = *p++ & 0xffffff;
		total_block_size += cur->block_sz + 4;
		cur->block_header_crc = *p++;
		cur->link_block.next = cur->link_block.prev = NULL;
		file_offset = (block_start-&cfg_data[0]);
		if (verbosity > 0)
			dev_info(&spi->dev, "Config Block Id=0x%x size=%d CRC=%x offset=%d total_size=%d\n",
			cur->block_id,
			cur->block_sz,
			cur->block_header_crc,
			file_offset,
			total_block_size);

		if (cur->block_sz) {
			if (remaining_sz < (cur->block_sz+3)) {
				dev_err(&spi->dev, "Config Block length=%d incompatible with file size=%d at offset=%d\n",
				cur->block_sz,
				cfg_word_size,
				file_offset
				);
			}

			cur->block_data = p;
			p += cur->block_sz;
			cur->block_crc = *p++;
			/* Parse attribute array so as to check block consistency and tick presence of mandatory items */
			for (i = 0; i < NB_BLOCK_TYPES; i++) {
				if (sja1105p_block_attr_tb[i].block_id == cur->block_id) {
					block_mask &= ~BIT(i);
					if (sja1105p_block_attr_tb[i].expected_length != 0 && sja1105p_block_attr_tb[i].expected_length != cur->block_sz) {
						if (verbosity > 0) dev_err(&spi->dev, "Config Block %02xh length incorrect %d expected %d",
							cur->block_id,
							cur->block_sz,
							sja1105p_block_attr_tb[i].expected_length);
						goto err;
					} else {
						break;
					}
				}
			} /* for */
			if ( i >= NB_BLOCK_TYPES)
				dev_err(&spi->dev, "Unknown block id=%02x, remaining_sz=%d \n", cur->block_id, remaining_sz);
		}
		else {
			if (remaining_sz == 3) { /* The last word is the global crc, the two before are unused */
				if (verbosity > 0) dev_info(&spi->dev, "Final Block\n");
				cur->block_data = NULL;
				cur->block_id = 0;
				break; /* while */
			}
			else {
				if (verbosity > 0) dev_info(&spi->dev, "Spurious data at end of config file: remaining_sz=%d\n", remaining_sz);
			}
		}
		remaining_sz -= (p - block_start);
		if (!cur->block_data) {
			if (remaining_sz != 0) {
				if (verbosity > 0) dev_err(&spi->dev, "SJA1105P config file misbuilt: zero length block\n");
				goto err;
			}
		}
		/* Append current block to list */
		list_add_tail(&cur->link_block, block_list);
	} /* while */
	if (block_mask != 0) {
		if (verbosity > 0)  dev_err(&spi->dev, "SJA1105P missing mandatory configuration block mask=%x", block_mask);
		goto err;
	}

	return 0;
err:
	while (!list_empty(block_list)) {
		cur = list_entry(block_list->next, struct sja1105p_cfg_block, link_block);
		list_del(&cur->link_block);
		devm_kfree(&spi->dev, cur);
	}

	return -1;
}

/**
* \brief Seek for sja1105p_cfg_block matching a certain id
* \pre   the list must have been priorily built by a call to sja1105p_config_file_split_blocks
*
* \param[in]  block_list linked list of struct sja1105p_cfg_block
* \param[in]  id_to_search block identifier to be serached for in list
*
* \return struct sja1105p_cfg_block *  pointer on sought block matching NULL if failed
*/
struct sja1105p_cfg_block * sja1105p_config_block_seek(struct list_head * block_list, u8 id_to_search)
{
	struct sja1105p_cfg_block * block = NULL;
	struct list_head * next;
	list_for_each(next, block_list) {
		block = list_entry(next, struct sja1105p_cfg_block, link_block);
		if (block->block_id == id_to_search) {
			return block;
		}
	}
	return NULL;
}
