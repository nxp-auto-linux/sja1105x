/******************************************************************************
* Copyright (c) NXP B.V. 2016 - 2017. All rights reserved.
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
* \file NXP_SJA1105P_config.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Static configuration of the switch and clock settings
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_portConfig.h"
#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/**** User Settings Start ****/

#define DEFAULT_VLAN 555U

/**** User Settings End ****/

/* Application Board Port Mapping */
#define APPBOARD_PORT_MAPPING {{4, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}} /**< Mapping between logical ports used in the driver API and the physical ports on the ICs. Syntax: {{physical port of port 0, physical switch of port 0}, ...} */
/* Port 0 --> Port 4 on switch 0 */
/* Port 1 --> Port 0 on switch 0 */
/* Port 2 --> Port 1 on switch 0 */
/* Port 3 --> Port 2 on switch 0 */
/* Port 4 --> Port 3 on switch 0 */

/* Reference Board Port Mapping */
#define REFBOARD_PORT_MAPPING {{0, 0}, {2, 1}, {3, 1}, {0, 1}, {1, 1}, {1, 0}, {3, 0}, {2, 0}} /**< Mapping between logical ports used in the driver API and the physical ports on the ICs. Syntax: {{physical port of port 0, physical switch of port 0}, ...} */
/* Port 0 --> Port 0 on switch 0 */
/* Port 1 --> Port 2 on switch 1 */
/* Port 2 --> Port 3 on switch 1 */
/* Port 3 --> Port 1 on switch 1 */
/* Port 4 --> Port 2 on switch 1 */
/* Port 5 --> Port 1 on switch 0 */
/* Port 6 --> Port 3 on switch 0 */
/* Port 7 --> Port 2 on switch 0 */

/* Select the correct port mapping for the respective board */
#ifdef SJA1105P_REFBOARD
	#define PORT_MAPPING REFBOARD_PORT_MAPPING
#else
	#define PORT_MAPPING APPBOARD_PORT_MAPPING /**< Mapping between logical ports used in the driver API and the physical ports on the ICs. Syntax: {{physical port of port 0, physical switch of port 0}, ...} */
#endif

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

/* General Parameters Table */
SJA1105P_generalParameters_t SJA1105P_g_generalParameters;

/* AVB Parameters Table */
SJA1105P_avbParameters_t SJA1105P_g_avbParameters;
uint8_t SJA1105P_g_ptpMasterSwitch = 0;

/* MAC Configuration Table */
uint16_t SJA1105P_g_macConfigurationVlanid = DEFAULT_VLAN;

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/* Physical Setup */
static SJA1105P_port_t g_portMapping[SJA1105P_N_LOGICAL_PORTS];
static uint8_t  g_inversePortMapping[SJA1105P_N_SWITCHES][SJA1105P_N_PORTS];
static uint16_t g_logicalPortVectorMask[SJA1105P_N_SWITCHES] = {0};

/* Propagation delays */
static uint16_t g_phyPropagationDelay[SJA1105P_N_LOGICAL_PORTS][2] = {{0}};

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
extern uint8_t SJA1105P_initSwitch(void)
{
	uint8_t configId;
	uint8_t ret = 0;
	uint8_t switchId;
	SJA1105P_port_t portMapping[SJA1105P_N_LOGICAL_PORTS] = PORT_MAPPING;
	SJA1105P_configurationFlagsArgument_t configFlags;

	#ifdef SJA1105P_REFBOARD
		configId = 1;
	#else
		configId = 0;
	#endif

	/* Load static configurations */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += SJA1105P_loadConfig((uint8_t) (configId + switchId), switchId);
		ret += SJA1105P_getConfigurationFlags(&configFlags, switchId);
		if (configFlags.configs == 0U)
		{  /* configuration invalid */
			ret += 1U;
		}
	}

	/* Configure port clocks and I/O Pads */
	if (ret == 0U)
	{  /* static configuration was successful */
		ret  = SJA1105P_synchSwitchConfiguration();
		ret += SJA1105P_initManualPortMapping(portMapping);
		ret += SJA1105P_autoConfigPorts();
	}

	return ret;
}

