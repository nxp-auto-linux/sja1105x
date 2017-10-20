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
* \file NXP_SJA1105P_sgmii.h
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief SGMII
*
*****************************************************************************/

#ifndef NXP_SJA1105P_SGMII_H
#define NXP_SJA1105P_SGMII_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_oui_NXP     = 24631,  /**< NXP device */
	SJA1105P_e_oui_UNKNOWN = 0       /**< Unknown device */
} SJA1105P_oui_t;

typedef enum 
{
	SJA1105P_e_pause_MIXED_PAUSE = 3,  /**< Symmetric Pause and Asymmetric Pause towards the local device */
	SJA1105P_e_pause_SYMM_PAUSE  = 2,  /**< Symmetric Pause */
	SJA1105P_e_pause_ASYMM_PAUSE = 1,  /**< Asymmetric Pause towards the link partner */
	SJA1105P_e_pause_NO_PAUSE    = 0   /**< No Pause */
} SJA1105P_pause_t;

typedef enum 
{
	SJA1105P_e_testPattern_RESERVED        = 3,  /**< Reserved */
	SJA1105P_e_testPattern_MIXED_FREQUENCY = 2,  /**< Mixed frequency test pattern */
	SJA1105P_e_testPattern_LOW_FREQUENCY   = 1,  /**< Low frequency test pattern */
	SJA1105P_e_testPattern_HIGH_FREQUENCY  = 0   /**< High frequency test pattern */
} SJA1105P_testPattern_t;

typedef enum 
{
	SJA1105P_e_typeNo_SJA1105RS_SGMII = 45,  /**< SGMII core of the SJA1105R/S */
	SJA1105P_e_typeNo_UNKNOWN         = 0    /**< Unknown device */
} SJA1105P_typeNo_t;

typedef enum 
{
	SJA1105P_e_txEdgerate_RESERVED = 3,  /**< Reserved */
	SJA1105P_e_txEdgerate_SLOW     = 2,  /**< slow edge rate */
	SJA1105P_e_txEdgerate_MEDIUM   = 1,  /**< medium edge rate */
	SJA1105P_e_txEdgerate_FAST     = 0   /**< fast edge rate */
} SJA1105P_txEdgerate_t;

typedef enum 
{
	SJA1105P_e_remoteFault_AUTONEG_ERROR = 3,  /**< Auto-negotiation error */
	SJA1105P_e_remoteFault_LINK_FAIL     = 2,  /**< Link failure */
	SJA1105P_e_remoteFault_OFFLINE       = 1,  /**< Offline */
	SJA1105P_e_remoteFault_NO_ERROR      = 0   /**< No Error */
} SJA1105P_remoteFault_t;

typedef enum 
{
	SJA1105P_e_autonegMode_CL37_SGMII = 2,  /**< Clause 37 auto-negotiation for SGMII mode */
	SJA1105P_e_autonegMode_INVALID    = 0   /**< Invalid auto-negotiation mode */
} SJA1105P_autonegMode_t;

typedef enum 
{
	SJA1105P_e_macWidth_8BIT = 1,  /**< 8-bit MII */
	SJA1105P_e_macWidth_4BIT = 0   /**< 4-bit MII */
} SJA1105P_macWidth_t;

typedef enum 
{
	SJA1105P_e_pseqState_MPLL_OFF        = 7,  /**< MPLL OFF */
	SJA1105P_e_pseqState_WAIT_MPLL_OFF   = 6,  /**< Wait for MPLL OFF */
	SJA1105P_e_pseqState_WAIT_TX_RX_DOWN = 5,  /**< Wait for Tx/Rx down (MPLL still ON) */
	SJA1105P_e_pseqState_TX_RX_STABLE    = 4,  /**< Tx/Rx Stable (Power_Good state) */
	SJA1105P_e_pseqState_WAIT_RX_DPLL    = 3,  /**< Wait for Rx DPLL lock */
	SJA1105P_e_pseqState_WAIT_RXCLK      = 2,  /**< Wait for RXCLK */
	SJA1105P_e_pseqState_WAIT_TXCLK      = 1,  /**< Wait for TXCLK */
	SJA1105P_e_pseqState_WAIT_MPLL_ON    = 0   /**< Wait for MPLL ON */
} SJA1105P_pseqState_t;

