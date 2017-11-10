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
* \file NXP_SJA1105_auxiliaryConfigurationUnit.h
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

#ifndef NXP_SJA1105_AUXILIARYCONFIGURATIONUNIT_H
#define NXP_SJA1105_AUXILIARYCONFIGURATIONUNIT_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category cfg_pad_miix */
#define SJA1105_CFG_PAD_MIIX_PORT_INC (0x2)     
#define SJA1105_CFG_PAD_MIIX_ADDR     (0x100800)  /**< Address of the cfgPadMiix register */

/* register category cfg_pad_misc */
#define SJA1105_CFG_PAD_MISC_ADDR (0x100840)  /**< Address of the cfgPadMisc register */

/* register category cfg_pad_spi */
#define SJA1105_CFG_PAD_SPI_ADDR (0x100880)  /**< Address of the cfgPadSpi register */

/* register category cfg_pad_jtag */
#define SJA1105_CFG_PAD_JTAG_ADDR (0x100881)  /**< Address of the cfgPadJtag register */

/* register category port_status_miix */
#define SJA1105_PORT_S_MIIX_ADDR (0x100900)  /**< Address of the portStatusMiix register */

/* register category ts_config */
#define SJA1105_TS_C_ADDR (0x100a00)  /**< Address of the tsConfig register */

/* register category ts_status */
#define SJA1105_TS_S_ADDR (0x100a01)  /**< Address of the tsStatus register */

/* register category rgmii_meas_setup */
#define SJA1105_R_MEAS_S_ADDR (0x100a80)  /**< Address of the rgmiiMeasSetup register */

/* register category prod_cfg */
#define SJA1105_PROD_CFG_ADDR (0x100bc0)  /**< Address of the prodCfg register */

/* register category prod_id */
#define SJA1105_PROD_ID_ADDR (0x100bc3)  /**< Address of the prodId register */

/* variable masks */

/* register category cfg_pad_miix */
#define SJA1105_CFG_PAD_MIIX_D32_OS_SHIFT    (27)           /**< Shift value for the D32_OS field */
#define SJA1105_CFG_PAD_MIIX_D32_OS_MASK     (0x18000000U)  /**< Bit mask for the D32_OS field */
#define SJA1105_CFG_PAD_MIIX_D32_IH_SHIFT    (26)           /**< Shift value for the D32_IH field */
#define SJA1105_CFG_PAD_MIIX_D32_IH_MASK     (0x04000000U)  /**< Bit mask for the D32_IH field */
#define SJA1105_CFG_PAD_MIIX_D32_IPUD_SHIFT  (24)           /**< Shift value for the D32_IPUD field */
#define SJA1105_CFG_PAD_MIIX_D32_IPUD_MASK   (0x03000000U)  /**< Bit mask for the D32_IPUD field */
#define SJA1105_CFG_PAD_MIIX_D10_OS_SHIFT    (19)           /**< Shift value for the D10_OS field */
#define SJA1105_CFG_PAD_MIIX_D10_OS_MASK     (0x00180000U)  /**< Bit mask for the D10_OS field */
#define SJA1105_CFG_PAD_MIIX_D10_IH_SHIFT    (18)           /**< Shift value for the D10_IH field */
#define SJA1105_CFG_PAD_MIIX_D10_IH_MASK     (0x00040000U)  /**< Bit mask for the D10_IH field */
#define SJA1105_CFG_PAD_MIIX_D10_IPUD_SHIFT  (16)           /**< Shift value for the D10_IPUD field */
#define SJA1105_CFG_PAD_MIIX_D10_IPUD_MASK   (0x00030000U)  /**< Bit mask for the D10_IPUD field */
#define SJA1105_CFG_PAD_MIIX_CTRL_OS_SHIFT   (11)           /**< Shift value for the CTRL_OS field */
#define SJA1105_CFG_PAD_MIIX_CTRL_OS_MASK    (0x00001800U)  /**< Bit mask for the CTRL_OS field */
#define SJA1105_CFG_PAD_MIIX_CTRL_IH_SHIFT   (10)           /**< Shift value for the CTRL_IH field */
#define SJA1105_CFG_PAD_MIIX_CTRL_IH_MASK    (0x00000C00U)  /**< Bit mask for the CTRL_IH field */
#define SJA1105_CFG_PAD_MIIX_CTRL_IPUD_SHIFT (8)            /**< Shift value for the CTRL_IPUD field */
#define SJA1105_CFG_PAD_MIIX_CTRL_IPUD_MASK  (0x00000300U)  /**< Bit mask for the CTRL_IPUD field */
#define SJA1105_CFG_PAD_MIIX_CLK_OS_SHIFT    (3)            /**< Shift value for the CLK_OS field */
#define SJA1105_CFG_PAD_MIIX_CLK_OS_MASK     (0x00000018U)  /**< Bit mask for the CLK_OS field */
#define SJA1105_CFG_PAD_MIIX_CLK_IH_SHIFT    (2)            /**< Shift value for the CLK_IH field */
#define SJA1105_CFG_PAD_MIIX_CLK_IH_MASK     (0x00000004U)  /**< Bit mask for the CLK_IH field */
#define SJA1105_CFG_PAD_MIIX_CLK_IPUD_MASK   (0x00000003U)  /**< Bit mask for the CLK_IPUD field */