/**
* \brief Synchronize software settings with the hardware configuration
*
* This function loads configuration parameters from the static configuration
* into the software configuration. This resolves having to edit the config-
* uration at multiple locations
*
* Settings updated:
*   - General parameters
*   - AVB parameters
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105P_synchSwitchConfiguration(void)
{
	uint8_t ret = 0;
	uint8_t switchId;

	SJA1105P_generalParametersEntryArgument_t      generalParametersEntry;
	SJA1105P_generalParametersControlSetArgument_t generalParametersControlSet;

	SJA1105P_avbParametersEntryArgument_t   avbParametersEntry;
	SJA1105P_avbParametersControlArgument_t avbParametersControl = {1, 0};  /* read access */

	generalParametersControlSet.valid = 1;

	/* General Parameters */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		/* read */
		generalParametersControlSet.rdwrset = 0;  /* read access */
		ret += SJA1105P_setGeneralParametersControl(&generalParametersControlSet, switchId);
		ret += SJA1105P_getGeneralParametersEntry(&generalParametersEntry, switchId);
		/* modify SW configuration */
		SJA1105P_g_generalParameters.switchId[switchId]   = generalParametersEntry.id;
		SJA1105P_g_generalParameters.cascPort[switchId]   = generalParametersEntry.cascadingPort;
		SJA1105P_g_generalParameters.hostPort[switchId]   = generalParametersEntry.hostPort;
		SJA1105P_g_generalParameters.mirrorPort[switchId] = generalParametersEntry.mirrorp;

		if (switchId == SJA1105P_MASTER_SWITCH)
		{  /* These settings are required to be equal across the cascaded switches and will only be read once */
			SJA1105P_g_generalParameters.managementPriority      = generalParametersEntry.mgmtPrio;
			SJA1105P_g_generalParameters.macFltres[1]            = generalParametersEntry.macFilterResult1;
			SJA1105P_g_generalParameters.macFltres[0]            = generalParametersEntry.macFilterResult0;
			SJA1105P_g_generalParameters.macFlt[1]               = generalParametersEntry.macFilter1;
			SJA1105P_g_generalParameters.macFlt[0]               = generalParametersEntry.macFilter0;
			SJA1105P_g_generalParameters.inclSrcpt[1]            = generalParametersEntry.includeSrcPort1;
			SJA1105P_g_generalParameters.inclSrcpt[0]            = generalParametersEntry.includeSrcPort0;
			SJA1105P_g_generalParameters.sendMeta[1]             = generalParametersEntry.sendFollowUpTs1;
			SJA1105P_g_generalParameters.sendMeta[0]             = generalParametersEntry.sendFollowUpTs0;
			SJA1105P_g_generalParameters.egressMirroringOuterTag = generalParametersEntry.egrMirrOuterTag;
			SJA1105P_g_generalParameters.egressMirroringOuterPcp = generalParametersEntry.egrMirrOuterPcp;
			SJA1105P_g_generalParameters.egressMirroringOuterDei = generalParametersEntry.egrMirrOuterDei;
		}
	}

	/* AVB Parameters */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += SJA1105P_setAvbParametersControl(&avbParametersControl, switchId);
		ret += SJA1105P_getAvbParametersEntry(&avbParametersEntry, switchId);
		if (switchId == SJA1105P_MASTER_SWITCH)
		{  /* These settings are required to be equal across the cascaded switches and will only be read once */
			SJA1105P_g_avbParameters.srcMeta = avbParametersEntry.srcMeta;
			SJA1105P_g_avbParameters.dstMeta = avbParametersEntry.dstMeta;
		}
		SJA1105P_g_avbParameters.ptpMaster[switchId] = avbParametersEntry.ptpMaster;
		if (avbParametersEntry.ptpMaster == 1U)
		{
			SJA1105P_g_ptpMasterSwitch = switchId;
		}
		if (ret != 0U)
		{
			break;
		}
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
extern void SJA1105P_setPhyPropagationDelay(uint16_t propagationDelay, uint8_t port, SJA1105P_direction_t direction)
{
	g_phyPropagationDelay[port][direction] = propagationDelay;
}

