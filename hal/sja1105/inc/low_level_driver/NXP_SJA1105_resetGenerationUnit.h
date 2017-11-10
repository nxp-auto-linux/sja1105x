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
* \file NXP_SJA1105_resetGenerationUnit.h
*
* \author NXP Semiconductors
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief Provides reset sources across the device and can be used to trigger a software cold or hard reset via the SPI bus
*
*****************************************************************************/

#ifndef NXP_SJA1105_RESETGENERATIONUNIT_H
#define NXP_SJA1105_RESETGENERATIONUNIT_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category reset_ctrl */
#define SJA1105_R_CTRL_ADDR (0x100440)  /**< Address of the resetCtrl register */

/* variable masks */

/* register category reset_ctrl */
#define SJA1105_R_CTRL_RST_CTRL_MASK (0x000001FFU)  /**< Bit mask for the RST_CTRL field */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category reset_ctrl */
extern uint8_t SJA1105_setResetCtrl(uint16_t rstCtrl, uint8_t deviceSelect);

#endif  /* NXP_SJA1105_RESETGENERATIONUNIT_H */
