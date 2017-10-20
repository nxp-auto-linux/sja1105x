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
* \file NXP_SJA1105P_cbs.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
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
