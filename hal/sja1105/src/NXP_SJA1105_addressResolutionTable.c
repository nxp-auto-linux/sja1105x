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
* \file NXP_SJA1105_addressResolutionTable.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Functions for the L2 Lookup Table configuration
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "NXP_SJA1105_config.h"
#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_addressResolutionTable.h"
#include "NXP_SJA1105_crc.h"

/******************************************************************************
* Defines
*****************************************************************************/

#define ADDR_RESOLUTION_TABLE_COLUMNS 4U
#define ADDR_RESOLUTION_TABLE_ROWS 256U

#define N_BYTES_TO_BE_HASHED 8U

#define N_L2_ENTRIES ((uint16_t) (ADDR_RESOLUTION_TABLE_ROWS * ADDR_RESOLUTION_TABLE_COLUMNS))

#define MAC_ADDRESS_TOP 7U
#define MAC_ADDRESS_BYTES 6U
#define BYTE 8U
 
/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t getHashSum(uint16_t vlanId, uint64_t macAddress);
static uint8_t getAvailableTableIndex(uint16_t vlanId, uint64_t macAddress, uint16_t *p_index);
static uint8_t requestARTableEntryAtIndex(uint16_t requestedIndex, uint8_t switchId);
static uint8_t findEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
static void    convertLowLevelEntryToAbstractEntry(const SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *p_srcRegisterEntry, SJA1105_addressResolutionTableEntry_t *p_dstTableEntry, uint8_t switchId);
static uint8_t checkRequestedEntryIfEnabled(uint8_t *p_enabled, uint8_t switchId);
static uint8_t removeRequestedEntry(uint8_t switchId);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Insert an entry into the Address Resolution Table.
*
* Adds a new entry into the Address Resolution Table by matching the internally calculated hash
* to an empty/free table entry.
*
* \param[in,out] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored. \p p_addressResolutionTableEntry->index would be assigned
* the physical index that the entry got written into.}
*
* \return uint8_t: {0: config successful, else: failed}
*/
extern uint8_t SJA1105_addArlTableEntry(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t destSwitches;
	uint8_t firstDestSwitchId = SJA1105_N_SWITCHES;
	uint8_t lastDestSwitchId = 0;
	SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t arlSwitchMemoryEntry;
	SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t entryStatusWrite;

	/* Check if any of the indeces calculated through the hash sum of VLAN ID and MAC address,
	   point to an entry that is free to write to. */
	if (getAvailableTableIndex(p_addressResolutionTableEntry->vlanId, p_addressResolutionTableEntry->dstMacAddress, &(p_addressResolutionTableEntry->index)) != 0U)
	{
		/* If not, assign an invalid index and abort the writing operation. */
		p_addressResolutionTableEntry->index = N_L2_ENTRIES;
		/* Entry has not been added, the structure contains data that was not enabled. */
		p_addressResolutionTableEntry->enabled = 0;
		ret += 1U;
	}
	else
	{
		/* Configure status for a write operation. */
		entryStatusWrite.valid = 1;
		entryStatusWrite.rdwrset = 1;
		entryStatusWrite.valident = 1;
		entryStatusWrite.mgmtroute = 0;

		/* Copy the contents into the LLD version of the structure. */
		arlSwitchMemoryEntry.macaddr = p_addressResolutionTableEntry->dstMacAddress;
		arlSwitchMemoryEntry.vlanid = p_addressResolutionTableEntry->vlanId;
		arlSwitchMemoryEntry.enfport = p_addressResolutionTableEntry->enforcePorts;
		arlSwitchMemoryEntry.index = p_addressResolutionTableEntry->index;


		/* Determine physical switch setup */
		SJA1105_getSwitchesFromPorts(p_addressResolutionTableEntry->ports, &destSwitches);
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			if (((destSwitches >> switchId) & 1U) == 1U)
			{  /* switchId is a destination of this MAC-VLAN combination */
				firstDestSwitchId = (firstDestSwitchId == SJA1105_N_SWITCHES) ? switchId : firstDestSwitchId;
				lastDestSwitchId = (switchId > lastDestSwitchId) ? switchId : lastDestSwitchId;
			}
		}
		/* Configure ARL tables */
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			/* configure external ports */
			SJA1105_getPhysicalPortVector(p_addressResolutionTableEntry->ports, switchId, &(arlSwitchMemoryEntry.destports));
			/* configure inter switch forwarding */
			if (switchId < lastDestSwitchId)
			{  /* forward to next switch */
				arlSwitchMemoryEntry.destports |= (uint8_t) (((uint8_t) 1) << SJA1105_gk_generalParameters.cascPort[switchId]);
			}
			if (switchId > firstDestSwitchId)
			{  /* forward to previous switch */
				arlSwitchMemoryEntry.destports |= (uint8_t) (((uint8_t) 1) << SJA1105_gk_generalParameters.hostPort[switchId]);
			}
			/* Request and perform writing of the entry into L2 table. */
			ret += SJA1105_setL2AddressLookupTableReconfigurationEntry(&arlSwitchMemoryEntry, switchId);
			ret += SJA1105_setL2AddressLookupTableReconfigurationControl(&entryStatusWrite, switchId);
			/* Entry has been added, the structure contains valid data. */
		}
		if (ret == 0U)
		{  /* everything went well */
			p_addressResolutionTableEntry->enabled = 1;
		}
	}

	return ret;
}