/* register category cfg_pad_misc */
#define SJA1105_CFG_PAD_MISC_PTPCLK_EN_SHIFT   (21)           /**< Shift value for the PTPCLK_EN field */
#define SJA1105_CFG_PAD_MISC_PTPCLK_EN_MASK    (0x00200000U)  /**< Bit mask for the PTPCLK_EN field */
#define SJA1105_CFG_PAD_MISC_PTPCLK_OS_SHIFT   (19)           /**< Shift value for the PTPCLK_OS field */
#define SJA1105_CFG_PAD_MISC_PTPCLK_OS_MASK    (0x00180000U)  /**< Bit mask for the PTPCLK_OS field */
#define SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_SHIFT (16)           /**< Shift value for the PTPCLK_IPUD field */
#define SJA1105_CFG_PAD_MISC_PTPCLK_IPUD_MASK  (0x00030000U)  /**< Bit mask for the PTPCLK_IPUD field */
#define SJA1105_CFG_PAD_MISC_RSTN_IH_SHIFT     (10)           /**< Shift value for the RSTN_IH field */
#define SJA1105_CFG_PAD_MISC_RSTN_IH_MASK      (0x00000400U)  /**< Bit mask for the RSTN_IH field */
#define SJA1105_CFG_PAD_MISC_RSTN_IPUD_SHIFT   (8)            /**< Shift value for the RSTN_IPUD field */
#define SJA1105_CFG_PAD_MISC_RSTN_IPUD_MASK    (0x00000300U)  /**< Bit mask for the RSTN_IPUD field */
#define SJA1105_CFG_PAD_MISC_CLKOUT_EN_SHIFT   (5)            /**< Shift value for the CLKOUT_EN field */
#define SJA1105_CFG_PAD_MISC_CLKOUT_EN_MASK    (0x00000020U)  /**< Bit mask for the CLKOUT_EN field */
#define SJA1105_CFG_PAD_MISC_CLKOUT_OS_SHIFT   (3)            /**< Shift value for the CLKOUT_OS field */
#define SJA1105_CFG_PAD_MISC_CLKOUT_OS_MASK    (0x00000018U)  /**< Bit mask for the CLKOUT_OS field */
#define SJA1105_CFG_PAD_MISC_CLKOUT_IPUD_MASK  (0x00000003U)  /**< Bit mask for the CLKOUT_IPUD field */

