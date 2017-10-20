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
* \file NXP_SJA1105P_auxiliaryConfigurationUnit.c
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief The auxiliary configuration unit controls the I/O characteristics and provides auxiliary functionality.
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category cfg_pad_miix */
#define CFG_PAD_MIIX_PORT_INC (0x2)
#define CFG_PAD_MIIX_ADDR     (0x100800)  /**< Address of the cfgPadMiix register */

/* register category cfg_pad_miix_id */
#define CFG_PAD_MIIX_ID_ADDR (0x100810)  /**< Address of the cfgPadMiixId register */

/* register category cfg_pad_misc */
#define CFG_PAD_MISC_ADDR (0x100840)  /**< Address of the cfgPadMisc register */

/* register category cfg_pad_spi */
#define CFG_PAD_SPI_ADDR (0x100880)  /**< Address of the cfgPadSpi register */

/* register category cfg_pad_jtag */
#define CFG_PAD_JTAG_ADDR (0x100881)  /**< Address of the cfgPadJtag register */

/* register category port_status_miix */
#define PORT_S_MIIX_ADDR (0x100900)  /**< Address of the portStatusMiix register */

/* register category ts_config */
#define TS_C_ADDR (0x100a00)  /**< Address of the tsConfig register */

/* register category ts_status */
#define TS_S_ADDR (0x100a01)  /**< Address of the tsStatus register */

/* register category rgmii_meas_setup */
#define R_MEAS_S_ADDR (0x100a80)  /**< Address of the rgmiiMeasSetup register */

/* register category prod_cfg */
#define PROD_CFG_ADDR (0x100bc0)  /**< Address of the prodCfg register */

/* register category prod_id */
#define PROD_ID_ADDR (0x100bc3)  /**< Address of the prodId register */

/* variable masks */

/* register category cfg_pad_miix */
#define CFG_PAD_MIIX_D32_OS_SHIFT    (27)           /**< Shift value for the D32_OS field */
#define CFG_PAD_MIIX_D32_OS_MASK     (0x18000000U)  /**< Bit mask for the D32_OS field */
#define CFG_PAD_MIIX_D32_IH_SHIFT    (26)           /**< Shift value for the D32_IH field */
#define CFG_PAD_MIIX_D32_IH_MASK     (0x04000000U)  /**< Bit mask for the D32_IH field */
#define CFG_PAD_MIIX_D32_IPUD_SHIFT  (24)           /**< Shift value for the D32_IPUD field */
#define CFG_PAD_MIIX_D32_IPUD_MASK   (0x03000000U)  /**< Bit mask for the D32_IPUD field */
#define CFG_PAD_MIIX_D10_OS_SHIFT    (19)           /**< Shift value for the D10_OS field */
#define CFG_PAD_MIIX_D10_OS_MASK     (0x00180000U)  /**< Bit mask for the D10_OS field */
#define CFG_PAD_MIIX_D10_IH_SHIFT    (18)           /**< Shift value for the D10_IH field */
#define CFG_PAD_MIIX_D10_IH_MASK     (0x00040000U)  /**< Bit mask for the D10_IH field */
#define CFG_PAD_MIIX_D10_IPUD_SHIFT  (16)           /**< Shift value for the D10_IPUD field */
#define CFG_PAD_MIIX_D10_IPUD_MASK   (0x00030000U)  /**< Bit mask for the D10_IPUD field */
#define CFG_PAD_MIIX_CTRL_OS_SHIFT   (11)           /**< Shift value for the CTRL_OS field */
#define CFG_PAD_MIIX_CTRL_OS_MASK    (0x00001800U)  /**< Bit mask for the CTRL_OS field */
#define CFG_PAD_MIIX_CTRL_IH_SHIFT   (10)           /**< Shift value for the CTRL_IH field */
#define CFG_PAD_MIIX_CTRL_IH_MASK    (0x00000C00U)  /**< Bit mask for the CTRL_IH field */
#define CFG_PAD_MIIX_CTRL_IPUD_SHIFT (8)            /**< Shift value for the CTRL_IPUD field */
#define CFG_PAD_MIIX_CTRL_IPUD_MASK  (0x00000300U)  /**< Bit mask for the CTRL_IPUD field */
#define CFG_PAD_MIIX_CLK_OS_SHIFT    (3)            /**< Shift value for the CLK_OS field */
#define CFG_PAD_MIIX_CLK_OS_MASK     (0x00000018U)  /**< Bit mask for the CLK_OS field */
#define CFG_PAD_MIIX_CLK_IH_SHIFT    (2)            /**< Shift value for the CLK_IH field */
#define CFG_PAD_MIIX_CLK_IH_MASK     (0x00000004U)  /**< Bit mask for the CLK_IH field */
#define CFG_PAD_MIIX_CLK_IPUD_MASK   (0x00000003U)  /**< Bit mask for the CLK_IPUD field */

