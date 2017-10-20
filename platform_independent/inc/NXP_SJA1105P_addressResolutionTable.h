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
* \file NXP_SJA1105P_addressResolutionTable.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Functions for the address resolution lookup
*
*****************************************************************************/

#ifndef NXP_SJA1105P_ADDRESSRESOLUTIONTABLE_H
#define NXP_SJA1105P_ADDRESSRESOLUTIONTABLE_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
    uint64_t  dstMacAddressMask;  /**< Mask for the destination MAC address used in the address lookup process */
    uint16_t  vlanIdMask;         /**< Mask for the destination MAC address used in the address lookup process */
    uint8_t   innerOuterVlan;     /**< Specifies if the VLAN ID used in the search/learn key is a inner VLAN (0) or an outer VLAN (1). */
    uint8_t   mirroring;          /**< Specifies if traffic matching this entry should be mirrored */
    uint8_t   retagging;          /**< Specifies if traffic matching this entry should be retagged */
    uint16_t  retaggingVlanId;    /**< Specifies the VLAN ID to be used in the retagging process */
} SJA1105P_extendedAddressResolutionTableEntry_t;

typedef struct
{
    uint16_t ports;          /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
    uint8_t  enforcePorts;   /**< If this flag is set, MACADDR will be enforced as the source MAC address on ports having their flag set in DESTPORTS */
    uint64_t dstMacAddress;  /**< The MAC address associated with this entry */
    uint16_t vlanId;         /**< The VLAN ID associated with this entry. */
    uint16_t index;          /**< Index of the entry within th address resolution table. It is assigned when adding an entry and can be referred to afterwards for incremental changes to the entry. */
    uint8_t  enabled;        /**< A flag indicating whether or not the entry stores valid data (initialized entry that is also valid: 1U, otherwise: 0U). */
    SJA1105P_extendedAddressResolutionTableEntry_t *p_extension;  /**< Pointer to extension of the entry. If pointer towards NULL, default values are used */
} SJA1105P_addressResolutionTableEntry_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_addArlTableEntry(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry);

extern uint8_t SJA1105P_readArlTableEntryByAddress(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
extern uint8_t SJA1105P_readArlTableEntryByIndex(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry);

extern uint8_t SJA1105P_removeArlTableEntryByAddress(SJA1105P_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
extern uint8_t SJA1105P_removeArlTableEntryByIndex(const SJA1105P_addressResolutionTableEntry_t *kp_addressResolutionTableEntry);

extern uint8_t SJA1105P_enableArlMirroring(uint16_t arlEntryIndex, uint8_t enable);
extern uint8_t SJA1105P_enableArlRetagging(uint16_t arlEntryIndex, uint16_t retaggingVlanId, uint8_t enable);

#endif /* NXP_SJA1105P_ADDRESSRESOLUTIONTABLE_H */
