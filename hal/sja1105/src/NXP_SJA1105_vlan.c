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
* \file NXP_SJA1105_vlan.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Configuration of VLANs
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_vlan.h"
#include "NXP_SJA1105_config.h"
#include "NXP_SJA1105_switchCore.h"

/******************************************************************************
* INTERNAL TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
	uint8_t tagPort;    /**< Defines the set of ports on which a frame tagged with the respective VLAN ID may be received. All bits must be set in order to deactivate VLAN-based ingress port policing. */
	uint8_t vlanBc;     /**< This field restricts the broadcast domain of the specific VLAN. If a bit is cleared, the respective port cannot be reached by a frame tagged with the specific VLAN ID. All bits have to be set to de-activate reachability limitations for certain VLANs. */
	uint8_t vmembPort;  /**< Defines the set of ports on which a frame tagged with the respective VLAN ID may be received. All bits must be set in order to deactivate VLAN-based ingress port policing. */
	uint8_t vegrMirr;   /**< All traffic tagged with VLANID and forwarded to any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
	uint8_t vingMirr;   /**< All traffic tagged with VLANID and received on any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
} vlanLookupTableReconfigurationEntry_t;

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/* Shadow tables because of limited read-out capabilities */
static vlanLookupTableReconfigurationEntry_t g_vlanLookupTable[SJA1105_N_SWITCHES][SJA1105_N_VLAN_ENTRIES];
static SJA1105_retaggingTableReconfigurationEntryArgument_t g_retaggingTable[SJA1105_N_SWITCHES][SJA1105_N_RETAG_ENTRIES];

/* Auxiliary configuration information for acceleration */
static uint8_t g_vlanValid[SJA1105_N_VLAN_ENTRIES] = {0U};
static uint8_t g_retaggingValid[SJA1105_N_RETAG_ENTRIES] = {0U};
static uint8_t g_bypassingValid[SJA1105_N_RETAG_ENTRIES] = {0U};

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t storeVlanEntry(SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanEntry, uint8_t switchId);
static void loadVlanEntry(uint16_t vlanId, SJA1105_vlanLookupTableReconfigurationEntryArgument_t *p_vlanEntry, uint8_t switchId);

