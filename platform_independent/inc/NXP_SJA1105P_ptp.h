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
* \file NXP_SJA1105P_ptp.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Functions for the Precision Time Protocol (PTP)
*
*****************************************************************************/

#ifndef NXP_SJA1105P_PTP_H
#define NXP_SJA1105P_PTP_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* Defines
*****************************************************************************/

#define SJA1105P_INITIAL_CLK_RATIO 0x80000000U                   /* No rate ratio correction */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_initPtp(void);

extern uint8_t SJA1105P_getPtpClk(uint64_t *p_clkVal);
extern uint8_t SJA1105P_setPtpClk(uint64_t clkVal);

extern uint8_t SJA1105P_setPtpClkRatio(uint32_t clkRatio);
extern uint8_t SJA1105P_getPtpClkRatio(uint32_t *p_clkRatio);

extern uint8_t SJA1105P_addOffsetToPtpClk(uint64_t clkAddVal);
extern uint8_t SJA1105P_subtractOffsetFromPtpClk(uint64_t clkSubVal);

extern uint8_t SJA1105P_syncCascadedClocks(void);

#endif /* NXP_SJA1105P_PTP_H */
