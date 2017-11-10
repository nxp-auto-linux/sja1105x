/******************************************************************************
* Copyright (c) NXP B.V. 2017. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*****************************************************************************/

/**
*
* \file NXP_SJA1105_config.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Static configuration parameters of the switch
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_config.h"
#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_portConfig.h"
#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"
 
/******************************************************************************
* DEFINES
*****************************************************************************/

/**** User Settings Start ****/

/* Application Board Port Mapping */
#define PORT_MAPPING {{4, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}} /**< Mapping between logical ports used in the driver API and the physical ports on the ICs. Syntax: {{physical port of port 0, physical switch of port 0}, ...} */
/* Port 0 --> Port 4 on switch 0 */
/* Port 1 --> Port 0 on switch 0 */
/* Port 2 --> Port 1 on switch 0 */
/* Port 3 --> Port 2 on switch 0 */
/* Port 4 --> Port 3 on switch 0 */

/* Note: These following settings must be aligned with the static configuration loaded to the switch (the ".hex" configuration) */

#define SWITCH_IDS {0, 1, 2}  /**< Array containing the switch IDs - typically {0, 1 ... N-1} */
#define HOST_PORTS {4}  /**< Array containing the host ports, each entry corresponds to one switch */
#define CASC_PORTS {6}  /**< Array containing the cascaded ports, each entry corresponds to one switch */

#define PTP_MULTICAST_ADDR0 0x0180C200000EU
#define PTP_MULTICAST_ADDR1 0x011B19000000U

#define MAC_FLT0 0xFFFFFF0000FFU
#define MAC_FLT1 0xFFFFFFFFFFFFU

#define SRC_META 0x026037C0FFEEU
#define DST_META 0x026037DECADEU

#define DEFAULT_VLAN 0U

#define CRC_POLY 0x97U

/**** User Settings End ****/

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

/* General Parameters Table */
const SJA1105_generalParameters_t SJA1105_gk_generalParameters =
{
	SWITCH_IDS,
	{PTP_MULTICAST_ADDR0, PTP_MULTICAST_ADDR1},
	{MAC_FLT0, MAC_FLT1},
	{0, 1},
	{1, 1},
	HOST_PORTS,
	CASC_PORTS
};

/* AVB Parameters Table */
const SJA1105_avbParameters_t SJA1105_gk_avbParameters =
{
	SRC_META,
	DST_META
};

/* L2 Lookup Parameters Table */
uint16_t SJA1105_g_poly = CRC_POLY;  /**< CRC polynomial */

/* MAC Configuration Table */
const uint16_t SJA1105_g_macConfigurationVlanid = DEFAULT_VLAN;

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/* Physical Setup */
static SJA1105_port_t g_portMapping[SJA1105_N_LOGICAL_PORTS];
static uint8_t  g_inversePortMapping[SJA1105_N_SWITCHES][SJA1105_N_PORTS];
static uint16_t g_logicalPortVectorMask[SJA1105_N_SWITCHES] = {0};

/* Propagation delays */
static uint16_t g_phyPropagationDelay[SJA1105_N_LOGICAL_PORTS][2] = {0};

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Initialize the switch
*
* This function loads a static configuration into the switch and confirms success.
* Afterwards, it brings the ports into operation by configuring the clocks.
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105_initSwitch(void)
{
	uint8_t ret = 0;
	uint8_t switchId;
	SJA1105_port_t portMapping[SJA1105_N_LOGICAL_PORTS] = PORT_MAPPING;
	SJA1105_generalStatusDevice1Argument_t deviceStatus;

	/* Load static configurations */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		ret += SJA1105_loadConfig(switchId, switchId);
		ret += SJA1105_getGeneralStatusDevice1(&deviceStatus, switchId);
		if (deviceStatus.configs == 0U)
		{  /* configuration invalid */
			ret += 1U;
		}
	}

	/* Configure port clocks and I/O Pads */
	if (ret == 0U)
	{  /* static configuration was successful */
		ret  = SJA1105_autoConfigPorts();
		ret += SJA1105_initManualPortMapping(portMapping);
	}

	return ret;
}