/* register category cfg_pad_miix_id */
#define CFG_PAD_MIIX_ID_RXC_DELAY_SHIFT  (10)           /**< Shift value for the RXC_DELAY field */
#define CFG_PAD_MIIX_ID_RXC_DELAY_MASK   (0x00007C00U)  /**< Bit mask for the RXC_DELAY field */
#define CFG_PAD_MIIX_ID_RXC_BYPASS_SHIFT (9)            /**< Shift value for the RXC_BYPASS field */
#define CFG_PAD_MIIX_ID_RXC_BYPASS_MASK  (0x00000200U)  /**< Bit mask for the RXC_BYPASS field */
#define CFG_PAD_MIIX_ID_RXC_PD_SHIFT     (8)            /**< Shift value for the RXC_PD field */
#define CFG_PAD_MIIX_ID_RXC_PD_MASK      (0x00000100U)  /**< Bit mask for the RXC_PD field */
#define CFG_PAD_MIIX_ID_TXC_DELAY_SHIFT  (2)            /**< Shift value for the TXC_DELAY field */
#define CFG_PAD_MIIX_ID_TXC_DELAY_MASK   (0x0000007CU)  /**< Bit mask for the TXC_DELAY field */
#define CFG_PAD_MIIX_ID_TXC_BYPASS_SHIFT (1)            /**< Shift value for the TXC_BYPASS field */
#define CFG_PAD_MIIX_ID_TXC_BYPASS_MASK  (0x00000002U)  /**< Bit mask for the TXC_BYPASS field */
#define CFG_PAD_MIIX_ID_TXC_PD_MASK      (0x00000001U)  /**< Bit mask for the TXC_PD field */

/* register category cfg_pad_misc */
#define CFG_PAD_MISC_PTPCLK_OS_SHIFT   (19)           /**< Shift value for the PTPCLK_OS field */
#define CFG_PAD_MISC_PTPCLK_OS_MASK    (0x00180000U)  /**< Bit mask for the PTPCLK_OS field */
#define CFG_PAD_MISC_PTPCLK_IH_SHIFT   (18)           /**< Shift value for the PTPCLK_IH field */
#define CFG_PAD_MISC_PTPCLK_IH_MASK    (0x00040000U)  /**< Bit mask for the PTPCLK_IH field */
#define CFG_PAD_MISC_PTPCLK_IPUD_SHIFT (16)           /**< Shift value for the PTPCLK_IPUD field */
#define CFG_PAD_MISC_PTPCLK_IPUD_MASK  (0x00030000U)  /**< Bit mask for the PTPCLK_IPUD field */
#define CFG_PAD_MISC_RSTN_IH_SHIFT     (10)           /**< Shift value for the RSTN_IH field */
#define CFG_PAD_MISC_RSTN_IH_MASK      (0x00000400U)  /**< Bit mask for the RSTN_IH field */
#define CFG_PAD_MISC_RSTN_IPUD_SHIFT   (8)            /**< Shift value for the RSTN_IPUD field */
#define CFG_PAD_MISC_RSTN_IPUD_MASK    (0x00000300U)  /**< Bit mask for the RSTN_IPUD field */
#define CFG_PAD_MISC_CLKOUT_EN_SHIFT   (5)            /**< Shift value for the CLKOUT_EN field */
#define CFG_PAD_MISC_CLKOUT_EN_MASK    (0x00000020U)  /**< Bit mask for the CLKOUT_EN field */
#define CFG_PAD_MISC_CLKOUT_OS_SHIFT   (3)            /**< Shift value for the CLKOUT_OS field */
#define CFG_PAD_MISC_CLKOUT_OS_MASK    (0x00000018U)  /**< Bit mask for the CLKOUT_OS field */
#define CFG_PAD_MISC_CLKOUT_IPUD_MASK  (0x00000003U)  /**< Bit mask for the CLKOUT_IPUD field */

/* register category cfg_pad_spi */
#define CFG_PAD_SPI_SDO_OS_SHIFT   (27)           /**< Shift value for the SDO_OS field */
#define CFG_PAD_SPI_SDO_OS_MASK    (0x18000000U)  /**< Bit mask for the SDO_OS field */
#define CFG_PAD_SPI_SDO_IPUD_SHIFT (24)           /**< Shift value for the SDO_IPUD field */
#define CFG_PAD_SPI_SDO_IPUD_MASK  (0x03000000U)  /**< Bit mask for the SDO_IPUD field */
#define CFG_PAD_SPI_SDI_IH_SHIFT   (18)           /**< Shift value for the SDI_IH field */
#define CFG_PAD_SPI_SDI_IH_MASK    (0x00040000U)  /**< Bit mask for the SDI_IH field */
#define CFG_PAD_SPI_SDI_IPUD_SHIFT (16)           /**< Shift value for the SDI_IPUD field */
#define CFG_PAD_SPI_SDI_IPUD_MASK  (0x00030000U)  /**< Bit mask for the SDI_IPUD field */
#define CFG_PAD_SPI_SSN_IH_SHIFT   (10)           /**< Shift value for the SSN_IH field */
#define CFG_PAD_SPI_SSN_IH_MASK    (0x00000400U)  /**< Bit mask for the SSN_IH field */
#define CFG_PAD_SPI_SSN_IPUD_SHIFT (8)            /**< Shift value for the SSN_IPUD field */
#define CFG_PAD_SPI_SSN_IPUD_MASK  (0x00000300U)  /**< Bit mask for the SSN_IPUD field */
#define CFG_PAD_SPI_SCK_IH_SHIFT   (2)            /**< Shift value for the SCK_IH field */
#define CFG_PAD_SPI_SCK_IH_MASK    (0x00000004U)  /**< Bit mask for the SCK_IH field */
#define CFG_PAD_SPI_SCK_IPUD_MASK  (0x00000003U)  /**< Bit mask for the SCK_IPUD field */

