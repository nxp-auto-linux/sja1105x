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
* \file NXP_SJA1105P_portConfig.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
*
* \brief Configure speed and mode of a port
*
*****************************************************************************/

#ifndef NXP_SJA1105P_PORT_CONFIG_H
#define NXP_SJA1105P_PORT_CONFIG_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_sgmii.h"

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_autoConfigPorts(void);
extern uint8_t SJA1105P_setupClockDelay(uint16_t delay, uint8_t port, uint8_t switchId, SJA1105P_direction_t direction);
extern uint8_t initSgmii(uint8_t switchId, uint8_t autoNegotiation, SJA1105P_speed_t speed, uint8_t phyMode);
extern uint8_t SJA1105P_autoConfigPorts_extended(uint8_t switchId);
extern uint8_t SJA1105P_synchSwitchConfiguration_extended(uint8_t switchId);
#endif /* NXP_SJA1105P_PORT_CONFIG_H */
