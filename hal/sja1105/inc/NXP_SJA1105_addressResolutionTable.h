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
* \file NXP_SJA1105_addressResolutionTable.h
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Functions for the L2 Lookup Tables
*
*****************************************************************************/

#ifndef NXP_SJA1105_ADDRESSRESOLUTIONTABLE_H
#define NXP_SJA1105_ADDRESSRESOLUTIONTABLE_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
    uint16_t ports;          /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
    uint8_t  enforcePorts;   /**< If this flag is set, MACADDR will be enforced as the source MAC address on ports having their flag set in DESTPORTS */
    uint64_t dstMacAddress;  /**< The MAC address associated with this entry */
    uint16_t vlanId;         /**< The VLAN ID associated with this entry. */
    uint16_t index;          /**< Index of the entry within th address resolution table */
    uint8_t  enabled;        /**< A flag indicating whether or not the entry stores valid data (initialized entry that is also valid: 1U, otherwise: 0U). */
} SJA1105_addressResolutionTableEntry_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105_addArlTableEntry(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);

extern uint8_t SJA1105_readArlTableEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
extern uint8_t SJA1105_readArlTableEntryByIndex(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);

extern uint8_t SJA1105_removeArlTableEntryByAddress(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);
extern uint8_t SJA1105_removeArlTableEntryByIndex(SJA1105_addressResolutionTableEntry_t *p_addressResolutionTableEntry);

#endif /* NXP_SJA1105_ADDRESSRESOLUTIONTABLE_H */