/* register category cfg_pad_jtag */
#define CFG_PAD_JTAG_TDO_IPUD_SHIFT      (24)           /**< Shift value for the TDO_IPUD field */
#define CFG_PAD_JTAG_TDO_IPUD_MASK       (0x03000000U)  /**< Bit mask for the TDO_IPUD field */
#define CFG_PAD_JTAG_TDI_IPUD_SHIFT      (16)           /**< Shift value for the TDI_IPUD field */
#define CFG_PAD_JTAG_TDI_IPUD_MASK       (0x00030000U)  /**< Bit mask for the TDI_IPUD field */
#define CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT (8)            /**< Shift value for the TRSTNTMS_IPUD field */
#define CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK  (0x00000300U)  /**< Bit mask for the TRSTNTMS_IPUD field */
#define CFG_PAD_JTAG_TCK_IPUD_MASK       (0x00000003U)  /**< Bit mask for the TCK_IPUD field */

/* register category port_status_miix */
#define PORT_S_MIIX_RXC_ID_FREQ_MODE_SHIFT (20)           /**< Shift value for the RXC_ID_FREQ_MODE field */
#define PORT_S_MIIX_RXC_ID_FREQ_MODE_MASK  (0x00300000U)  /**< Bit mask for the RXC_ID_FREQ_MODE field */
#define PORT_S_MIIX_RXC_ID_CTL_MODE_SHIFT  (18)           /**< Shift value for the RXC_ID_CTL_MODE field */
#define PORT_S_MIIX_RXC_ID_CTL_MODE_MASK   (0x000C0000U)  /**< Bit mask for the RXC_ID_CTL_MODE field */
#define PORT_S_MIIX_RXC_DELAY_STABLE_SHIFT (17)           /**< Shift value for the RXC_DELAY_STABLE field */
#define PORT_S_MIIX_RXC_DELAY_STABLE_MASK  (0x00020000U)  /**< Bit mask for the RXC_DELAY_STABLE field */
#define PORT_S_MIIX_RXC_DELAY_PD_SHIFT     (16)           /**< Shift value for the RXC_DELAY_PD field */
#define PORT_S_MIIX_RXC_DELAY_PD_MASK      (0x00010000U)  /**< Bit mask for the RXC_DELAY_PD field */
#define PORT_S_MIIX_TXC_ID_FREQ_MODE_SHIFT (12)           /**< Shift value for the TXC_ID_FREQ_MODE field */
#define PORT_S_MIIX_TXC_ID_FREQ_MODE_MASK  (0x00003000U)  /**< Bit mask for the TXC_ID_FREQ_MODE field */
#define PORT_S_MIIX_TXC_ID_CTL_MODE_SHIFT  (10)           /**< Shift value for the TXC_ID_CTL_MODE field */
#define PORT_S_MIIX_TXC_ID_CTL_MODE_MASK   (0x00000C00U)  /**< Bit mask for the TXC_ID_CTL_MODE field */
#define PORT_S_MIIX_TXC_DELAY_STABLE_SHIFT (9)            /**< Shift value for the TXC_DELAY_STABLE field */
#define PORT_S_MIIX_TXC_DELAY_STABLE_MASK  (0x00000200U)  /**< Bit mask for the TXC_DELAY_STABLE field */
#define PORT_S_MIIX_TXC_DELAY_PD_SHIFT     (8)            /**< Shift value for the TXC_DELAY_PD field */
#define PORT_S_MIIX_TXC_DELAY_PD_MASK      (0x00000100U)  /**< Bit mask for the TXC_DELAY_PD field */
#define PORT_S_MIIX_SPEED_SHIFT            (3)            /**< Shift value for the SPEED field */
#define PORT_S_MIIX_SPEED_MASK             (0x00000018U)  /**< Bit mask for the SPEED field */
#define PORT_S_MIIX_PHY_MODE_SHIFT         (2)            /**< Shift value for the PHY_MODE field */
#define PORT_S_MIIX_PHY_MODE_MASK          (0x00000004U)  /**< Bit mask for the PHY_MODE field */
#define PORT_S_MIIX_XMII_MODE_MASK         (0x00000003U)  /**< Bit mask for the XMII_MODE field */

/* register category ts_config */
#define TS_C_PD_SHIFT       (6)            /**< Shift value for the PD field */
#define TS_C_PD_MASK        (0x00000040U)  /**< Bit mask for the PD field */
#define TS_C_THRESHOLD_MASK (0x0000003FU)  /**< Bit mask for the THRESHOLD field */

/* register category ts_status */
#define TS_S_EXCEEDED_MASK (0x00000001U)  /**< Bit mask for the EXCEEDED field */

