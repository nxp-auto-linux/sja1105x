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
* \file NXP_SJA1105P_clockGenerationUnit.h
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

#ifndef NXP_SJA1105P_CLOCKGENERATIONUNIT_H
#define NXP_SJA1105P_CLOCKGENERATIONUNIT_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_clksrc_IDIV4       = 21,  /**< Integer Divider 4 */
	SJA1105P_e_clksrc_IDIV3       = 20,  /**< Integer Divider 3 */
	SJA1105P_e_clksrc_IDIV2       = 19,  /**< Integer Divider 2 */
	SJA1105P_e_clksrc_IDIV1       = 18,  /**< Integer Divider 1 */
	SJA1105P_e_clksrc_IDIV0       = 17,  /**< Integer Divider 0 */
	SJA1105P_e_clksrc_PLL1_240    = 16,  /**< Phase-locked loop (PLL) 1, 240 degrees phase */
	SJA1105P_e_clksrc_PLL1_120    = 15,  /**< Phase-locked loop (PLL) 1, 120 degrees phase */
	SJA1105P_e_clksrc_PLL1        = 14,  /**< Phase-locked loop (PLL) 1 */
	SJA1105P_e_clksrc_PLL0_240    = 13,  /**< Phase-locked loop (PLL) 0, 240 degrees phase */
	SJA1105P_e_clksrc_PLL0_120    = 12,  /**< Phase-locked loop (PLL) 0, 120 degrees phase */
	SJA1105P_e_clksrc_PLL0        = 11,  /**< Phase-locked loop (PLL) 0 */
	SJA1105P_e_clksrc_MII4_RX_CLK = 9,   /**< MII 4 Rx clock */
	SJA1105P_e_clksrc_MII4_TX_CLK = 8,   /**< MII 4 Tx clock */
	SJA1105P_e_clksrc_MII3_RX_CLK = 7,   /**< MII 3 Rx clock */
	SJA1105P_e_clksrc_MII3_TX_CLK = 6,   /**< MII 3 Tx clock */
	SJA1105P_e_clksrc_MII2_RX_CLK = 5,   /**< MII 2 Rx clock */
	SJA1105P_e_clksrc_MII2_TX_CLK = 4,   /**< MII 2 Tx clock */
	SJA1105P_e_clksrc_MII1_RX_CLK = 3,   /**< MII 1 Rx clock */
	SJA1105P_e_clksrc_MII1_TX_CLK = 2,   /**< MII 1 Tx clock */
	SJA1105P_e_clksrc_MII0_RX_CLK = 1,   /**< MII 0 Rx clock */
	SJA1105P_e_clksrc_MII0_TX_CLK = 0    /**< MII 0 Tx clock */
} SJA1105P_clksrc_t;

typedef enum 
{
	SJA1105P_e_miixInternalClk_EXT_RX_CLK   = 5,  /**< External MII Rx clock */
	SJA1105P_e_miixInternalClk_EXT_TX_CLK   = 4,  /**< External MII Tx clock / RMII Ref clock */
	SJA1105P_e_miixInternalClk_RGMII_TX_CLK = 3,  /**< Internal/External RGMII Tx clock */
	SJA1105P_e_miixInternalClk_RMII_REF_CLK = 2,  /**< RMII reference clock */
	SJA1105P_e_miixInternalClk_MII_RX_CLK   = 1,  /**< MII Rx clock */
	SJA1105P_e_miixInternalClk_MII_TX_CLK   = 0   /**< MII Tx clock */
} SJA1105P_miixInternalClk_t;

typedef enum 
{
	SJA1105P_e_idiv_BY_10 = 9,  /**< Divide by 10 */
	SJA1105P_e_idiv_BY_1  = 0   /**< Divide by 1 */
} SJA1105P_idiv_t;

typedef struct
{
	uint8_t msel;        /**< M divider value: {0x00: disabled, 0x01: RMII clock generation} */
	uint8_t autoblock;   /**< Block clock automatically when settings are being changed: {0: disabled, 1: RMII clock generation} */
	uint8_t psel;        /**< P divider value: {00: disabled, 01: RMII clock generation} */
	uint8_t direct;      /**< Direct cco clock output control: {0: RMII setting: clock signal goes through post divider, 1: clock signal goes directly to output} */
	uint8_t fbsel;       /**< PLL feedback: {0: disabled, 1: RGMII} */
	uint8_t phaseoutEn;  /**< Enable phase output of the PLL at 120 and 240 degrees */
	uint8_t bypass;      /**< PLL bypass: {0: PLL not bypassed; must be set to 0 to enable RMII clock generation, 1: PLL bypassed} */
	uint8_t pd;          /**< PLL power down: {0: PLL enabled, 1: PLL disabled} */
} SJA1105P_pllControlArgument_t;

typedef struct
{
	uint8_t         autoblock;  /**< Block clock reference automatically when settings are being changed: {0: disabled, 1: enabled (recommended when configuration settings are being changed)} */
	SJA1105P_idiv_t idiv;       /**< Integer IDIV divide by value */
	uint8_t         pd;         /**< IDIV clock power down: {0: IDIV enabled, 1: IDIV disabled} */
} SJA1105P_idivCControlRegisterArgument_t;

typedef struct
{
	SJA1105P_clksrc_t clksrc;     /**< Input clock selection */
	uint8_t           autoblock;  /**< Block clock reference automatically when settings are being changed: {0: disabled, 1: enabled (recommended when configuration settings are being changed)} */
	uint8_t           pd;         /**< MIIx clock power down: {0: MIIx clock enabled, 1: MIIx clock disabled} */
} SJA1105P_miixClockControlRegisterArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category pll_status */
extern uint8_t SJA1105P_getPllStatus(uint8_t *p_lock, uint8_t pll, uint8_t deviceSelect);

/* register category pll_control */
extern uint8_t SJA1105P_setPllControl(const SJA1105P_pllControlArgument_t *pk_pllControl, uint8_t pll, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPllControl(SJA1105P_pllControlArgument_t *p_pllControl, uint8_t pll, uint8_t deviceSelect);

/* register category idiv_c_control_register */
extern uint8_t SJA1105P_setIdivCControlRegister(const SJA1105P_idivCControlRegisterArgument_t *pk_idivCControlRegister, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getIdivCControlRegister(SJA1105P_idivCControlRegisterArgument_t *p_idivCControlRegister, uint8_t port, uint8_t deviceSelect);

/* register category miix_clock_control_register */
extern uint8_t SJA1105P_setMiixClockControlRegister(const SJA1105P_miixClockControlRegisterArgument_t *pk_miixClockControlRegister, uint8_t port, SJA1105P_miixInternalClk_t internalClk, uint8_t deviceSelect);
extern uint8_t SJA1105P_getMiixClockControlRegister(SJA1105P_miixClockControlRegisterArgument_t *p_miixClockControlRegister, uint8_t port, SJA1105P_miixInternalClk_t internalClk, uint8_t deviceSelect);

extern SJA1105P_clksrc_t SJA1105P_convertToClksrc(uint32_t stdintValue);
extern SJA1105P_idiv_t SJA1105P_convertToIdiv(uint32_t stdintValue);

#endif  /* NXP_SJA1105P_CLOCKGENERATIONUNIT_H */