static uint8_t getRetaggingEntryIndex(uint16_t egressVlanId);
static uint8_t storeRetaggingEntry(SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry, uint8_t index, uint8_t validEntry, uint8_t switchId);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Configure forwarding rules within a VLAN
*
* \param[in]  vlan VLAN to be configured
* \param[in]  p_vlanForwarding Memory location of a list describing the forwarding type of each port.
* \param[in]  enable Each bit specifies whether VLAN traffic should be enabled for the corresponding port.
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
extern uint8_t SJA1105_writeVlanConfig(uint16_t vlanId, const SJA1105_vlanForwarding_t *pk_vlanForwarding, uint16_t enable)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint16_t port;
	uint8_t enabledPhysicalPorts;
	uint8_t firstMemberSwitchId = SJA1105_N_SWITCHES;
	uint8_t lastMemberSwitchId = 0;
	uint8_t firstBroadcastSwitchId = SJA1105_N_SWITCHES;
	uint8_t lastBroadcastSwitchId = 0;
	SJA1105_port_t physicalPort;
	SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanLookupTableEntry[SJA1105_N_SWITCHES] = {0};

	/* determine vlan lookup configuration for logical port and translate to physical lookups */
	for (port = 0U; port < SJA1105_N_LOGICAL_PORTS; port++)
	{
		ret += SJA1105_getPhysicalPort(port, &physicalPort);
		switch (pk_vlanForwarding[port])
		{
			case e_vlanForwarding_TAGGED:
			{
				vlanLookupTableEntry[physicalPort.switchId].tagPort |= (uint8_t) ((uint8_t) 1 << physicalPort.physicalPort);
				vlanLookupTableEntry[physicalPort.switchId].vlanBc  |= (uint8_t) ((uint8_t) 1 << physicalPort.physicalPort);
				break;
			}
			case e_vlanForwarding_UNTAGGED:
			{
				vlanLookupTableEntry[physicalPort.switchId].vlanBc  |= (uint8_t) ((uint8_t) 1 << physicalPort.physicalPort);
				break;
			}
			default:
			{
				break;
			}
		}
	}

	/* determine member ports */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		SJA1105_getPhysicalPortVector(enable, switchId, &enabledPhysicalPorts);
		vlanLookupTableEntry[switchId].vmembPort = enabledPhysicalPorts;
	}

	/* determine vlan lookup configuration for the internal ports (cascaded) */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		if (vlanLookupTableEntry[switchId].vmembPort != 0U)
		{  /* at least one port in this switch is VLAN member */
			firstMemberSwitchId = (firstMemberSwitchId == SJA1105_N_SWITCHES) ? (switchId) : (firstMemberSwitchId);
			lastMemberSwitchId  = (switchId > lastMemberSwitchId) ? (switchId) : (firstMemberSwitchId);
		}
		if (vlanLookupTableEntry[switchId].vlanBc != 0U)
		{  /* at least one port in this switch is VLAN member */
			firstBroadcastSwitchId = (firstBroadcastSwitchId == SJA1105_N_SWITCHES) ? (switchId) : (firstBroadcastSwitchId);
			lastBroadcastSwitchId  = (switchId > lastBroadcastSwitchId) ? (switchId) : (firstBroadcastSwitchId);
		}
	}

	/* configure internal ports VLAN forwarding */
	for (switchId = 0; switchId < (SJA1105_N_SWITCHES - 1U); switchId++)
	{
		if (switchId >= firstMemberSwitchId)
		{
			if (switchId < lastMemberSwitchId)
			{
				if (switchId < lastBroadcastSwitchId)
				{
					/* enable on this switch to forward to the cascaded port */
					vlanLookupTableEntry[switchId].vlanBc  |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.cascPort[switchId]);
					vlanLookupTableEntry[switchId].tagPort |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.cascPort[switchId]);
					/* also enable receiving on the connecting counter part of cascaded port */
					vlanLookupTableEntry[switchId + 1U].vmembPort |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.hostPort[switchId + 1U]);
					vlanLookupTableEntry[switchId + 1U].tagPort   |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.hostPort[switchId + 1U]);
				}
				if (switchId >= firstBroadcastSwitchId)
				{
					/* enable on this switch to receive from the cascaded port */
					vlanLookupTableEntry[switchId].vmembPort |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.cascPort[switchId]);
					vlanLookupTableEntry[switchId].tagPort   |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.cascPort[switchId]);
					/* also enable forwarding on the connecting counter part of cascaded port */
					vlanLookupTableEntry[switchId + 1U].vlanBc  |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.hostPort[switchId + 1U]);
					vlanLookupTableEntry[switchId + 1U].tagPort |= (uint8_t) ((uint8_t) 1 << SJA1105_gk_generalParameters.hostPort[switchId + 1U]);
				}
			}
		}
	}

	/* Download the VLAN configuration */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		vlanLookupTableEntry[switchId].vlanid = vlanId;
		ret += storeVlanEntry(vlanLookupTableEntry[switchId], switchId);
	}
	g_vlanValid[vlanId] = ((ret == 0U) ? (1U) : (0U));

	return ret;
}

/**
* \brief Read configuration of a VLAN.\n
*
* A configuration can only be read if it was configured through ::SJA1105_writeVlanConfig.
* Statically
*
* \param[out] vlan VLAN of which the configuration should be read
* \param[out] p_vlanForwarding Pointer to the memory location of a list describing the forwarding type of each port
* \param[out] p_enable Pointer to the memory location of a bit vector, where each bit specifies whether VLAN traffic should be enabled
*
* \return uint8_t Returns 0 upon successful read. Else, the configuration could not be read.
*/
extern uint8_t SJA1105_readVlanConfig(uint16_t vlanId, SJA1105_vlanForwarding_t *p_vlanForwarding, uint16_t *p_enable)
{
	uint8_t ret = 1;
	uint8_t port;
	SJA1105_port_t physicalPort;
	
	if (g_vlanValid[vlanId] == 1U)
	{
		ret = 0;
		*p_enable = 0;
		for (port = 0; port < SJA1105_N_LOGICAL_PORTS; port++)
		{
			ret += SJA1105_getPhysicalPort(port, &physicalPort);
			if ((g_vlanLookupTable[physicalPort.switchId][vlanId].vlanBc & ((uint8_t) ((uint8_t) 1 << physicalPort.physicalPort))) != 0U)
			{
				p_vlanForwarding[port] = ((g_vlanLookupTable[physicalPort.switchId][vlanId].tagPort & ((uint8_t) ((uint8_t) 1 << physicalPort.physicalPort))) != 0U)
				                          ? e_vlanForwarding_TAGGED : e_vlanForwarding_UNTAGGED;
			}
			else
			{
				p_vlanForwarding[port] = e_vlanForwarding_NOT;
			}
			if ((g_vlanLookupTable[physicalPort.switchId][vlanId].vmembPort & ((uint16_t) ((uint16_t) 1 << physicalPort.physicalPort))) != 0U)
			{  /* VLAN is enabled on this port */
				*p_enable |= ((uint16_t) 1) << port;
			}
		}
	}
	return ret;
}

