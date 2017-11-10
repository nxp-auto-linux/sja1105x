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
* \file NXP_SJA1105_clockGenerationUnit.h
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

#ifndef NXP_SJA1105_CLOCKGENERATIONUNIT_H
#define NXP_SJA1105_CLOCKGENERATIONUNIT_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category pll_0_s_clock_status */
#define SJA1105_PLL_0_S_CS_ADDR (0x100007)  /**< Address of the pll0SClockStatus register */

/* register category pll_1_s_clock_status */
#define SJA1105_PLL_1_S_CS_ADDR (0x100009)  /**< Address of the pll1SClockStatus register */

/* register category pll_1_c_control_register */
#define SJA1105_PLL_1_C_CR_ADDR (0x10000a)  /**< Address of the pll1CControlRegister register */

/* register category idiv_c_control_register */
#define SJA1105_IDIV_C_CR_ADDR (0x10000b)  /**< Address of the idivCControlRegister register */

/* register category miix_clock_control_register */
#define SJA1105_MIIX_CCR_PORT_INC (0x7)     
#define SJA1105_MIIX_CCR_ADDR     (0x100013)  /**< Address of the miixClockControlRegister register */

/* variable masks */

/* register category pll_0_s_clock_status */
#define SJA1105_PLL_0_S_CS_LOCK_MASK (0x00000001U)  /**< Bit mask for the LOCK field */

/* register category pll_1_s_clock_status */
#define SJA1105_PLL_1_S_CS_LOCK_MASK (0x00000001U)  /**< Bit mask for the LOCK field */

/* register category pll_1_c_control_register */
#define SJA1105_PLL_1_C_CR_MSEL_SHIFT      (16)           /**< Shift value for the MSEL field */
#define SJA1105_PLL_1_C_CR_AUTOBLOCK_SHIFT (11)           /**< Shift value for the AUTOBLOCK field */
#define SJA1105_PLL_1_C_CR_AUTOBLOCK_MASK  (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define SJA1105_PLL_1_C_CR_PSEL_SHIFT      (8)            /**< Shift value for the PSEL field */
#define SJA1105_PLL_1_C_CR_PSEL_MASK       (0x00000300U)  /**< Bit mask for the PSEL field */
#define SJA1105_PLL_1_C_CR_DIRECT_SHIFT    (7)            /**< Shift value for the DIRECT field */
#define SJA1105_PLL_1_C_CR_DIRECT_MASK     (0x00000080U)  /**< Bit mask for the DIRECT field */
#define SJA1105_PLL_1_C_CR_FBSEL_SHIFT     (6)            /**< Shift value for the FBSEL field */
#define SJA1105_PLL_1_C_CR_FBSEL_MASK      (0x00000040U)  /**< Bit mask for the FBSEL field */
#define SJA1105_PLL_1_C_CR_BYPASS_SHIFT    (1)            /**< Shift value for the BYPASS field */
#define SJA1105_PLL_1_C_CR_BYPASS_MASK     (0x00000002U)  /**< Bit mask for the BYPASS field */
#define SJA1105_PLL_1_C_CR_PD_MASK         (0x00000001U)  /**< Bit mask for the PD field */

/* register category idiv_c_control_register */
#define SJA1105_IDIV_C_CR_AUTOBLOCK_SHIFT (11)           /**< Shift value for the AUTOBLOCK field */
#define SJA1105_IDIV_C_CR_AUTOBLOCK_MASK  (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define SJA1105_IDIV_C_CR_IDIV_SHIFT      (2)            /**< Shift value for the IDIV field */
#define SJA1105_IDIV_C_CR_IDIV_MASK       (0x0000003CU)  /**< Bit mask for the IDIV field */
#define SJA1105_IDIV_C_CR_PD_MASK         (0x00000001U)  /**< Bit mask for the PD field */

/* register category miix_clock_control_register */
#define SJA1105_MIIX_CCR_CLKSRC_SHIFT    (24)           /**< Shift value for the CLKSRC field */
#define SJA1105_MIIX_CCR_CLKSRC_MASK     (0x1F000000U)  /**< Bit mask for the CLKSRC field */
#define SJA1105_MIIX_CCR_AUTOBLOCK_SHIFT (11)           /**< Shift value for the AUTOBLOCK field */
#define SJA1105_MIIX_CCR_AUTOBLOCK_MASK  (0x00000800U)  /**< Bit mask for the AUTOBLOCK field */
#define SJA1105_MIIX_CCR_PD_MASK         (0x00000001U)  /**< Bit mask for the PD field */

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	e_clksrc_IDIV4       = 21,  /**< Integer Divider 4 */
	e_clksrc_IDIV3       = 20,  /**< Integer Divider 3 */
	e_clksrc_IDIV2       = 19,  /**< Integer Divider 2 */
	e_clksrc_IDIV1       = 18,  /**< Integer Divider 1 */
	e_clksrc_IDIV0       = 17,  /**< Integer Divider 0 */
	e_clksrc_PLL1        = 14,  /**< Phase-locked loop (PLL) 1 */
	e_clksrc_PLL0        = 11,  /**< Phase-locked loop (PLL) 0 */
	e_clksrc_MII4_RX_CLK = 9,   /**< MII 4 Rx clock */
	e_clksrc_MII4_TX_CLK = 8,   /**< MII 4 Tx clock */
	e_clksrc_MII3_RX_CLK = 7,   /**< MII 3 Rx clock */
	e_clksrc_MII3_TX_CLK = 6,   /**< MII 3 Tx clock */
	e_clksrc_MII2_RX_CLK = 5,   /**< MII 2 Rx clock */
	e_clksrc_MII2_TX_CLK = 4,   /**< MII 2 Tx clock */
	e_clksrc_MII1_RX_CLK = 3,   /**< MII 1 Rx clock */
	e_clksrc_MII1_TX_CLK = 2,   /**< MII 1 Tx clock */
	e_clksrc_MII0_RX_CLK = 1,   /**< MII 0 Rx clock */
	e_clksrc_MII0_TX_CLK = 0    /**< MII 0 Tx clock */
} SJA1105_clksrc_t;