typedef struct
{
	uint8_t          duplexMode;     /**< 0: Half duplex - 1: Full duplex */
	uint8_t          reAutoneg;      /**< When the host writes this bit, the auto-negotiation process is initiated. */
	uint8_t          powerDown;      /**< This bit controls the power-down mode. */
	uint8_t          autonegEnable;  /**< Enable Auto-Negotiation When set to 1, this bit enables the Clause 37 auto-negotiation process. */
	SJA1105P_speed_t speedSelect;    /**< 11: Reserved; 10: 1000 Mbps; 01: 100 Mbps; 00: 10 Mbps */
	uint8_t          loopback;       /**< When set, this bit loops the PHY Tx lanes back to the PHY Rx lanes. */
	uint8_t          reset;          /**< When the host sets this bit, the CSR block triggers the software reset process in which all internal blocks are reset, except the Management Interface block. */
} SJA1105P_basicControlArgument_t;

typedef struct
{
	uint8_t e100BaseT4;             /**< PHY able to perform 100BASE-T4 */
	uint8_t e100BaseXFullDuplex;    /**< PHY able to 100BASE-X Full Duplex */
	uint8_t e100BaseXHalfDuplex;    /**< PHY able to 100BASE-X Half Duplex */
	uint8_t e10MbpsFullDuplex;      /**< PHY able to 10Mb/s Full Duplex */
	uint8_t e10MbpsHalfDuplex;      /**< PHY able to 10Mb/s Half Duplex */
	uint8_t e100BaseT2FullDuplex;   /**< PHY able to 100BASE-T2 Full Duplex */
	uint8_t e100BaseT2HalfDuplex;   /**< PHY able to 100BASE-T2 Half Duplex */
	uint8_t extendedStatus;         /**< Extended status information in Register 15 */
	uint8_t unidirectionalAbility;  /**< PHY able to transmit from MII regardless of whether the PHY has determined that a valid link has been established */
	uint8_t mfPreambleSuppression;  /**< PHY will accept management frames with preamble suppressed */
	uint8_t autonegComplete;        /**< Autonegotiation process completed */
	uint8_t remoteFault;            /**< Remote fault condition detected */
	uint8_t autonegAbility;         /**< PHY is able to perform Autonegotiation */
	uint8_t linkStatus;             /**< Link is up */
	uint8_t jabberDetect;           /**< Jabber condition detected */
	uint8_t extendedReg;            /**< extended register capabilities */
} SJA1105P_basicStatusArgument_t;

typedef struct
{
	SJA1105P_oui_t    phyId;       /**< Organizationally Unique Identifier (OUI) */
	SJA1105P_typeNo_t typeNo;      /**< Six-bit manufacturer type number */
	uint8_t           revisionNo;  /**< Four-bit manufacturer revision number */
} SJA1105P_phyIdentifierArgument_t;

typedef struct
{
	uint8_t                fullDuplex;   /**< When this bit is set, it indicates that the device can operate in the full-duplex mode. */
	uint8_t                halfDuplex;   /**< When this bit is set, it indicates that the device can operate in the half-duplex mode. */
	SJA1105P_pause_t       pause;        /**< When this bit is set, it indicates that the device can operate in the half-duplex mode. */
	SJA1105P_remoteFault_t remoteFault;  /**< This field indicates the fault signaling of the local device to be communicated to the link partner. */
} SJA1105P_autonegAdvArgument_t;

typedef struct
{
	uint8_t                fullDuplex;   /**< When this bit is set, it indicates that the link partner can operate in the full-duplex mode. */
	uint8_t                halfDuplex;   /**< When this bit is set, it indicates that the link partner can operate in the half-duplex mode. */
	SJA1105P_pause_t       pause;        /**< When this bit is set, it indicates that the link partner can operate in the half-duplex mode. */
	SJA1105P_remoteFault_t remoteFault;  /**< This field indicates the fault signaling of the local link partner to be communicated to the link partner. */
	uint8_t                ack;          /**< ACK bit from the Link Partner. This bit indicates that the link partner has successfully received the page sent by the local device */
	uint8_t                nextPage;     /**< returns this bit as 0 because the Next Page feature is not supported. */
} SJA1105P_autonegLpBablArgument_t;