/**
* \brief En-/Disable transmission within a VLAN on a specific port
*
* \param[in]  vlan VLAN to be configured
* \param[in]  ingressPort Port to be configured
* \param[in]  enable If set to 1, enables communication on the specific VLAN. If 0, ingress traffic tagged with this VLAN will be dropped.
*
* \return uint8_t Returns 0 upon successful configuration, else failed. It is only possible to enable a VLAN for a port for configuration done at run-time.
*/
extern uint8_t SJA1105_enableVlan(uint16_t vlanId, uint8_t ingressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105_port_t physicalIngressPort;
	SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanLookupTableEntry;
	if (g_vlanValid[vlanId] == 1U)
	{
		if (SJA1105_getPhysicalPort(ingressPort, &physicalIngressPort) == 0U)
		{
			loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalIngressPort.switchId);
			vlanLookupTableEntry.vmembPort &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalIngressPort.physicalPort));  /* set corresponding flag low */
			vlanLookupTableEntry.vmembPort |= (uint8_t) ((uint16_t) enable << physicalIngressPort.physicalPort);
			ret = storeVlanEntry(vlanLookupTableEntry, physicalIngressPort.switchId);
		}
	}

	return ret;
}

/**
* \brief En-/Disable ingress mirroring of a VLAN on a specific port
*
* \param[in]  vlan VLAN to be mirrored
* \param[in]  ingressPort Port to be mirrored
* \param[in]  enable If set to 1, enables ingress mirroring. 0 disables ingress mirroring
*
* \return uint8_t Returns 0 upon successful configuration, else failed. It is only possible to enable a VLAN for a port for configuration done at run-time.
*/
extern uint8_t SJA1105_enableVlanIngressMirroring(uint16_t vlanId, uint8_t ingressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105_port_t physicalIngressPort;
	SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanLookupTableEntry;
	if (g_vlanValid[vlanId] == 1U)
	{
		if (SJA1105_getPhysicalPort(ingressPort, &physicalIngressPort) == 0U)
		{
			loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalIngressPort.switchId);
			vlanLookupTableEntry.vingMirr &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalIngressPort.physicalPort));  /* set corresponding flag low */
			vlanLookupTableEntry.vingMirr |= (uint8_t) ((uint16_t) enable << physicalIngressPort.physicalPort);
			ret = storeVlanEntry(vlanLookupTableEntry, physicalIngressPort.switchId);
		}
	}

	return ret;
}

/**
* \brief En-/Disable egress mirroring of a VLAN on a specific port
*
* \param[in]  vlan VLAN to be mirrored
* \param[in]  egressPort Port to be mirrored
* \param[in]  enable If set to 1, enables egress mirroring. 0 disables egress mirroring
*
* \return uint8_t Returns 0 upon successful configuration, else failed. It is only possible to enable a VLAN for a port for configuration done at run-time.
*/
extern uint8_t SJA1105_enableVlanEgressMirroring(uint16_t vlanId, uint8_t egressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105_port_t physicalEgressPort;
	SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanLookupTableEntry;
	if (g_vlanValid[vlanId] == 1U)
	{
		if (SJA1105_getPhysicalPort(egressPort, &physicalEgressPort) == 0U)
		{
			loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalEgressPort.switchId);
			vlanLookupTableEntry.vegrMirr &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalEgressPort.physicalPort));  /* set corresponding flag low */
			vlanLookupTableEntry.vegrMirr |= (uint8_t) ((uint16_t) enable << physicalEgressPort.physicalPort);			
			ret = storeVlanEntry(vlanLookupTableEntry, physicalEgressPort.switchId);
		}
	}
	return ret;
}

