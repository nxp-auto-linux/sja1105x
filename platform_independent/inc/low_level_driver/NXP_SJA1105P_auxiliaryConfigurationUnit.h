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
* \file NXP_SJA1105P_auxiliaryConfigurationUnit.h
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

#ifndef NXP_SJA1105P_AUXILIARYCONFIGURATIONUNIT_H
#define NXP_SJA1105P_AUXILIARYCONFIGURATIONUNIT_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_direction_RX = 1,  /**< Receive */
	SJA1105P_e_direction_TX = 0   /**< Transmit */
} SJA1105P_direction_t;

typedef enum 
{
	SJA1105P_e_padOutputStageSpeed_HIGH_SPEED   = 3,  /**< High Noise/High Speed */
	SJA1105P_e_padOutputStageSpeed_FAST_SPEED   = 2,  /**< Medium noise/Fast Speed */
	SJA1105P_e_padOutputStageSpeed_MEDIUM_SPEED = 1,  /**< Low noise/Medium Speed */
	SJA1105P_e_padOutputStageSpeed_LOW_SPEED    = 0   /**< Very low noise/Low Speed */
} SJA1105P_padOutputStageSpeed_t;

typedef enum 
{
	SJA1105P_e_padInputStageSelection_PULL_DOWN   = 3,  /**< Pull-down */
	SJA1105P_e_padInputStageSelection_PLAIN_INPUT = 2,  /**< Plain input */
	SJA1105P_e_padInputStageSelection_REPEATER    = 1,  /**< Repeater */
	SJA1105P_e_padInputStageSelection_PULL_UP     = 0   /**< Pull-up */
} SJA1105P_padInputStageSelection_t;

typedef enum 
{
	SJA1105P_e_padInputHysteresis_SCHMITT     = 1,  /**< Schmitt */
	SJA1105P_e_padInputHysteresis_NON_SCHMITT = 0   /**< non Schmitt */
} SJA1105P_padInputHysteresis_t;

typedef enum 
{
	SJA1105P_e_tempThreshold_POSITIVE135P5 = 39,  /**< 135.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE130P5 = 38,  /**< 130.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE125P5 = 37,  /**< 125.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE120P7 = 36,  /**< 120.7 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE116P0 = 35,  /**< 116.0 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE111P4 = 34,  /**< 111.4 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE106P9 = 33,  /**< 106.9 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE102P5 = 32,  /**< 102.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE98P2  = 31,  /**< 98.2 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE92P8  = 30,  /**< 92.8 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE87P5  = 29,  /**< 87.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE82P4  = 28,  /**< 82.4 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE77P4  = 27,  /**< 77.4 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE72P6  = 26,  /**< 72.6 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE67P9  = 25,  /**< 67.9 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE63P3  = 24,  /**< 63.3 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE58P8  = 23,  /**< 58.8 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE54P5  = 22,  /**< 54.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE50P2  = 21,  /**< 50.2 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE46P1  = 20,  /**< 46.1 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE42P0  = 19,  /**< 42.0 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE36P4  = 18,  /**< 36.4 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE30P9  = 17,  /**< 30.9 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE25P6  = 16,  /**< 25.6 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE20P6  = 15,  /**< 20.6 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE15P7  = 14,  /**< 15.7 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE11P0  = 13,  /**< 11.0 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE6P4   = 12,  /**< 6.5 degrees Celsius */
	SJA1105P_e_tempThreshold_POSITIVE2P1   = 11,  /**< 2.1 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE2P1   = 10,  /**< -2.1 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE6P1   = 9,   /**< -6.1 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE11P4  = 8,   /**< -11.4 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE18P3  = 7,   /**< -18.3 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE23P5  = 6,   /**< -23.5 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE28P4  = 5,   /**< -28.4 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE33P0  = 4,   /**< -33.0 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE37P5  = 3,   /**< -37.5 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE41P7  = 2,   /**< -41.7 degrees Celsius */
	SJA1105P_e_tempThreshold_NEGATIVE45P7  = 1,   /**< -45.7 degrees Celsius */
	SJA1105P_e_tempThreshold_NOT_VALID     = 0    /**< No valid output */
} SJA1105P_tempThreshold_t;

typedef enum 
{
	SJA1105P_e_xmiiMode_MII   = 0,  /**< MII interface mode */
	SJA1105P_e_xmiiMode_RMII  = 1,  /**< RMII interface mode */
	SJA1105P_e_xmiiMode_RGMII = 2,  /**< RGMII interface mode */
	SJA1105P_e_xmiiMode_SGMII = 3   /**< SGMII interface mode */
} SJA1105P_xmiiMode_t;

