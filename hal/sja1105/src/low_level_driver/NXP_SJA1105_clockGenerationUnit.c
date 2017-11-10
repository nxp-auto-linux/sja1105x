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
* \file NXP_SJA1105_clockGenerationUnit.c
*
* \author NXP Semiconductors
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief Generates multiple internal clocks to drive the xMII ports
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_clockGenerationUnit.h"
#include "NXP_SJA1105_spi.h"

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to GET data of the pll_0_s_clock_status
*
* \param[out] p_lock PLL lock indicator: {0: PLL not locked, 1: PLL locked}
* \return uint8_t
*/
uint8_t SJA1105_getPll0SClockStatus(uint8_t *p_lock, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_PLL_0_S_CS_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_0_S_CS_LOCK_MASK);   /* mask desired bits */
	*p_lock = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the pll_1_s_clock_status
*
* \param[out] p_lock PLL lock indicator: {0: PLL not locked, 1: PLL locked}
* \return uint8_t
*/
uint8_t SJA1105_getPll1SClockStatus(uint8_t *p_lock, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_PLL_1_S_CS_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_S_CS_LOCK_MASK);   /* mask desired bits */
	*p_lock = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the pll_1_c_control_register
*
* \param[in]  pll1CControlRegister Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_pll1CControlRegisterArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setPll1CControlRegister(const SJA1105_pll1CControlRegisterArgument_t *pk_pll1CControlRegister, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_pll1CControlRegister->autoblock <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pll1CControlRegister->psel <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_pll1CControlRegister->direct <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pll1CControlRegister->fbsel <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pll1CControlRegister->bypass <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pll1CControlRegister->pd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_pll1CControlRegister->msel) << ((uint8_t) SJA1105_PLL_1_C_CR_MSEL_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_pll1CControlRegister->autoblock) << ((uint8_t) SJA1105_PLL_1_C_CR_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pll1CControlRegister->psel) << ((uint8_t) SJA1105_PLL_1_C_CR_PSEL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_PSEL_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pll1CControlRegister->direct) << ((uint8_t) SJA1105_PLL_1_C_CR_DIRECT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_DIRECT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pll1CControlRegister->fbsel) << ((uint8_t) SJA1105_PLL_1_C_CR_FBSEL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_FBSEL_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pll1CControlRegister->bypass) << ((uint8_t) SJA1105_PLL_1_C_CR_BYPASS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_BYPASS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_PLL_1_C_CR_PD_MASK) & pk_pll1CControlRegister->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_PLL_1_C_CR_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the pll_1_c_control_register
*
* \param[out] p_pll1CControlRegister Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_pll1CControlRegisterArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getPll1CControlRegister(SJA1105_pll1CControlRegisterArgument_t *p_pll1CControlRegister, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_PLL_1_C_CR_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_MSEL_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->msel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_PSEL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_PSEL_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->psel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_DIRECT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_DIRECT_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->direct = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_FBSEL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_FBSEL_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->fbsel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_BYPASS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_PLL_1_C_CR_BYPASS_SHIFT);  /* shift to right aligned position */
	p_pll1CControlRegister->bypass = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_PLL_1_C_CR_PD_MASK);   /* mask desired bits */
	p_pll1CControlRegister->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the idiv_c_control_register