/**
* \brief Get the propagation delay expected from the PHYs
*
* Description see SJA1105P_setPhyPropagationDelay()
*
* \param[in]  port Logical port of the switch
* \param[in]  direction Direction (Tx: 0, Rx: 1)
*
* \return propagationDelay (8 ns) Propagation delay at PHY
*/
extern uint16_t SJA1105P_getPhyPropagationDelay(uint8_t port, SJA1105P_direction_t direction)
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
extern uint8_t SJA1105P_getPhysicalPort(uint8_t logicalPort, SJA1105P_port_t *p_physicalPort)
{
	uint8_t ret = 0;
	if (logicalPort >= SJA1105P_N_LOGICAL_PORTS)
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
extern uint8_t SJA1105P_getLogicalPort(uint8_t *p_logicalPort, const SJA1105P_port_t *kp_physicalPort)
{
	uint8_t ret = 0;

	*p_logicalPort = g_inversePortMapping[kp_physicalPort->switchId][kp_physicalPort->physicalPort];

	if (*p_logicalPort >= SJA1105P_N_LOGICAL_PORTS)
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
extern void SJA1105P_getPhysicalPortVector(uint16_t logicalPorts, uint8_t switchId, uint8_t *p_physicalPorts)
{
	uint8_t port;

	*p_physicalPorts = 0;
	logicalPorts &= g_logicalPortVectorMask[switchId];  /* only consider ports from specific switchId */
	for (port = 0; port < SJA1105P_N_LOGICAL_PORTS; port++)
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
extern void SJA1105P_getPortVectorFromPhysicalPortVector(uint8_t physicalPorts, uint8_t switchId, uint16_t *p_logicalPorts)
{
	uint8_t port;
	*p_logicalPorts = 0;
	for (port = 0; port < SJA1105P_N_PORTS; port++)
	{
		if (((physicalPorts >> port) & 1U) == 1U)
		{
			if (g_inversePortMapping[switchId][port] < SJA1105P_N_LOGICAL_PORTS)
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
extern void SJA1105P_getSwitchesFromPorts(uint16_t logicalPorts, uint8_t *p_switches)
{
	uint8_t switchId;
	uint8_t physicalPorts = 0;
	uint8_t switches = 0;
	uint16_t localLogicalPorts;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		localLogicalPorts = logicalPorts & g_logicalPortVectorMask[switchId];   /* only consider ports from specific switchId */
		SJA1105P_getPhysicalPortVector(localLogicalPorts, switchId, &physicalPorts);
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
extern void SJA1105P_initAutoPortMapping(void)
{
	uint8_t switchId;
	uint8_t physicalPort;
	uint8_t logicalPort = 0;
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		for (physicalPort = 0; physicalPort < SJA1105P_N_PORTS; physicalPort++)
		{
			g_inversePortMapping[switchId][physicalPort] = SJA1105P_N_LOGICAL_PORTS;


			if (physicalPort != SJA1105P_g_generalParameters.cascPort[switchId])
			{  /* not a cascaded port */
				if ((physicalPort != SJA1105P_g_generalParameters.hostPort[switchId])
					|| (switchId == SJA1105P_MASTER_SWITCH))
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
extern uint8_t SJA1105P_initManualPortMapping(const SJA1105P_port_t k_portMapping[SJA1105P_N_LOGICAL_PORTS])
{
	uint8_t ret = 0;
	uint8_t logicalPort;
	uint8_t physicalPort;
	uint8_t switchId;

	/* initialize with invalid port number */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		for (physicalPort = 0; physicalPort < SJA1105P_N_PORTS; physicalPort++)
		{
			g_inversePortMapping[switchId][physicalPort] = SJA1105P_N_LOGICAL_PORTS;
		}
	}

	/* Store the mapping */
	for (logicalPort = 0; logicalPort < SJA1105P_N_LOGICAL_PORTS; logicalPort++)
	{
		physicalPort = k_portMapping[logicalPort].physicalPort;
		switchId = k_portMapping[logicalPort].switchId;

		if ((physicalPort >= SJA1105P_N_PORTS) || (switchId >= SJA1105P_N_SWITCHES))
		{  /* invalid port or switch number */
			ret = 1;
		}
		else if (physicalPort != SJA1105P_g_generalParameters.cascPort[switchId])
		{  /* not a cascaded port */
			if ((physicalPort != SJA1105P_g_generalParameters.hostPort[switchId])
				|| (switchId == SJA1105P_MASTER_SWITCH))
			{  /* not a host port connected to a cascaded port */
				/* map this port to the next logical port */

				/* sanity check passed */
				g_portMapping[logicalPort] = k_portMapping[logicalPort];
				g_inversePortMapping[switchId][physicalPort] = logicalPort;
				g_logicalPortVectorMask[switchId]           |= (uint16_t)(((uint16_t) 1U) << logicalPort);
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
