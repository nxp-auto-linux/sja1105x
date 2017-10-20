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
* \file NXP_SJA1105P_clockGenerationUnit.c
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief Generates multiple internal clocks to drive the xMII ports
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_clockGenerationUnit.h"
#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category pll_status */
#define PLL_S_PLL_INC (0x2)     
#define PLL_S_ADDR    (0x100007)  /**< Address of the pllStatus register */

/* register category pll_control */
#define PLL_C_PLL_INC (0x2)     
#define PLL_C_ADDR    (0x100008)  /**< Address of the pllControl register */

/* register category idiv_c_control_register */
#define IDIV_C_CR_ADDR (0x10000b)  /**< Address of the idivCControlRegister register */

/* register category miix_clock_control_register */
#define MIIX_CCR_PORT_INC (0x6)     
#define MIIX_CCR_ADDR     (0x100013)  /**< Address of the miixClockControlRegister register */

/* variable masks */

/* register category pll_status */
#define PLL_S_LOCK_MASK (0x00000001U)  /**< Bit mask for the LOCK field */

/* register category pll_control */
#define PLL_C_MSEL_SHIFT        (16)           /**< Shift value for the MSEL field */
#define PLL_C_AUTOBLOCK_SHIFT   (11)           /**< Shift value for the AUTOBLOCK field */
#define PLL_C_AUTOBLOCK_MASK    (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define PLL_C_PSEL_SHIFT        (8)            /**< Shift value for the PSEL field */
#define PLL_C_PSEL_MASK         (0x00000300U)  /**< Bit mask for the PSEL field */
#define PLL_C_DIRECT_SHIFT      (7)            /**< Shift value for the DIRECT field */
#define PLL_C_DIRECT_MASK       (0x00000080U)  /**< Bit mask for the DIRECT field */
#define PLL_C_FBSEL_SHIFT       (6)            /**< Shift value for the FBSEL field */
#define PLL_C_FBSEL_MASK        (0x00000040U)  /**< Bit mask for the FBSEL field */
#define PLL_C_PHASEOUT_EN_SHIFT (2)            /**< Shift value for the PHASEOUT_EN field */
#define PLL_C_PHASEOUT_EN_MASK  (0x00000004U)  /**< Bit mask for the PHASEOUT_EN field */
#define PLL_C_BYPASS_SHIFT      (1)            /**< Shift value for the BYPASS field */
#define PLL_C_BYPASS_MASK       (0x00000002U)  /**< Bit mask for the BYPASS field */
#define PLL_C_PD_MASK           (0x00000001U)  /**< Bit mask for the PD field */

/* register category idiv_c_control_register */
#define IDIV_C_CR_AUTOBLOCK_SHIFT (11)           /**< Shift value for the AUTOBLOCK field */
#define IDIV_C_CR_AUTOBLOCK_MASK  (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define IDIV_C_CR_IDIV_SHIFT      (2)            /**< Shift value for the IDIV field */
#define IDIV_C_CR_IDIV_MASK       (0x0000003CU)  /**< Bit mask for the IDIV field */
#define IDIV_C_CR_PD_MASK         (0x00000001U)  /**< Bit mask for the PD field */

/* register category miix_clock_control_register */
#define MIIX_CCR_CLKSRC_SHIFT    (24)           /**< Shift value for the CLKSRC field */
#define MIIX_CCR_CLKSRC_MASK     (0x1F000000U)  /**< Bit mask for the CLKSRC field */
#define MIIX_CCR_AUTOBLOCK_SHIFT (11)           /**< Shift value for the AUTOBLOCK field */
#define MIIX_CCR_AUTOBLOCK_MASK  (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define MIIX_CCR_PD_MASK         (0x00000001U)  /**< Bit mask for the PD field */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to GET data of the pll_status
*
* \param[out] p_lock PLL lock indicator - 0: PLL not locked, 1: PLL locked
* \param[in]  pll PLL index {0:1}
* \return uint8_t
*/
uint8_t SJA1105P_getPllStatus(uint8_t *p_lock, uint8_t pll, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) PLL_S_ADDR + ((uint32_t) PLL_S_PLL_INC * pll)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_S_LOCK_MASK);   /* mask desired bits */
	*p_lock = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the pll_control
