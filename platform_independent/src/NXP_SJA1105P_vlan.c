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
* \file NXP_SJA1105P_vlan.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Configuration of VLANs
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_vlan.h"
#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_switchCore.h"

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

/* Auxiliary configuration information for acceleration */
static uint16_t g_retaggingTableVlanId[SJA1105P_N_RETAG_ENTRIES] = {0};  /**< Look-up of stored rules with respect to egress VLAN ID */
static uint8_t g_retaggingValid[SJA1105P_N_RETAG_ENTRIES] = {0U};
static uint8_t g_bypassingValid[SJA1105P_N_RETAG_ENTRIES] = {0U};

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t storeVlanEntry(const SJA1105P_vlanLookupTableEntryArgument_t *kp_vlanEntry, uint8_t switchId);
static uint8_t loadVlanEntry(uint16_t vlanId, SJA1105P_vlanLookupTableEntryArgument_t *p_vlanEntry, uint8_t switchId);

static uint8_t getRetaggingEntryIndex(uint16_t egressVlanId);
static uint8_t storeRetaggingEntry(const SJA1105P_retaggingTableEntryArgument_t *kp_retaggingEntry, uint8_t index, uint8_t validEntry, uint8_t switchId);
static uint8_t loadRetaggingEntry(SJA1105P_retaggingTableEntryArgument_t *p_retaggingEntry, uint8_t index, uint8_t *p_validEntry, uint8_t switchId);

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
extern uint8_t SJA1105P_writeVlanConfig(uint16_t vlanId, const SJA1105P_vlanForwarding_t *pk_vlanForwarding, uint16_t enable)
{
	uint8_t  ret = 0;
	uint8_t  switchId;
	uint8_t port;
	uint8_t  enabledPhysicalPorts;
	uint8_t  firstMemberSwitchId = SJA1105P_N_SWITCHES;
	uint8_t  lastMemberSwitchId  = 0;
	uint8_t  firstBroadcastSwitchId = SJA1105P_N_SWITCHES;
	uint8_t  lastBroadcastSwitchId  = 0;
	SJA1105P_port_t physicalPort;
	SJA1105P_vlanLookupTableEntryArgument_t vlanLookupTableEntry[SJA1105P_N_SWITCHES] = {{0}};

	/* determine VLAN lookup configuration for logical port and translate to physical lookups */
	for (port = 0U; port < SJA1105P_N_LOGICAL_PORTS; port++)
	{
		ret += SJA1105P_getPhysicalPort(port, &physicalPort);
		switch (pk_vlanForwarding[port])
		{
			case SJA1105P_e_vlanForwarding_TAGGED:
			{
				vlanLookupTableEntry[physicalPort.switchId].tagPort |= (uint8_t) ((uint8_t) 1 << physicalPort.physicalPort);
				vlanLookupTableEntry[physicalPort.switchId].vlanBc  |= (uint8_t) ((uint8_t) 1 << physicalPort.physicalPort);
				break;
			}
			case SJA1105P_e_vlanForwarding_UNTAGGED:
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
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		SJA1105P_getPhysicalPortVector(enable, switchId, &enabledPhysicalPorts);
		vlanLookupTableEntry[switchId].vmembPort = enabledPhysicalPorts;
	}

	/* determine vlan lookup configuration for the internal ports (cascaded) */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		if (vlanLookupTableEntry[switchId].vmembPort != 0U)
		{  /* at least one port in this switch is VLAN member */
			firstMemberSwitchId = (firstMemberSwitchId == SJA1105P_N_SWITCHES) ? (switchId) : (firstMemberSwitchId);
			lastMemberSwitchId  = (switchId > lastMemberSwitchId) ? (switchId) : (firstMemberSwitchId);
		}
		if (vlanLookupTableEntry[switchId].vlanBc != 0U)
		{  /* at least one port in this switch is VLAN member */
			firstBroadcastSwitchId = (firstBroadcastSwitchId == SJA1105P_N_SWITCHES) ? (switchId) : (firstBroadcastSwitchId);
			lastBroadcastSwitchId  = (switchId > lastBroadcastSwitchId) ? (switchId) : (firstBroadcastSwitchId);
		}
	}

	/* configure internal ports VLAN forwarding */
	for (switchId = 0; switchId < (SJA1105P_N_SWITCHES - 1U); switchId++)
	{
		if (switchId >= firstMemberSwitchId)
		{
			if (switchId < lastMemberSwitchId)
			{
				if (switchId < lastBroadcastSwitchId)
				{
					/* enable on this switch to forward to the cascaded port */
					vlanLookupTableEntry[switchId].vlanBc  |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.cascPort[switchId]);
					vlanLookupTableEntry[switchId].tagPort |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.cascPort[switchId]);
					/* also enable receiving on the connecting counter part of cascaded port */
					vlanLookupTableEntry[switchId + 1U].vmembPort |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.hostPort[switchId + 1U]);
					vlanLookupTableEntry[switchId + 1U].tagPort   |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.hostPort[switchId + 1U]);
				}
				if (switchId >= firstBroadcastSwitchId)
				{
					/* enable on this switch to receive from the cascaded port */
					vlanLookupTableEntry[switchId].vmembPort |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.cascPort[switchId]);
					vlanLookupTableEntry[switchId].tagPort   |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.cascPort[switchId]);
					/* also enable forwarding on the connecting counter part of cascaded port */
					vlanLookupTableEntry[switchId + 1U].vlanBc  |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.hostPort[switchId + 1U]);
					vlanLookupTableEntry[switchId + 1U].tagPort |= (uint8_t) ((uint8_t) 1 << SJA1105P_g_generalParameters.hostPort[switchId + 1U]);
				}
			}
		}
	}

	/* Download the VLAN configuration */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		vlanLookupTableEntry[switchId].vlanid = vlanId;
		ret += storeVlanEntry(&(vlanLookupTableEntry[switchId]), switchId);
	}

	return ret;
}