typedef enum 
{
	SJA1105P_e_idCtlMode_BYPASS         = 0,  /**< Bypass mode */
	SJA1105P_e_idCtlMode_DELAYED        = 1,  /**< Delayed clock */
	SJA1105P_e_idCtlMode_OUPUT_DISABLED = 2   /**< Output clock disabled */
} SJA1105P_idCtlMode_t;

typedef enum 
{
	SJA1105P_e_partNr_SJA1105S = 39559,  /**< SJA1105S (plus feature set, SGMII, TT) */
	SJA1105P_e_partNr_SJA1105R = 39558,  /**< SJA1105R (plus feature set, SGMII) */
	SJA1105P_e_partNr_SJA1105Q = 39557,  /**< SJA1105Q (plus feature set, TT) */
	SJA1105P_e_partNr_SJA1105P = 39556,  /**< SJA1105P (plus feature set) */
	SJA1105P_e_partNr_SJA1105T = 39555,  /**< SJA1105T (TT) */
	SJA1105P_e_partNr_SJA1105  = 39554,  /**< SJA1105 */
	SJA1105P_e_partNr_UNKNOWN  = 0       /**< Unknown device */
} SJA1105P_partNr_t;

typedef enum 
{
	SJA1105P_e_rgmiiSmpl_RISING_AND_FALLING = 3,  /**< falling-edge enabled; rising-edge enabled */
	SJA1105P_e_rgmiiSmpl_FALLING            = 2,  /**< falling-edge enabled; rising-edge disabled */
	SJA1105P_e_rgmiiSmpl_RISING             = 1,  /**< falling-edge disabled; rising-edge enabled */
	SJA1105P_e_rgmiiSmpl_DISABLED           = 0   /**< falling-edge disabled; rising-edge disabled */
} SJA1105P_rgmiiSmpl_t;

typedef enum 
{
	SJA1105P_e_idFreqMode_F2P5 = 0,  /**< 2.5 MHz */
	SJA1105P_e_idFreqMode_F25  = 1,  /**< 25 MHz */
	SJA1105P_e_idFreqMode_F50  = 2,  /**< 55 MHz */
	SJA1105P_e_idFreqMode_F125 = 3   /**< 125 MHz */
} SJA1105P_idFreqMode_t;

typedef enum 
{
	SJA1105P_e_speed_10_MBPS  = 0,  /**< 10 Mbps */
	SJA1105P_e_speed_100_MBPS = 1,  /**< 100 Mbps */
	SJA1105P_e_speed_1_GBPS   = 2   /**< 1 Gbps */
} SJA1105P_speed_t;

typedef struct
{
	SJA1105P_padOutputStageSpeed_t    d32Os;     /**< TXD3 and TXD2 pad output stage speed selection */
	SJA1105P_padInputHysteresis_t     d32Ih;     /**< RXD2 and RXD3 pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t d32Ipud;   /**< TXD2/RXD2 and TXD3/RXD3 pad input stage (weak) pull-up/pull-down selection */
	SJA1105P_padOutputStageSpeed_t    d10Os;     /**< TXD1 and TXD0 pad output stage speed selection */
	SJA1105P_padInputHysteresis_t     d10Ih;     /**< RXD1 and RXD0 pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t d10Ipud;   /**< TXD0/RXD0 and TXD1/RXD1 pad input stage (weak) pull-up/pull-down selection */
	SJA1105P_padOutputStageSpeed_t    ctrlOs;    /**< Tx only - CTRL_EHS pad output stage speed selection */
	SJA1105P_padInputHysteresis_t     ctrlIh;    /**< RX_DV/CRS_DV/RX_CTL and RX_ER pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t ctrlIpud;  /**< CTRL_EHS pad input stage (weak) pull-up/pull-down selection */
	SJA1105P_padOutputStageSpeed_t    clkOs;     /**< CLK_EHS pad output stage speed selection */
	SJA1105P_padInputHysteresis_t     clkIh;     /**< CLK_EHS pad input stage hysteresis selection */
	SJA1105P_padInputStageSelection_t clkIpud;   /**< CLK_EHS pad input stage (weak) pull-up/-down selection */
} SJA1105P_cfgPadMiixArgument_t;

typedef struct
{
	uint8_t rxcDelay;   /**< RXC Internal Delay value (0x08 == 81 degrees), step size 0.9 degrees */
	uint8_t rxcBypass;  /**< RXC Internal Delay bypass (when set) */
	uint8_t rxcPd;      /**< RXC Internal Delay power-down (when set) */
	uint8_t txcDelay;   /**< TXC Internal Delay value (0x08 == 81 degrees), step size 0.9 degrees */
	uint8_t txcBypass;  /**< TXC Internal Delay bypass (when set) */
	uint8_t txcPd;      /**< TXC Internal Delay power-down (when set) */
} SJA1105P_cfgPadMiixIdArgument_t;