/**
* \brief Start retagging frames with the specified configuration.
*
* \param[in]  ingressPorts Ingress ports at which frames matching the ingress VLAN ID will be retagged
* \param[in]  egressPorts Egress ports at which retagged frames are transmitted
* \param[in]  ingressVlanId
* \param[in]  egressVlanId
*
* \return uint8_t Return 0 upon success, else failed.
*/
extern uint8_t SJA1105_startRetagging(uint16_t ingressPorts, uint16_t egressPorts, uint16_t ingressVlanId, uint16_t egressVlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t physicalEgressPorts;
	uint8_t physicalIngressPorts;
	uint8_t retaggingEntryIndex;
	SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(egressVlanId);
	if (retaggingEntryIndex < SJA1105_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			retaggingEntry         = g_retaggingTable[switchId][retaggingEntryIndex];
			retaggingEntry.vlanIng = ingressVlanId;
			retaggingEntry.vlanEgr = egressVlanId;
			SJA1105_getPhysicalPortVector(ingressPorts, switchId, &physicalIngressPorts);
			SJA1105_getPhysicalPortVector(egressPorts, switchId, &physicalEgressPorts);
			retaggingEntry.ingPort = physicalIngressPorts;
			retaggingEntry.egrPort = physicalEgressPorts;

			ret += storeRetaggingEntry(retaggingEntry, retaggingEntryIndex, 1U, switchId);
		}
		if (ret == 0U)
		{  /* retagging successfully started on all switches */
			g_retaggingValid[retaggingEntryIndex] = 1;
		}
	}
	return ret;
}

/**
* \brief Stop retagging frames with the specified configuration.
*
* \param[in]  egressVlanId VLAN ID for which retagging should be disabled
*
* \return uint8_t Returns 0 if successful, else failed.
*/
extern uint8_t SJA1105_stopRetagging(uint16_t egressVlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t retaggingEntryIndex;
	SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(egressVlanId);
	if (retaggingEntryIndex < SJA1105_N_RETAG_ENTRIES)
	{  /* retagging entry exists */
		ret = 0;
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			retaggingEntry         = g_retaggingTable[switchId][retaggingEntryIndex];
			retaggingEntry.ingPort = 0;
			retaggingEntry.egrPort = 0;
			ret += storeRetaggingEntry(retaggingEntry, retaggingEntryIndex, g_bypassingValid[retaggingEntryIndex], switchId); /* if no bypassing rule is in place, entry can be invalidated */
		}
		if (ret == 0U)
		{  /* retagging sucessfully stopped */
			g_retaggingValid[retaggingEntryIndex] = 0;
		}
	}
	return ret;
}

extern uint8_t SJA1105_enableBypassing(uint16_t vlanId, uint16_t destPorts, uint8_t learningDisabled)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t physicalDestPorts;
	uint8_t retaggingEntryIndex;
	SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(vlanId);

	if (retaggingEntryIndex < SJA1105_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			retaggingEntry              = g_retaggingTable[switchId][retaggingEntryIndex];
			retaggingEntry.vlanEgr      = vlanId;
			retaggingEntry.doNotLearn   = learningDisabled;
			retaggingEntry.useDestports = 1;
			SJA1105_getPhysicalPortVector(destPorts, switchId, &physicalDestPorts);
			retaggingEntry.destports = physicalDestPorts;
			ret += storeRetaggingEntry(retaggingEntry, retaggingEntryIndex, 1, switchId);
		}
		if (ret == 0U)
		{  /* Bypassing successfully started */
			g_bypassingValid[retaggingEntryIndex] = 1;
		}
	}
	return ret;
}

extern uint8_t SJA1105_disableBypassing(uint16_t vlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t retaggingEntryIndex;
	SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(vlanId);
	if (retaggingEntryIndex < SJA1105_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			retaggingEntry              = g_retaggingTable[switchId][retaggingEntryIndex];
			retaggingEntry.doNotLearn   = 0;
			retaggingEntry.useDestports = 0;

			ret += storeRetaggingEntry(retaggingEntry, retaggingEntryIndex, g_retaggingValid[retaggingEntryIndex], switchId);
		}
		if (ret == 0U)
		{  /* bypassing successfully disabled */
			g_bypassingValid[retaggingEntryIndex] = 0;
		}
	}
	return ret;
}