/**
* \brief Read configuration of a VLAN.
*
* \param[out] vlan VLAN of which the configuration should be read
* \param[out] p_vlanForwarding Pointer to the memory location of a list describing the forwarding type of each port
* \param[out] p_enable Pointer to the memory location of a bit vector, where each bit specifies whether VLAN traffic should be enabled
*
* \return uint8_t Returns 0 upon successful read. Else, the configuration could not be read.
*/
extern uint8_t SJA1105P_readVlanConfig(uint16_t vlanId, SJA1105P_vlanForwarding_t *p_vlanForwarding, uint16_t *p_enable)
{
	uint8_t ret = 0;
	uint8_t port;
	uint8_t switchId;
	SJA1105P_port_t physicalPort;
	SJA1105P_vlanLookupTableEntryArgument_t vlanLookupTableEntry[SJA1105P_N_SWITCHES] = {{0}};

	*p_enable = 0;
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += loadVlanEntry(vlanId, &vlanLookupTableEntry[switchId], switchId);
	}

	for (port = 0; port < SJA1105P_N_LOGICAL_PORTS; port++)
	{
		ret += SJA1105P_getPhysicalPort(port, &physicalPort);
		if ((vlanLookupTableEntry[physicalPort.switchId].vlanBc & ((uint8_t) ((uint8_t) 1 << physicalPort.physicalPort))) != 0U)
		{
			p_vlanForwarding[port] = ((vlanLookupTableEntry[physicalPort.switchId].tagPort & ((uint8_t) ((uint8_t) 1 << physicalPort.physicalPort))) != 0U)
			                          ? SJA1105P_e_vlanForwarding_TAGGED : SJA1105P_e_vlanForwarding_UNTAGGED;
		}
		else
		{
			p_vlanForwarding[port] = SJA1105P_e_vlanForwarding_NOT;
		}
		if ((vlanLookupTableEntry[physicalPort.switchId].vmembPort & ((uint16_t) ((uint16_t) 1 << physicalPort.physicalPort))) != 0U)
		{  /* VLAN is enabled on this port */
			*p_enable |= ((uint16_t) 1) << port;
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
extern uint8_t SJA1105P_enableVlan(uint16_t vlanId, uint8_t ingressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105P_port_t physicalIngressPort;
	SJA1105P_vlanLookupTableEntryArgument_t vlanLookupTableEntry;

	if (SJA1105P_getPhysicalPort(ingressPort, &physicalIngressPort) == 0U)
	{
		ret  = loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalIngressPort.switchId);
		vlanLookupTableEntry.vmembPort &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalIngressPort.physicalPort));  /* set corresponding flag low */
		vlanLookupTableEntry.vmembPort |= (uint8_t) ((uint16_t) enable << physicalIngressPort.physicalPort);
		ret += storeVlanEntry(&vlanLookupTableEntry, physicalIngressPort.switchId);
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
extern uint8_t SJA1105P_enableVlanIngressMirroring(uint16_t vlanId, uint8_t ingressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105P_port_t physicalIngressPort;
	SJA1105P_vlanLookupTableEntryArgument_t vlanLookupTableEntry;

	if (SJA1105P_getPhysicalPort(ingressPort, &physicalIngressPort) == 0U)
	{
		ret  = loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalIngressPort.switchId);
		vlanLookupTableEntry.vingMirr &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalIngressPort.physicalPort));  /* set corresponding flag low */
		vlanLookupTableEntry.vingMirr |= (uint8_t) ((uint16_t) enable << physicalIngressPort.physicalPort);
		ret += storeVlanEntry(&vlanLookupTableEntry, physicalIngressPort.switchId);
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
extern uint8_t SJA1105P_enableVlanEgressMirroring(uint16_t vlanId, uint8_t egressPort, uint8_t enable)
{
	uint8_t ret = 1;
	SJA1105P_port_t physicalEgressPort;
	SJA1105P_vlanLookupTableEntryArgument_t vlanLookupTableEntry;

	if (SJA1105P_getPhysicalPort(egressPort, &physicalEgressPort) == 0U)
	{
		ret  = loadVlanEntry(vlanId, &vlanLookupTableEntry, physicalEgressPort.switchId);
		vlanLookupTableEntry.vegrMirr &= (uint8_t) ~((uint8_t) ((uint16_t) 1 << physicalEgressPort.physicalPort));  /* set corresponding flag low */
		vlanLookupTableEntry.vegrMirr |= (uint8_t) ((uint16_t) enable << physicalEgressPort.physicalPort);			
		ret += storeVlanEntry(&vlanLookupTableEntry, physicalEgressPort.switchId);
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
extern uint8_t SJA1105P_startRetagging(uint16_t ingressPorts, uint16_t egressPorts, uint16_t ingressVlanId, uint16_t egressVlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t physicalEgressPorts;
	uint8_t physicalIngressPorts;
	uint8_t retaggingEntryIndex;
	uint8_t validEntry;
	SJA1105P_retaggingTableEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(egressVlanId);

	if (retaggingEntryIndex < SJA1105P_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += loadRetaggingEntry(&retaggingEntry, retaggingEntryIndex, &validEntry, switchId);

			/* modify parts relevant to the retagging */
			retaggingEntry.vlanIng = ingressVlanId;
			retaggingEntry.vlanEgr = egressVlanId;
			SJA1105P_getPhysicalPortVector(ingressPorts, switchId, &physicalIngressPorts);
			SJA1105P_getPhysicalPortVector(egressPorts, switchId, &physicalEgressPorts);
			retaggingEntry.ingPort = physicalIngressPorts;
			retaggingEntry.egrPort = physicalEgressPorts;

			ret += storeRetaggingEntry(&retaggingEntry, retaggingEntryIndex, 1U, switchId);
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
extern uint8_t SJA1105P_stopRetagging(uint16_t egressVlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t retaggingEntryIndex;
	uint8_t validEntry;
	SJA1105P_retaggingTableEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(egressVlanId);
	if (retaggingEntryIndex < SJA1105P_N_RETAG_ENTRIES)
	{  /* retagging entry exists */
		ret = 0;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += loadRetaggingEntry(&retaggingEntry, retaggingEntryIndex, &validEntry, switchId);

			retaggingEntry.ingPort = 0;
			retaggingEntry.egrPort = 0;

			ret += storeRetaggingEntry(&retaggingEntry, retaggingEntryIndex, g_bypassingValid[retaggingEntryIndex], switchId); /* if no bypassing rule is in place, entry can be invalidated */
		}
		if (ret == 0U)
		{  /* retagging sucessfully stopped */
			g_retaggingValid[retaggingEntryIndex] = 0;
		}
	}
	return ret;
}

extern uint8_t SJA1105P_enableBypassing(uint16_t vlanId, uint16_t destPorts, uint8_t learningDisabled)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t physicalDestPorts;
	uint8_t retaggingEntryIndex;
	uint8_t validEntry;
	SJA1105P_retaggingTableEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(vlanId);

	if (retaggingEntryIndex < SJA1105P_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += loadRetaggingEntry(&retaggingEntry, retaggingEntryIndex, &validEntry, switchId);

			retaggingEntry.vlanEgr      = vlanId;
			retaggingEntry.doNotLearn   = learningDisabled;
			retaggingEntry.useDestports = 1;
			SJA1105P_getPhysicalPortVector(destPorts, switchId, &physicalDestPorts);
			retaggingEntry.destports = physicalDestPorts;

			ret += storeRetaggingEntry(&retaggingEntry, retaggingEntryIndex, 1, switchId);
		}
		if (ret == 0U)
		{  /* Bypassing successfully started */
			g_bypassingValid[retaggingEntryIndex] = 1;
		}
	}
	return ret;
}

extern uint8_t SJA1105P_disableBypassing(uint16_t vlanId)
{
	uint8_t ret = 1;
	uint8_t switchId;
	uint8_t retaggingEntryIndex;
	uint8_t validEntry;
	SJA1105P_retaggingTableEntryArgument_t retaggingEntry;

	retaggingEntryIndex = getRetaggingEntryIndex(vlanId);
	if (retaggingEntryIndex < SJA1105P_N_RETAG_ENTRIES)
	{
		ret = 0;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += loadRetaggingEntry(&retaggingEntry, retaggingEntryIndex, &validEntry, switchId);

			retaggingEntry.doNotLearn   = 0;
			retaggingEntry.useDestports = 0;

			ret += storeRetaggingEntry(&retaggingEntry, retaggingEntryIndex, g_retaggingValid[retaggingEntryIndex], switchId);
		}
		if (ret == 0U)
		{  /* bypassing successfully disabled */
			g_bypassingValid[retaggingEntryIndex] = 0;
		}
	}
	return ret;
}

