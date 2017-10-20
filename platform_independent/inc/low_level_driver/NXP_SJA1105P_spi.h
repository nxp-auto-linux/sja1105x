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
* \file NXP_SJA1105P_spi.h
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief Register read and write functions using SPI
*
*****************************************************************************/

#ifndef NXP_SJA1105P_SPI_H
#define NXP_SJA1105P_SPI_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef uint8_t (*SJA1105P_spiRead32_cb_t)(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);  /**< Type of the function used to perform read operations via SPI */
typedef uint8_t (*SJA1105P_spiWrite32_cb_t)(uint8_t deviceSelect, uint8_t wordCount, uint32_t registerAddress, uint32_t *p_registerValue);  /**< Type of the function used to perform write operations via SPI */

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

extern SJA1105P_spiRead32_cb_t SJA1105P_gpf_spiRead32;  /**< Callback function used to perform read operations via SPI */
extern SJA1105P_spiWrite32_cb_t SJA1105P_gpf_spiWrite32;  /**< Callback function used to perform write operations via SPI */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern void SJA1105P_registerSpiRead32CB(SJA1105P_spiRead32_cb_t pf_spiRead32);  /**< Function used to register the callback function to perform read operations via SPI */
extern void SJA1105P_registerSpiWrite32CB(SJA1105P_spiWrite32_cb_t pf_spiWrite32);  /**< Function used to register the callback function to perform write operations via SPI */

#endif  /* NXP_SJA1105P_SPI_H */