/**
* \brief Remove the entry from the Address Resolution table using VLAN ID and MAC address. \n
*
* The information concerning the VLAN ID and physical MAC address is stored as parts of a structure that \p p_addressResolutionTableEntry
* points to: \p p_addressResolutionTableEntry->vlanid and \p p_addressResolutionTableEntry->macaddr respectively.
*
* \param[in] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored.}
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105_removeArlTableEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret = 0;

	ret += findEntryByAddress(p_addressResolutionTableEntry);
	
	if (ret == 0U)
	{
		ret += SJA1105_removeArlTableEntryByIndex(p_addressResolutionTableEntry);
	}

	return ret;
}

/**
* \brief Remove the entry from the Address Resolution table using physical index. \n
*
* The information concerning the index of the entry that needs to be removed is stored as part of a structure that \p p_addressResolutionTableEntry
* points to: \p p_addressResolutionTableEntry->index
*
* \param[in] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored.}
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105_removeArlTableEntryByIndex(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret = 0;
	uint8_t switchId;
	
	/* Index of the entry to be removed is passed as part of the SJA1105_addressResolutionTableEntry_t structure.
		Request the data about the entry at this index. */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		ret += requestARTableEntryAtIndex(p_addressResolutionTableEntry->index, switchId);	
		ret += removeRequestedEntry(switchId);
	}
	
	return ret;
}

/**
* \brief Read the entry from the Address Resolution table using physical index. \n
*
* The information concerning the index of the entry that needs to be read is stored as part of a structure that \p p_addressResolutionTableEntry
* points to: \p p_addressResolutionTableEntry->index. All other parameters of the structure are filled as the result of the execution of this function.
* \p p_addressResolutionTableEntry->enabled flag tells whether or not the entry contains valid data. Upon reading an empty entry, all fields, except for
* \p p_addressResolutionTableEntry->index are reset, however it does not generate an error for the cumulative return value.
*
* \param[in,out] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored.}
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105_readArlTableEntryByIndex(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret = 0;
	uint8_t switchId;
	SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t arlSwitchMemoryEntry;
	
	/* Make sure that the requested index does not exceed the size of the ARL table. */
	if (p_addressResolutionTableEntry->index < N_L2_ENTRIES)
	{
		for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
		{
			/* Request to read the entry at an index, provided within the SJA1105_addressResolutionTableEntry_t structure. */
			ret += requestARTableEntryAtIndex(p_addressResolutionTableEntry->index, switchId);
			
			/* Read the contents of entry registers. */
			ret += SJA1105_getL2AddressLookupTableReconfigurationEntry(&arlSwitchMemoryEntry, switchId);
			/* Convert from the low-level structure of the entry to a HAL level structure of the entry. */
			convertLowLevelEntryToAbstractEntry(&arlSwitchMemoryEntry, p_addressResolutionTableEntry, switchId);
			
			ret += checkRequestedEntryIfEnabled(&(p_addressResolutionTableEntry->enabled), switchId);
		}
	}
	else
	{
		ret = 1;
	}
	return ret;
}