/**
* \brief Store configuration in switch
*
* \param[in]  vlanEntry Entry to be stored
* \param[in]  switchId switch to which the entry will be stored
*
* \return uint8_t 0 upon success, else failed
*/
static uint8_t storeVlanEntry(const SJA1105P_vlanLookupTableEntryArgument_t *kp_vlanEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105P_vlanLookupTableControlArgument_t vlanLookupTableControl;

	vlanLookupTableControl.valid    = 1;
	vlanLookupTableControl.valident = 1;
	vlanLookupTableControl.rdwrset  = 1;  /* write access */

	ret  = SJA1105P_setVlanLookupTableEntry(kp_vlanEntry, switchId);
	ret += SJA1105P_setVlanLookupTableControl(&vlanLookupTableControl, switchId);

	return ret;
}

/**
* \brief Load configuration from switch
*
* \param[in]  vlanId VLAN ID of the entry to be loaded
* \param[out] p_vlanEntry Memory location where the entry will be loaded
* \param[in]  switchId switch from which the entry will be loaded
*/
static uint8_t loadVlanEntry(uint16_t vlanId, SJA1105P_vlanLookupTableEntryArgument_t *p_vlanEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105P_vlanLookupTableControlArgument_t vlanLookupTableControl;

	vlanLookupTableControl.valid    = 1;
	vlanLookupTableControl.valident = 1;
	vlanLookupTableControl.rdwrset  = 0;  /* read access */

	p_vlanEntry->vlanid = vlanId;

	ret  = SJA1105P_setVlanLookupTableEntry(p_vlanEntry, switchId);
	ret += SJA1105P_setVlanLookupTableControl(&vlanLookupTableControl, switchId);
	ret += SJA1105P_getVlanLookupTableEntry(p_vlanEntry, switchId);

	return ret;
}