typedef struct
{
	uint8_t pageRecvd;    /**< This bit indicates that the local device received a page from the link partner */
	uint8_t nextPageAbl;  /**< Always returns this bit as 0 because it does not support Next Page. */
} SJA1105P_autonegExpnArgument_t;

typedef struct
{
	uint8_t e1000BaseXFullDuplex;  /**< PHY able to 1000BASE-X Full Duplex */
	uint8_t e1000BaseXHalfDuplex;  /**< PHY able to 1000BASE-X Half Duplex */
	uint8_t e1000BaseTFullDuplex;  /**< PHY able to 1000BASE-T Full Duplex */
	uint8_t e1000BaseTHalfDuplex;  /**< PHY able to 1000BASE-T Half Duplex */
} SJA1105P_extendedStatusArgument_t;

typedef struct
{
	uint8_t vsReset;           /**< Vedor-specific software reset process in which all internal blocks are reset, except the Management Interface block and the CSR block */
	uint8_t gmiiLoopback;      /**< Loopback on the internal GMII port from Rx to TX */
	uint8_t enVsmmd1;          /**< Loopback on the internal GMII port from Rx to TX */
	uint8_t powerSave;         /**< triggers the power-down mode by turning off the PHY Receiver and Transmitter, and without turning off MPLL */
	uint8_t clockStopEn;       /**< Clock stop enable */
	uint8_t macAutoSw;         /**< Auto speed mode change after auto-negotiation (only in MAC mode) */
	uint8_t init;              /**< flush/initialize the internal datapath FIFOs */
	uint8_t txDisable;         /**< Tx lane disable */
	uint8_t autonegTimerOvrr;  /**< Override auto-negotiation timer */
	uint8_t bypPowerup;        /**< Bypass power-up sequence */
	uint8_t phyModeControl;    /**< SGMII PHY mode control (only in PHY mode) - 0: Advertise values programmed in AUTONEG_ADV register, 1: Advertise default values */
} SJA1105P_digitalControl1Argument_t;

typedef struct
{
	SJA1105P_macWidth_t    miiControl;   /**< Width of the MAC interface when the current SGMII speed mode is 10 Mbps or 100 Mbps */
	uint8_t                sgmiiLink;    /**< 0: Link is down, 1: Link is up */
	uint8_t                phyMode;      /**< When set, port is in PHY mode, else MAC mode */
	SJA1105P_autonegMode_t autonegMode;  /**< Auto-negotiation mode */
} SJA1105P_autonegControlArgument_t;

typedef struct
{
	uint8_t          linkStatus;  /**< Width of the MAC interface when the current SGMII speed mode is 10 Mbps or 100 Mbps */
	SJA1105P_speed_t speed;       /**< Link speed after auto-negotiation */
	uint8_t          duplexMode;  /**< Duplex mode after auto-negotiation (1 Full Duplex, 0 Half Duplex) */
	uint8_t          complete;    /**< The bit is set upon completion of the auto-negotiation. The host must clear this bit by writing 0 to it. */
} SJA1105P_autonegIntrStatusArgument_t;

typedef struct
{
	uint8_t                tpEnable;  /**< Test pattern enable */
	SJA1105P_testPattern_t tpSelect;  /**< Test pattern select */
} SJA1105P_testControlArgument_t;

typedef struct
{
	uint8_t supressLos;   /**< Suppress loss of signal detection. Loss of signal will be ignored for the receive link status if this bit is set. */
	uint8_t restartSync;  /**< Restarts the Rx synchronization state machine. The host must clear this bit to 0 before setting it to 1 next time to restart synchronization. */
} SJA1105P_debugControlArgument_t;

