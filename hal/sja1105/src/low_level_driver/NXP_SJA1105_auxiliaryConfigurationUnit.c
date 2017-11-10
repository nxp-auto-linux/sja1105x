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
* \file NXP_SJA1105_auxiliaryConfigurationUnit.c
*
* \author NXP Semiconductors
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief The auxiliary configuration unit controls the I/O characteristics and provides auxiliary functionality.
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105_spi.h"

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to SET data of the cfg_pad_miix
*
* \param[in]  cfgPadMiix Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  direction Direction (Transmit/Receive), possible values: ::SJA1105_direction_t
* \return uint8_t
*/
uint8_t SJA1105_setCfgPadMiix(const SJA1105_cfgPadMiixArgument_t *pk_cfgPadMiix, uint8_t port, SJA1105_direction_t direction, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_cfgPadMiix->d32Os) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->d32Ih) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->d32Ipud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->d10Os) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->d10Ih) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->d10Ipud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->ctrlOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->ctrlIh) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->ctrlIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->clkOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->clkIh) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMiix->clkIpud) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Os)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D32_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D32_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Ih)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D32_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D32_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Ipud)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D32_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D32_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Os)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D10_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D10_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Ih)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D10_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D10_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Ipud)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_D10_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_D10_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlOs)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_CTRL_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CTRL_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlIh)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_CTRL_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CTRL_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlIpud)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_CTRL_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CTRL_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->clkOs)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_CLK_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CLK_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->clkIh)) << ((uint8_t) SJA1105_CFG_PAD_MIIX_CLK_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CLK_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MIIX_CLK_IPUD_MASK) & ((uint8_t) pk_cfgPadMiix->clkIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_CFG_PAD_MIIX_ADDR + ((uint32_t) SJA1105_CFG_PAD_MIIX_PORT_INC * port) + ((uint32_t) direction)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_miix
*
* \param[out] p_cfgPadMiix Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  direction Direction (Transmit/Receive), possible values: ::SJA1105_direction_t
* \return uint8_t
*/
uint8_t SJA1105_getCfgPadMiix(SJA1105_cfgPadMiixArgument_t *p_cfgPadMiix, uint8_t port, SJA1105_direction_t direction, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_CFG_PAD_MIIX_ADDR + ((uint32_t) SJA1105_CFG_PAD_MIIX_PORT_INC * port) + ((uint32_t) direction)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Os = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Ih = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D32_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Ipud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Os = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Ih = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_D10_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Ipud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlOs = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_CTRL_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CLK_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_CLK_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->clkOs = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CLK_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MIIX_CLK_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->clkIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MIIX_CLK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadMiix->clkIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_misc
*
* \param[in]  cfgPadMisc Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadMiscArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setCfgPadMisc(const SJA1105_cfgPadMiscArgument_t *pk_cfgPadMisc, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_cfgPadMisc->ptpclkEn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->ptpclkOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->ptpclkIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->rstnIh) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->rstnIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMisc->clkoutEn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->clkoutOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->clkoutIpud) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_cfgPadMisc->ptpclkEn) << ((uint8_t) SJA1105_CFG_PAD_MISC_PTPCLK_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_PTPCLK_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->ptpclkOs)) << ((uint8_t) SJA1105_CFG_PAD_MISC_PTPCLK_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_PTPCLK_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->ptpclkIpud)) << ((uint8_t) SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->rstnIh)) << ((uint8_t) SJA1105_CFG_PAD_MISC_RSTN_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_RSTN_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->rstnIpud)) << ((uint8_t) SJA1105_CFG_PAD_MISC_RSTN_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_RSTN_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_cfgPadMisc->clkoutEn) << ((uint8_t) SJA1105_CFG_PAD_MISC_CLKOUT_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_CLKOUT_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->clkoutOs)) << ((uint8_t) SJA1105_CFG_PAD_MISC_CLKOUT_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_CLKOUT_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_CFG_PAD_MISC_CLKOUT_IPUD_MASK) & ((uint8_t) pk_cfgPadMisc->clkoutIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_MISC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_misc
*
* \param[out] p_cfgPadMisc Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadMiscArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getCfgPadMisc(SJA1105_cfgPadMiscArgument_t *p_cfgPadMisc, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_MISC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_EN_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkOs = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_RSTN_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_RSTN_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->rstnIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_RSTN_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_RSTN_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->rstnIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_CLKOUT_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_CLKOUT_EN_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->clkoutEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_CLKOUT_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_MISC_CLKOUT_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->clkoutOs = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_MISC_CLKOUT_IPUD_MASK);   /* mask desired bits */
	p_cfgPadMisc->clkoutIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_spi
*
* \param[in]  cfgPadSpi Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadSpiArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setCfgPadSpi(const SJA1105_cfgPadSpiArgument_t *pk_cfgPadSpi, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_cfgPadSpi->sdoOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->sdoIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->sdiIh) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->sdiIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->ssnIh) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->ssnIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->sckIh) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadSpi->sckIpud) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdoOs)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SDO_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SDO_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdoIpud)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SDO_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SDO_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdiIh)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SDI_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SDI_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdiIpud)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SDI_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SDI_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->ssnIh)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SSN_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SSN_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->ssnIpud)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SSN_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SSN_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sckIh)) << ((uint8_t) SJA1105_CFG_PAD_SPI_SCK_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SCK_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_CFG_PAD_SPI_SCK_IPUD_MASK) & ((uint8_t) pk_cfgPadSpi->sckIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_SPI_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_spi
*
* \param[out] p_cfgPadSpi Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadSpiArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getCfgPadSpi(SJA1105_cfgPadSpiArgument_t *p_cfgPadSpi, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_SPI_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SDO_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SDO_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdoOs = SJA1105_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SDO_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SDO_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdoIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SDI_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SDI_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdiIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SDI_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SDI_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdiIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SSN_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SSN_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->ssnIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SSN_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SSN_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->ssnIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SCK_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_SPI_SCK_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sckIh = SJA1105_convertToPadInputStageHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_SPI_SCK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadSpi->sckIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_jtag
*
* \param[in]  cfgPadJtag Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadJtagArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setCfgPadJtag(const SJA1105_cfgPadJtagArgument_t *pk_cfgPadJtag, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_cfgPadJtag->tdoIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadJtag->tdiIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadJtag->trstntmsIpud) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadJtag->tckIpud) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->tdoIpud)) << ((uint8_t) SJA1105_CFG_PAD_JTAG_TDO_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_JTAG_TDO_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->tdiIpud)) << ((uint8_t) SJA1105_CFG_PAD_JTAG_TDI_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_JTAG_TDI_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->trstntmsIpud)) << ((uint8_t) SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_CFG_PAD_JTAG_TCK_IPUD_MASK) & ((uint8_t) pk_cfgPadJtag->tckIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_JTAG_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_jtag
*
* \param[out] p_cfgPadJtag Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_cfgPadJtagArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getCfgPadJtag(SJA1105_cfgPadJtagArgument_t *p_cfgPadJtag, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_CFG_PAD_JTAG_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_JTAG_TDO_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_JTAG_TDO_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->tdoIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_JTAG_TDI_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_JTAG_TDI_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->tdiIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->trstntmsIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_CFG_PAD_JTAG_TCK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadJtag->tckIpud = SJA1105_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the port_status_miix
*
* \param[out] p_portStatusMiix Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_portStatusMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getPortStatusMiix(SJA1105_portStatusMiixArgument_t *p_portStatusMiix, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_PORT_S_MIIX_ADDR + ((uint32_t) port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PORT_S_MIIX_SPEED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PORT_S_MIIX_SPEED_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->speed = SJA1105_convertToSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PORT_S_MIIX_PHY_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PORT_S_MIIX_PHY_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->phyMode = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PORT_S_MIIX_XMII_MODE_MASK);   /* mask desired bits */
	p_portStatusMiix->xmiiMode = SJA1105_convertToXmiiMode(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ts_config
*
* \param[in]  tsConfig Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_tsConfigArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setTsConfig(const SJA1105_tsConfigArgument_t *pk_tsConfig, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_tsConfig->pd <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_tsConfig->threshold) <= 0x3FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_tsConfig->pd) << ((uint8_t) SJA1105_TS_C_PD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_TS_C_PD_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) SJA1105_TS_C_THRESHOLD_MASK) & ((uint8_t) pk_tsConfig->threshold);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_TS_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ts_config
*
* \param[out] p_tsConfig Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_tsConfigArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getTsConfig(SJA1105_tsConfigArgument_t *p_tsConfig, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_TS_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_TS_C_PD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_TS_C_PD_SHIFT);  /* shift to right aligned position */
	p_tsConfig->pd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_TS_C_THRESHOLD_MASK);   /* mask desired bits */
	p_tsConfig->threshold = SJA1105_convertToTempThreshold(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the ts_status
*
* \param[out] p_exceeded temperature detection status {0: active, 1: power-down}
* \return uint8_t
*/
uint8_t SJA1105_getTsStatus(uint8_t *p_exceeded, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_TS_S_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_TS_S_EXCEEDED_MASK);   /* mask desired bits */
	*p_exceeded = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the rgmii_meas_setup
*
* \param[in]  rgmiiMeasSetup Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_rgmiiMeasSetupArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setRgmiiMeasSetup(const SJA1105_rgmiiMeasSetupArgument_t *pk_rgmiiMeasSetup, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_rgmiiMeasSetup->mii4RxSmplEn) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_rgmiiMeasSetup->mii3RxSmplEn) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_rgmiiMeasSetup->mii2RxSmplEn) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_rgmiiMeasSetup->mii1RxSmplEn) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_rgmiiMeasSetup->mii0RxSmplEn) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii4RxSmplEn)) << ((uint8_t) SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii3RxSmplEn)) << ((uint8_t) SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii2RxSmplEn)) << ((uint8_t) SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii1RxSmplEn)) << ((uint8_t) SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_R_MEAS_S_MII0_RX_SMPL_EN_MASK) & ((uint8_t) pk_rgmiiMeasSetup->mii0RxSmplEn);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_R_MEAS_S_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the rgmii_meas_setup
*
* \param[out] p_rgmiiMeasSetup Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_rgmiiMeasSetupArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getRgmiiMeasSetup(SJA1105_rgmiiMeasSetupArgument_t *p_rgmiiMeasSetup, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_R_MEAS_S_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii4RxSmplEn = SJA1105_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii3RxSmplEn = SJA1105_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii2RxSmplEn = SJA1105_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii1RxSmplEn = SJA1105_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_R_MEAS_S_MII0_RX_SMPL_EN_MASK);   /* mask desired bits */
	p_rgmiiMeasSetup->mii0RxSmplEn = SJA1105_convertToRgmiiSmpl(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the prod_cfg
*
* \param[out] p_disableTteth TTEthernet features of switch: {0: disabled, 1: enabled}
* \return uint8_t
*/
uint8_t SJA1105_getProdCfg(uint8_t *p_disableTteth, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_PROD_CFG_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PROD_CFG_DISABLE_TTETH_MASK);   /* mask desired bits */
	*p_disableTteth = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the prod_id
*
* \param[out] p_prodId Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_prodIdArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getProdId(SJA1105_prodIdArgument_t *p_prodId, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_PROD_ID_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_PROD_ID_PART_NR_SHIFT);  /* shift to right aligned position */
	p_prodId->partNr = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PROD_ID_VERSION_MASK);   /* mask desired bits */
	p_prodId->version = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_padOutputStageSpeed_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_padOutputStageSpeed_t