/**
* \brief Find the index matching the VLAN ID.
*
* If no matching entry exists, the lowest unused index is used instead.
*
* \param[in]  egressVlanId VLAN ID for which the entry is searched
*
* \return uint8_t Return the index of the retagging entry. Returns invalid index if no resources are available.
*/
static uint8_t getRetaggingEntryIndex(uint16_t egressVlanId)
{
	uint8_t retaggingEntryIndex = SJA1105P_N_RETAG_ENTRIES;
	uint8_t i;
	for (i = 0; i < SJA1105P_N_RETAG_ENTRIES; i++)
	{  /* find available retagging resources */
		if (((g_retaggingValid[i] == 1U) || (g_bypassingValid[i] == 1U)) && (g_retaggingTableVlanId[i] == egressVlanId))
		{  /* there is already a rule in place. It will be overwritten with the new settings */
			retaggingEntryIndex = i;
			break;
		}
		else
		{
			if ((g_retaggingValid[i] == 0U) && (g_bypassingValid[i] == 0U) && (retaggingEntryIndex == SJA1105P_N_RETAG_ENTRIES))
			{  /* no matching rule found yet. This is the first available entry. */
				g_retaggingTableVlanId[i] = egressVlanId;
				retaggingEntryIndex = i;
			}
		}
	}
	return retaggingEntryIndex;
}