typedef enum 
{
	e_miixInternalClk_EXT_RX_CLK   = 6,  /**< External Rx clock */
	e_miixInternalClk_EXT_TX_CLK   = 5,  /**< External Tx clock */
	e_miixInternalClk_RGMII_RX_CLK = 4,  /**< RGMII Rx clock */
	e_miixInternalClk_RGMII_TX_CLK = 3,  /**< RGMII Tx clock */
	e_miixInternalClk_RMII_REF_CLK = 2,  /**< RMII reference clock */
	e_miixInternalClk_MII_RX_CLK   = 1,  /**< MII Rx clock */
	e_miixInternalClk_MII_TX_CLK   = 0   /**< MII Tx clock */
} SJA1105_miixInternalClk_t;

typedef struct
{
	uint8_t msel;       /**< M divider value: {0x00: disabled, 0x01: RMII clock generation} */
	uint8_t autoblock;  /**< Block clock automatically when settings are being changed: {0: disabled, 1: RMII clock generation} */
	uint8_t psel;       /**< P divider value: {00: disabled, 01: RMII clock generation} */
	uint8_t direct;     /**< Direct cco clock output control: {0: RMII setting: clock signal goes through post divider, 1: clock signal goes directly to output} */
	uint8_t fbsel;      /**< PLL feedback: {0: disabled, 1: RGMII} */
	uint8_t bypass;     /**< PLL bypass: {0: PLL not bypassed; must be set to 0 to enable RMII clock generation, 1: PLL bypassed} */
	uint8_t pd;         /**< PLL power down: {0: PLL enabled, 1: PLL disabled} */
} SJA1105_pll1CControlRegisterArgument_t;

typedef struct
{
	uint8_t autoblock;  /**< Block clock reference automatically when settings are being changed: {0: disabled, 1: enabled (recommended when configuration settings are being changed)} */
	uint8_t idiv;       /**< Integer IDIV divide by value */
	uint8_t pd;         /**< IDIV clock power down: {0: IDIV enabled, 1: IDIV disabled} */
} SJA1105_idivCControlRegisterArgument_t;

typedef struct
{
	SJA1105_clksrc_t clksrc;     /**< Input clock selection */
	uint8_t          autoblock;  /**< Block clock reference automatically when settings are being changed: {0: disabled, 1: enabled (recommended when configuration settings are being changed)} */
	uint8_t          pd;         /**< MIIx clock power down: {0: MIIx clock enabled, 1: MIIx clock disabled} */
} SJA1105_miixClockControlRegisterArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category pll_0_s_clock_status */
extern uint8_t SJA1105_getPll0SClockStatus(uint8_t *p_lock, uint8_t deviceSelect);

/* register category pll_1_s_clock_status */
extern uint8_t SJA1105_getPll1SClockStatus(uint8_t *p_lock, uint8_t deviceSelect);

/* register category pll_1_c_control_register */
extern uint8_t SJA1105_setPll1CControlRegister(const SJA1105_pll1CControlRegisterArgument_t *pk_pll1CControlRegister, uint8_t deviceSelect);
extern uint8_t SJA1105_getPll1CControlRegister(SJA1105_pll1CControlRegisterArgument_t *p_pll1CControlRegister, uint8_t deviceSelect);

/* register category idiv_c_control_register */
extern uint8_t SJA1105_setIdivCControlRegister(const SJA1105_idivCControlRegisterArgument_t *pk_idivCControlRegister, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getIdivCControlRegister(SJA1105_idivCControlRegisterArgument_t *p_idivCControlRegister, uint8_t port, uint8_t deviceSelect);

/* register category miix_clock_control_register */
extern uint8_t SJA1105_setMiixClockControlRegister(const SJA1105_miixClockControlRegisterArgument_t *pk_miixClockControlRegister, uint8_t port, SJA1105_miixInternalClk_t internalClk, uint8_t deviceSelect);
extern uint8_t SJA1105_getMiixClockControlRegister(SJA1105_miixClockControlRegisterArgument_t *p_miixClockControlRegister, uint8_t port, SJA1105_miixInternalClk_t internalClk, uint8_t deviceSelect);

extern SJA1105_clksrc_t SJA1105_convertToClksrc(uint32_t stdintValue);

#endif  /* NXP_SJA1105_CLOCKGENERATIONUNIT_H */