*/
SJA1105_padOutputStageSpeed_t SJA1105_convertToPadOutputStageSpeed(uint32_t stdintValue)
{
	SJA1105_padOutputStageSpeed_t ret;
	switch (stdintValue)
	{
		case 3U: ret = e_padOutputStageSpeed_HIGH_SPEED; break;
		case 2U: ret = e_padOutputStageSpeed_FAST_SPEED; break;
		case 1U: ret = e_padOutputStageSpeed_MEDIUM_SPEED; break;
		default: ret = e_padOutputStageSpeed_LOW_SPEED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_padInputStageSelection_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_padInputStageSelection_t
*/
SJA1105_padInputStageSelection_t SJA1105_convertToPadInputStageSelection(uint32_t stdintValue)
{
	SJA1105_padInputStageSelection_t ret;
	switch (stdintValue)
	{
		case 3U: ret = e_padInputStageSelection_PULL_DOWN; break;
		case 2U: ret = e_padInputStageSelection_PLAIN_INPUT; break;
		case 1U: ret = e_padInputStageSelection_REPEATER; break;
		default: ret = e_padInputStageSelection_PULL_UP; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_speed_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_speed_t
*/
SJA1105_speed_t SJA1105_convertToSpeed(uint32_t stdintValue)
{
	SJA1105_speed_t ret;
	switch (stdintValue)
	{
		case 0U: ret = e_speed_10_MBPS; break;
		case 1U: ret = e_speed_100_MBPS; break;
		default: ret = e_speed_1_GBPS; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_xmiiMode_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_xmiiMode_t
*/
SJA1105_xmiiMode_t SJA1105_convertToXmiiMode(uint32_t stdintValue)
{
	SJA1105_xmiiMode_t ret;
	switch (stdintValue)
	{
		case 0U: ret = e_xmiiMode_MII; break;
		case 1U: ret = e_xmiiMode_RMII; break;
		case 2U: ret = e_xmiiMode_RGMII; break;
		default: ret = e_xmiiMode_UNUSED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_padInputStageHysteresis_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_padInputStageHysteresis_t
*/
SJA1105_padInputStageHysteresis_t SJA1105_convertToPadInputStageHysteresis(uint32_t stdintValue)
{
	SJA1105_padInputStageHysteresis_t ret;
	switch (stdintValue)
	{
		case 1U: ret = e_padInputStageHysteresis_SCHMITT; break;
		default: ret = e_padInputStageHysteresis_NON_SCHMITT; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_rgmiiSmpl_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_rgmiiSmpl_t
*/
SJA1105_rgmiiSmpl_t SJA1105_convertToRgmiiSmpl(uint32_t stdintValue)
{
	SJA1105_rgmiiSmpl_t ret;
	switch (stdintValue)
	{
		case 3U: ret = e_rgmiiSmpl_RISING_AND_FALLING; break;
		case 2U: ret = e_rgmiiSmpl_FALLING; break;
		case 1U: ret = e_rgmiiSmpl_RISING; break;
		default: ret = e_rgmiiSmpl_DISABLED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_tempThreshold_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_tempThreshold_t
*/
SJA1105_tempThreshold_t SJA1105_convertToTempThreshold(uint32_t stdintValue)
{
	SJA1105_tempThreshold_t ret;
	switch (stdintValue)
	{
		case 39U: ret = e_tempThreshold_POSITIVE135P5; break;
		case 38U: ret = e_tempThreshold_POSITIVE130P5; break;
		case 37U: ret = e_tempThreshold_POSITIVE125P5; break;
		case 36U: ret = e_tempThreshold_POSITIVE120P7; break;
		case 35U: ret = e_tempThreshold_POSITIVE116P0; break;
		case 34U: ret = e_tempThreshold_POSITIVE111P4; break;
		case 33U: ret = e_tempThreshold_POSITIVE106P9; break;
		case 32U: ret = e_tempThreshold_POSITIVE102P5; break;
		case 31U: ret = e_tempThreshold_POSITIVE98P2; break;
		case 30U: ret = e_tempThreshold_POSITIVE92P8; break;
		case 29U: ret = e_tempThreshold_POSITIVE87P5; break;
		case 28U: ret = e_tempThreshold_POSITIVE82P4; break;
		case 27U: ret = e_tempThreshold_POSITIVE77P4; break;
		case 26U: ret = e_tempThreshold_POSITIVE72P6; break;
		case 25U: ret = e_tempThreshold_POSITIVE67P9; break;
		case 24U: ret = e_tempThreshold_POSITIVE63P3; break;
		case 23U: ret = e_tempThreshold_POSITIVE58P8; break;
		case 22U: ret = e_tempThreshold_POSITIVE54P5; break;
		case 21U: ret = e_tempThreshold_POSITIVE50P2; break;
		case 20U: ret = e_tempThreshold_POSITIVE46P1; break;
		case 19U: ret = e_tempThreshold_POSITIVE42P0; break;
		case 18U: ret = e_tempThreshold_POSITIVE36P4; break;
		case 17U: ret = e_tempThreshold_POSITIVE30P9; break;
		case 16U: ret = e_tempThreshold_POSITIVE25P6; break;
		case 15U: ret = e_tempThreshold_POSITIVE20P6; break;
		case 14U: ret = e_tempThreshold_POSITIVE15P7; break;
		case 13U: ret = e_tempThreshold_POSITIVE11P0; break;
		case 12U: ret = e_tempThreshold_POSITIVE6P4; break;
		case 11U: ret = e_tempThreshold_POSITIVE2P1; break;
		case 10U: ret = e_tempThreshold_NEGATIVE2P1; break;
		case 9U: ret = e_tempThreshold_NEGATIVE6P1; break;
		case 8U: ret = e_tempThreshold_NEGATIVE11P4; break;
		case 7U: ret = e_tempThreshold_NEGATIVE18P3; break;
		case 6U: ret = e_tempThreshold_NEGATIVE23P5; break;
		case 5U: ret = e_tempThreshold_NEGATIVE28P4; break;
		case 4U: ret = e_tempThreshold_NEGATIVE33P0; break;
		case 3U: ret = e_tempThreshold_NEGATIVE37P5; break;
		case 2U: ret = e_tempThreshold_NEGATIVE41P7; break;
		case 1U: ret = e_tempThreshold_NEGATIVE45P7; break;
		default: ret = e_tempThreshold_NOT_VALID; break;
	}
	return ret;
}