/* register category cfg_pad_spi */
#define SJA1105_CFG_PAD_SPI_SDO_OS_SHIFT   (27)           /**< Shift value for the SDO_OS field */
#define SJA1105_CFG_PAD_SPI_SDO_OS_MASK    (0x18000000U)  /**< Bit mask for the SDO_OS field */
#define SJA1105_CFG_PAD_SPI_SDO_IPUD_SHIFT (24)           /**< Shift value for the SDO_IPUD field */
#define SJA1105_CFG_PAD_SPI_SDO_IPUD_MASK  (0x03000000U)  /**< Bit mask for the SDO_IPUD field */
#define SJA1105_CFG_PAD_SPI_SDI_IH_SHIFT   (18)           /**< Shift value for the SDI_IH field */
#define SJA1105_CFG_PAD_SPI_SDI_IH_MASK    (0x00040000U)  /**< Bit mask for the SDI_IH field */
#define SJA1105_CFG_PAD_SPI_SDI_IPUD_SHIFT (16)           /**< Shift value for the SDI_IPUD field */
#define SJA1105_CFG_PAD_SPI_SDI_IPUD_MASK  (0x00030000U)  /**< Bit mask for the SDI_IPUD field */
#define SJA1105_CFG_PAD_SPI_SSN_IH_SHIFT   (10)           /**< Shift value for the SSN_IH field */
#define SJA1105_CFG_PAD_SPI_SSN_IH_MASK    (0x00000400U)  /**< Bit mask for the SSN_IH field */
#define SJA1105_CFG_PAD_SPI_SSN_IPUD_SHIFT (8)            /**< Shift value for the SSN_IPUD field */
#define SJA1105_CFG_PAD_SPI_SSN_IPUD_MASK  (0x00000300U)  /**< Bit mask for the SSN_IPUD field */
#define SJA1105_CFG_PAD_SPI_SCK_IH_SHIFT   (2)            /**< Shift value for the SCK_IH field */
#define SJA1105_CFG_PAD_SPI_SCK_IH_MASK    (0x00000004U)  /**< Bit mask for the SCK_IH field */
#define SJA1105_CFG_PAD_SPI_SCK_IPUD_MASK  (0x00000003U)  /**< Bit mask for the SCK_IPUD field */

/* register category cfg_pad_jtag */
#define SJA1105_CFG_PAD_JTAG_TDO_IPUD_SHIFT      (24)           /**< Shift value for the TDO_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TDO_IPUD_MASK       (0x03000000U)  /**< Bit mask for the TDO_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TDI_IPUD_SHIFT      (16)           /**< Shift value for the TDI_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TDI_IPUD_MASK       (0x00030000U)  /**< Bit mask for the TDI_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_SHIFT (8)            /**< Shift value for the TRSTNTMS_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TRSTNTMS_IPUD_MASK  (0x00000300U)  /**< Bit mask for the TRSTNTMS_IPUD field */
#define SJA1105_CFG_PAD_JTAG_TCK_IPUD_MASK       (0x00000003U)  /**< Bit mask for the TCK_IPUD field */

/* register category port_status_miix */
#define SJA1105_PORT_S_MIIX_SPEED_SHIFT    (3)            /**< Shift value for the SPEED field */
#define SJA1105_PORT_S_MIIX_SPEED_MASK     (0x00000018U)  /**< Bit mask for the SPEED field */
#define SJA1105_PORT_S_MIIX_PHY_MODE_SHIFT (2)            /**< Shift value for the PHY_MODE field */
#define SJA1105_PORT_S_MIIX_PHY_MODE_MASK  (0x00000004U)  /**< Bit mask for the PHY_MODE field */
#define SJA1105_PORT_S_MIIX_XMII_MODE_MASK (0x00000003U)  /**< Bit mask for the XMII_MODE field */

/* register category ts_config */
#define SJA1105_TS_C_PD_SHIFT       (6)            /**< Shift value for the PD field */
#define SJA1105_TS_C_PD_MASK        (0x00000040U)  /**< Bit mask for the PD field */
#define SJA1105_TS_C_THRESHOLD_MASK (0x0000003FU)  /**< Bit mask for the THRESHOLD field */

/* register category ts_status */
#define SJA1105_TS_S_EXCEEDED_MASK (0x00000001U)  /**< Bit mask for the EXCEEDED field */

/* register category rgmii_meas_setup */
#define SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_SHIFT (8)            /**< Shift value for the MII4_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII4_RX_SMPL_EN_MASK  (0x00000300U)  /**< Bit mask for the MII4_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_SHIFT (6)            /**< Shift value for the MII3_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII3_RX_SMPL_EN_MASK  (0x000000C0U)  /**< Bit mask for the MII3_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_SHIFT (4)            /**< Shift value for the MII2_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII2_RX_SMPL_EN_MASK  (0x00000030U)  /**< Bit mask for the MII2_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_SHIFT (2)            /**< Shift value for the MII1_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII1_RX_SMPL_EN_MASK  (0x0000000CU)  /**< Bit mask for the MII1_RX_SMPL_EN field */
#define SJA1105_R_MEAS_S_MII0_RX_SMPL_EN_MASK  (0x00000003U)  /**< Bit mask for the MII0_RX_SMPL_EN field */