/* register category rgmii_meas_setup */
#define R_MEAS_S_MII4_RX_SMPL_EN_SHIFT (8)            /**< Shift value for the MII4_RX_SMPL_EN field */
#define R_MEAS_S_MII4_RX_SMPL_EN_MASK  (0x00000300U)  /**< Bit mask for the MII4_RX_SMPL_EN field */
#define R_MEAS_S_MII3_RX_SMPL_EN_SHIFT (6)            /**< Shift value for the MII3_RX_SMPL_EN field */
#define R_MEAS_S_MII3_RX_SMPL_EN_MASK  (0x000000C0U)  /**< Bit mask for the MII3_RX_SMPL_EN field */
#define R_MEAS_S_MII2_RX_SMPL_EN_SHIFT (4)            /**< Shift value for the MII2_RX_SMPL_EN field */
#define R_MEAS_S_MII2_RX_SMPL_EN_MASK  (0x00000030U)  /**< Bit mask for the MII2_RX_SMPL_EN field */
#define R_MEAS_S_MII1_RX_SMPL_EN_SHIFT (2)            /**< Shift value for the MII1_RX_SMPL_EN field */
#define R_MEAS_S_MII1_RX_SMPL_EN_MASK  (0x0000000CU)  /**< Bit mask for the MII1_RX_SMPL_EN field */
#define R_MEAS_S_MII0_RX_SMPL_EN_MASK  (0x00000003U)  /**< Bit mask for the MII0_RX_SMPL_EN field */

/* register category prod_cfg */
#define PROD_CFG_DISABLE_TTETH_MASK (0x00000001U)  /**< Bit mask for the DISABLE_TTETH field */

/* register category prod_id */
#define PROD_ID_PART_NR_SHIFT (4)            /**< Shift value for the PART_NR field */
#define PROD_ID_VERSION_MASK  (0x0000000FU)  /**< Bit mask for the VERSION field */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to SET data of the cfg_pad_miix
*
* \param[in]  cfgPadMiix Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  direction Direction (Transmit/Receive), possible values: ::SJA1105P_direction_t
* \return uint8_t
*/
uint8_t SJA1105P_setCfgPadMiix(const SJA1105P_cfgPadMiixArgument_t *pk_cfgPadMiix, uint8_t port, SJA1105P_direction_t direction, uint8_t deviceSelect)
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

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Os)) << ((uint8_t) CFG_PAD_MIIX_D32_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D32_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Ih)) << ((uint8_t) CFG_PAD_MIIX_D32_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D32_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d32Ipud)) << ((uint8_t) CFG_PAD_MIIX_D32_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D32_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Os)) << ((uint8_t) CFG_PAD_MIIX_D10_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D10_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Ih)) << ((uint8_t) CFG_PAD_MIIX_D10_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D10_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->d10Ipud)) << ((uint8_t) CFG_PAD_MIIX_D10_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_D10_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlOs)) << ((uint8_t) CFG_PAD_MIIX_CTRL_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_CTRL_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlIh)) << ((uint8_t) CFG_PAD_MIIX_CTRL_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_CTRL_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->ctrlIpud)) << ((uint8_t) CFG_PAD_MIIX_CTRL_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_CTRL_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->clkOs)) << ((uint8_t) CFG_PAD_MIIX_CLK_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_CLK_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMiix->clkIh)) << ((uint8_t) CFG_PAD_MIIX_CLK_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_CLK_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CFG_PAD_MIIX_CLK_IPUD_MASK) & ((uint8_t) pk_cfgPadMiix->clkIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) CFG_PAD_MIIX_ADDR + ((uint32_t) CFG_PAD_MIIX_PORT_INC * port) + ((uint32_t) direction)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_miix
*
* \param[out] p_cfgPadMiix Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  direction Direction (Transmit/Receive), possible values: ::SJA1105P_direction_t
* \return uint8_t
*/
uint8_t SJA1105P_getCfgPadMiix(SJA1105P_cfgPadMiixArgument_t *p_cfgPadMiix, uint8_t port, SJA1105P_direction_t direction, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) CFG_PAD_MIIX_ADDR + ((uint32_t) CFG_PAD_MIIX_PORT_INC * port) + ((uint32_t) direction)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D32_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D32_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Os = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D32_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D32_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Ih = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D32_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D32_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d32Ipud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D10_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D10_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Os = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D10_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D10_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Ih = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_D10_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_D10_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->d10Ipud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CTRL_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_CTRL_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlOs = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CTRL_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_CTRL_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CTRL_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_CTRL_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->ctrlIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CLK_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_CLK_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->clkOs = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CLK_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_CLK_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiix->clkIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_CLK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadMiix->clkIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_miix_id
*
* \param[in]  cfgPadMiixId Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiixIdArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_setCfgPadMiixId(const SJA1105P_cfgPadMiixIdArgument_t *pk_cfgPadMiixId, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_cfgPadMiixId->rxcDelay <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMiixId->rxcBypass <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMiixId->rxcPd <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMiixId->txcDelay <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMiixId->txcBypass <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_cfgPadMiixId->txcPd <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_cfgPadMiixId->rxcDelay) << ((uint8_t) CFG_PAD_MIIX_ID_RXC_DELAY_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_RXC_DELAY_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_cfgPadMiixId->rxcBypass) << ((uint8_t) CFG_PAD_MIIX_ID_RXC_BYPASS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_RXC_BYPASS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_cfgPadMiixId->rxcPd) << ((uint8_t) CFG_PAD_MIIX_ID_RXC_PD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_RXC_PD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_cfgPadMiixId->txcDelay) << ((uint8_t) CFG_PAD_MIIX_ID_TXC_DELAY_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_TXC_DELAY_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_cfgPadMiixId->txcBypass) << ((uint8_t) CFG_PAD_MIIX_ID_TXC_BYPASS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_TXC_BYPASS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CFG_PAD_MIIX_ID_TXC_PD_MASK) & pk_cfgPadMiixId->txcPd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) ((uint32_t) CFG_PAD_MIIX_ID_ADDR + ((uint32_t) port)), &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_miix_id
*
* \param[out] p_cfgPadMiixId Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiixIdArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getCfgPadMiixId(SJA1105P_cfgPadMiixIdArgument_t *p_cfgPadMiixId, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) CFG_PAD_MIIX_ID_ADDR + ((uint32_t) port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_RXC_DELAY_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_ID_RXC_DELAY_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiixId->rxcDelay = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_RXC_BYPASS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_ID_RXC_BYPASS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiixId->rxcBypass = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_RXC_PD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_ID_RXC_PD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiixId->rxcPd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_TXC_DELAY_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_ID_TXC_DELAY_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiixId->txcDelay = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_TXC_BYPASS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MIIX_ID_TXC_BYPASS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMiixId->txcBypass = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MIIX_ID_TXC_PD_MASK);   /* mask desired bits */
	p_cfgPadMiixId->txcPd = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_misc
