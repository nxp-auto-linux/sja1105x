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
* \file NXP_SJA1105P_addressResolutionTable.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Functions for the address resolution lookup
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_addressResolutionTable.h"

/******************************************************************************
* Defines
*****************************************************************************/

#define N_ARL_ENTRIES 1024U

#define MAC_ADDRESS_BYTES 6U
#define MAC_ADDRESS_MASK  (0xFFFFFFFFFFUL)

#define BYTE 8U

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t findEntryIndex(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
static uint8_t findFreeEntry(uint16_t *p_freeEntryIndex, uint8_t switchId);
static uint8_t executeTcamCommand(SJA1105P_hostCmd_t hostCmd, SJA1105P_l2ArtLockedEntryArgument_t *p_physicalArlTableEntry, SJA1105P_l2AddressLookupTableControlGetArgument_t *p_physicalArlTableStatus, uint8_t switchId);

static void convertToPhysicalEntry(const SJA1105P_addressResolutionTableEntry_t *kp_arlTableEntry, SJA1105P_l2ArtLockedEntryArgument_t *p_physicalArlTableEntry, uint8_t switchId);
static void convertFromPhysicalEntry(SJA1105P_addressResolutionTableEntry_t *p_arlTableEntry, const SJA1105P_l2ArtLockedEntryArgument_t *kp_physicalArlTableEntry, uint8_t switchId);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Insert an entry into the Address Resolution Table.
*
* \param[in,out] p_addressResolutionTableEntry Memory location where the ARL entry data is stored. After success, the index field is updated.
*
* \return uint8_t: {0: config successful, else: failed}
*/
extern uint8_t SJA1105P_addArlTableEntry(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret;
	uint8_t switchId;
	uint8_t destSwitches;
	uint8_t firstDestSwitchId = SJA1105P_N_SWITCHES;
	uint8_t lastDestSwitchId = 0;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry;
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	/* find free entry in the first switch and add entry there */
	convertToPhysicalEntry(p_addressResolutionTableEntry, &physicalArlTableEntry, SJA1105P_MASTER_SWITCH);
	ret = findFreeEntry(&(physicalArlTableEntry.index), SJA1105P_MASTER_SWITCH);
	if ((ret == 0U) && (physicalArlTableEntry.index != N_ARL_ENTRIES))
	{
		p_addressResolutionTableEntry->index = physicalArlTableEntry.index;

		/* Determine physical switch setup */
		SJA1105P_getSwitchesFromPorts(p_addressResolutionTableEntry->ports, &destSwitches);
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			if (((destSwitches >> switchId) & 1U) == 1U)
			{  /* switchId is a destination of this MAC-VLAN combination */
				firstDestSwitchId = (firstDestSwitchId == SJA1105P_N_SWITCHES) ? switchId : firstDestSwitchId;
				lastDestSwitchId = (switchId > lastDestSwitchId) ? switchId : lastDestSwitchId;
			}
		}

		/* add entries to the switch instances */
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			/* configure external ports */
			SJA1105P_getPhysicalPortVector(p_addressResolutionTableEntry->ports, switchId, &(physicalArlTableEntry.destports)); 

			/* configure inter switch forwarding */
			if (switchId < lastDestSwitchId)
			{  /* forward to next switch */
				physicalArlTableEntry.destports |= (uint8_t) (((uint8_t) 1) << SJA1105P_g_generalParameters.cascPort[switchId]);
			}
			if (switchId > firstDestSwitchId)
			{  /* forward to previous switch */
				physicalArlTableEntry.destports |= (uint8_t) (((uint8_t) 1) << SJA1105P_g_generalParameters.hostPort[switchId]);
			}

			ret += executeTcamCommand(SJA1105P_e_hostCmd_WRITE, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
		}
	}
	return ret;
}

/**
* \brief Read the entry from the Address Resolution table base on VLAN and MAC address information.
*
* A partial entry can be provided to this function containing only VLAN and MAC information.
* The function will return the complete entry containing the full configuration of the entry.
*
* \param[inout] p_addressResolutionTableEntry ARL entry that should be read (in: VLAN/MAC information. Out: complete entry)
*
* \return uint8_t: Returns 0 when successful, else failed or entry didn't exist
*/
extern uint8_t SJA1105P_readArlTableEntryByAddress(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret ;

	ret = findEntryIndex(p_addressResolutionTableEntry);
	if ((ret == 0U) && (p_addressResolutionTableEntry->index <= N_ARL_ENTRIES))
	{  /* an entry was found */
		ret = SJA1105P_readArlTableEntryByIndex(p_addressResolutionTableEntry);
	}

	return ret;
}