/* register category prod_cfg */
#define SJA1105_PROD_CFG_DISABLE_TTETH_MASK (0x00000001U)  /**< Bit mask for the DISABLE_TTETH field */

/* register category prod_id */
#define SJA1105_PROD_ID_PART_NR_SHIFT (4)            /**< Shift value for the PART_NR field */
#define SJA1105_PROD_ID_VERSION_MASK  (0x0000000FU)  /**< Bit mask for the VERSION field */

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	e_direction_RX = 1,  /**< Receive */
	e_direction_TX = 0   /**< Transmit */
} SJA1105_direction_t;

typedef enum 
{
	e_padOutputStageSpeed_HIGH_SPEED   = 3,  /**< High Noise/High Speed */
	e_padOutputStageSpeed_FAST_SPEED   = 2,  /**< Medium noise/Fast Speed */
	e_padOutputStageSpeed_MEDIUM_SPEED = 1,  /**< Low noise/Medium Speed */
	e_padOutputStageSpeed_LOW_SPEED    = 0   /**< Very low noise/Low Speed */
} SJA1105_padOutputStageSpeed_t;

typedef enum 
{
	e_padInputStageSelection_PULL_DOWN   = 3,  /**< Pull-down */
	e_padInputStageSelection_PLAIN_INPUT = 2,  /**< Plain input */
	e_padInputStageSelection_REPEATER    = 1,  /**< Repeater */
	e_padInputStageSelection_PULL_UP     = 0   /**< Pull-up */
} SJA1105_padInputStageSelection_t;

typedef enum 
{
	e_tempThreshold_POSITIVE135P5 = 39,  /**< 135.5 degrees Celsius */
	e_tempThreshold_POSITIVE130P5 = 38,  /**< 130.5 degrees Celsius */
	e_tempThreshold_POSITIVE125P5 = 37,  /**< 125.5 degrees Celsius */
	e_tempThreshold_POSITIVE120P7 = 36,  /**< 120.7 degrees Celsius */
	e_tempThreshold_POSITIVE116P0 = 35,  /**< 116.0 degrees Celsius */
	e_tempThreshold_POSITIVE111P4 = 34,  /**< 111.4 degrees Celsius */
	e_tempThreshold_POSITIVE106P9 = 33,  /**< 106.9 degrees Celsius */
	e_tempThreshold_POSITIVE102P5 = 32,  /**< 102.5 degrees Celsius */
	e_tempThreshold_POSITIVE98P2  = 31,  /**< 98.2 degrees Celsius */
	e_tempThreshold_POSITIVE92P8  = 30,  /**< 92.8 degrees Celsius */
	e_tempThreshold_POSITIVE87P5  = 29,  /**< 87.5 degrees Celsius */
	e_tempThreshold_POSITIVE82P4  = 28,  /**< 82.4 degrees Celsius */
	e_tempThreshold_POSITIVE77P4  = 27,  /**< 77.4 degrees Celsius */
	e_tempThreshold_POSITIVE72P6  = 26,  /**< 72.6 degrees Celsius */
	e_tempThreshold_POSITIVE67P9  = 25,  /**< 67.9 degrees Celsius */
	e_tempThreshold_POSITIVE63P3  = 24,  /**< 63.3 degrees Celsius */
	e_tempThreshold_POSITIVE58P8  = 23,  /**< 58.8 degrees Celsius */
	e_tempThreshold_POSITIVE54P5  = 22,  /**< 54.5 degrees Celsius */
	e_tempThreshold_POSITIVE50P2  = 21,  /**< 50.2 degrees Celsius */
	e_tempThreshold_POSITIVE46P1  = 20,  /**< 46.1 degrees Celsius */
	e_tempThreshold_POSITIVE42P0  = 19,  /**< 42.0 degrees Celsius */
	e_tempThreshold_POSITIVE36P4  = 18,  /**< 36.4 degrees Celsius */
	e_tempThreshold_POSITIVE30P9  = 17,  /**< 30.9 degrees Celsius */
	e_tempThreshold_POSITIVE25P6  = 16,  /**< 25.6 degrees Celsius */
	e_tempThreshold_POSITIVE20P6  = 15,  /**< 20.6 degrees Celsius */
	e_tempThreshold_POSITIVE15P7  = 14,  /**< 15.7 degrees Celsius */
	e_tempThreshold_POSITIVE11P0  = 13,  /**< 11.0 degrees Celsius */
	e_tempThreshold_POSITIVE6P4   = 12,  /**< 6.5 degrees Celsius */
	e_tempThreshold_POSITIVE2P1   = 11,  /**< 2.1 degrees Celsius */
	e_tempThreshold_NEGATIVE2P1   = 10,  /**< -2.1 degrees Celsius */
	e_tempThreshold_NEGATIVE6P1   = 9,   /**< -6.1 degrees Celsius */
	e_tempThreshold_NEGATIVE11P4  = 8,   /**< -11.4 degrees Celsius */
	e_tempThreshold_NEGATIVE18P3  = 7,   /**< -18.3 degrees Celsius */
	e_tempThreshold_NEGATIVE23P5  = 6,   /**< -23.5 degrees Celsius */
	e_tempThreshold_NEGATIVE28P4  = 5,   /**< -28.4 degrees Celsius */
	e_tempThreshold_NEGATIVE33P0  = 4,   /**< -33.0 degrees Celsius */
	e_tempThreshold_NEGATIVE37P5  = 3,   /**< -37.5 degrees Celsius */
	e_tempThreshold_NEGATIVE41P7  = 2,   /**< -41.7 degrees Celsius */
	e_tempThreshold_NEGATIVE45P7  = 1,   /**< -45.7 degrees Celsius */
	e_tempThreshold_NOT_VALID     = 0    /**< No valid output */
} SJA1105_tempThreshold_t;

