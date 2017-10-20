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
* \file NXP_SJA1105P_vlan.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Configuration of VLANs
*
*****************************************************************************/

#ifndef NXP_SJA1105P_VLAN_H
#define NXP_SJA1105P_VLAN_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_vlanForwarding_NOT      = 2U,  /**< Frames are not forwarded */
	SJA1105P_e_vlanForwarding_UNTAGGED = 1U,  /**< Frames are forwarded untagged */
	SJA1105P_e_vlanForwarding_TAGGED   = 0U   /**< Frames are forwarded tagged */
} SJA1105P_vlanForwarding_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_writeVlanConfig(uint16_t vlanId, const SJA1105P_vlanForwarding_t *pk_vlanForwarding, uint16_t enable);
extern uint8_t SJA1105P_readVlanConfig(uint16_t vlanId, SJA1105P_vlanForwarding_t *p_vlanForwarding, uint16_t *p_enable);
extern uint8_t SJA1105P_enableVlan(uint16_t vlanId, uint8_t ingressPort, uint8_t enable);

extern uint8_t SJA1105P_enableVlanIngressMirroring(uint16_t vlanId, uint8_t ingressPort, uint8_t enable);
extern uint8_t SJA1105P_enableVlanEgressMirroring(uint16_t vlanId, uint8_t egressPort, uint8_t enable);

extern uint8_t SJA1105P_startRetagging(uint16_t ingressPorts, uint16_t egressPorts, uint16_t ingressVlanId, uint16_t egressVlanId);
extern uint8_t SJA1105P_stopRetagging(uint16_t egressVlanId);

extern uint8_t SJA1105P_enableBypassing(uint16_t vlanId, uint16_t destPorts, uint8_t learningDisabled);
extern uint8_t SJA1105P_disableBypassing(uint16_t vlanId);

#endif /* NXP_SJA1105P_VLAN_H */