/**
* \brief Read the entry from the Address Resolution table base on the index.
*
* A partial entry can be provided to this function containing only VLAN and MAC information.
* The function will return the complete entry containing the full configuration of the entry.
*
* \param[inout] p_addressResolutionTableEntry ARL entry that should be read (in: index. Out: complete entry)
*
* \return uint8_t: Returns 0 when successful, else failed
*/
extern uint8_t SJA1105P_readArlTableEntryByIndex(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t  ret = 0;
	uint8_t  switchId;
	uint16_t logicalDestPorts;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry = {0};
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	physicalArlTableEntry.index = p_addressResolutionTableEntry->index;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += executeTcamCommand(SJA1105P_e_hostCmd_READ, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
		if (switchId == 0U)
		{
			p_addressResolutionTableEntry->enabled = physicalArlTableControlStatus.valident;
			convertFromPhysicalEntry(p_addressResolutionTableEntry, &physicalArlTableEntry, switchId);
		}
		else
		{  /* for incremental switches, only update the port vector */
			SJA1105P_getPortVectorFromPhysicalPortVector(physicalArlTableEntry.destports, switchId, &logicalDestPorts);
			p_addressResolutionTableEntry->ports |= logicalDestPorts;
		}
	}

	return ret;
}

/**
* \brief Remove the entry from the Address Resolution table base on VLAN and MAC address information.
*
* \param[in] p_addressResolutionTableEntry ARL entry that should be removed
*
* \return uint8_t: Returns 0 when successful, else failed or entry didn't exist
*/
extern uint8_t SJA1105P_removeArlTableEntryByAddress(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret;

	ret = findEntryIndex(p_addressResolutionTableEntry);
	if ((ret == 0U) && (p_addressResolutionTableEntry->index <= N_ARL_ENTRIES))
	{  /* an entry was found */
		ret = SJA1105P_removeArlTableEntryByIndex(p_addressResolutionTableEntry);
	}

	return ret;
}

/**
* \brief Remove the entry from the Address Resolution table base at a specific index
*
* \param[in] p_addressResolutionTableEntry ARL entry that should be removed
*
* \return uint8_t: Returns 0 when successful, else failed
*/
extern uint8_t SJA1105P_removeArlTableEntryByIndex(const SJA1105P_addressResolutionTableEntry_t *kp_addressResolutionTableEntry)
{
	uint8_t ret = 0;
	uint8_t switchId;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry = {0};
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	physicalArlTableEntry.index = kp_addressResolutionTableEntry->index;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += executeTcamCommand(SJA1105P_e_hostCmd_INVALIDATE, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
	}
	return ret;
}