/**
* \brief Read the entry from the Address Resolution table using VLAN ID and MAC address. \n
*
* The information concerning the VLAN ID and physical MAC address is stored as parts of a structure that \p p_addressResolutionTableEntry
* points to: \p p_addressResolutionTableEntry->vlanid and \p p_addressResolutionTableEntry->macaddr respectively. All other parameters of the structure are 
* filled as the result of the execution of this function. \p p_addressResolutionTableEntry->enabled flag tells whether or not the entry contains 
* valid data. Upon reading an empty entry, all fields, except for \p p_addressResolutionTableEntry->index are reset.
*
* \param[in,out] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored.}
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105_readArlTableEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret;

	ret = findEntryByAddress(p_addressResolutionTableEntry);
	if (ret == 0U)
	{
		ret = SJA1105_readArlTableEntryByIndex(p_addressResolutionTableEntry);
	}
	
	return ret;
}

/**
* \brief Search for an available table entry that is not occupied by statically or dynamically loaded configuration \n
*
* This function iterates through the columns within switch memory, starting with the index, which the hash sum points to.
* The first entry within the ARL that does not contain data that has been loaded dynamically, or the one that contains an identical
* combination of VLAN ID and MAC address (overwriting an entry), is returned via \p p_index.
* If an entry was only added as a result of autolearning, it may still get overwritten by dynamic reconfiguration.
*
* \param[in] vlaId VLAN ID of the entry
* \param[in] macAddress MAC address of the entry
* \param[out] p_index Memory location of an entry
*
* \return uint8_t: {0: successful, else: failed}
*/
static uint8_t getAvailableTableIndex(uint16_t vlanId, uint64_t macAddress, uint16_t *p_index)
{
	uint8_t i;
	uint8_t ret = 0;
	uint8_t hashSum = getHashSum(vlanId, macAddress);
	uint16_t tempIndex = hashSum * ((uint16_t) ADDR_RESOLUTION_TABLE_COLUMNS); /* Variable for iteration over indeces. */
	SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t entryStatusRead;
	SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t arlSwitchMemoryEntry;
	uint16_t unoccupiedIndex = N_L2_ENTRIES;

	/* Default to invalid entry. */
	*p_index = N_L2_ENTRIES;

	/* Find the first unoccupied table entry that matches the hash
		provided as an argument among 4 possible columns */
	for (i = 0; i < ADDR_RESOLUTION_TABLE_COLUMNS; i++)
	{
		/* The function bypasses the low level drivers to decrease the number of SPI accesses. */
		ret += requestARTableEntryAtIndex(tempIndex, SJA1105_MASTER_SWITCH);
		ret += SJA1105_getL2AddressLookupTableReconfigurationControl(&entryStatusRead, SJA1105_MASTER_SWITCH); /* Read configuration register */
		
		/* Check whether or not the entry is valid and locked */
		if ((entryStatusRead.valident != 0U) && (entryStatusRead.lockeds != 0U))
		{
			ret += SJA1105_getL2AddressLookupTableReconfigurationEntry(&arlSwitchMemoryEntry, SJA1105_MASTER_SWITCH); /* Read uvEntry register */
			/* Check for an attempt to overwrite an existing entry. */
			if ((vlanId == arlSwitchMemoryEntry.vlanid) && (macAddress == arlSwitchMemoryEntry.macaddr))
			{  /* an entry already exists for the given MAC-VLAN pair - overwrite it */
				*p_index = tempIndex;
				break;  /* index found */
			}
			tempIndex++; /* If the entry is different, also valid and locked, check the next index */
		}
		/* An unoccupied index needs to be remembered in case no index matching requested MAC and VLAN is found. */
		else
		{
			if (unoccupiedIndex == N_L2_ENTRIES)  /* Check if one such index has already been found */
			{
				unoccupiedIndex = tempIndex; /* If the variable has not been changed from default, remember the index. */
			}
		}
	}
	
	if (*p_index == N_L2_ENTRIES) /* No entry to overwrite was found. */
	{
		if (unoccupiedIndex != N_L2_ENTRIES)
		{
			*p_index = unoccupiedIndex; /* An unoccupied index was found. */
		}
		else
		{
			ret += 1U; /* No available index was found. */
		}
	}

	return ret;
}