*
* \param[in]  pllControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_pllControlArgument_t
* \param[in]  pll PLL index {0:1}
* \return uint8_t
*/
uint8_t SJA1105P_setPllControl(const SJA1105P_pllControlArgument_t *pk_pllControl, uint8_t pll, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_pllControl->autoblock <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->psel <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->direct <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->fbsel <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->phaseoutEn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->bypass <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_pllControl->pd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_pllControl->msel) << ((uint8_t) PLL_C_MSEL_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_pllControl->autoblock) << ((uint8_t) PLL_C_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pllControl->psel) << ((uint8_t) PLL_C_PSEL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_PSEL_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pllControl->direct) << ((uint8_t) PLL_C_DIRECT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_DIRECT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pllControl->fbsel) << ((uint8_t) PLL_C_FBSEL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_FBSEL_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pllControl->phaseoutEn) << ((uint8_t) PLL_C_PHASEOUT_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_PHASEOUT_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_pllControl->bypass) << ((uint8_t) PLL_C_BYPASS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PLL_C_BYPASS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) PLL_C_PD_MASK) & pk_pllControl->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) PLL_C_ADDR + ((uint32_t) PLL_C_PLL_INC * pll)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the pll_control
*
* \param[out] p_pllControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_pllControlArgument_t
* \param[in]  pll PLL index {0:1}
* \return uint8_t
*/
uint8_t SJA1105P_getPllControl(SJA1105P_pllControlArgument_t *p_pllControl, uint8_t pll, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) PLL_C_ADDR + ((uint32_t) PLL_C_PLL_INC * pll)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (PLL_C_MSEL_SHIFT);  /* shift to right aligned position */
	p_pllControl->msel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_pllControl->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_PSEL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_PSEL_SHIFT);  /* shift to right aligned position */
	p_pllControl->psel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_DIRECT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_DIRECT_SHIFT);  /* shift to right aligned position */
	p_pllControl->direct = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_FBSEL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_FBSEL_SHIFT);  /* shift to right aligned position */
	p_pllControl->fbsel = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_PHASEOUT_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_PHASEOUT_EN_SHIFT);  /* shift to right aligned position */
	p_pllControl->phaseoutEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_BYPASS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PLL_C_BYPASS_SHIFT);  /* shift to right aligned position */
	p_pllControl->bypass = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PLL_C_PD_MASK);   /* mask desired bits */
	p_pllControl->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the idiv_c_control_register
*
* \param[in]  idivCControlRegister Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_idivCControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_setIdivCControlRegister(const SJA1105P_idivCControlRegisterArgument_t *pk_idivCControlRegister, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_idivCControlRegister->autoblock <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_idivCControlRegister->idiv) <= 0xFU) ? 0U : 1U);
		valueCheckFailed += ((pk_idivCControlRegister->pd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_idivCControlRegister->autoblock) << ((uint8_t) IDIV_C_CR_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) IDIV_C_CR_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_idivCControlRegister->idiv)) << ((uint8_t) IDIV_C_CR_IDIV_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) IDIV_C_CR_IDIV_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) IDIV_C_CR_PD_MASK) & pk_idivCControlRegister->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) IDIV_C_CR_ADDR + ((uint32_t) port)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the idiv_c_control_register