/**
* \brief Store configuration in a local shadow
*
* \param[in]  vlanEntry Entry to be stored
*
* \return uint8_t 0 upon success, else failed
*/
static uint8_t storeVlanEntry(SJA1105_vlanLookupTableReconfigurationEntryArgument_t vlanEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105_vlanLookupTableReconfigurationControlArgument_t vlanLookupTableControl;

	vlanLookupTableControl.valid    = 1;
	vlanLookupTableControl.valident = 1;

	ret = SJA1105_setVlanLookupTableReconfigurationEntry(&vlanEntry, switchId);
	ret += SJA1105_setVlanLookupTableReconfigurationControl(&vlanLookupTableControl, switchId);
	if (ret == 0U)
	{  /* VLAN configuration written successfully. Make local copy of settings. */
		g_vlanLookupTable[switchId][vlanEntry.vlanid].tagPort   = vlanEntry.tagPort;
		g_vlanLookupTable[switchId][vlanEntry.vlanid].vlanBc    = vlanEntry.vlanBc;
		g_vlanLookupTable[switchId][vlanEntry.vlanid].vmembPort = vlanEntry.vmembPort;
		g_vlanLookupTable[switchId][vlanEntry.vlanid].vegrMirr  = vlanEntry.vegrMirr;
		g_vlanLookupTable[switchId][vlanEntry.vlanid].vingMirr  = vlanEntry.vingMirr;
	}

	return ret;
}

/**
* \brief Load configuration from local shadow
*
* \param[in]  vlanId VLAN ID of the entry to be loaded
* \param[out] p_vlanEntry Memory location where the entry will be loaded
*/
static void loadVlanEntry(uint16_t vlanId, SJA1105_vlanLookupTableReconfigurationEntryArgument_t *p_vlanEntry, uint8_t switchId)
{
	p_vlanEntry->vlanid    = vlanId;
	p_vlanEntry->tagPort   = g_vlanLookupTable[switchId][vlanId].tagPort;
	p_vlanEntry->vlanBc    = g_vlanLookupTable[switchId][vlanId].vlanBc;
	p_vlanEntry->vmembPort = g_vlanLookupTable[switchId][vlanId].vmembPort;
	p_vlanEntry->vegrMirr  = g_vlanLookupTable[switchId][vlanId].vegrMirr;
	p_vlanEntry->vingMirr  = g_vlanLookupTable[switchId][vlanId].vingMirr;
}

/**
* \brief Find the index matching the VLAN ID.\n
* If no matching entry exists, the lowest unused index is used instead.
*
* \param[in]  egressVlanId VLAN ID for which the entry is searched
*
* \return uint8_t Return the index of the retagging entry. Returns invalid index if no resources are available.
*/
static uint8_t getRetaggingEntryIndex(uint16_t egressVlanId)
{
	uint8_t retaggingEntryIndex = SJA1105_N_RETAG_ENTRIES;
	uint8_t i;
	for (i = 0; i < SJA1105_N_RETAG_ENTRIES; i++)
	{  /* find available retagging resources */
		if (((g_retaggingValid[i] == 1U) || (g_bypassingValid[i] == 1U)) && (g_retaggingTable[SJA1105_MASTER_SWITCH][i].vlanEgr == egressVlanId))
		{  /* there is already a rule in place. It will be overwritten with the new settings */
			retaggingEntryIndex = i;
			break;
		}
		else
		{
			if ((g_retaggingValid[i] == 0U) && (g_bypassingValid[i] == 0U) && (retaggingEntryIndex == SJA1105_N_RETAG_ENTRIES))
			{  /* no matching rule found yet. This is the first available entry. */
				retaggingEntryIndex = i;
			}
		}
	}
	return retaggingEntryIndex;
}


static uint8_t storeRetaggingEntry(SJA1105_retaggingTableReconfigurationEntryArgument_t retaggingEntry, uint8_t index, uint8_t validEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105_retaggingTableReconfigurationControlArgument_t retaggingControl;
	uint8_t errors = 0;

	retaggingControl.valid    = 1;
	retaggingControl.valident = validEntry;
	retaggingControl.index    = index;

	ret = SJA1105_setRetaggingTableReconfigurationEntry(&retaggingEntry, switchId);
	ret += SJA1105_setRetaggingTableReconfigurationControl(&retaggingControl, switchId);
	ret += SJA1105_getRetaggingTableReconfigurationControl(&errors, switchId);
	if (errors != 0U)
	{
		ret++;
	}
	if (ret == 0U)
	{
		g_retaggingTable[switchId][index] = retaggingEntry;
	}

	return ret;
}