*
* \param[in]  cfgPadMisc Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiscArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setCfgPadMisc(const SJA1105P_cfgPadMiscArgument_t *pk_cfgPadMisc, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_cfgPadMisc->ptpclkOs) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_cfgPadMisc->ptpclkIh) <= 0x1U) ? 0U : 1U);
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

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->ptpclkOs)) << ((uint8_t) CFG_PAD_MISC_PTPCLK_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_PTPCLK_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->ptpclkIh)) << ((uint8_t) CFG_PAD_MISC_PTPCLK_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_PTPCLK_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->ptpclkIpud)) << ((uint8_t) CFG_PAD_MISC_PTPCLK_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_PTPCLK_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->rstnIh)) << ((uint8_t) CFG_PAD_MISC_RSTN_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_RSTN_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->rstnIpud)) << ((uint8_t) CFG_PAD_MISC_RSTN_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_RSTN_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_cfgPadMisc->clkoutEn) << ((uint8_t) CFG_PAD_MISC_CLKOUT_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_CLKOUT_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadMisc->clkoutOs)) << ((uint8_t) CFG_PAD_MISC_CLKOUT_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_MISC_CLKOUT_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CFG_PAD_MISC_CLKOUT_IPUD_MASK) & ((uint8_t) pk_cfgPadMisc->clkoutIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) CFG_PAD_MISC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_misc
*
* \param[out] p_cfgPadMisc Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadMiscArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getCfgPadMisc(SJA1105P_cfgPadMiscArgument_t *p_cfgPadMisc, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) CFG_PAD_MISC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_PTPCLK_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_PTPCLK_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkOs = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_PTPCLK_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_PTPCLK_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_PTPCLK_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_PTPCLK_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->ptpclkIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_RSTN_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_RSTN_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->rstnIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_RSTN_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_RSTN_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->rstnIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_CLKOUT_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_CLKOUT_EN_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->clkoutEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_CLKOUT_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_MISC_CLKOUT_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadMisc->clkoutOs = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_MISC_CLKOUT_IPUD_MASK);   /* mask desired bits */
	p_cfgPadMisc->clkoutIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_spi
*
* \param[in]  cfgPadSpi Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadSpiArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setCfgPadSpi(const SJA1105P_cfgPadSpiArgument_t *pk_cfgPadSpi, uint8_t deviceSelect)
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

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdoOs)) << ((uint8_t) CFG_PAD_SPI_SDO_OS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SDO_OS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdoIpud)) << ((uint8_t) CFG_PAD_SPI_SDO_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SDO_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdiIh)) << ((uint8_t) CFG_PAD_SPI_SDI_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SDI_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sdiIpud)) << ((uint8_t) CFG_PAD_SPI_SDI_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SDI_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->ssnIh)) << ((uint8_t) CFG_PAD_SPI_SSN_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SSN_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->ssnIpud)) << ((uint8_t) CFG_PAD_SPI_SSN_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SSN_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadSpi->sckIh)) << ((uint8_t) CFG_PAD_SPI_SCK_IH_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_SPI_SCK_IH_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CFG_PAD_SPI_SCK_IPUD_MASK) & ((uint8_t) pk_cfgPadSpi->sckIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) CFG_PAD_SPI_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_spi
*
* \param[out] p_cfgPadSpi Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadSpiArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getCfgPadSpi(SJA1105P_cfgPadSpiArgument_t *p_cfgPadSpi, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) CFG_PAD_SPI_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SDO_OS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SDO_OS_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdoOs = SJA1105P_convertToPadOutputStageSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SDO_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SDO_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdoIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SDI_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SDI_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdiIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SDI_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SDI_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sdiIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SSN_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SSN_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->ssnIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SSN_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SSN_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->ssnIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SCK_IH_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_SPI_SCK_IH_SHIFT);  /* shift to right aligned position */
	p_cfgPadSpi->sckIh = SJA1105P_convertToPadInputHysteresis(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_SPI_SCK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadSpi->sckIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the cfg_pad_jtag
*
* \param[in]  cfgPadJtag Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadJtagArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setCfgPadJtag(const SJA1105P_cfgPadJtagArgument_t *pk_cfgPadJtag, uint8_t deviceSelect)
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

	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->tdoIpud)) << ((uint8_t) CFG_PAD_JTAG_TDO_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_JTAG_TDO_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->tdiIpud)) << ((uint8_t) CFG_PAD_JTAG_TDI_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_JTAG_TDI_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_cfgPadJtag->trstntmsIpud)) << ((uint8_t) CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CFG_PAD_JTAG_TCK_IPUD_MASK) & ((uint8_t) pk_cfgPadJtag->tckIpud);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) CFG_PAD_JTAG_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the cfg_pad_jtag