typedef struct
{
	SJA1105P_padOutputStageSpeed_t    ptpclkOs;    /**< PTP_CLK pad output stage speed selection */
	SJA1105P_padInputHysteresis_t     ptpclkIh;    /**< PTP_CLK pad input stage hysteresis selection */
	SJA1105P_padInputStageSelection_t ptpclkIpud;  /**< PTP_CLK pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputHysteresis_t     rstnIh;      /**< RST_N pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t rstnIpud;    /**< RST_N pad input stage (weak) pull-up/pull-down */
	uint8_t                           clkoutEn;    /**< CLK_OUT pad output stage enable (active low) */
	SJA1105P_padOutputStageSpeed_t    clkoutOs;    /**< CLK_OUT pad output stage speed selection */
	SJA1105P_padInputStageSelection_t clkoutIpud;  /**< CLK_OUT pad input stage (weak) pull-up/pull-down */
} SJA1105P_cfgPadMiscArgument_t;

typedef struct
{
	SJA1105P_padOutputStageSpeed_t    sdoOs;    /**< SDO pad output stage speed selection */
	SJA1105P_padInputStageSelection_t sdoIpud;  /**< SDO pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputHysteresis_t     sdiIh;    /**< SDI pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t sdiIpud;  /**< SDI pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputHysteresis_t     ssnIh;    /**< SSN pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t ssnIpud;  /**< SSN pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputHysteresis_t     sckIh;    /**< SCK pad input stage hysteresis */
	SJA1105P_padInputStageSelection_t sckIpud;  /**< SCK pad input stage (weak) pull-up/pull-down */
} SJA1105P_cfgPadSpiArgument_t;

typedef struct
{
	SJA1105P_padInputStageSelection_t tdoIpud;       /**< TDO pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputStageSelection_t tdiIpud;       /**< TDI pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputStageSelection_t trstntmsIpud;  /**< TRSTNTMS_IPUD pad input stage (weak) pull-up/pull-down */
	SJA1105P_padInputStageSelection_t tckIpud;       /**< TCK pad input stage (weak) pull-up/pull-down */
} SJA1105P_cfgPadJtagArgument_t;

typedef struct
{
	SJA1105P_idFreqMode_t rxcIdFreqMode;   /**< RXC Internal Delay frequency mode selected */
	SJA1105P_idCtlMode_t  rxcIdCtlMode;    /**< RXC Internal Delay operating mode */
	uint8_t               rxcDelayStable;  /**< RXC Internal Delay started-up */
	uint8_t               rxcDelayPd;      /**< RXC Internal Delay power-down (when set) */
	SJA1105P_idFreqMode_t txcIdFreqMode;   /**< TXC Internal Delay frequency mode selected */
	SJA1105P_idCtlMode_t  txcIdCtlMode;    /**< TXC Internal Delay operating mode */
	uint8_t               txcDelayStable;  /**< TXC Internal Delay started-up */
	uint8_t               txcDelayPd;      /**< TXC Internal Delay power-down (when set) */
	SJA1105P_speed_t      speed;           /**< Port speed setting */
	uint8_t               phyMode;         /**< When set, port is in PHY mode, else MAC mode */
	SJA1105P_xmiiMode_t   xmiiMode;        /**< Port xMII mode setting */
} SJA1105P_portStatusMiixArgument_t;

typedef struct
{
	uint8_t                  pd;         /**< Temperature sensor power-down control {0: active, 1: power-down} */
	SJA1105P_tempThreshold_t threshold;  /**< temperature threshold selection (valid values in range 01h to 39h) */
} SJA1105P_tsConfigArgument_t;

typedef struct
{
	SJA1105P_rgmiiSmpl_t mii4RxSmplEn;  /**< RGMII4 input sampling enable (data and control) */
	SJA1105P_rgmiiSmpl_t mii3RxSmplEn;  /**< RGMII3 input sampling enable (data and control) */
	SJA1105P_rgmiiSmpl_t mii2RxSmplEn;  /**< RGMII2 input sampling enable (data and control) */
	SJA1105P_rgmiiSmpl_t mii1RxSmplEn;  /**< RGMII1 input sampling enable (data and control) */
	SJA1105P_rgmiiSmpl_t mii0RxSmplEn;  /**< RGMII0 input sampling enable (data and control) */
} SJA1105P_rgmiiMeasSetupArgument_t;