static uint8_t storeRetaggingEntry(const SJA1105P_retaggingTableEntryArgument_t *kp_retaggingEntry, uint8_t index, uint8_t validEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105P_retaggingTableControlArgument_t retaggingControl;
	uint8_t errors = 0;

	retaggingControl.valid    = 1;
	retaggingControl.valident = validEntry;
	retaggingControl.rdwrset  = 1;  /* write */
	retaggingControl.index    = index;

	ret =  SJA1105P_setRetaggingTableEntry(kp_retaggingEntry, switchId);
	ret += SJA1105P_setRetaggingTableControl(&retaggingControl, switchId);
	ret += SJA1105P_getRetaggingTableControl(&errors, switchId);
	if (errors != 0U)
	{
		ret++;
	}

	if (validEntry == 0U)
	{
		g_retaggingTableVlanId[index] = 0;  /* reset the vlan id stored in lookup */
	}

	return ret;
}

static uint8_t loadRetaggingEntry(SJA1105P_retaggingTableEntryArgument_t *p_retaggingEntry, uint8_t index, uint8_t *p_validEntry, uint8_t switchId)
{
	uint8_t ret;
	SJA1105P_retaggingTableControlArgument_t retaggingControl = {0};
	uint8_t errors = 0;

	retaggingControl.valid    = 1;
	retaggingControl.rdwrset  = 0;  /* read */
	retaggingControl.index    = index;

	ret  = SJA1105P_setRetaggingTableControl(&retaggingControl, switchId);
	ret += SJA1105P_getRetaggingTableControl(&errors, switchId);
	if (errors != 0U)
	{
		ret++;
	}
	else
	{
		if (ret == 0U)
		{
			ret = SJA1105P_getRetaggingTableEntry(p_retaggingEntry, switchId);
			*p_validEntry = retaggingControl.valident;
		}
	}

	return ret;
}