typedef enum 
{
	e_xmiiMode_MII    = 0,  /**< MII interface mode */
	e_xmiiMode_RMII   = 1,  /**< RMII interface mode */
	e_xmiiMode_RGMII  = 2,  /**< RGMII interface mode */
	e_xmiiMode_UNUSED = 3   /**< Unused */
} SJA1105_xmiiMode_t;

typedef enum 
{
	e_padInputStageHysteresis_SCHMITT     = 1,  /**< Schmitt */
	e_padInputStageHysteresis_NON_SCHMITT = 0   /**< non Schmitt */
} SJA1105_padInputStageHysteresis_t;

typedef enum 
{
	e_rgmiiSmpl_RISING_AND_FALLING = 3,  /**< falling-edge enabled; rising-edge enabled */
	e_rgmiiSmpl_FALLING            = 2,  /**< falling-edge enabled; rising-edge disabled */
	e_rgmiiSmpl_RISING             = 1,  /**< falling-edge disabled; rising-edge enabled */
	e_rgmiiSmpl_DISABLED           = 0   /**< falling-edge disabled; rising-edge disabled */
} SJA1105_rgmiiSmpl_t;

typedef enum 
{
	e_speed_10_MBPS  = 0,  /**< 10 Mbps */
	e_speed_100_MBPS = 1,  /**< 100 Mbps */
	e_speed_1_GBPS   = 2   /**< 1 Gbps */
} SJA1105_speed_t;