/**
* \brief Request information about an entry at a certain index to be available at the next read operation \n
*
* The function sends a request to the switch to place an entry at index \p requestedIndex into the registers that provide
* the entry contents. An access to fetch the ARL entry immediately after a call to this function would result in obtaining
* the contents of an entry at index \p requestedIndex.
*
* \param[in] requestedIndex Index of the entry that needs to be requested to become available for reading
* \param[in] switchId SwitchId where the index is requested
*
* \return uint8_t
*/
static uint8_t requestARTableEntryAtIndex(uint16_t requestedIndex, uint8_t switchId)
{
	uint8_t ret = 0;
	SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t entryStatusRead;
	SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t entryStatusWrite;
	SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t arlSwitchMemoryEntry;
	
	/* Check that the index belongs to the number of possible indeces. */
	if (requestedIndex < N_L2_ENTRIES)
	{
		/* Supply the requested index via the entry structure, all other parameters do not carry useful data. */
		arlSwitchMemoryEntry.index = requestedIndex; /*Read index*/
		arlSwitchMemoryEntry.destports = 0;
		arlSwitchMemoryEntry.enfport   = 0;            
		arlSwitchMemoryEntry.macaddr   = 0;
		arlSwitchMemoryEntry.vlanid    = 0;

		/* Write bValid flag to access and bWrRdB to read */
		entryStatusWrite.valid = 1;
		entryStatusWrite.rdwrset = 0;
		entryStatusWrite.mgmtroute = 0;
		entryStatusWrite.valident = 1;

		/* Wait for the VALID flag to be cleared to allow write access */
		do
		{
			ret += SJA1105_getL2AddressLookupTableReconfigurationControl(&entryStatusRead, switchId);
		}
		while (entryStatusRead.valid == 1U);

		/* Write the index value into the uvEntry field */
		ret += SJA1105_setL2AddressLookupTableReconfigurationEntry(&arlSwitchMemoryEntry, switchId);
		/* Request a write operation. */
		ret += SJA1105_setL2AddressLookupTableReconfigurationControl(&entryStatusWrite, switchId);
	}
	else
	{
		ret = 1U;
	}
	return ret;
}