*
* \param[out] p_idivCControlRegister Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_idivCControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getIdivCControlRegister(SJA1105P_idivCControlRegisterArgument_t *p_idivCControlRegister, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) IDIV_C_CR_ADDR + ((uint32_t) port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (IDIV_C_CR_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (IDIV_C_CR_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_idivCControlRegister->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (IDIV_C_CR_IDIV_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (IDIV_C_CR_IDIV_SHIFT);  /* shift to right aligned position */
	p_idivCControlRegister->idiv = SJA1105P_convertToIdiv(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (IDIV_C_CR_PD_MASK);   /* mask desired bits */
	p_idivCControlRegister->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the miix_clock_control_register
*
* \param[in]  miixClockControlRegister Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_miixClockControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  internalClk Internal clock, possible values: ::SJA1105P_miixInternalClk_t
* \return uint8_t
*/
uint8_t SJA1105P_setMiixClockControlRegister(const SJA1105P_miixClockControlRegisterArgument_t *pk_miixClockControlRegister, uint8_t port, SJA1105P_miixInternalClk_t internalClk, uint8_t deviceSelect)
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

	tmp = ((uint32_t) ((uint8_t) pk_miixClockControlRegister->clksrc)) << ((uint8_t) MIIX_CCR_CLKSRC_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MIIX_CCR_CLKSRC_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_miixClockControlRegister->autoblock) << ((uint8_t) MIIX_CCR_AUTOBLOCK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MIIX_CCR_AUTOBLOCK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) MIIX_CCR_PD_MASK) & pk_miixClockControlRegister->pd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) MIIX_CCR_ADDR + ((uint32_t) MIIX_CCR_PORT_INC * port) + ((uint32_t) internalClk)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the miix_clock_control_register
*
* \param[out] p_miixClockControlRegister Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_miixClockControlRegisterArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  internalClk Internal clock, possible values: ::SJA1105P_miixInternalClk_t
* \return uint8_t
*/
uint8_t SJA1105P_getMiixClockControlRegister(SJA1105P_miixClockControlRegisterArgument_t *p_miixClockControlRegister, uint8_t port, SJA1105P_miixInternalClk_t internalClk, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) MIIX_CCR_ADDR + ((uint32_t) MIIX_CCR_PORT_INC * port) + ((uint32_t) internalClk)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MIIX_CCR_CLKSRC_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MIIX_CCR_CLKSRC_SHIFT);  /* shift to right aligned position */
	p_miixClockControlRegister->clksrc = SJA1105P_convertToClksrc(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MIIX_CCR_AUTOBLOCK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MIIX_CCR_AUTOBLOCK_SHIFT);  /* shift to right aligned position */
	p_miixClockControlRegister->autoblock = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MIIX_CCR_PD_MASK);   /* mask desired bits */
	p_miixClockControlRegister->pd = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_clksrc_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_clksrc_t
*/
SJA1105P_clksrc_t SJA1105P_convertToClksrc(uint32_t stdintValue)
{
	SJA1105P_clksrc_t ret;
	switch (stdintValue)
	{
		case 21U: ret = SJA1105P_e_clksrc_IDIV4; break;
		case 20U: ret = SJA1105P_e_clksrc_IDIV3; break;
		case 19U: ret = SJA1105P_e_clksrc_IDIV2; break;
		case 18U: ret = SJA1105P_e_clksrc_IDIV1; break;
		case 17U: ret = SJA1105P_e_clksrc_IDIV0; break;
		case 16U: ret = SJA1105P_e_clksrc_PLL1_240; break;
		case 15U: ret = SJA1105P_e_clksrc_PLL1_120; break;
		case 14U: ret = SJA1105P_e_clksrc_PLL1; break;
		case 13U: ret = SJA1105P_e_clksrc_PLL0_240; break;
		case 12U: ret = SJA1105P_e_clksrc_PLL0_120; break;
		case 11U: ret = SJA1105P_e_clksrc_PLL0; break;
		case 9U: ret = SJA1105P_e_clksrc_MII4_RX_CLK; break;
		case 8U: ret = SJA1105P_e_clksrc_MII4_TX_CLK; break;
		case 7U: ret = SJA1105P_e_clksrc_MII3_RX_CLK; break;
		case 6U: ret = SJA1105P_e_clksrc_MII3_TX_CLK; break;
		case 5U: ret = SJA1105P_e_clksrc_MII2_RX_CLK; break;
		case 4U: ret = SJA1105P_e_clksrc_MII2_TX_CLK; break;
		case 3U: ret = SJA1105P_e_clksrc_MII1_RX_CLK; break;
		case 2U: ret = SJA1105P_e_clksrc_MII1_TX_CLK; break;
		case 1U: ret = SJA1105P_e_clksrc_MII0_RX_CLK; break;
		default: ret = SJA1105P_e_clksrc_MII0_TX_CLK; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_idiv_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_idiv_t
*/
SJA1105P_idiv_t SJA1105P_convertToIdiv(uint32_t stdintValue)
{
	SJA1105P_idiv_t ret;
	switch (stdintValue)
	{
		case 9U: ret = SJA1105P_e_idiv_BY_10; break;
		default: ret = SJA1105P_e_idiv_BY_1; break;
	}
	return ret;
}