typedef struct
{
	SJA1105_padOutputStageSpeed_t     d32Os;     /**< TXD3 and TXD2 pad output stage speed selection */
	SJA1105_padInputStageHysteresis_t d32Ih;     /**< RXD2 and RXD3 pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  d32Ipud;   /**< TXD2/RXD2 and TXD3/RXD3 pad input stage (weak) pull-up/pull-down selection */
	SJA1105_padOutputStageSpeed_t     d10Os;     /**< TXD1 and TXD0 pad output stage speed selection */
	SJA1105_padInputStageHysteresis_t d10Ih;     /**< RXD1 and RXD0 pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  d10Ipud;   /**< TXD0/RXD0 and TXD1/RXD1 pad input stage (weak) pull-up/pull-down selection */
	SJA1105_padOutputStageSpeed_t     ctrlOs;    /**< Tx only - CTRL_EHS pad output stage speed selection */
	SJA1105_padInputStageHysteresis_t ctrlIh;    /**< RX_DV/CRS_DV/RX_CTL and RX_ER pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  ctrlIpud;  /**< CTRL_EHS pad input stage (weak) pull-up/pull-down selection */
	SJA1105_padOutputStageSpeed_t     clkOs;     /**< CLK_EHS pad output stage speed selection */
	SJA1105_padInputStageHysteresis_t clkIh;     /**< CLK_EHS pad input stage hysteresis selection */
	SJA1105_padInputStageSelection_t  clkIpud;   /**< CLK_EHS pad input stage (weak) pull-up/-down selection */
} SJA1105_cfgPadMiixArgument_t;

typedef struct
{
	uint8_t                           ptpclkEn;    /**< PTP_CLK pad output stage enable (active low) */
	SJA1105_padOutputStageSpeed_t     ptpclkOs;    /**< PTP_CLK pad output stage speed selection */
	SJA1105_padInputStageSelection_t  ptpclkIpud;  /**< PTP_CLK pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageHysteresis_t rstnIh;      /**< RST_N pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  rstnIpud;    /**< RST_N pad input stage (weak) pull-up/pull-down */
	uint8_t                           clkoutEn;    /**< CLK_OUT pad output stage enable (active low) */
	SJA1105_padOutputStageSpeed_t     clkoutOs;    /**< CLK_OUT pad output stage speed selection */
	SJA1105_padInputStageSelection_t  clkoutIpud;  /**< CLK_OUT pad input stage (weak) pull-up/pull-down */
} SJA1105_cfgPadMiscArgument_t;

typedef struct
{
	SJA1105_padOutputStageSpeed_t     sdoOs;    /**< SDO pad output stage speed selection */
	SJA1105_padInputStageSelection_t  sdoIpud;  /**< SDO pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageHysteresis_t sdiIh;    /**< SDI pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  sdiIpud;  /**< SDI pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageHysteresis_t ssnIh;    /**< SSN pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  ssnIpud;  /**< SSN pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageHysteresis_t sckIh;    /**< SCK pad input stage hysteresis */
	SJA1105_padInputStageSelection_t  sckIpud;  /**< SCK pad input stage (weak) pull-up/pull-down */
} SJA1105_cfgPadSpiArgument_t;

typedef struct
{
	SJA1105_padInputStageSelection_t tdoIpud;       /**< TDO pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageSelection_t tdiIpud;       /**< TDI pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageSelection_t trstntmsIpud;  /**< TRSTNTMS_IPUD pad input stage (weak) pull-up/pull-down */
	SJA1105_padInputStageSelection_t tckIpud;       /**< TCK pad input stage (weak) pull-up/pull-down */
} SJA1105_cfgPadJtagArgument_t;

typedef struct
{
	SJA1105_speed_t    speed;     /**< Port speed setting */
	uint8_t            phyMode;   /**< When set, port is in PHY mode, else MAC mode */
	SJA1105_xmiiMode_t xmiiMode;  /**< Port xMII mode setting */
} SJA1105_portStatusMiixArgument_t;

typedef struct
{
	uint8_t                 pd;         /**< Temperature sensor power-down control {0: active, 1: power-down} */
	SJA1105_tempThreshold_t threshold;  /**< temperature threshold selection (valid values in range 01h to 39h) */
} SJA1105_tsConfigArgument_t;

typedef struct
{
	SJA1105_rgmiiSmpl_t mii4RxSmplEn;  /**< RGMII4 input sampling enable (data and control) */
	SJA1105_rgmiiSmpl_t mii3RxSmplEn;  /**< RGMII3 input sampling enable (data and control) */
	SJA1105_rgmiiSmpl_t mii2RxSmplEn;  /**< RGMII2 input sampling enable (data and control) */
	SJA1105_rgmiiSmpl_t mii1RxSmplEn;  /**< RGMII1 input sampling enable (data and control) */
	SJA1105_rgmiiSmpl_t mii0RxSmplEn;  /**< RGMII0 input sampling enable (data and control) */
} SJA1105_rgmiiMeasSetupArgument_t;