typedef struct
{
	SJA1105P_partNr_t partNr;   /**< Part number */
	uint8_t           version;  /**< Version number */
} SJA1105P_prodIdArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category cfg_pad_miix */
extern uint8_t SJA1105P_setCfgPadMiix(const SJA1105P_cfgPadMiixArgument_t *pk_cfgPadMiix, uint8_t port, SJA1105P_direction_t direction, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCfgPadMiix(SJA1105P_cfgPadMiixArgument_t *p_cfgPadMiix, uint8_t port, SJA1105P_direction_t direction, uint8_t deviceSelect);

/* register category cfg_pad_miix_id */
extern uint8_t SJA1105P_setCfgPadMiixId(const SJA1105P_cfgPadMiixIdArgument_t *pk_cfgPadMiixId, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCfgPadMiixId(SJA1105P_cfgPadMiixIdArgument_t *p_cfgPadMiixId, uint8_t port, uint8_t deviceSelect);

/* register category cfg_pad_misc */
extern uint8_t SJA1105P_setCfgPadMisc(const SJA1105P_cfgPadMiscArgument_t *pk_cfgPadMisc, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCfgPadMisc(SJA1105P_cfgPadMiscArgument_t *p_cfgPadMisc, uint8_t deviceSelect);

/* register category cfg_pad_spi */
extern uint8_t SJA1105P_setCfgPadSpi(const SJA1105P_cfgPadSpiArgument_t *pk_cfgPadSpi, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCfgPadSpi(SJA1105P_cfgPadSpiArgument_t *p_cfgPadSpi, uint8_t deviceSelect);

/* register category cfg_pad_jtag */
extern uint8_t SJA1105P_setCfgPadJtag(const SJA1105P_cfgPadJtagArgument_t *pk_cfgPadJtag, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCfgPadJtag(SJA1105P_cfgPadJtagArgument_t *p_cfgPadJtag, uint8_t deviceSelect);

/* register category port_status_miix */
extern uint8_t SJA1105P_getPortStatusMiix(SJA1105P_portStatusMiixArgument_t *p_portStatusMiix, uint8_t port, uint8_t deviceSelect);

/* register category ts_config */
extern uint8_t SJA1105P_setTsConfig(const SJA1105P_tsConfigArgument_t *pk_tsConfig, uint8_t deviceSelect);
extern uint8_t SJA1105P_getTsConfig(SJA1105P_tsConfigArgument_t *p_tsConfig, uint8_t deviceSelect);

/* register category ts_status */
extern uint8_t SJA1105P_getTsStatus(uint8_t *p_exceeded, uint8_t deviceSelect);

/* register category rgmii_meas_setup */
extern uint8_t SJA1105P_setRgmiiMeasSetup(const SJA1105P_rgmiiMeasSetupArgument_t *pk_rgmiiMeasSetup, uint8_t deviceSelect);
extern uint8_t SJA1105P_getRgmiiMeasSetup(SJA1105P_rgmiiMeasSetupArgument_t *p_rgmiiMeasSetup, uint8_t deviceSelect);

/* register category prod_cfg */
extern uint8_t SJA1105P_getProdCfg(uint8_t *p_disableTteth, uint8_t deviceSelect);

/* register category prod_id */
extern uint8_t SJA1105P_getProdId(SJA1105P_prodIdArgument_t *p_prodId, uint8_t deviceSelect);

extern SJA1105P_padOutputStageSpeed_t SJA1105P_convertToPadOutputStageSpeed(uint32_t stdintValue);
extern SJA1105P_padInputStageSelection_t SJA1105P_convertToPadInputStageSelection(uint32_t stdintValue);
extern SJA1105P_speed_t SJA1105P_convertToSpeed(uint32_t stdintValue);
extern SJA1105P_idCtlMode_t SJA1105P_convertToIdCtlMode(uint32_t stdintValue);
extern SJA1105P_padInputHysteresis_t SJA1105P_convertToPadInputHysteresis(uint32_t stdintValue);
extern SJA1105P_xmiiMode_t SJA1105P_convertToXmiiMode(uint32_t stdintValue);
extern SJA1105P_partNr_t SJA1105P_convertToPartNr(uint32_t stdintValue);
extern SJA1105P_rgmiiSmpl_t SJA1105P_convertToRgmiiSmpl(uint32_t stdintValue);
extern SJA1105P_idFreqMode_t SJA1105P_convertToIdFreqMode(uint32_t stdintValue);
extern SJA1105P_tempThreshold_t SJA1105P_convertToTempThreshold(uint32_t stdintValue);

#endif  /* NXP_SJA1105P_AUXILIARYCONFIGURATIONUNIT_H */