/**
* \brief Set the propagation delay expected from the PHYs
*
* If these values are set, they will be used to correct ingress
* and egress timestamps within the switch.
* The delay is defined as the time between xMII and MDI at the PHY.
* It is measured in multiples of 8 ns to match the internal clock
* precision.
* If the delay varies, a minimum value should be configured. Otherwise,
* negative residence times and peer delays can occur.
*
* \param[in]  propagationDelay (8 ns) Propagation delay at PHY
* \param[in]  port Logical port of the switch
* \param[in]  direction Direction (Tx: 0, Rx: 1)
*/
extern void SJA1105_setPhyPropagationDelay(uint16_t propagationDelay, uint8_t port, SJA1105_direction_t direction)
{
	g_phyPropagationDelay[port][direction] = propagationDelay;
}

/**
* \brief Get the propagation delay expected from the PHYs
*
* Description see SJA1105_setPhyPropagationDelay()
*
* \param[out] p_propagationDelay (8 ns) Propagation delay at PHY
* \param[in]  port Logical port of the switch
* \param[in]  direction Direction (Tx: 0, Rx: 1)
*/
extern uint16_t SJA1105_getPhyPropagationDelay(uint8_t port, SJA1105_direction_t direction)
{
	return g_phyPropagationDelay[port][direction];
}

/**
* \brief Extract physical port information from a logical port
*
* Based on a logical port index, this function will return information of
* the switch ID and the physical port associated.
* A logical port is unique within a set of cascaded switches, where as a physical
* port is only unique at the respective switch.
*
* \param[in]  logicalPort Logical port in a set of cascaded switches
* \param[out] p_physicalPort Information on the physical port
*
* \return uint8_t: {0: successful, else: port does not exist}
*/
extern uint8_t SJA1105_getPhysicalPort(uint8_t logicalPort, SJA1105_port_t *p_physicalPort)
{
	uint8_t ret = 0;
	if (logicalPort >= SJA1105_N_LOGICAL_PORTS)
	{  /* port does not exist */
		ret = 1;
	}
	else
	{
		*p_physicalPort = g_portMapping[logicalPort];
	}

	return ret;
}

/**
* \brief Determine the logical port number of a physical port
*
* \param[out] p_logicalPort Logical port
* \param[in]  p_physicalPort Information on the physical port
*
* \return uint8_t: {0: successful, else: port does not have a logical port number}
*/
extern uint8_t SJA1105_getLogicalPort(uint8_t *p_logicalPort, SJA1105_port_t *p_physicalPort)
{
	uint8_t ret = 0;

	*p_logicalPort = g_inversePortMapping[p_physicalPort->switchId][p_physicalPort->physicalPort];

	if (*p_logicalPort >= SJA1105_N_LOGICAL_PORTS)
	{  /* port does not exist */
		ret = 1;
	}

	return ret;
}

/**
* \brief Calculate a bit vector containing the physical ports of a switch instance from a set of logical ports
*
*
* \param[in]  logicalPorts Vector of logical port in a set of cascaded switches
* \param[in]  switchId Switch ID of the switch on which the port vector is requested
* \param[out] p_physicalPorts Bit vector of physical ports
*
*/
extern void SJA1105_getPhysicalPortVector(uint16_t logicalPorts, uint8_t switchId, uint8_t *p_physicalPorts)
{
	uint8_t port;

	*p_physicalPorts = 0;
	logicalPorts &= g_logicalPortVectorMask[switchId];  /* only consider ports from specific switchId */
	for (port = 0; port < SJA1105_N_LOGICAL_PORTS; port++)
	{
		if (((logicalPorts >> port) & 1U) == 1U)
		{  /* this logical port belongs to the requested switch */
			*p_physicalPorts |= (uint8_t) (((uint8_t) 1U) << (g_portMapping[port].physicalPort));
		}
	}
}

/**
* \brief Return a logical port vector based on a physical port vector of a specific switch
*
* \param[in]  physicalPorts Bit vector of physical ports
* \param[in]  switchId Switch ID of the switch on which the port vector is requested
* \param[out] p_logicalPorts Vector of logical port in a set of cascaded switches
*
*/
extern void SJA1105_getPortVectorFromPhysicalPortVector(uint8_t physicalPorts, uint8_t switchId, uint16_t *p_logicalPorts)
{
	uint8_t port;
	*p_logicalPorts = 0;
	for (port = 0; port < SJA1105_N_PORTS; port++)
	{
		if (((physicalPorts >> port) & 1U) == 1U)
		{
			if (g_inversePortMapping[switchId][port] < SJA1105_N_LOGICAL_PORTS)
			{  /* port is an external port */
				*p_logicalPorts |= (uint16_t) ((uint16_t) 1U << g_inversePortMapping[switchId][port]);
			}
		}
	}
	
}