*
* \param[out] p_cfgPadJtag Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_cfgPadJtagArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getCfgPadJtag(SJA1105P_cfgPadJtagArgument_t *p_cfgPadJtag, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) CFG_PAD_JTAG_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_JTAG_TDO_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_JTAG_TDO_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->tdoIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_JTAG_TDI_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_JTAG_TDI_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->tdiIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT);  /* shift to right aligned position */
	p_cfgPadJtag->trstntmsIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CFG_PAD_JTAG_TCK_IPUD_MASK);   /* mask desired bits */
	p_cfgPadJtag->tckIpud = SJA1105P_convertToPadInputStageSelection(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the port_status_miix
*
* \param[out] p_portStatusMiix Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_portStatusMiixArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getPortStatusMiix(SJA1105P_portStatusMiixArgument_t *p_portStatusMiix, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) PORT_S_MIIX_ADDR + ((uint32_t) port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_RXC_ID_FREQ_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_RXC_ID_FREQ_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->rxcIdFreqMode = SJA1105P_convertToIdFreqMode(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_RXC_ID_CTL_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_RXC_ID_CTL_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->rxcIdCtlMode = SJA1105P_convertToIdCtlMode(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_RXC_DELAY_STABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_RXC_DELAY_STABLE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->rxcDelayStable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_RXC_DELAY_PD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_RXC_DELAY_PD_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->rxcDelayPd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_TXC_ID_FREQ_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_TXC_ID_FREQ_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->txcIdFreqMode = SJA1105P_convertToIdFreqMode(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_TXC_ID_CTL_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_TXC_ID_CTL_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->txcIdCtlMode = SJA1105P_convertToIdCtlMode(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_TXC_DELAY_STABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_TXC_DELAY_STABLE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->txcDelayStable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_TXC_DELAY_PD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_TXC_DELAY_PD_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->txcDelayPd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_SPEED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_SPEED_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->speed = SJA1105P_convertToSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_PHY_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PORT_S_MIIX_PHY_MODE_SHIFT);  /* shift to right aligned position */
	p_portStatusMiix->phyMode = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PORT_S_MIIX_XMII_MODE_MASK);   /* mask desired bits */
	p_portStatusMiix->xmiiMode = SJA1105P_convertToXmiiMode(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ts_config
*
* \param[in]  tsConfig Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_tsConfigArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setTsConfig(const SJA1105P_tsConfigArgument_t *pk_tsConfig, uint8_t deviceSelect)
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

	tmp = ((uint32_t) pk_tsConfig->pd) << ((uint8_t) TS_C_PD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) TS_C_PD_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) TS_C_THRESHOLD_MASK) & ((uint8_t) pk_tsConfig->threshold);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TS_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ts_config
*
* \param[out] p_tsConfig Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_tsConfigArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getTsConfig(SJA1105P_tsConfigArgument_t *p_tsConfig, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TS_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TS_C_PD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (TS_C_PD_SHIFT);  /* shift to right aligned position */
	p_tsConfig->pd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TS_C_THRESHOLD_MASK);   /* mask desired bits */
	p_tsConfig->threshold = SJA1105P_convertToTempThreshold(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the ts_status
*
* \param[out] p_exceeded temperature detection status {0: active, 1: power-down}
* \return uint8_t
*/
uint8_t SJA1105P_getTsStatus(uint8_t *p_exceeded, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TS_S_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TS_S_EXCEEDED_MASK);   /* mask desired bits */
	*p_exceeded = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the rgmii_meas_setup
*
* \param[in]  rgmiiMeasSetup Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_rgmiiMeasSetupArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setRgmiiMeasSetup(const SJA1105P_rgmiiMeasSetupArgument_t *pk_rgmiiMeasSetup, uint8_t deviceSelect)
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

	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii4RxSmplEn)) << ((uint8_t) R_MEAS_S_MII4_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) R_MEAS_S_MII4_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii3RxSmplEn)) << ((uint8_t) R_MEAS_S_MII3_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) R_MEAS_S_MII3_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii2RxSmplEn)) << ((uint8_t) R_MEAS_S_MII2_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) R_MEAS_S_MII2_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_rgmiiMeasSetup->mii1RxSmplEn)) << ((uint8_t) R_MEAS_S_MII1_RX_SMPL_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) R_MEAS_S_MII1_RX_SMPL_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) R_MEAS_S_MII0_RX_SMPL_EN_MASK) & ((uint8_t) pk_rgmiiMeasSetup->mii0RxSmplEn);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) R_MEAS_S_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the rgmii_meas_setup
