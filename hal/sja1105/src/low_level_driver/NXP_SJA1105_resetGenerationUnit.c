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
* \file NXP_SJA1105_resetGenerationUnit.c
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

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_resetGenerationUnit.h"
#include "NXP_SJA1105_spi.h"

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to SET data of the reset_ctrl
*
* \param[in]  rstCtrl Reset trigger: {04: trigger a cold reset, 08: trigger a warm reset}
* \return uint8_t
*/
uint8_t SJA1105_setResetCtrl(uint16_t rstCtrl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((rstCtrl <= 0x1FFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) SJA1105_R_CTRL_RST_CTRL_MASK) & rstCtrl;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_R_CTRL_ADDR, &cData);  /* write data via SPI */

	return ret;
}
