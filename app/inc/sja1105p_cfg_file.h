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
* \file sja1105p_cfg_file.h
*
* \author Philippe guasch
*
* \date 2016-02-22
*
* \brief public functions and data structures related to configuration file handling
*
*****************************************************************************/
#ifndef _SJA1105P_CFG_FILE_H__
#define _SJA1105P_CFG_FILE_H__


#include <linux/list.h>

#define SJA1105P_PORT_NB 5

#define SJA1105P_NB_REV 2
#define SJA1105QS_VAL_DEVICEID              0xae00030eUL
#define SJA1105PR_VAL_DEVICEID              0xaf00030eUL


#define SJA1105P_BLOCK_L2_ADDR_LOOKUP_TB     0x05
#define SJA1105P_BLOCK_L2_POLICING_TB        0x06
#define SJA1105P_BLOCK_VLAN_LOOKUP_TB        0x07
#define SJA1105P_BLOCK_L2_FORWARDING_TB      0x08
#define SJA1105P_BLOCK_MAC_CONFIG_TB         0x09
#define SJA1105P_BLOCK_L2_ADDR_LOOKUP_PARAMS 0x0d
#define SJA1105P_BLOCK_L2_FORWARDING_PARAMS  0x0e
#define SJA1105P_BLOCK_AVB_PARAMETERS        0x10
#define SJA1105P_BLOCK_GENERAL_PARAMETERS    0x11
#define SJA1105P_BLOCK_RETAGGING_TB          0x12
#define SJA1105P_CREDIT_BASED_SHAPING_TB     0x13
#define SJA1105P_BLOCK_xMII_PARAMETERS       0x4e
#define SJA1105P_BLOCK_CGU_CONFIG_PARAMETERS 0x80
#define SJA1105P_BLOCK_RGU_CONFIG_PARAMETERS 0x81
#define SJA1105P_BLOCK_ACU_CONFIG_PARAMETERS 0x82



/*************************************************************************************************
 * Public Types
 ************************************************************************************************/

/**
 * sja1105p_cfg_block
 * Structure extracted after parsing the SJA1105P configuration file
 * Direct access to configuration blocks is required for system wide checking
 * For instance AVB parameters or XMII port settings are fixed for a given hardware and need
 * to be compared or fixed between configuration and actual hardware.
 * No method provided to recalculate the CRC32.
 */

struct sja1105p_cfg_block {

  struct list_head link_block; /**<  forward and backward chaining structure */

  u8 block_id;             /**< Identifier of configuration type */
  u16 block_sz;            /**< Size of block in number of words, excluding header and CRC */
  u32 block_header_crc;    /**< CRC32 of block header  */
  u32 * block_data;        /**, pointer of block data field, NULL if size is 0 */
  u32 block_crc;           /**< CRC32 of whole block body field */

};

/*************************************************************************************************
 * Public variables
 ************************************************************************************************/
/*
 * List of device Ids of supported devices: SJA1105P, SJA1105Q, SJA1105R, SJA1105S
 */
extern const u32 device_id_list[];

extern int sja1105p_config_file_split_blocks(struct spi_device *spi, struct list_head * block_list, u32* cfg_data, u32 cfg_word_size, int verbosity);

extern struct sja1105p_cfg_block * sja1105p_config_block_seek(struct list_head* block_list, u8 id_to_search);

#endif