*
* \param[out] p_rgmiiMeasSetup Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_rgmiiMeasSetupArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getRgmiiMeasSetup(SJA1105P_rgmiiMeasSetupArgument_t *p_rgmiiMeasSetup, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) R_MEAS_S_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (R_MEAS_S_MII4_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (R_MEAS_S_MII4_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii4RxSmplEn = SJA1105P_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (R_MEAS_S_MII3_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (R_MEAS_S_MII3_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii3RxSmplEn = SJA1105P_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (R_MEAS_S_MII2_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (R_MEAS_S_MII2_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii2RxSmplEn = SJA1105P_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (R_MEAS_S_MII1_RX_SMPL_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (R_MEAS_S_MII1_RX_SMPL_EN_SHIFT);  /* shift to right aligned position */
	p_rgmiiMeasSetup->mii1RxSmplEn = SJA1105P_convertToRgmiiSmpl(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (R_MEAS_S_MII0_RX_SMPL_EN_MASK);   /* mask desired bits */
	p_rgmiiMeasSetup->mii0RxSmplEn = SJA1105P_convertToRgmiiSmpl(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the prod_cfg
*
* \param[out] p_disableTteth TTEthernet features of switch: {0: disabled, 1: enabled}
* \return uint8_t
*/
uint8_t SJA1105P_getProdCfg(uint8_t *p_disableTteth, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) PROD_CFG_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PROD_CFG_DISABLE_TTETH_MASK);   /* mask desired bits */
	*p_disableTteth = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the prod_id
*
* \param[out] p_prodId Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_prodIdArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getProdId(SJA1105P_prodIdArgument_t *p_prodId, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) PROD_ID_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (PROD_ID_PART_NR_SHIFT);  /* shift to right aligned position */
	p_prodId->partNr = SJA1105P_convertToPartNr(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PROD_ID_VERSION_MASK);   /* mask desired bits */
	p_prodId->version = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_padOutputStageSpeed_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_padOutputStageSpeed_t
*/
SJA1105P_padOutputStageSpeed_t SJA1105P_convertToPadOutputStageSpeed(uint32_t stdintValue)
{
	SJA1105P_padOutputStageSpeed_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_padOutputStageSpeed_HIGH_SPEED; break;
		case 2U: ret = SJA1105P_e_padOutputStageSpeed_FAST_SPEED; break;
		case 1U: ret = SJA1105P_e_padOutputStageSpeed_MEDIUM_SPEED; break;
		default: ret = SJA1105P_e_padOutputStageSpeed_LOW_SPEED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_padInputStageSelection_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_padInputStageSelection_t
*/
SJA1105P_padInputStageSelection_t SJA1105P_convertToPadInputStageSelection(uint32_t stdintValue)
{
	SJA1105P_padInputStageSelection_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_padInputStageSelection_PULL_DOWN; break;
		case 2U: ret = SJA1105P_e_padInputStageSelection_PLAIN_INPUT; break;
		case 1U: ret = SJA1105P_e_padInputStageSelection_REPEATER; break;
		default: ret = SJA1105P_e_padInputStageSelection_PULL_UP; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_speed_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_speed_t
*/
SJA1105P_speed_t SJA1105P_convertToSpeed(uint32_t stdintValue)
{
	SJA1105P_speed_t ret;
	switch (stdintValue)
	{
		case 0U: ret = SJA1105P_e_speed_10_MBPS; break;
		case 1U: ret = SJA1105P_e_speed_100_MBPS; break;
		default: ret = SJA1105P_e_speed_1_GBPS; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_idCtlMode_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_idCtlMode_t
*/
SJA1105P_idCtlMode_t SJA1105P_convertToIdCtlMode(uint32_t stdintValue)
{
	SJA1105P_idCtlMode_t ret;
	switch (stdintValue)
	{
		case 0U: ret = SJA1105P_e_idCtlMode_BYPASS; break;
		case 1U: ret = SJA1105P_e_idCtlMode_DELAYED; break;
		default: ret = SJA1105P_e_idCtlMode_OUPUT_DISABLED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_padInputHysteresis_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_padInputHysteresis_t
*/
SJA1105P_padInputHysteresis_t SJA1105P_convertToPadInputHysteresis(uint32_t stdintValue)
{
	SJA1105P_padInputHysteresis_t ret;
	switch (stdintValue)
	{
		case 1U: ret = SJA1105P_e_padInputHysteresis_SCHMITT; break;
		default: ret = SJA1105P_e_padInputHysteresis_NON_SCHMITT; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_xmiiMode_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_xmiiMode_t
*/
SJA1105P_xmiiMode_t SJA1105P_convertToXmiiMode(uint32_t stdintValue)
{
	SJA1105P_xmiiMode_t ret;
	switch (stdintValue)
	{
		case 0U: ret = SJA1105P_e_xmiiMode_MII; break;
		case 1U: ret = SJA1105P_e_xmiiMode_RMII; break;
		case 2U: ret = SJA1105P_e_xmiiMode_RGMII; break;
		default: ret = SJA1105P_e_xmiiMode_SGMII; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_partNr_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_partNr_t
*/
SJA1105P_partNr_t SJA1105P_convertToPartNr(uint32_t stdintValue)
{
	SJA1105P_partNr_t ret;
	switch (stdintValue)
	{
		case 39559U: ret = SJA1105P_e_partNr_SJA1105S; break;
		case 39558U: ret = SJA1105P_e_partNr_SJA1105R; break;
		case 39557U: ret = SJA1105P_e_partNr_SJA1105Q; break;
		case 39556U: ret = SJA1105P_e_partNr_SJA1105P; break;
		case 39555U: ret = SJA1105P_e_partNr_SJA1105T; break;
		case 39554U: ret = SJA1105P_e_partNr_SJA1105; break;
		default: ret = SJA1105P_e_partNr_UNKNOWN; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_rgmiiSmpl_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_rgmiiSmpl_t
*/
SJA1105P_rgmiiSmpl_t SJA1105P_convertToRgmiiSmpl(uint32_t stdintValue)
{
	SJA1105P_rgmiiSmpl_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_rgmiiSmpl_RISING_AND_FALLING; break;
		case 2U: ret = SJA1105P_e_rgmiiSmpl_FALLING; break;
		case 1U: ret = SJA1105P_e_rgmiiSmpl_RISING; break;
		default: ret = SJA1105P_e_rgmiiSmpl_DISABLED; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_idFreqMode_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_idFreqMode_t
*/
SJA1105P_idFreqMode_t SJA1105P_convertToIdFreqMode(uint32_t stdintValue)
{
	SJA1105P_idFreqMode_t ret;
	switch (stdintValue)
	{
		case 0U: ret = SJA1105P_e_idFreqMode_F2P5; break;
		case 1U: ret = SJA1105P_e_idFreqMode_F25; break;
		case 2U: ret = SJA1105P_e_idFreqMode_F50; break;
		default: ret = SJA1105P_e_idFreqMode_F125; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_tempThreshold_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_tempThreshold_t
*/
SJA1105P_tempThreshold_t SJA1105P_convertToTempThreshold(uint32_t stdintValue)
{
	SJA1105P_tempThreshold_t ret;
	switch (stdintValue)
	{
		case 39U: ret = SJA1105P_e_tempThreshold_POSITIVE135P5; break;
		case 38U: ret = SJA1105P_e_tempThreshold_POSITIVE130P5; break;
		case 37U: ret = SJA1105P_e_tempThreshold_POSITIVE125P5; break;
		case 36U: ret = SJA1105P_e_tempThreshold_POSITIVE120P7; break;
		case 35U: ret = SJA1105P_e_tempThreshold_POSITIVE116P0; break;
		case 34U: ret = SJA1105P_e_tempThreshold_POSITIVE111P4; break;
		case 33U: ret = SJA1105P_e_tempThreshold_POSITIVE106P9; break;
		case 32U: ret = SJA1105P_e_tempThreshold_POSITIVE102P5; break;
		case 31U: ret = SJA1105P_e_tempThreshold_POSITIVE98P2; break;
		case 30U: ret = SJA1105P_e_tempThreshold_POSITIVE92P8; break;
		case 29U: ret = SJA1105P_e_tempThreshold_POSITIVE87P5; break;
		case 28U: ret = SJA1105P_e_tempThreshold_POSITIVE82P4; break;
		case 27U: ret = SJA1105P_e_tempThreshold_POSITIVE77P4; break;
		case 26U: ret = SJA1105P_e_tempThreshold_POSITIVE72P6; break;
		case 25U: ret = SJA1105P_e_tempThreshold_POSITIVE67P9; break;
		case 24U: ret = SJA1105P_e_tempThreshold_POSITIVE63P3; break;
		case 23U: ret = SJA1105P_e_tempThreshold_POSITIVE58P8; break;
		case 22U: ret = SJA1105P_e_tempThreshold_POSITIVE54P5; break;
		case 21U: ret = SJA1105P_e_tempThreshold_POSITIVE50P2; break;
		case 20U: ret = SJA1105P_e_tempThreshold_POSITIVE46P1; break;
		case 19U: ret = SJA1105P_e_tempThreshold_POSITIVE42P0; break;
		case 18U: ret = SJA1105P_e_tempThreshold_POSITIVE36P4; break;
		case 17U: ret = SJA1105P_e_tempThreshold_POSITIVE30P9; break;
		case 16U: ret = SJA1105P_e_tempThreshold_POSITIVE25P6; break;
		case 15U: ret = SJA1105P_e_tempThreshold_POSITIVE20P6; break;
		case 14U: ret = SJA1105P_e_tempThreshold_POSITIVE15P7; break;
		case 13U: ret = SJA1105P_e_tempThreshold_POSITIVE11P0; break;
		case 12U: ret = SJA1105P_e_tempThreshold_POSITIVE6P4; break;
		case 11U: ret = SJA1105P_e_tempThreshold_POSITIVE2P1; break;
		case 10U: ret = SJA1105P_e_tempThreshold_NEGATIVE2P1; break;
		case 9U: ret = SJA1105P_e_tempThreshold_NEGATIVE6P1; break;
		case 8U: ret = SJA1105P_e_tempThreshold_NEGATIVE11P4; break;
		case 7U: ret = SJA1105P_e_tempThreshold_NEGATIVE18P3; break;
		case 6U: ret = SJA1105P_e_tempThreshold_NEGATIVE23P5; break;
		case 5U: ret = SJA1105P_e_tempThreshold_NEGATIVE28P4; break;
		case 4U: ret = SJA1105P_e_tempThreshold_NEGATIVE33P0; break;
		case 3U: ret = SJA1105P_e_tempThreshold_NEGATIVE37P5; break;
		case 2U: ret = SJA1105P_e_tempThreshold_NEGATIVE41P7; break;
		case 1U: ret = SJA1105P_e_tempThreshold_NEGATIVE45P7; break;
		default: ret = SJA1105P_e_tempThreshold_NOT_VALID; break;
	}
	return ret;
}