typedef struct
{
	uint8_t              rxfifoOvf;   /**< Rx clock rate compensation FIFO overflow */
	uint8_t              rxfifoUndf;  /**< Rx clock rate compensation FIFO underflow */
	SJA1105P_pseqState_t pseqState;   /**< Rx clock rate compensation FIFO underflow */
	uint8_t              loopback;    /**< Set if loopback is activated */
} SJA1105P_digitalStatusArgument_t;

typedef struct
{
	uint8_t losLvl;  /**< Loss-of-Signal Level Control: This field is used to control the loss-of-signal (LOS) detection threshold. The programmed threshold is given by (LOS_LVL+1)/(32x16) x1.21V. The Peak Level at which LOS is asserted is between the Programmed Threshold + 2mV and Programmed Threshold + 55 mV. */
	uint8_t txLvl;   /**< This field is used to set the resolution of the Tx signal level. Peak to Peak output is given by (900+10xTX_LVL) mV without attenuation. */
} SJA1105P_levelControlArgument_t;

typedef struct
{
	uint8_t txPolInv;  /**< Set to reverse data polarity on Tx differential lines */
	uint8_t rxPolInv;  /**< Set to reverse data polarity on Rx differential lines */
} SJA1105P_digitalControl2Argument_t;

typedef struct
{
	uint8_t iccEcEn;  /**< Invalid code group error counter enable */
	uint8_t cor;      /**< Clear on read. If set, clear any error counter that is read */
} SJA1105P_digitalErrorCntArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category basic_control */
extern uint8_t SJA1105P_setBasicControl(const SJA1105P_basicControlArgument_t *pk_basicControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getBasicControl(SJA1105P_basicControlArgument_t *p_basicControl, uint8_t deviceSelect);

/* register category basic_status */
extern uint8_t SJA1105P_getBasicStatus(SJA1105P_basicStatusArgument_t *p_basicStatus, uint8_t deviceSelect);

/* register category phy_identifier */
extern uint8_t SJA1105P_getPhyIdentifier(SJA1105P_phyIdentifierArgument_t *p_phyIdentifier, uint8_t deviceSelect);

/* register category autoneg_adv */
extern uint8_t SJA1105P_setAutonegAdv(const SJA1105P_autonegAdvArgument_t *pk_autonegAdv, uint8_t deviceSelect);
extern uint8_t SJA1105P_getAutonegAdv(SJA1105P_autonegAdvArgument_t *p_autonegAdv, uint8_t deviceSelect);

/* register category autoneg_lp_babl */
extern uint8_t SJA1105P_getAutonegLpBabl(SJA1105P_autonegLpBablArgument_t *p_autonegLpBabl, uint8_t deviceSelect);

/* register category autoneg_expn */
extern uint8_t SJA1105P_getAutonegExpn(SJA1105P_autonegExpnArgument_t *p_autonegExpn, uint8_t deviceSelect);

/* register category extended_status */
extern uint8_t SJA1105P_getExtendedStatus(SJA1105P_extendedStatusArgument_t *p_extendedStatus, uint8_t deviceSelect);

/* register category digital_control_1 */
extern uint8_t SJA1105P_setDigitalControl1(const SJA1105P_digitalControl1Argument_t *pk_digitalControl1, uint8_t deviceSelect);
extern uint8_t SJA1105P_getDigitalControl1(SJA1105P_digitalControl1Argument_t *p_digitalControl1, uint8_t deviceSelect);

/* register category autoneg_control */
extern uint8_t SJA1105P_setAutonegControl(const SJA1105P_autonegControlArgument_t *pk_autonegControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getAutonegControl(SJA1105P_autonegControlArgument_t *p_autonegControl, uint8_t deviceSelect);

/* register category autoneg_intr_status */
extern uint8_t SJA1105P_setAutonegIntrStatus(uint8_t complete, uint8_t deviceSelect);
extern uint8_t SJA1105P_getAutonegIntrStatus(SJA1105P_autonegIntrStatusArgument_t *p_autonegIntrStatus, uint8_t deviceSelect);

/* register category test_control */
extern uint8_t SJA1105P_setTestControl(const SJA1105P_testControlArgument_t *pk_testControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getTestControl(SJA1105P_testControlArgument_t *p_testControl, uint8_t deviceSelect);

/* register category debug_control */
extern uint8_t SJA1105P_setDebugControl(const SJA1105P_debugControlArgument_t *pk_debugControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getDebugControl(SJA1105P_debugControlArgument_t *p_debugControl, uint8_t deviceSelect);

/* register category digital_status */
extern uint8_t SJA1105P_getDigitalStatus(SJA1105P_digitalStatusArgument_t *p_digitalStatus, uint8_t deviceSelect);

/* register category icc_error_cnt */
extern uint8_t SJA1105P_getIccErrorCnt(uint8_t *p_errorCnt, uint8_t deviceSelect);

/* register category tx_boost_control */
extern uint8_t SJA1105P_setTxBoostControl(uint8_t txBoost, uint8_t deviceSelect);
extern uint8_t SJA1105P_getTxBoostControl(uint8_t *p_txBoost, uint8_t deviceSelect);

/* register category tx_attn_control */
extern uint8_t SJA1105P_setTxAttnControl(uint8_t txAttn, uint8_t deviceSelect);
extern uint8_t SJA1105P_getTxAttnControl(uint8_t *p_txAttn, uint8_t deviceSelect);

/* register category tx_edge_control */
extern uint8_t SJA1105P_setTxEdgeControl(SJA1105P_txEdgerate_t txEdgerate, uint8_t deviceSelect);
extern uint8_t SJA1105P_getTxEdgeControl(SJA1105P_txEdgerate_t *p_txEdgerate, uint8_t deviceSelect);

/* register category rx_eq_control */
extern uint8_t SJA1105P_setRxEqControl(uint8_t rxEqVal, uint8_t deviceSelect);
extern uint8_t SJA1105P_getRxEqControl(uint8_t *p_rxEqVal, uint8_t deviceSelect);

/* register category level_control */
extern uint8_t SJA1105P_setLevelControl(const SJA1105P_levelControlArgument_t *pk_levelControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getLevelControl(SJA1105P_levelControlArgument_t *p_levelControl, uint8_t deviceSelect);

/* register category digital_control_2 */
extern uint8_t SJA1105P_setDigitalControl2(const SJA1105P_digitalControl2Argument_t *pk_digitalControl2, uint8_t deviceSelect);
extern uint8_t SJA1105P_getDigitalControl2(SJA1105P_digitalControl2Argument_t *p_digitalControl2, uint8_t deviceSelect);

/* register category digital_error_cnt */
extern uint8_t SJA1105P_setDigitalErrorCnt(const SJA1105P_digitalErrorCntArgument_t *pk_digitalErrorCnt, uint8_t deviceSelect);
extern uint8_t SJA1105P_getDigitalErrorCnt(SJA1105P_digitalErrorCntArgument_t *p_digitalErrorCnt, uint8_t deviceSelect);

extern SJA1105P_typeNo_t SJA1105P_convertToTypeNo(uint32_t stdintValue);
extern SJA1105P_pseqState_t SJA1105P_convertToPseqState(uint32_t stdintValue);
extern SJA1105P_pause_t SJA1105P_convertToPause(uint32_t stdintValue);
extern SJA1105P_oui_t SJA1105P_convertToOui(uint32_t stdintValue);
extern SJA1105P_remoteFault_t SJA1105P_convertToRemoteFault(uint32_t stdintValue);
extern SJA1105P_autonegMode_t SJA1105P_convertToAutonegMode(uint32_t stdintValue);
extern SJA1105P_macWidth_t SJA1105P_convertToMacWidth(uint32_t stdintValue);
extern SJA1105P_testPattern_t SJA1105P_convertToTestPattern(uint32_t stdintValue);
extern SJA1105P_txEdgerate_t SJA1105P_convertToTxEdgerate(uint32_t stdintValue);

#endif  /* NXP_SJA1105P_SGMII_H */