typedef struct
{
	uint16_t partNr;   /**< Part number */
	uint8_t  version;  /**< Version number */
} SJA1105_prodIdArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category cfg_pad_miix */
extern uint8_t SJA1105_setCfgPadMiix(const SJA1105_cfgPadMiixArgument_t *pk_cfgPadMiix, uint8_t port, SJA1105_direction_t direction, uint8_t deviceSelect);
extern uint8_t SJA1105_getCfgPadMiix(SJA1105_cfgPadMiixArgument_t *p_cfgPadMiix, uint8_t port, SJA1105_direction_t direction, uint8_t deviceSelect);

/* register category cfg_pad_misc */
extern uint8_t SJA1105_setCfgPadMisc(const SJA1105_cfgPadMiscArgument_t *pk_cfgPadMisc, uint8_t deviceSelect);
extern uint8_t SJA1105_getCfgPadMisc(SJA1105_cfgPadMiscArgument_t *p_cfgPadMisc, uint8_t deviceSelect);

/* register category cfg_pad_spi */
extern uint8_t SJA1105_setCfgPadSpi(const SJA1105_cfgPadSpiArgument_t *pk_cfgPadSpi, uint8_t deviceSelect);
extern uint8_t SJA1105_getCfgPadSpi(SJA1105_cfgPadSpiArgument_t *p_cfgPadSpi, uint8_t deviceSelect);

/* register category cfg_pad_jtag */
extern uint8_t SJA1105_setCfgPadJtag(const SJA1105_cfgPadJtagArgument_t *pk_cfgPadJtag, uint8_t deviceSelect);
extern uint8_t SJA1105_getCfgPadJtag(SJA1105_cfgPadJtagArgument_t *p_cfgPadJtag, uint8_t deviceSelect);

/* register category port_status_miix */
extern uint8_t SJA1105_getPortStatusMiix(SJA1105_portStatusMiixArgument_t *p_portStatusMiix, uint8_t port, uint8_t deviceSelect);

/* register category ts_config */
extern uint8_t SJA1105_setTsConfig(const SJA1105_tsConfigArgument_t *pk_tsConfig, uint8_t deviceSelect);
extern uint8_t SJA1105_getTsConfig(SJA1105_tsConfigArgument_t *p_tsConfig, uint8_t deviceSelect);

/* register category ts_status */
extern uint8_t SJA1105_getTsStatus(uint8_t *p_exceeded, uint8_t deviceSelect);

/* register category rgmii_meas_setup */
extern uint8_t SJA1105_setRgmiiMeasSetup(const SJA1105_rgmiiMeasSetupArgument_t *pk_rgmiiMeasSetup, uint8_t deviceSelect);
extern uint8_t SJA1105_getRgmiiMeasSetup(SJA1105_rgmiiMeasSetupArgument_t *p_rgmiiMeasSetup, uint8_t deviceSelect);

/* register category prod_cfg */
extern uint8_t SJA1105_getProdCfg(uint8_t *p_disableTteth, uint8_t deviceSelect);

/* register category prod_id */
extern uint8_t SJA1105_getProdId(SJA1105_prodIdArgument_t *p_prodId, uint8_t deviceSelect);

extern SJA1105_padOutputStageSpeed_t SJA1105_convertToPadOutputStageSpeed(uint32_t stdintValue);
extern SJA1105_padInputStageSelection_t SJA1105_convertToPadInputStageSelection(uint32_t stdintValue);
extern SJA1105_speed_t SJA1105_convertToSpeed(uint32_t stdintValue);
extern SJA1105_xmiiMode_t SJA1105_convertToXmiiMode(uint32_t stdintValue);
extern SJA1105_padInputStageHysteresis_t SJA1105_convertToPadInputStageHysteresis(uint32_t stdintValue);
extern SJA1105_rgmiiSmpl_t SJA1105_convertToRgmiiSmpl(uint32_t stdintValue);
extern SJA1105_tempThreshold_t SJA1105_convertToTempThreshold(uint32_t stdintValue);

#endif  /* NXP_SJA1105_AUXILIARYCONFIGURATIONUNIT_H */
