/******************************************************************************
* (c) NXP B.V. 2016 - 2017. All rights reserved.
*
* Disclaimer
* 1. The NXP Software/Source Code is provided to Licensee "AS IS" without any
* warranties of any kind. NXP makes no warranties to Licensee and shall not
* indemnify Licensee or hold it harmless for any reason related to the NXP
* Software/Source Code or otherwise be liable to the NXP customer. The NXP
* customer acknowledges and agrees that the NXP Software/Source Code is
* provided AS-IS and accepts all risks of utilizing the NXP Software under the
* conditions set forth according to this disclaimer.
*
* 2. NXP EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING, BUT
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS. NXP
* SHALL HAVE NO LIABILITY TO THE NXP CUSTOMER, OR ITS SUBSIDIARIES, AFFILIATES,
* OR ANY OTHER THIRD PARTY FOR ANY DAMAGES, INCLUDING WITHOUT LIMITATION,
* DAMAGES RESULTING OR ALLEGDED TO HAVE RESULTED FROM ANY DEFECT, ERROR OR
* OMMISSION IN THE NXP SOFTWARE/SOURCE CODE, THIRD PARTY APPLICATION SOFTWARE
* AND/OR DOCUMENTATION, OR AS A RESULT OF ANY INFRINGEMENT OF ANY INTELLECTUAL
* PROPERTY RIGHT OF ANY THIRD PARTY. IN NO EVENT SHALL NXP BE LIABLE FOR ANY
* INCIDENTAL, INDIRECT, SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL DAMAGES
* (INCLUDING LOST PROFITS) SUFFERED BY NXP CUSTOMER OR ITS SUBSIDIARIES,
* AFFILIATES, OR ANY OTHER THIRD PARTY ARISING OUT OF OR RELATED TO THE NXP
* SOFTWARE/SOURCE CODE EVEN IF NXP HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGES.
*
* 3. NXP reserves the right to make changes to the NXP Software/Sourcecode any
* time, also without informing customer.
*
* 4. Licensee agrees to indemnify and hold harmless NXP and its affiliated
* companies from and against any claims, suits, losses, damages, liabilities,
* costs and expenses (including reasonable attorney's fees) resulting from
* Licensee's and/or Licensee customer's/licensee's use of the NXP
* Software/Source Code.
*****************************************************************************/

/**
*
* \file NXP_SJA1105P_addressResolutionTable.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
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
