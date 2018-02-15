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
* \file sja1105p_cbs.h
*
* \author Laurent Brando
*
* \date 2017-01-17
*
* \brief Functions definitions for SJA1105P General Status Information
*
*****************************************************************************/
#ifndef _SJA1105P_GENERAL_STATUS_H__
#define _SJA1105P_GENERAL_STATUS_H__

struct _SJA_CONF {
	unsigned long num_ports;
};

struct _SJA_CONF_STATUS {
	unsigned char configs;
	unsigned char crcchkl;
	unsigned char ids;
	unsigned char crcchkg;
	unsigned char nslot;
};

int sja_get_device_id(unsigned int *device_id, unsigned int device_select);
int sja_get_configuration_status(struct _SJA_CONF_STATUS *config_status, unsigned int device_select);

#endif /* _SJA1105P_GENERAL_STATUS_H__ */