/**
* \brief Start/Stop mirroring based on an entry in the ARL table
*
* \param[in]  arlEntryIndex Index of the entry to be used for mirroring
* \param[in]  enable 0 to turn mirroring off, 1 to start mirroring
*
* \return uint8_t: Returns 0 on success, else failed.
*/
extern uint8_t SJA1105P_enableArlMirroring(uint16_t arlEntryIndex, uint8_t enable)
{
	uint8_t ret = 0;
	uint8_t switchId;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry = {0};
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	physicalArlTableEntry.index = arlEntryIndex;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += executeTcamCommand(SJA1105P_e_hostCmd_READ, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
		physicalArlTableEntry.mirror = enable;
		ret += executeTcamCommand(SJA1105P_e_hostCmd_WRITE, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
	}

	return ret;
}

/**
* \brief Start/Stop retagging based on an entry in the ARL table
*
* \param[in]  arlEntryIndex Index of the entry to be used for retagging
* \param[in]  retaggingVlanId VLAN ID used in the retagging process
* \param[in]  enable 0 to turn retagging off, 1 to start retagging
*
* \return uint8_t: Returns 0 on success, else failed.
*/
extern uint8_t SJA1105P_enableArlRetagging(uint16_t arlEntryIndex, uint16_t retaggingVlanId, uint8_t enable)
{
	uint8_t ret = 0;
	uint8_t switchId;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry = {0};
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	physicalArlTableEntry.index = arlEntryIndex;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += executeTcamCommand(SJA1105P_e_hostCmd_READ, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
		physicalArlTableEntry.retag = enable;
		physicalArlTableEntry.mirroredVlan = retaggingVlanId;
		ret += executeTcamCommand(SJA1105P_e_hostCmd_WRITE, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
	}
	
	return ret;
}


/**
* \brief Find the index corresponding to a TCAM entry.
*
* \param[inout] p_freeEntryIndex Index of the free index. Invalid index if entry is not found
* \param[in]    switchId Reference to the physical switch on which the entry will be searched for
*
* \return uint8_t: Returns 0 on success, else failed.
*/
static uint8_t findEntryIndex(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret;

	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry;
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	convertToPhysicalEntry(p_addressResolutionTableEntry, &physicalArlTableEntry, SJA1105P_MASTER_SWITCH);
	ret = executeTcamCommand(SJA1105P_e_hostCmd_SEARCH, &physicalArlTableEntry, &physicalArlTableControlStatus, SJA1105P_MASTER_SWITCH);

	p_addressResolutionTableEntry->index = N_ARL_ENTRIES;  /* init with invalid index */
	if (physicalArlTableControlStatus.valident == 1U)
	{
		p_addressResolutionTableEntry->index = physicalArlTableEntry.index;
	}

	return ret;
}

/**
* \brief Iterate through TCAM to find the first free entry
*
* \param[out]   p_freeEntryIndex Index of the free index. Invalid index if TCAM is full
* \param[in]    switchId Reference to the physical switch on which the command will be executed
*
* \return uint8_t: Returns 0 on success, else failed.
*/
static uint8_t findFreeEntry(uint16_t *p_freeEntryIndex, uint8_t switchId)
{
	uint8_t ret = 0;
	uint16_t entryIndex;
	SJA1105P_l2ArtLockedEntryArgument_t physicalArlTableEntry;
	SJA1105P_l2AddressLookupTableControlGetArgument_t physicalArlTableControlStatus;

	*p_freeEntryIndex = N_ARL_ENTRIES;
	for (entryIndex = 0; entryIndex < N_ARL_ENTRIES; entryIndex++)
	{
		physicalArlTableEntry.index = entryIndex;
		ret += executeTcamCommand(SJA1105P_e_hostCmd_READ, &physicalArlTableEntry, &physicalArlTableControlStatus, switchId);
		if ((ret != 0U) || (*p_freeEntryIndex != N_ARL_ENTRIES))
		{  /* either on failure or success, break the loop */
			break;
		}
		if ((physicalArlTableControlStatus.valident == 0U) ||
		    (physicalArlTableControlStatus.lockeds == 0U))
		{  /* There is no valid, static entry at this index */
			*p_freeEntryIndex = entryIndex; /* use this index */
		}
	}

	return ret;
}

/**
* \brief Executes a host command towards the TCAM (read, write, search, invalidate)
*
* \param[in]    hostCmd Command to be executed
* \param[inout] p_physicalArlTableEntry Entry within the switch. Depending on the command, this serves as input or output
* \param[out]   p_physicalArlTableStatus After successful operation, the current status of the control register is returned
* \param[in]    switchId Reference to the physical switch on which the command will be executed
*
* \return uint8_t: Returns 0 on success, else failed.
*/
static uint8_t executeTcamCommand(SJA1105P_hostCmd_t hostCmd, SJA1105P_l2ArtLockedEntryArgument_t *p_physicalArlTableEntry, SJA1105P_l2AddressLookupTableControlGetArgument_t *p_physicalArlTableStatus, uint8_t switchId)
{
	uint8_t ret;

	SJA1105P_l2AddressLookupTableControlSetArgument_t controlSetArg;

	controlSetArg.valid     = 1;  /* Required for any read/write operation */
	controlSetArg.lockeds   = 1;  /* Only permanent entries are handled here */
	controlSetArg.valident  = 1;  /* TODO: Does this conflict with invalidation? */
	controlSetArg.mgmtroute = 0;  /* Only regular ARL entries are handled here */
	controlSetArg.hostCmd   = hostCmd;

	switch (hostCmd)
	{
		case SJA1105P_e_hostCmd_INVALIDATE:
			/* Fallthrough */
		case SJA1105P_e_hostCmd_WRITE:
			controlSetArg.rdwrset = 1;
			break;
		default:
			/* read access */
			controlSetArg.rdwrset = 0;
			break;
	}

	ret  = SJA1105P_setL2ArtLockedEntry(p_physicalArlTableEntry, switchId);
	ret += SJA1105P_setL2AddressLookupTableControl(&controlSetArg, switchId);

	do
	{  /* check status until operation is complete or failed */
		ret += SJA1105P_getL2AddressLookupTableControl(p_physicalArlTableStatus, switchId);
	}
	while ((p_physicalArlTableStatus->valid == 1U) && (ret == 0U));  /* Operation not yet complete, but no error */

	/* complete read operations */
	if ((ret == 0U) && (p_physicalArlTableStatus->errors == 0U))
	{  /* operation complete and went fine */
		if (controlSetArg.rdwrset == 0U)
		{
			ret = SJA1105P_getL2ArtLockedEntry(p_physicalArlTableEntry, switchId);
		}
	}
	else
	{
		ret = 1;
	}

	return ret;
}

/**
* \brief Converts an ARL entry from the driver API to the description in the physical switch
*
* \param[in]  kp_arlTableEntry Entry to be converted
* \param[out] p_physicalArlTableEntry The converted entry
* \param[in]  switchId Reference to the physical switch for which the entry will be converted
*
*/
static void convertToPhysicalEntry(const SJA1105P_addressResolutionTableEntry_t *kp_arlTableEntry, SJA1105P_l2ArtLockedEntryArgument_t *p_physicalArlTableEntry, uint8_t switchId)
{
	p_physicalArlTableEntry->takeTs  = 0;  /* currently not used in the driver */
	p_physicalArlTableEntry->tsReg   = 0;  /* not applicable */

	p_physicalArlTableEntry->index   = kp_arlTableEntry->index;
	p_physicalArlTableEntry->enfport = kp_arlTableEntry->enforcePorts;
	p_physicalArlTableEntry->macaddr = kp_arlTableEntry->dstMacAddress;
	p_physicalArlTableEntry->vlanid  = kp_arlTableEntry->vlanId;

	if (kp_arlTableEntry->p_extension == NULL)
	{  /* No extended configuration, use default values */
		p_physicalArlTableEntry->innerOuterVlan = 1;  /* outer VLAN */
		p_physicalArlTableEntry->mask           = 1;  /* exact match */
		p_physicalArlTableEntry->retag          = 0;  /* no retagging */
		p_physicalArlTableEntry->mirror         = 0;  /* no mirroring */
		p_physicalArlTableEntry->mirroredVlan   = 0;  /* not applicable */
	}
	else
	{
		p_physicalArlTableEntry->innerOuterVlan = kp_arlTableEntry->p_extension->innerOuterVlan;
		p_physicalArlTableEntry->mask           = (((uint64_t) kp_arlTableEntry->p_extension->vlanIdMask << (MAC_ADDRESS_BYTES * BYTE)) | (kp_arlTableEntry->p_extension->dstMacAddressMask & MAC_ADDRESS_MASK));
		p_physicalArlTableEntry->retag          = kp_arlTableEntry->p_extension->retagging;
		p_physicalArlTableEntry->mirror         = kp_arlTableEntry->p_extension->mirroring;
		p_physicalArlTableEntry->mirroredVlan   = kp_arlTableEntry->p_extension->retaggingVlanId;
	}

	SJA1105P_getPhysicalPortVector(kp_arlTableEntry->ports, switchId, &(p_physicalArlTableEntry->destports));
}


/**
* \brief Converts an ARL entry from the physical switch to the description for the driver API
*
* \param[out] p_arlTableEntry Entry to be converted
* \param[in]  kp_physicalArlTableEntry The converted entry
* \param[in]  switchId Reference to the physical switch for which the entry will be converted
*
*/
static void convertFromPhysicalEntry(SJA1105P_addressResolutionTableEntry_t *p_arlTableEntry, const SJA1105P_l2ArtLockedEntryArgument_t *kp_physicalArlTableEntry, uint8_t switchId)
{
	p_arlTableEntry->index         = kp_physicalArlTableEntry->index;
	p_arlTableEntry->enforcePorts  = kp_physicalArlTableEntry->enfport;
	p_arlTableEntry->dstMacAddress = kp_physicalArlTableEntry->macaddr;
	p_arlTableEntry->vlanId        = kp_physicalArlTableEntry->vlanid;

	if (p_arlTableEntry->p_extension != NULL)
	{  /* also write back extended settings */
		p_arlTableEntry->p_extension->innerOuterVlan  = kp_physicalArlTableEntry->innerOuterVlan;

		p_arlTableEntry->p_extension->retagging       = kp_physicalArlTableEntry->retag;
		p_arlTableEntry->p_extension->mirroring       = kp_physicalArlTableEntry->mirror;
		p_arlTableEntry->p_extension->retaggingVlanId = kp_physicalArlTableEntry->mirroredVlan;

		p_arlTableEntry->p_extension->vlanIdMask        = (uint16_t) (kp_physicalArlTableEntry->mask >> (MAC_ADDRESS_BYTES * BYTE));
		p_arlTableEntry->p_extension->dstMacAddressMask = (uint16_t) (kp_physicalArlTableEntry->mask & MAC_ADDRESS_MASK);
	}

	SJA1105P_getPortVectorFromPhysicalPortVector(kp_physicalArlTableEntry->destports, switchId, &(p_arlTableEntry->ports));
}
