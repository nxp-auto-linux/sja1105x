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
* \file NXP_SJA1105P_cbs.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
*
* \brief Configuration of the Credit Based Shaper (CBS) blocks
*
*****************************************************************************/

#ifndef NXP_SJA1105P_CBS_H
#define NXP_SJA1105P_CBS_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_config.h"

/******************************************************************************
* Defines
*****************************************************************************/
#define SJA1105P_INVALID_SHAPER_ID ((uint8_t) (SJA1105P_N_SHAPER * SJA1105P_N_SWITCHES))

/* AVB class measurement intervals in us */
#define SJA1105P_CLASS_A_MEASUREMENT_INTERVAL 125
#define SJA1105P_CLASS_B_MEASUREMENT_INTERVAL 250
#define SJA1105P_CLASS_C_MEASUREMENT_INTERVAL 1333
#define SJA1105P_CLASS_D_MEASUREMENT_INTERVAL 1451

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
	uint32_t idleSlope;  /**< (B/s) Idle slope in bytes per second */
	uint32_t sendSlope;  /**< (B/s) Send slope in bytes per second */
	uint32_t creditHi;   /**< (B) Credit High */
	uint32_t creditLo;   /**< (B) Credit Low */
} SJA1105P_creditBasedShaperParameters_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_registerStreamToCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio);
extern uint8_t SJA1105P_deregisterStreamFromCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio);
extern uint8_t SJA1105P_configCbs(const SJA1105P_creditBasedShaperParameters_t *kp_cbsParameters, uint8_t port, uint8_t vlanPrio);
extern uint8_t SJA1105P_removeCbs(uint8_t shaperId);

#endif /* NXP_SJA1105P_CBS_H */
