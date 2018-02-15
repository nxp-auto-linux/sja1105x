/*
* AVB switch driver module for SJA1105
* Copyright (C) 2017 NXP Semiconductors
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
* \file sja1105p_spi_linux.h
*
* \author NXP Semiconductors
*
* \date 2017-07-31
*
* \brief Functions definitions for Linux platform dependent SPI read and write functions
*
*****************************************************************************/
#ifndef _SJA1105P_SPI_LINUX_H
#define _SJA1105P_SPI_LINUX_H

/*
 * The configuration must be loaded into SJA1105P starting from 0x20000
 * The configuration must be split in 64 words block transfers
 */
#define SJA1105P_CONFIG_START_ADDRESS 0x20000UL
#define SJA1105P_CONFIG_WORDS_PER_BLOCK 64

#define CMD_RWOP_SHIFT 31
#define CMD_RD_OP 0
#define CMD_WR_OP 1
#define CMD_REG_ADDR_SHIFT 4
#define CMD_REG_ADDR_WIDTH 21
#define CMD_REG_RD_CNT_SHIFT 25
#define CMD_REG_RD_CNT_WIDTH 6

#define WIDTH2MASK(_w_) ((1 << (_w_)) - 1)

#define CMD_ENCODE_RWOP(_write_)   ((_write_)<< CMD_RWOP_SHIFT)
#define CMD_ENCODE_ADDR(_addr_)    (((_addr_) & WIDTH2MASK(CMD_REG_ADDR_WIDTH)) << CMD_REG_ADDR_SHIFT)
#define CMD_ENCODE_WRD_CNT(_cnt_)  (((_cnt_) & WIDTH2MASK(CMD_REG_RD_CNT_WIDTH)) << CMD_REG_RD_CNT_SHIFT)


void register_spi_callback(struct spi_device *spi, int device_select, int active_switches);
void unregister_spi_callback(int active_switches);

u32 sja1105p_read_reg32(struct spi_device *spi, u32 reg_addr);
int sja1105p_cfg_block_write(struct spi_device *spi, u32 reg_addr, u32 *data, int nb_words);

#endif /* _SJA1105P_SPI_LINUX_H */
