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
* \file sja1105p_init.h
*
* \author Philippe guasch, Laurent Brando
*
* \date 2017-08-02
*
* \brief Functions definitions for SJA1105P spi
*
*****************************************************************************/
#ifndef _SJA1105P_SPI_H__
#define _SJA1105P_SPI_H__

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/*
 * structure definitions
 *
 */
struct port_desc {
	bool is_host;                 /**< whether port is connected to Host - Switch management */
	bool phy_not_mac;             /**< if false connected to real phy true if Null Phy direct connection */
	int logical_port_num;         /**< The logical port number of the physical port */
};

struct sja1105p_platform_data {
	const char * fw_name;                  /**< Name of SJA1105P configuration binary file */
	int host_port_id;                      /**< Number of port to which Host is tied */
	struct port_desc ports[SJA1105P_PORT_NB];
};

struct sja1105p_context_data {
	struct spi_device	*spi_dev;        /**< passsed at spi probing*/
	struct device_node	*of_node;        /**< Open Firmware node describing device */
	int device_select;			/**< The switch ID */

	char fw_name[64];               /**< Constructed filename depends on number of SJA1105
				     chips in configuration and their revision MRA1 or MRA2 */
	int sja1105p_chip_revision;
	struct sja1105p_platform_data *pdata;

	struct list_head cfg_block_list;        /**< Block list resulting from call to configuration file parsing function */
};


#endif /* _SJA1105P_SPI_H__ */