/**
* \brief Get the switches related to a set of ports
*
* This function determines, on which physical switches the logical ports are located.
*
* \param[in]  logicalPorts Bit vector with logical switch ports
* \param[out] physicalPort Bit vector, where each set bit corresponds to a related switch
*
*/
extern void SJA1105_getSwitchesFromPorts(uint16_t logicalPorts, uint8_t *p_switches)
{
	uint8_t switchId;
	uint8_t physicalPorts = 0;
	uint8_t switches = 0;
	uint16_t localLogicalPorts;

	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		localLogicalPorts = logicalPorts & g_logicalPortVectorMask[switchId];   /* only consider ports from specific switchId */
		SJA1105_getPhysicalPortVector(localLogicalPorts, switchId, &physicalPorts);
		if (physicalPorts != 0U)
		{
			switches |= (uint8_t) (((uint8_t) 1U) << switchId);
		}
	}
	*p_switches = switches;
}

/**
* \brief Initialize an automatic mapping from physical to logical ports
*
* This function loops through all switches and ports and assigns a logical
* port index to all external ports.
*
*/
extern void SJA1105_initAutoPortMapping(void)
{
	uint8_t switchId;
	uint8_t physicalPort;
	uint8_t logicalPort = 0;
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		for (physicalPort = 0; physicalPort < SJA1105_N_PORTS; physicalPort++)
		{
			g_inversePortMapping[switchId][physicalPort] = SJA1105_N_LOGICAL_PORTS;


			if (physicalPort != SJA1105_gk_generalParameters.cascPort[switchId])
			{  /* not a cascaded port */
				if ((physicalPort != SJA1105_gk_generalParameters.hostPort[switchId])
					|| (switchId == SJA1105_MASTER_SWITCH))
				{  /* not a host port connected to a cascaded port */
					/* map this port to the next logical port */
					g_portMapping[logicalPort].physicalPort      = physicalPort;
					g_portMapping[logicalPort].switchId          = switchId;
					g_inversePortMapping[switchId][physicalPort] = logicalPort;
					g_logicalPortVectorMask[switchId]           |= (uint16_t)(((uint16_t) 1U) << logicalPort);
					logicalPort++;
				}
			}
		}
	}
}

/**
* \brief Set the mapping from physical to logical ports
*
* This function allows to do a manual port mapping.
* Typically, this can be done to match enumeration on silk screen or cases.
* The function performs a sanity check on the actual mapping.
*
* \param[in] portMapping Array containing a list of physical ports mapped to the specific logical port
*
* \return uint8_t Returns 0 upon success. Else, the mapping did not pass the sanity check.
*/
extern uint8_t SJA1105_initManualPortMapping(const SJA1105_port_t k_portMapping[SJA1105_N_LOGICAL_PORTS])
{
	uint8_t ret = 0;
	uint8_t logicalPort;
	uint8_t physicalPort;
	uint8_t switchId;

	/* initialize with invalid port number */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		for (physicalPort = 0; physicalPort < SJA1105_N_PORTS; physicalPort++)
		{
			g_inversePortMapping[switchId][physicalPort] = SJA1105_N_LOGICAL_PORTS;
		}
	}

	/* Store the mapping */
	for (logicalPort = 0; logicalPort < SJA1105_N_LOGICAL_PORTS; logicalPort++)
	{
		physicalPort = k_portMapping[logicalPort].physicalPort;
		switchId = k_portMapping[logicalPort].switchId;

		if ((physicalPort >= SJA1105_N_PORTS) || (switchId > SJA1105_N_SWITCHES))
		{  /* invalid port or switch number */
			ret = 1;
		}

		if (physicalPort != SJA1105_gk_generalParameters.cascPort[switchId])
		{  /* not a cascaded port */
			if ((physicalPort != SJA1105_gk_generalParameters.hostPort[switchId])
				|| (switchId == SJA1105_MASTER_SWITCH))
			{  /* not a host port connected to a cascaded port */
				/* map this port to the next logical port */

				/* sanity check passed */
				g_portMapping[logicalPort] = k_portMapping[logicalPort];
				g_inversePortMapping[switchId][physicalPort] = logicalPort;
				g_logicalPortVectorMask[switchId]          |= (uint16_t)(((uint16_t) 1U) << logicalPort);
			}
			else
			{  /* invalid mapping: host port connected to another switch cannot be external port */
				ret = 1;
			}
		}
		else
		{  /* invalid mapping: cascaded port cannot be external port */
			ret = 1;
		}			
		if (ret != 0U)
		{
			break;
		}
	}
	return ret;
}