/**
* \brief Find the switch index of an entry knowing its VLAN ID and MAC address. \n
*
* The information concerning the VLAN ID and physical MAC address is stored as parts of a structure that \p p_addressResolutionTableEntry
* points to: \p p_addressResolutionTableEntry->vlanid and \p p_addressResolutionTableEntry->macaddr respectively. All other parameters of the structure are 
* filled as the result of the execution of this function. The function iterates through the possible entries (indeces) that the hash sum of VLAN ID and MAC
* address pair points to until it finds a match. If a match is not found after going through available columns, the index of \p p_addressResolutionTableEntry
* is assigned an invalid value and the function returns a non-zero value.
*
* \param[in,out] p_addressResolutionTableEntry {Memory location where the ARL entry data is stored.}
*
* \return uint8_t Returns 0 when successful, else failed or entry not found
*/
static uint8_t findEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry)
{
	uint8_t ret = 0U;
	/* Variable for iterating over the entries within a column. Starting point is the hash sum of
	VLAN ID and MAC address. */
	uint16_t tempIndex = ADDR_RESOLUTION_TABLE_COLUMNS * ((uint16_t) getHashSum(p_addressResolutionTableEntry->vlanId, p_addressResolutionTableEntry->dstMacAddress));
	uint8_t ctr;
	SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t arlSwitchMemoryEntry;

	p_addressResolutionTableEntry->index =N_L2_ENTRIES;  /* Initialize with invalid index */

	for (ctr = 0; ctr < ADDR_RESOLUTION_TABLE_COLUMNS; ctr++)
	{
		/* Request an entry with index of the current iteration. */
		ret += requestARTableEntryAtIndex(tempIndex, SJA1105_MASTER_SWITCH);
		/* Read the entry. */
		ret += SJA1105_getL2AddressLookupTableReconfigurationEntry(&arlSwitchMemoryEntry, SJA1105_MASTER_SWITCH);
		/* Check that the MAC and VLAN ID of the read entry match the requested values, and no error ocurred during the previous accesses. */
		if ((arlSwitchMemoryEntry.macaddr == p_addressResolutionTableEntry->dstMacAddress) && (arlSwitchMemoryEntry.vlanid == p_addressResolutionTableEntry->vlanId) && (ret == 0U))
		{
			p_addressResolutionTableEntry->enforcePorts = arlSwitchMemoryEntry.enfport;
			p_addressResolutionTableEntry->ports = arlSwitchMemoryEntry.destports;
			p_addressResolutionTableEntry->index = tempIndex;
			break;  /* Entry has been found. */
		}
		else 
		{
			/* Entry has not been found, repeat the search with an incremented index (next column in memory). */
			tempIndex++;
		}
	}
	if (p_addressResolutionTableEntry->index == N_L2_ENTRIES)
	{  /* Entry not found after iterating through all available columns. */
		ret = 1;
	}
	return ret;
}

/**
* \brief Calculate the hash sum based on the chosen CRC algorithm (here: CRC-8, Baicheva polynomial) \n
*
* \param[in] vlanId VLAN ID of the entry
* \param[in] macAddress Physical (MAC) address of the entry
*
* \return uint8_t: hash sum of the provided VLAN ID and MAC address
*/
static uint8_t getHashSum(uint16_t vlanId, uint64_t macAddress)
{
	uint8_t i;
	uint8_t dataToEncode[N_BYTES_TO_BE_HASHED];
	uint8_t hashFunction;
	
	/* Arrange the combination of VLAN ID and MAC ADDRESS in appropriate order:
		[VLAN_ID_MSbyte <-2 bytes->VLAN_ID_LSbyte;MAC_MSbyte<-6 bytes->MAC_LSbyte] for the total of 8 bytes. */
	for (i = 0; i < MAC_ADDRESS_BYTES; i++)
	{  /* MAC Address */
		dataToEncode[MAC_ADDRESS_TOP - i] = (uint8_t) (macAddress >> (i * BYTE));
	}
	for (i = 0; i < 2U; i++)
	{  /* VLAN ID */
		dataToEncode[MAC_ADDRESS_TOP - MAC_ADDRESS_BYTES - i] = (uint8_t) (vlanId >> (i * BYTE));
	}

	/* Initialize and run the CRC (hash sum) calculation. */
	hashFunction = SJA1105_calculateHash(dataToEncode, N_BYTES_TO_BE_HASHED);

	return hashFunction;
}