*
* \param[in]  idivCControlRegister Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_idivCControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_setIdivCControlRegister(const SJA1105_idivCControlRegisterArgument_t *pk_idivCControlRegister, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_idivCControlRegister->autoblock <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_idivCControlRegister->idiv <= 0xFU) ? 0U : 1U);
		valueCheckFailed += ((pk_idivCControlRegister->pd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_idivCControlRegister->autoblock) << ((uint8_t) SJA1105_IDIV_C_CR_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_IDIV_C_CR_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_idivCControlRegister->idiv) << ((uint8_t) SJA1105_IDIV_C_CR_IDIV_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_IDIV_C_CR_IDIV_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_IDIV_C_CR_PD_MASK) & pk_idivCControlRegister->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_IDIV_C_CR_ADDR + ((uint32_t) port)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the idiv_c_control_register
*
* \param[out] p_idivCControlRegister Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_idivCControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getIdivCControlRegister(SJA1105_idivCControlRegisterArgument_t *p_idivCControlRegister, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_IDIV_C_CR_ADDR + ((uint32_t) port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_IDIV_C_CR_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_IDIV_C_CR_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_idivCControlRegister->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_IDIV_C_CR_IDIV_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_IDIV_C_CR_IDIV_SHIFT);  /* shift to right aligned position */
	p_idivCControlRegister->idiv = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_IDIV_C_CR_PD_MASK);   /* mask desired bits */
	p_idivCControlRegister->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the miix_clock_control_register
*
* \param[in]  miixClockControlRegister Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_miixClockControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  internalClk Internal clock, possible values: ::SJA1105_miixInternalClk_t
* \return uint8_t
*/
uint8_t SJA1105_setMiixClockControlRegister(const SJA1105_miixClockControlRegisterArgument_t *pk_miixClockControlRegister, uint8_t port, SJA1105_miixInternalClk_t internalClk, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_miixClockControlRegister->clksrc) <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_miixClockControlRegister->autoblock <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_miixClockControlRegister->pd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_miixClockControlRegister->clksrc)) << ((uint8_t) SJA1105_MIIX_CCR_CLKSRC_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MIIX_CCR_CLKSRC_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_miixClockControlRegister->autoblock) << ((uint8_t) SJA1105_MIIX_CCR_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MIIX_CCR_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_MIIX_CCR_PD_MASK) & pk_miixClockControlRegister->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_MIIX_CCR_ADDR + ((uint32_t) SJA1105_MIIX_CCR_PORT_INC * port) + ((uint32_t) internalClk)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the miix_clock_control_register
*
* \param[out] p_miixClockControlRegister Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_miixClockControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  internalClk Internal clock, possible values: ::SJA1105_miixInternalClk_t
* \return uint8_t
*/
uint8_t SJA1105_getMiixClockControlRegister(SJA1105_miixClockControlRegisterArgument_t *p_miixClockControlRegister, uint8_t port, SJA1105_miixInternalClk_t internalClk, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_MIIX_CCR_ADDR + ((uint32_t) SJA1105_MIIX_CCR_PORT_INC * port) + ((uint32_t) internalClk)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MIIX_CCR_CLKSRC_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MIIX_CCR_CLKSRC_SHIFT);  /* shift to right aligned position */
	p_miixClockControlRegister->clksrc = SJA1105_convertToClksrc(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MIIX_CCR_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MIIX_CCR_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_miixClockControlRegister->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MIIX_CCR_PD_MASK);   /* mask desired bits */
	p_miixClockControlRegister->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105_clksrc_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105_clksrc_t
*/
SJA1105_clksrc_t SJA1105_convertToClksrc(uint32_t stdintValue)
{
	SJA1105_clksrc_t ret;
	switch (stdintValue)
	{
		case 21U: ret = e_clksrc_IDIV4; break;
		case 20U: ret = e_clksrc_IDIV3; break;
		case 19U: ret = e_clksrc_IDIV2; break;
		case 18U: ret = e_clksrc_IDIV1; break;
		case 17U: ret = e_clksrc_IDIV0; break;
		case 14U: ret = e_clksrc_PLL1; break;
		case 11U: ret = e_clksrc_PLL0; break;
		case 9U: ret = e_clksrc_MII4_RX_CLK; break;
		case 8U: ret = e_clksrc_MII4_TX_CLK; break;
		case 7U: ret = e_clksrc_MII3_RX_CLK; break;
		case 6U: ret = e_clksrc_MII3_TX_CLK; break;
		case 5U: ret = e_clksrc_MII2_RX_CLK; break;
		case 4U: ret = e_clksrc_MII2_TX_CLK; break;
		case 3U: ret = e_clksrc_MII1_RX_CLK; break;
		case 2U: ret = e_clksrc_MII1_TX_CLK; break;
		case 1U: ret = e_clksrc_MII0_RX_CLK; break;
		default: ret = e_clksrc_MII0_TX_CLK; break;
	}
	return ret;
}
