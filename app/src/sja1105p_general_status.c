/*
* AVB switch driver module
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
* \file sja1105p_general_status.c
*
* \author Laurent Brando
*
* \date 2017-01-17
*
* \brief IOCTLs handling for General Status Infocrmation
*
*****************************************************************************/

#include "sja1105p_general_status.h"
#include "NXP_SJA1105P_switchCore.h"

/******************************************************************************
* Exported variables
*****************************************************************************/


/******************************************************************************
* Local variables
*****************************************************************************/



/******************************************************************************
* Exported functions
*****************************************************************************/

/**
* \brief
* \pre
*
* \param[in]
* \param[out]
*
* \return
*/
int sja_get_device_id(unsigned int *device_id, unsigned int device_select)
{
	return SJA1105P_getDeviceId(device_id, device_select);
}


/**
* \brief
* \pre
*
* \param[in]
* \param[out]
*
* \return
*/
int sja_get_configuration_status(struct _SJA_CONF_STATUS *config_status, unsigned int device_select)
{
	return SJA1105P_getConfigurationFlags((SJA1105P_configurationFlagsArgument_t *)config_status, device_select);
}