/**
* \brief Convert a LLD ARL table entry to an abstract ARL entry.
*
* The function takes a LLD structure that represents an ARL table entry as source and copies its contents into a memory location of an
* abstract ARL table entry structure that \p p_dstTableEntry points to.
*
* \param[in]  p_srcRegisterEntry Memory location where the LLD ARL table entry is stored
* \param[out] p_dstTableEntry Memory location where the abstract ARL table entry is stored
* \param[in]  switchId SwitchId of the switch from which the entry should be converted
*
*/
static void convertLowLevelEntryToAbstractEntry(const SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *p_srcRegisterEntry, SJA1105_addressResolutionTableEntry_t *p_dstTableEntry, uint8_t switchId)
{
	uint16_t logicalDestPorts;
	uint16_t logicalDestPortsMask;

	if (switchId == SJA1105_MASTER_SWITCH)
	{
		p_dstTableEntry->enforcePorts  = p_srcRegisterEntry->enfport;
		p_dstTableEntry->dstMacAddress = p_srcRegisterEntry->macaddr;
		p_dstTableEntry->vlanId        = p_srcRegisterEntry->vlanid;
		p_dstTableEntry->index         = p_srcRegisterEntry->index;
		p_dstTableEntry->ports         = 0;  /* Will be filled in later */
	}
	SJA1105_getPortVectorFromPhysicalPortVector(~((uint8_t) 0), switchId, &logicalDestPortsMask);
	SJA1105_getPortVectorFromPhysicalPortVector(p_srcRegisterEntry->destports, switchId, &logicalDestPorts);
	p_dstTableEntry->ports |= logicalDestPorts & logicalDestPortsMask;
}

/**
* \brief Check if the current contents of the ARL entry registers contain a valid/enabled entry. \n
*
* For the entry currently residing in entry data registers, the function reads out the status register contents for the corresponding entry registers to
* determine if the entry is valid/enabled or not. based on that, the \p p_enabled flag would be set or cleared to
* match the contents of VALIDENT field of the status register. (Intended use here: following a call to requestARTableEntryAtIndex() or findEntryByAddress() 
* check the contents of the status register that matches the requested/found entry.)
*
* \param[out] p_enabled {Memory location where the enabled flag is stored.}
* \param[in]  switchId Switch Id where the Entry is checked
*
* \return uint8_t Returns 0 when successful, else failed
*/
static uint8_t checkRequestedEntryIfEnabled(uint8_t *p_enabled, uint8_t switchId)
{
	uint8_t ret = 0U;
	SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t entryStatusRead;
	
	ret += SJA1105_getL2AddressLookupTableReconfigurationControl(&entryStatusRead, switchId);
	*p_enabled = entryStatusRead.valident;	
	return ret;
}

/**
* \brief Remove the entry that is currently stored in entry data registers of the switch. \n
*
* For the entry currently residing in entry data registers, the function reads out the status register to determine if the entry can be removed from the ARL
* table. The decision is based on the contents of VALIDENT flag. An attempt to remove an invalid entry results in an error. Additionally, the function checks if 
* clearing the VALIDENT of the corresponding entry has been successful. The function also sets the \p p_enabled flag according to the
* execution result: successfully removing an entry clears it, failure to remove results in a set flag.(Intended use here: following a call to 
* requestARTableEntryAtIndex() or findEntryByAddress() check and manipulate the contents of the status register that matches the requested/found entry.)
*
* \param[in] switchId Switch ID where the entry should be removed
*
* \return uint8_t Returns 0 when successful, else failed 
*/
static uint8_t removeRequestedEntry(uint8_t switchId)
{
	uint8_t ret = 0U;
	SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t entryStatusWrite;
	SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t entryStatusRead;
	
	ret += SJA1105_getL2AddressLookupTableReconfigurationControl(&entryStatusRead, switchId);
	if ((ret == 0U) && (entryStatusRead.valident == 1U))
	{
		/* Configure a write operation with VALIDENT flag cleared to render the entry invalid. */
		entryStatusWrite.mgmtroute = 0;
		entryStatusWrite.rdwrset = 1;
		entryStatusWrite.valid = 1;
		entryStatusWrite.valident = 0;
		ret = SJA1105_setL2AddressLookupTableReconfigurationControl(&entryStatusWrite, switchId);
		
		/* Obtain the control data associated with the entry. */
		ret += SJA1105_getL2AddressLookupTableReconfigurationControl(&entryStatusRead, switchId);
		
		/* Check if the VALIDENT flag has been successfully cleared. */
		if (entryStatusRead.valident == 1U)
		{
			/* If it remained to be set, operation has failed. */
			ret = 1;
		}
	}
	else
	{
		/* Attempt to remove an entry that was not valid. */
		ret = 1U;
	}
	return ret;
}
