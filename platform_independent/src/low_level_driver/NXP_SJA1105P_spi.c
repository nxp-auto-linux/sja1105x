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
* \file NXP_SJA1105P_spi.c
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief Definition of SPI read and write functions
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

SJA1105P_spiRead32_cb_t SJA1105P_gpf_spiRead32 = NULL;
SJA1105P_spiWrite32_cb_t SJA1105P_gpf_spiWrite32 = NULL;

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

extern void SJA1105P_registerSpiRead32CB(SJA1105P_spiRead32_cb_t pf_spiRead32)
{
	SJA1105P_gpf_spiRead32 = pf_spiRead32;
}
extern void SJA1105P_registerSpiWrite32CB(SJA1105P_spiWrite32_cb_t pf_spiWrite32)
{
	SJA1105P_gpf_spiWrite32 = pf_spiWrite32;
}
