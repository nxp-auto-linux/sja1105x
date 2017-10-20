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
* \file NXP_SJA1105P_sgmii.c
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

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_sgmii.h"
#include "NXP_SJA1105P_spi.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define WORD0 0U
#define WORD1 1U
#define WORD2 2U
/* register addresses */

/* register category basic_control */
#define BC_ADDR (0x1f0000)  /**< Address of the basicControl register */

/* register category basic_status */
#define BS_ADDR (0x1f0001)  /**< Address of the basicStatus register */

/* register category phy_identifier */
#define PHY_I_WORD_COUNT (2)         /**< Number of data words contained in the phyIdentifier register */
#define PHY_I_ADDR       (0x1f0002)  /**< Address of the phyIdentifier register */

/* register category autoneg_adv */
#define A_ADV_ADDR (0x1f0004)  /**< Address of the autonegAdv register */

/* register category autoneg_lp_babl */
#define A_LP_BABL_ADDR (0x1f0005)  /**< Address of the autonegLpBabl register */

/* register category autoneg_expn */
#define A_EXPN_ADDR (0x1f0006)  /**< Address of the autonegExpn register */

/* register category extended_status */
#define ES_ADDR (0x1f000f)  /**< Address of the extendedStatus register */

/* register category digital_control_1 */
#define DC_1_ADDR (0x1f8000)  /**< Address of the digitalControl1 register */

/* register category autoneg_control */
#define AC_ADDR (0x1f8001)  /**< Address of the autonegControl register */

/* register category autoneg_intr_status */
#define A_INTR_S_ADDR (0x1f8002)  /**< Address of the autonegIntrStatus register */

/* register category test_control */
#define TEST_C_ADDR (0x1f8003)  /**< Address of the testControl register */

/* register category debug_control */
#define DC_ADDR (0x1f8005)  /**< Address of the debugControl register */

/* register category digital_status */
#define DS_ADDR (0x1f8010)  /**< Address of the digitalStatus register */

/* register category icc_error_cnt */
#define ICC_E_CNT_ADDR (0x1f8011)  /**< Address of the iccErrorCnt register */

/* register category tx_boost_control */
#define TX_BC_ADDR (0x1f8030)  /**< Address of the txBoostControl register */

/* register category tx_attn_control */
#define TX_ATTN_C_ADDR (0x1f8031)  /**< Address of the txAttnControl register */

/* register category tx_edge_control */
#define TX_EDGE_C_ADDR (0x1f8033)  /**< Address of the txEdgeControl register */

/* register category rx_eq_control */
#define RX_EQ_C_ADDR (0x1f8051)  /**< Address of the rxEqControl register */

/* register category level_control */
#define LC_ADDR (0x1f8090)  /**< Address of the levelControl register */

/* register category digital_control_2 */
#define DC_2_ADDR (0x1f80e1)  /**< Address of the digitalControl2 register */

/* register category digital_error_cnt */
#define DE_CNT_ADDR (0x1f80e2)  /**< Address of the digitalErrorCnt register */

/* variable masks */

/* register category basic_control */
#define BC_DUPLEX_MODE_SHIFT       (8)            /**< Shift value for the DUPLEX_MODE field */
#define BC_DUPLEX_MODE_MASK        (0x00000100U)  /**< Bit mask for the DUPLEX_MODE field */
#define BC_RE_AUTONEG_SHIFT        (9)            /**< Shift value for the RE_AUTONEG field */
#define BC_RE_AUTONEG_MASK         (0x00000200U)  /**< Bit mask for the RE_AUTONEG field */
#define BC_POWER_DOWN_SHIFT        (11)           /**< Shift value for the POWER_DOWN field */
#define BC_POWER_DOWN_MASK         (0x00000800U)  /**< Bit mask for the POWER_DOWN field */
#define BC_AUTONEG_ENABLE_SHIFT    (12)           /**< Shift value for the AUTONEG_ENABLE field */
#define BC_AUTONEG_ENABLE_MASK     (0x00001000U)  /**< Bit mask for the AUTONEG_ENABLE field */
#define BC_SPEED_SELECT_SHIFT_0    (13)           /**< Shift value for the SPEED_SELECT field */
#define BC_SPEED_SELECT_MASK_0     (0x00002000U)  /**< Bit mask for the SPEED_SELECT field */
#define BC_SPEED_SELECT_SHIFT_1    (6)            /**< Shift value for the SPEED_SELECT field */
#define BC_SPEED_SELECT_MASK_1     (0x00000040U)  /**< Bit mask for the SPEED_SELECT field */
#define BC_SPEED_SELECT_SHIFT_OVLP (1)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping SPEED_SELECT field */
#define BC_LOOPBACK_SHIFT          (14)           /**< Shift value for the LOOPBACK field */
#define BC_LOOPBACK_MASK           (0x00004000U)  /**< Bit mask for the LOOPBACK field */
#define BC_RESET_SHIFT             (15)           /**< Shift value for the RESET field */
#define BC_RESET_MASK              (0x00008000U)  /**< Bit mask for the RESET field */

/* register category basic_status */
#define BS_E100_BASE_T4_SHIFT             (15)           /**< Shift value for the E100_BASE_T4 field */
#define BS_E100_BASE_T4_MASK              (0x00008000U)  /**< Bit mask for the E100_BASE_T4 field */
#define BS_E100_BASE_X_FULL_DUPLEX_SHIFT  (14)           /**< Shift value for the E100_BASE_X_FULL_DUPLEX field */
#define BS_E100_BASE_X_FULL_DUPLEX_MASK   (0x00004000U)  /**< Bit mask for the E100_BASE_X_FULL_DUPLEX field */
#define BS_E100_BASE_X_HALF_DUPLEX_SHIFT  (13)           /**< Shift value for the E100_BASE_X_HALF_DUPLEX field */
#define BS_E100_BASE_X_HALF_DUPLEX_MASK   (0x00002000U)  /**< Bit mask for the E100_BASE_X_HALF_DUPLEX field */
#define BS_E10_MBPS_FULL_DUPLEX_SHIFT     (12)           /**< Shift value for the E10_MBPS_FULL_DUPLEX field */
#define BS_E10_MBPS_FULL_DUPLEX_MASK      (0x00001000U)  /**< Bit mask for the E10_MBPS_FULL_DUPLEX field */
#define BS_E10_MBPS_HALF_DUPLEX_SHIFT     (11)           /**< Shift value for the E10_MBPS_HALF_DUPLEX field */
#define BS_E10_MBPS_HALF_DUPLEX_MASK      (0x00000800U)  /**< Bit mask for the E10_MBPS_HALF_DUPLEX field */
#define BS_E100_BASE_T2_FULL_DUPLEX_SHIFT (10)           /**< Shift value for the E100_BASE_T2_FULL_DUPLEX field */
#define BS_E100_BASE_T2_FULL_DUPLEX_MASK  (0x00000400U)  /**< Bit mask for the E100_BASE_T2_FULL_DUPLEX field */
#define BS_E100_BASE_T2_HALF_DUPLEX_SHIFT (9)            /**< Shift value for the E100_BASE_T2_HALF_DUPLEX field */
#define BS_E100_BASE_T2_HALF_DUPLEX_MASK  (0x00000200U)  /**< Bit mask for the E100_BASE_T2_HALF_DUPLEX field */
#define BS_EXTENDED_STATUS_SHIFT          (8)            /**< Shift value for the EXTENDED_STATUS field */
#define BS_EXTENDED_STATUS_MASK           (0x00000100U)  /**< Bit mask for the EXTENDED_STATUS field */
#define BS_UNIDIRECTIONAL_ABILITY_SHIFT   (7)            /**< Shift value for the UNIDIRECTIONAL_ABILITY field */
#define BS_UNIDIRECTIONAL_ABILITY_MASK    (0x00000080U)  /**< Bit mask for the UNIDIRECTIONAL_ABILITY field */
#define BS_MF_PREAMBLE_SUPPRESSION_SHIFT  (6)            /**< Shift value for the MF_PREAMBLE_SUPPRESSION field */
#define BS_MF_PREAMBLE_SUPPRESSION_MASK   (0x00000040U)  /**< Bit mask for the MF_PREAMBLE_SUPPRESSION field */
#define BS_AUTONEG_COMPLETE_SHIFT         (5)            /**< Shift value for the AUTONEG_COMPLETE field */
#define BS_AUTONEG_COMPLETE_MASK          (0x00000020U)  /**< Bit mask for the AUTONEG_COMPLETE field */
#define BS_REMOTE_FAULT_SHIFT             (4)            /**< Shift value for the REMOTE_FAULT field */
#define BS_REMOTE_FAULT_MASK              (0x00000010U)  /**< Bit mask for the REMOTE_FAULT field */
#define BS_AUTONEG_ABILITY_SHIFT          (3)            /**< Shift value for the AUTONEG_ABILITY field */
#define BS_AUTONEG_ABILITY_MASK           (0x00000008U)  /**< Bit mask for the AUTONEG_ABILITY field */
#define BS_LINK_STATUS_SHIFT              (2)            /**< Shift value for the LINK_STATUS field */
#define BS_LINK_STATUS_MASK               (0x00000004U)  /**< Bit mask for the LINK_STATUS field */
#define BS_JABBER_DETECT_SHIFT            (1)            /**< Shift value for the JABBER_DETECT field */
#define BS_JABBER_DETECT_MASK             (0x00000002U)  /**< Bit mask for the JABBER_DETECT field */
#define BS_EXTENDED_REG_MASK              (0x00000001U)  /**< Bit mask for the EXTENDED_REG field */

/* register category phy_identifier */
#define PHY_I_PHY_ID_MASK_0     (0x00003FFFU)  /**< Bit mask for the PHY_ID field */
#define PHY_I_PHY_ID_SHIFT_OVLP (6)            /**< Shift value for the to compensate for the alignment of the 1. part of the overlapping PHY_ID field */
#define PHY_I_PHY_ID_SHIFT_1    (10)           /**< Shift value for the PHY_ID field */
#define PHY_I_PHY_ID_MASK_1     (0x0000FC00U)  /**< Bit mask for the PHY_ID field */
#define PHY_I_TYPE_NO_SHIFT     (4)            /**< Shift value for the TYPE_NO field */
#define PHY_I_TYPE_NO_MASK      (0x000003F0U)  /**< Bit mask for the TYPE_NO field */
#define PHY_I_REVISION_NO_MASK  (0x0000000FU)  /**< Bit mask for the REVISION_NO field */

/* register category autoneg_adv */
#define A_ADV_FULL_DUPLEX_SHIFT  (5)            /**< Shift value for the FULL_DUPLEX field */
#define A_ADV_FULL_DUPLEX_MASK   (0x00000020U)  /**< Bit mask for the FULL_DUPLEX field */
#define A_ADV_HALF_DUPLEX_SHIFT  (6)            /**< Shift value for the HALF_DUPLEX field */
#define A_ADV_HALF_DUPLEX_MASK   (0x00000040U)  /**< Bit mask for the HALF_DUPLEX field */
#define A_ADV_PAUSE_SHIFT        (7)            /**< Shift value for the PAUSE field */
#define A_ADV_PAUSE_MASK         (0x00000180U)  /**< Bit mask for the PAUSE field */
#define A_ADV_REMOTE_FAULT_SHIFT (12)           /**< Shift value for the REMOTE_FAULT field */
#define A_ADV_REMOTE_FAULT_MASK  (0x00003000U)  /**< Bit mask for the REMOTE_FAULT field */

/* register category autoneg_lp_babl */
#define A_LP_BABL_FULL_DUPLEX_SHIFT  (5)            /**< Shift value for the FULL_DUPLEX field */
#define A_LP_BABL_FULL_DUPLEX_MASK   (0x00000020U)  /**< Bit mask for the FULL_DUPLEX field */
#define A_LP_BABL_HALF_DUPLEX_SHIFT  (6)            /**< Shift value for the HALF_DUPLEX field */
#define A_LP_BABL_HALF_DUPLEX_MASK   (0x00000040U)  /**< Bit mask for the HALF_DUPLEX field */
#define A_LP_BABL_PAUSE_SHIFT        (7)            /**< Shift value for the PAUSE field */
#define A_LP_BABL_PAUSE_MASK         (0x00000180U)  /**< Bit mask for the PAUSE field */
#define A_LP_BABL_REMOTE_FAULT_SHIFT (12)           /**< Shift value for the REMOTE_FAULT field */
#define A_LP_BABL_REMOTE_FAULT_MASK  (0x00003000U)  /**< Bit mask for the REMOTE_FAULT field */
#define A_LP_BABL_ACK_SHIFT          (14)           /**< Shift value for the ACK field */
#define A_LP_BABL_ACK_MASK           (0x00004000U)  /**< Bit mask for the ACK field */
#define A_LP_BABL_NEXT_PAGE_SHIFT    (15)           /**< Shift value for the NEXT_PAGE field */
#define A_LP_BABL_NEXT_PAGE_MASK     (0x00008000U)  /**< Bit mask for the NEXT_PAGE field */

/* register category autoneg_expn */
#define A_EXPN_PAGE_RECVD_SHIFT    (1)            /**< Shift value for the PAGE_RECVD field */
#define A_EXPN_PAGE_RECVD_MASK     (0x00000002U)  /**< Bit mask for the PAGE_RECVD field */
#define A_EXPN_NEXT_PAGE_ABL_SHIFT (2)            /**< Shift value for the NEXT_PAGE_ABL field */
#define A_EXPN_NEXT_PAGE_ABL_MASK  (0x00000004U)  /**< Bit mask for the NEXT_PAGE_ABL field */

/* register category extended_status */
#define ES_E1000_BASE_X_FULL_DUPLEX_SHIFT (15)           /**< Shift value for the E1000_BASE_X_FULL_DUPLEX field */
#define ES_E1000_BASE_X_FULL_DUPLEX_MASK  (0x00008000U)  /**< Bit mask for the E1000_BASE_X_FULL_DUPLEX field */
#define ES_E1000_BASE_X_HALF_DUPLEX_SHIFT (14)           /**< Shift value for the E1000_BASE_X_HALF_DUPLEX field */
#define ES_E1000_BASE_X_HALF_DUPLEX_MASK  (0x00004000U)  /**< Bit mask for the E1000_BASE_X_HALF_DUPLEX field */
#define ES_E1000_BASE_T_FULL_DUPLEX_SHIFT (13)           /**< Shift value for the E1000_BASE_T_FULL_DUPLEX field */
#define ES_E1000_BASE_T_FULL_DUPLEX_MASK  (0x00002000U)  /**< Bit mask for the E1000_BASE_T_FULL_DUPLEX field */
#define ES_E1000_BASE_T_HALF_DUPLEX_SHIFT (12)           /**< Shift value for the E1000_BASE_T_HALF_DUPLEX field */
#define ES_E1000_BASE_T_HALF_DUPLEX_MASK  (0x00001000U)  /**< Bit mask for the E1000_BASE_T_HALF_DUPLEX field */

/* register category digital_control_1 */
#define DC_1_VS_RESET_SHIFT           (15)           /**< Shift value for the VS_RESET field */
#define DC_1_VS_RESET_MASK            (0x00008000U)  /**< Bit mask for the VS_RESET field */
#define DC_1_GMII_LOOPBACK_SHIFT      (14)           /**< Shift value for the GMII_LOOPBACK field */
#define DC_1_GMII_LOOPBACK_MASK       (0x00004000U)  /**< Bit mask for the GMII_LOOPBACK field */
#define DC_1_EN_VSMMD1_SHIFT          (13)           /**< Shift value for the EN_VSMMD1 field */
#define DC_1_EN_VSMMD1_MASK           (0x00002000U)  /**< Bit mask for the EN_VSMMD1 field */
#define DC_1_POWER_SAVE_SHIFT         (11)           /**< Shift value for the POWER_SAVE field */
#define DC_1_POWER_SAVE_MASK          (0x00000800U)  /**< Bit mask for the POWER_SAVE field */
#define DC_1_CLOCK_STOP_EN_SHIFT      (10)           /**< Shift value for the CLOCK_STOP_EN field */
#define DC_1_CLOCK_STOP_EN_MASK       (0x00000400U)  /**< Bit mask for the CLOCK_STOP_EN field */
#define DC_1_MAC_AUTO_SW_SHIFT        (9)            /**< Shift value for the MAC_AUTO_SW field */
#define DC_1_MAC_AUTO_SW_MASK         (0x00000200U)  /**< Bit mask for the MAC_AUTO_SW field */
#define DC_1_INIT_SHIFT               (8)            /**< Shift value for the INIT field */
#define DC_1_INIT_MASK                (0x00000100U)  /**< Bit mask for the INIT field */
#define DC_1_TX_DISABLE_SHIFT         (4)            /**< Shift value for the TX_DISABLE field */
#define DC_1_TX_DISABLE_MASK          (0x00000010U)  /**< Bit mask for the TX_DISABLE field */
#define DC_1_AUTONEG_TIMER_OVRR_SHIFT (3)            /**< Shift value for the AUTONEG_TIMER_OVRR field */
#define DC_1_AUTONEG_TIMER_OVRR_MASK  (0x00000008U)  /**< Bit mask for the AUTONEG_TIMER_OVRR field */
#define DC_1_BYP_POWERUP_SHIFT        (1)            /**< Shift value for the BYP_POWERUP field */
#define DC_1_BYP_POWERUP_MASK         (0x00000002U)  /**< Bit mask for the BYP_POWERUP field */
#define DC_1_PHY_MODE_CONTROL_MASK    (0x00000001U)  /**< Bit mask for the PHY_MODE_CONTROL field */

/* register category autoneg_control */
#define AC_MII_CONTROL_SHIFT  (8)            /**< Shift value for the MII_CONTROL field */
#define AC_MII_CONTROL_MASK   (0x00000100U)  /**< Bit mask for the MII_CONTROL field */
#define AC_SGMII_LINK_SHIFT   (4)            /**< Shift value for the SGMII_LINK field */
#define AC_SGMII_LINK_MASK    (0x00000010U)  /**< Bit mask for the SGMII_LINK field */
#define AC_PHY_MODE_SHIFT     (3)            /**< Shift value for the PHY_MODE field */
#define AC_PHY_MODE_MASK      (0x00000008U)  /**< Bit mask for the PHY_MODE field */
#define AC_AUTONEG_MODE_SHIFT (1)            /**< Shift value for the AUTONEG_MODE field */
#define AC_AUTONEG_MODE_MASK  (0x00000006U)  /**< Bit mask for the AUTONEG_MODE field */

/* register category autoneg_intr_status */
#define A_INTR_S_LINK_STATUS_SHIFT (4)            /**< Shift value for the LINK_STATUS field */
#define A_INTR_S_LINK_STATUS_MASK  (0x00000010U)  /**< Bit mask for the LINK_STATUS field */
#define A_INTR_S_SPEED_SHIFT       (2)            /**< Shift value for the SPEED field */
#define A_INTR_S_SPEED_MASK        (0x0000000CU)  /**< Bit mask for the SPEED field */
#define A_INTR_S_DUPLEX_MODE_SHIFT (1)            /**< Shift value for the DUPLEX_MODE field */
#define A_INTR_S_DUPLEX_MODE_MASK  (0x00000002U)  /**< Bit mask for the DUPLEX_MODE field */
#define A_INTR_S_COMPLETE_MASK     (0x00000001U)  /**< Bit mask for the COMPLETE field */

/* register category test_control */
#define TEST_C_TP_ENABLE_SHIFT (2)            /**< Shift value for the TP_ENABLE field */
#define TEST_C_TP_ENABLE_MASK  (0x00000004U)  /**< Bit mask for the TP_ENABLE field */
#define TEST_C_TP_SELECT_MASK  (0x00000003U)  /**< Bit mask for the TP_SELECT field */

/* register category debug_control */
#define DC_SUPRESS_LOS_SHIFT (4)            /**< Shift value for the SUPRESS_LOS field */
#define DC_SUPRESS_LOS_MASK  (0x00000010U)  /**< Bit mask for the SUPRESS_LOS field */
#define DC_RESTART_SYNC_MASK (0x00000001U)  /**< Bit mask for the RESTART_SYNC field */

/* register category digital_status */
#define DS_RXFIFO_OVF_SHIFT  (6)            /**< Shift value for the RXFIFO_OVF field */
#define DS_RXFIFO_OVF_MASK   (0x00000040U)  /**< Bit mask for the RXFIFO_OVF field */
#define DS_RXFIFO_UNDF_SHIFT (5)            /**< Shift value for the RXFIFO_UNDF field */
#define DS_RXFIFO_UNDF_MASK  (0x00000020U)  /**< Bit mask for the RXFIFO_UNDF field */
#define DS_PSEQ_STATE_SHIFT  (2)            /**< Shift value for the PSEQ_STATE field */
#define DS_PSEQ_STATE_MASK   (0x0000001CU)  /**< Bit mask for the PSEQ_STATE field */
#define DS_LOOPBACK_SHIFT    (1)            /**< Shift value for the LOOPBACK field */
#define DS_LOOPBACK_MASK     (0x00000002U)  /**< Bit mask for the LOOPBACK field */

/* register category tx_boost_control */
#define TX_BC_TX_BOOST_MASK (0x0000000FU)  /**< Bit mask for the TX_BOOST field */

/* register category tx_attn_control */
#define TX_ATTN_C_TX_ATTN_MASK (0x00000007U)  /**< Bit mask for the TX_ATTN field */

/* register category tx_edge_control */
#define TX_EDGE_C_TX_EDGERATE_MASK (0x00000007U)  /**< Bit mask for the TX_EDGERATE field */

/* register category rx_eq_control */
#define RX_EQ_C_RX_EQ_VAL_MASK (0x00000007U)  /**< Bit mask for the RX_EQ_VAL field */

/* register category level_control */
#define LC_LOS_LVL_SHIFT (5)            /**< Shift value for the LOS_LVL field */
#define LC_LOS_LVL_MASK  (0x000003E0U)  /**< Bit mask for the LOS_LVL field */
#define LC_TX_LVL_MASK   (0x0000001FU)  /**< Bit mask for the TX_LVL field */

/* register category digital_control_2 */
#define DC_2_TX_POL_INV_SHIFT (4)            /**< Shift value for the TX_POL_INV field */
#define DC_2_TX_POL_INV_MASK  (0x00000010U)  /**< Bit mask for the TX_POL_INV field */
#define DC_2_RX_POL_INV_MASK  (0x00000001U)  /**< Bit mask for the RX_POL_INV field */

/* register category digital_error_cnt */
#define DE_CNT_ICC_EC_EN_SHIFT (4)            /**< Shift value for the ICC_EC_EN field */
#define DE_CNT_ICC_EC_EN_MASK  (0x00000010U)  /**< Bit mask for the ICC_EC_EN field */
#define DE_CNT_COR_MASK        (0x00000001U)  /**< Bit mask for the COR field */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to SET data of the basic_control
*
* \param[in]  basicControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_basicControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setBasicControl(const SJA1105P_basicControlArgument_t *pk_basicControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_basicControl->duplexMode <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_basicControl->reAutoneg <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_basicControl->powerDown <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_basicControl->autonegEnable <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_basicControl->speedSelect) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_basicControl->loopback <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_basicControl->reset <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_basicControl->duplexMode) << ((uint8_t) BC_DUPLEX_MODE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_DUPLEX_MODE_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_basicControl->reAutoneg) << ((uint8_t) BC_RE_AUTONEG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_RE_AUTONEG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_basicControl->powerDown) << ((uint8_t) BC_POWER_DOWN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_POWER_DOWN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_basicControl->autonegEnable) << ((uint8_t) BC_AUTONEG_ENABLE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_AUTONEG_ENABLE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_basicControl->speedSelect)) << ((uint8_t) BC_SPEED_SELECT_SHIFT_0);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_SPEED_SELECT_MASK_0) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = (uint32_t) ((uint32_t) ((uint8_t) pk_basicControl->speedSelect) >> (uint8_t) BC_SPEED_SELECT_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) tmp) << ((uint8_t) BC_SPEED_SELECT_SHIFT_1);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_SPEED_SELECT_MASK_1) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_basicControl->loopback) << ((uint8_t) BC_LOOPBACK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_LOOPBACK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_basicControl->reset) << ((uint8_t) BC_RESET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) BC_RESET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) BC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the basic_control
*
* \param[out] p_basicControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_basicControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getBasicControl(SJA1105P_basicControlArgument_t *p_basicControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;
	uint8_t speedSelectTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) BC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_DUPLEX_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_DUPLEX_MODE_SHIFT);  /* shift to right aligned position */
	p_basicControl->duplexMode = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_RE_AUTONEG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_RE_AUTONEG_SHIFT);  /* shift to right aligned position */
	p_basicControl->reAutoneg = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_POWER_DOWN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_POWER_DOWN_SHIFT);  /* shift to right aligned position */
	p_basicControl->powerDown = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_AUTONEG_ENABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_AUTONEG_ENABLE_SHIFT);  /* shift to right aligned position */
	p_basicControl->autonegEnable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_SPEED_SELECT_MASK_0);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_SPEED_SELECT_SHIFT_0);  /* shift to right aligned position */
	speedSelectTmp  = (uint8_t) cResult;

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_SPEED_SELECT_MASK_1);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_SPEED_SELECT_SHIFT_1);  /* shift to right aligned position */
	speedSelectTmp |= (uint8_t) ((uint8_t) cResult << (uint8_t) BC_SPEED_SELECT_SHIFT_OVLP);
	p_basicControl->speedSelect = SJA1105P_convertToSpeed(speedSelectTmp);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_LOOPBACK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_LOOPBACK_SHIFT);  /* shift to right aligned position */
	p_basicControl->loopback = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BC_RESET_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BC_RESET_SHIFT);  /* shift to right aligned position */
	p_basicControl->reset = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the basic_status
*
* \param[out] p_basicStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_basicStatusArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getBasicStatus(SJA1105P_basicStatusArgument_t *p_basicStatus, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) BS_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E100_BASE_T4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E100_BASE_T4_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e100BaseT4 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E100_BASE_X_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E100_BASE_X_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e100BaseXFullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E100_BASE_X_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E100_BASE_X_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e100BaseXHalfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E10_MBPS_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E10_MBPS_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e10MbpsFullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E10_MBPS_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E10_MBPS_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e10MbpsHalfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E100_BASE_T2_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E100_BASE_T2_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e100BaseT2FullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_E100_BASE_T2_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_E100_BASE_T2_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_basicStatus->e100BaseT2HalfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_EXTENDED_STATUS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_EXTENDED_STATUS_SHIFT);  /* shift to right aligned position */
	p_basicStatus->extendedStatus = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_UNIDIRECTIONAL_ABILITY_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_UNIDIRECTIONAL_ABILITY_SHIFT);  /* shift to right aligned position */
	p_basicStatus->unidirectionalAbility = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_MF_PREAMBLE_SUPPRESSION_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_MF_PREAMBLE_SUPPRESSION_SHIFT);  /* shift to right aligned position */
	p_basicStatus->mfPreambleSuppression = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_AUTONEG_COMPLETE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_AUTONEG_COMPLETE_SHIFT);  /* shift to right aligned position */
	p_basicStatus->autonegComplete = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_REMOTE_FAULT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_REMOTE_FAULT_SHIFT);  /* shift to right aligned position */
	p_basicStatus->remoteFault = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_AUTONEG_ABILITY_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_AUTONEG_ABILITY_SHIFT);  /* shift to right aligned position */
	p_basicStatus->autonegAbility = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_LINK_STATUS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_LINK_STATUS_SHIFT);  /* shift to right aligned position */
	p_basicStatus->linkStatus = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_JABBER_DETECT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (BS_JABBER_DETECT_SHIFT);  /* shift to right aligned position */
	p_basicStatus->jabberDetect = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (BS_EXTENDED_REG_MASK);   /* mask desired bits */
	p_basicStatus->extendedReg = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the phy_identifier
*
* \param[out] p_phyIdentifier Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_phyIdentifierArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getPhyIdentifier(SJA1105P_phyIdentifierArgument_t *p_phyIdentifier, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[PHY_I_WORD_COUNT] = {0};
	uint32_t phyIdTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, PHY_I_WORD_COUNT, (uint32_t) PHY_I_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (PHY_I_PHY_ID_MASK_0);   /* mask desired bits */
	phyIdTmp  = (uint32_t) ((uint32_t) cResult << (uint8_t) PHY_I_PHY_ID_SHIFT_OVLP);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (PHY_I_PHY_ID_MASK_1);   /* mask desired bits */
	cResult >>= (uint32_t) (PHY_I_PHY_ID_SHIFT_1);  /* shift to right aligned position */
	phyIdTmp |= (uint32_t) cResult;
	p_phyIdentifier->phyId = SJA1105P_convertToOui(phyIdTmp);  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (PHY_I_TYPE_NO_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PHY_I_TYPE_NO_SHIFT);  /* shift to right aligned position */
	p_phyIdentifier->typeNo = SJA1105P_convertToTypeNo(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (PHY_I_REVISION_NO_MASK);   /* mask desired bits */
	p_phyIdentifier->revisionNo = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the autoneg_adv
*
* \param[in]  autonegAdv Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegAdvArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setAutonegAdv(const SJA1105P_autonegAdvArgument_t *pk_autonegAdv, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_autonegAdv->fullDuplex <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_autonegAdv->halfDuplex <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_autonegAdv->pause) <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_autonegAdv->remoteFault) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_autonegAdv->fullDuplex) << ((uint8_t) A_ADV_FULL_DUPLEX_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) A_ADV_FULL_DUPLEX_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_autonegAdv->halfDuplex) << ((uint8_t) A_ADV_HALF_DUPLEX_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) A_ADV_HALF_DUPLEX_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_autonegAdv->pause)) << ((uint8_t) A_ADV_PAUSE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) A_ADV_PAUSE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_autonegAdv->remoteFault)) << ((uint8_t) A_ADV_REMOTE_FAULT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) A_ADV_REMOTE_FAULT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) A_ADV_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the autoneg_adv
*
* \param[out] p_autonegAdv Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegAdvArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAutonegAdv(SJA1105P_autonegAdvArgument_t *p_autonegAdv, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) A_ADV_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_ADV_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_ADV_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_autonegAdv->fullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_ADV_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_ADV_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_autonegAdv->halfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_ADV_PAUSE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_ADV_PAUSE_SHIFT);  /* shift to right aligned position */
	p_autonegAdv->pause = SJA1105P_convertToPause(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_ADV_REMOTE_FAULT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_ADV_REMOTE_FAULT_SHIFT);  /* shift to right aligned position */
	p_autonegAdv->remoteFault = SJA1105P_convertToRemoteFault(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the autoneg_lp_babl
*
* \param[out] p_autonegLpBabl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegLpBablArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAutonegLpBabl(SJA1105P_autonegLpBablArgument_t *p_autonegLpBabl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) A_LP_BABL_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->fullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->halfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_PAUSE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_PAUSE_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->pause = SJA1105P_convertToPause(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_REMOTE_FAULT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_REMOTE_FAULT_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->remoteFault = SJA1105P_convertToRemoteFault(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_ACK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_ACK_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->ack = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_LP_BABL_NEXT_PAGE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_LP_BABL_NEXT_PAGE_SHIFT);  /* shift to right aligned position */
	p_autonegLpBabl->nextPage = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the autoneg_expn
*
* \param[out] p_autonegExpn Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegExpnArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAutonegExpn(SJA1105P_autonegExpnArgument_t *p_autonegExpn, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) A_EXPN_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_EXPN_PAGE_RECVD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_EXPN_PAGE_RECVD_SHIFT);  /* shift to right aligned position */
	p_autonegExpn->pageRecvd = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_EXPN_NEXT_PAGE_ABL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_EXPN_NEXT_PAGE_ABL_SHIFT);  /* shift to right aligned position */
	p_autonegExpn->nextPageAbl = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the extended_status
*
* \param[out] p_extendedStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_extendedStatusArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getExtendedStatus(SJA1105P_extendedStatusArgument_t *p_extendedStatus, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ES_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (ES_E1000_BASE_X_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (ES_E1000_BASE_X_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_extendedStatus->e1000BaseXFullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (ES_E1000_BASE_X_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (ES_E1000_BASE_X_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_extendedStatus->e1000BaseXHalfDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (ES_E1000_BASE_T_FULL_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (ES_E1000_BASE_T_FULL_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_extendedStatus->e1000BaseTFullDuplex = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (ES_E1000_BASE_T_HALF_DUPLEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (ES_E1000_BASE_T_HALF_DUPLEX_SHIFT);  /* shift to right aligned position */
	p_extendedStatus->e1000BaseTHalfDuplex = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the digital_control_1
*
* \param[in]  digitalControl1 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalControl1Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_setDigitalControl1(const SJA1105P_digitalControl1Argument_t *pk_digitalControl1, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_digitalControl1->vsReset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->gmiiLoopback <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->enVsmmd1 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->powerSave <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->clockStopEn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->macAutoSw <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->init <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->txDisable <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->autonegTimerOvrr <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->bypPowerup <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl1->phyModeControl <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_digitalControl1->vsReset) << ((uint8_t) DC_1_VS_RESET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_VS_RESET_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_digitalControl1->gmiiLoopback) << ((uint8_t) DC_1_GMII_LOOPBACK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_GMII_LOOPBACK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->enVsmmd1) << ((uint8_t) DC_1_EN_VSMMD1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_EN_VSMMD1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->powerSave) << ((uint8_t) DC_1_POWER_SAVE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_POWER_SAVE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->clockStopEn) << ((uint8_t) DC_1_CLOCK_STOP_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_CLOCK_STOP_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->macAutoSw) << ((uint8_t) DC_1_MAC_AUTO_SW_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_MAC_AUTO_SW_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->init) << ((uint8_t) DC_1_INIT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_INIT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->txDisable) << ((uint8_t) DC_1_TX_DISABLE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_TX_DISABLE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->autonegTimerOvrr) << ((uint8_t) DC_1_AUTONEG_TIMER_OVRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_AUTONEG_TIMER_OVRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_digitalControl1->bypPowerup) << ((uint8_t) DC_1_BYP_POWERUP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_1_BYP_POWERUP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) DC_1_PHY_MODE_CONTROL_MASK) & pk_digitalControl1->phyModeControl;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) DC_1_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the digital_control_1
*
* \param[out] p_digitalControl1 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalControl1Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_getDigitalControl1(SJA1105P_digitalControl1Argument_t *p_digitalControl1, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) DC_1_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_VS_RESET_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_VS_RESET_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->vsReset = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_GMII_LOOPBACK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_GMII_LOOPBACK_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->gmiiLoopback = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_EN_VSMMD1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_EN_VSMMD1_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->enVsmmd1 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_POWER_SAVE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_POWER_SAVE_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->powerSave = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_CLOCK_STOP_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_CLOCK_STOP_EN_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->clockStopEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_MAC_AUTO_SW_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_MAC_AUTO_SW_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->macAutoSw = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_INIT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_INIT_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->init = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_TX_DISABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_TX_DISABLE_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->txDisable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_AUTONEG_TIMER_OVRR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_AUTONEG_TIMER_OVRR_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->autonegTimerOvrr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_BYP_POWERUP_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_1_BYP_POWERUP_SHIFT);  /* shift to right aligned position */
	p_digitalControl1->bypPowerup = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_1_PHY_MODE_CONTROL_MASK);   /* mask desired bits */
	p_digitalControl1->phyModeControl = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the autoneg_control
*
* \param[in]  autonegControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setAutonegControl(const SJA1105P_autonegControlArgument_t *pk_autonegControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) pk_autonegControl->miiControl) <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_autonegControl->sgmiiLink <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_autonegControl->phyMode <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_autonegControl->autonegMode) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) ((uint8_t) pk_autonegControl->miiControl)) << ((uint8_t) AC_MII_CONTROL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AC_MII_CONTROL_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_autonegControl->sgmiiLink) << ((uint8_t) AC_SGMII_LINK_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AC_SGMII_LINK_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_autonegControl->phyMode) << ((uint8_t) AC_PHY_MODE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AC_PHY_MODE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_autonegControl->autonegMode)) << ((uint8_t) AC_AUTONEG_MODE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AC_AUTONEG_MODE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) AC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the autoneg_control
*
* \param[out] p_autonegControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAutonegControl(SJA1105P_autonegControlArgument_t *p_autonegControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) AC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (AC_MII_CONTROL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AC_MII_CONTROL_SHIFT);  /* shift to right aligned position */
	p_autonegControl->miiControl = SJA1105P_convertToMacWidth(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (AC_SGMII_LINK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AC_SGMII_LINK_SHIFT);  /* shift to right aligned position */
	p_autonegControl->sgmiiLink = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (AC_PHY_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AC_PHY_MODE_SHIFT);  /* shift to right aligned position */
	p_autonegControl->phyMode = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (AC_AUTONEG_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AC_AUTONEG_MODE_SHIFT);  /* shift to right aligned position */
	p_autonegControl->autonegMode = SJA1105P_convertToAutonegMode(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the autoneg_intr_status
*
* \param[in]  complete The bit is set upon completion of the auto-negotiation. The host must clear this bit by writing 0 to it.
* \return uint8_t
*/
uint8_t SJA1105P_setAutonegIntrStatus(uint8_t complete, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((complete <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) A_INTR_S_COMPLETE_MASK) & complete;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) A_INTR_S_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the autoneg_intr_status
*
* \param[out] p_autonegIntrStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_autonegIntrStatusArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAutonegIntrStatus(SJA1105P_autonegIntrStatusArgument_t *p_autonegIntrStatus, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) A_INTR_S_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_INTR_S_LINK_STATUS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_INTR_S_LINK_STATUS_SHIFT);  /* shift to right aligned position */
	p_autonegIntrStatus->linkStatus = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_INTR_S_SPEED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_INTR_S_SPEED_SHIFT);  /* shift to right aligned position */
	p_autonegIntrStatus->speed = SJA1105P_convertToSpeed(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_INTR_S_DUPLEX_MODE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (A_INTR_S_DUPLEX_MODE_SHIFT);  /* shift to right aligned position */
	p_autonegIntrStatus->duplexMode = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (A_INTR_S_COMPLETE_MASK);   /* mask desired bits */
	p_autonegIntrStatus->complete = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the test_control
*
* \param[in]  testControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_testControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setTestControl(const SJA1105P_testControlArgument_t *pk_testControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_testControl->tpEnable <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_testControl->tpSelect) <= 0x3U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_testControl->tpEnable) << ((uint8_t) TEST_C_TP_ENABLE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) TEST_C_TP_ENABLE_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) TEST_C_TP_SELECT_MASK) & ((uint8_t) pk_testControl->tpSelect);  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TEST_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the test_control
*
* \param[out] p_testControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_testControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getTestControl(SJA1105P_testControlArgument_t *p_testControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TEST_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TEST_C_TP_ENABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (TEST_C_TP_ENABLE_SHIFT);  /* shift to right aligned position */
	p_testControl->tpEnable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TEST_C_TP_SELECT_MASK);   /* mask desired bits */
	p_testControl->tpSelect = SJA1105P_convertToTestPattern(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the debug_control
*
* \param[in]  debugControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_debugControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setDebugControl(const SJA1105P_debugControlArgument_t *pk_debugControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_debugControl->supressLos <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_debugControl->restartSync <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_debugControl->supressLos) << ((uint8_t) DC_SUPRESS_LOS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_SUPRESS_LOS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) DC_RESTART_SYNC_MASK) & pk_debugControl->restartSync;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) DC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the debug_control
*
* \param[out] p_debugControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_debugControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getDebugControl(SJA1105P_debugControlArgument_t *p_debugControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) DC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_SUPRESS_LOS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_SUPRESS_LOS_SHIFT);  /* shift to right aligned position */
	p_debugControl->supressLos = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_RESTART_SYNC_MASK);   /* mask desired bits */
	p_debugControl->restartSync = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the digital_status
*
* \param[out] p_digitalStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalStatusArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getDigitalStatus(SJA1105P_digitalStatusArgument_t *p_digitalStatus, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) DS_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DS_RXFIFO_OVF_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DS_RXFIFO_OVF_SHIFT);  /* shift to right aligned position */
	p_digitalStatus->rxfifoOvf = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DS_RXFIFO_UNDF_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DS_RXFIFO_UNDF_SHIFT);  /* shift to right aligned position */
	p_digitalStatus->rxfifoUndf = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DS_PSEQ_STATE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DS_PSEQ_STATE_SHIFT);  /* shift to right aligned position */
	p_digitalStatus->pseqState = SJA1105P_convertToPseqState(cResult);  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DS_LOOPBACK_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DS_LOOPBACK_SHIFT);  /* shift to right aligned position */
	p_digitalStatus->loopback = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the icc_error_cnt
*
* \param[out] p_errorCnt Invalid code group count
* \return uint8_t
*/
uint8_t SJA1105P_getIccErrorCnt(uint8_t *p_errorCnt, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ICC_E_CNT_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_errorCnt = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the tx_boost_control
*
* \param[in]  txBoost Tx Boost control: If 0h: Boost = 0 dB, Else: Boost = -20 log (1 - (TX_BOOST + 0.5) / 32) dB
* \return uint8_t
*/
uint8_t SJA1105P_setTxBoostControl(uint8_t txBoost, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((txBoost <= 0xFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) TX_BC_TX_BOOST_MASK) & txBoost;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TX_BC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the tx_boost_control
*
* \param[out] p_txBoost Tx Boost control: If 0h: Boost = 0 dB, Else: Boost = -20 log (1 - (TX_BOOST + 0.5) / 32) dB
* \return uint8_t
*/
uint8_t SJA1105P_getTxBoostControl(uint8_t *p_txBoost, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TX_BC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TX_BC_TX_BOOST_MASK);   /* mask desired bits */
	*p_txBoost = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the tx_attn_control
*
* \param[in]  txAttn Attenuation of the transmitter driver
* \return uint8_t
*/
uint8_t SJA1105P_setTxAttnControl(uint8_t txAttn, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((txAttn <= 0x7U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) TX_ATTN_C_TX_ATTN_MASK) & txAttn;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TX_ATTN_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the tx_attn_control
*
* \param[out] p_txAttn Attenuation of the transmitter driver
* \return uint8_t
*/
uint8_t SJA1105P_getTxAttnControl(uint8_t *p_txAttn, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TX_ATTN_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TX_ATTN_C_TX_ATTN_MASK);   /* mask desired bits */
	*p_txAttn = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the tx_edge_control
*
* \param[in]  txEdgerate Tx Edge Rate
* \return uint8_t
*/
uint8_t SJA1105P_setTxEdgeControl(SJA1105P_txEdgerate_t txEdgerate, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((((uint8_t) txEdgerate) <= 0x7U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) TX_EDGE_C_TX_EDGERATE_MASK) & ((uint8_t) txEdgerate);  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TX_EDGE_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the tx_edge_control
*
* \param[out] p_txEdgerate Tx Edge Rate
* \return uint8_t
*/
uint8_t SJA1105P_getTxEdgeControl(SJA1105P_txEdgerate_t *p_txEdgerate, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) TX_EDGE_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (TX_EDGE_C_TX_EDGERATE_MASK);   /* mask desired bits */
	*p_txEdgerate = SJA1105P_convertToTxEdgerate(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the rx_eq_control
*
* \param[in]  rxEqVal This field controls the internal linear equalizer boost. Note: must be set to 0h for SGMII operation.
* \return uint8_t
*/
uint8_t SJA1105P_setRxEqControl(uint8_t rxEqVal, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((rxEqVal <= 0x7U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) RX_EQ_C_RX_EQ_VAL_MASK) & rxEqVal;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) RX_EQ_C_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the rx_eq_control
*
* \param[out] p_rxEqVal This field controls the internal linear equalizer boost. Note: must be set to 0h for SGMII operation.
* \return uint8_t
*/
uint8_t SJA1105P_getRxEqControl(uint8_t *p_rxEqVal, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) RX_EQ_C_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (RX_EQ_C_RX_EQ_VAL_MASK);   /* mask desired bits */
	*p_rxEqVal = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the level_control
*
* \param[in]  levelControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_levelControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setLevelControl(const SJA1105P_levelControlArgument_t *pk_levelControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_levelControl->losLvl <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_levelControl->txLvl <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_levelControl->losLvl) << ((uint8_t) LC_LOS_LVL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) LC_LOS_LVL_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) LC_TX_LVL_MASK) & pk_levelControl->txLvl;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) LC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the level_control
*
* \param[out] p_levelControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_levelControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getLevelControl(SJA1105P_levelControlArgument_t *p_levelControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) LC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (LC_LOS_LVL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (LC_LOS_LVL_SHIFT);  /* shift to right aligned position */
	p_levelControl->losLvl = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (LC_TX_LVL_MASK);   /* mask desired bits */
	p_levelControl->txLvl = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the digital_control_2
*
* \param[in]  digitalControl2 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalControl2Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_setDigitalControl2(const SJA1105P_digitalControl2Argument_t *pk_digitalControl2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_digitalControl2->txPolInv <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalControl2->rxPolInv <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_digitalControl2->txPolInv) << ((uint8_t) DC_2_TX_POL_INV_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DC_2_TX_POL_INV_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) DC_2_RX_POL_INV_MASK) & pk_digitalControl2->rxPolInv;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) DC_2_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the digital_control_2
*
* \param[out] p_digitalControl2 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalControl2Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_getDigitalControl2(SJA1105P_digitalControl2Argument_t *p_digitalControl2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) DC_2_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_2_TX_POL_INV_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DC_2_TX_POL_INV_SHIFT);  /* shift to right aligned position */
	p_digitalControl2->txPolInv = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DC_2_RX_POL_INV_MASK);   /* mask desired bits */
	p_digitalControl2->rxPolInv = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the digital_error_cnt
*
* \param[in]  digitalErrorCnt Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalErrorCntArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setDigitalErrorCnt(const SJA1105P_digitalErrorCntArgument_t *pk_digitalErrorCnt, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_digitalErrorCnt->iccEcEn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_digitalErrorCnt->cor <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_digitalErrorCnt->iccEcEn) << ((uint8_t) DE_CNT_ICC_EC_EN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) DE_CNT_ICC_EC_EN_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) DE_CNT_COR_MASK) & pk_digitalErrorCnt->cor;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) DE_CNT_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the digital_error_cnt
*
* \param[out] p_digitalErrorCnt Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_digitalErrorCntArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getDigitalErrorCnt(SJA1105P_digitalErrorCntArgument_t *p_digitalErrorCnt, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) DE_CNT_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DE_CNT_ICC_EC_EN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (DE_CNT_ICC_EC_EN_SHIFT);  /* shift to right aligned position */
	p_digitalErrorCnt->iccEcEn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (DE_CNT_COR_MASK);   /* mask desired bits */
	p_digitalErrorCnt->cor = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_typeNo_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_typeNo_t
*/
SJA1105P_typeNo_t SJA1105P_convertToTypeNo(uint32_t stdintValue)
{
	SJA1105P_typeNo_t ret;
	switch (stdintValue)
	{
		case 45U: ret = SJA1105P_e_typeNo_SJA1105RS_SGMII; break;
		default: ret = SJA1105P_e_typeNo_UNKNOWN; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_pseqState_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_pseqState_t
*/
SJA1105P_pseqState_t SJA1105P_convertToPseqState(uint32_t stdintValue)
{
	SJA1105P_pseqState_t ret;
	switch (stdintValue)
	{
		case 7U: ret = SJA1105P_e_pseqState_MPLL_OFF; break;
		case 6U: ret = SJA1105P_e_pseqState_WAIT_MPLL_OFF; break;
		case 5U: ret = SJA1105P_e_pseqState_WAIT_TX_RX_DOWN; break;
		case 4U: ret = SJA1105P_e_pseqState_TX_RX_STABLE; break;
		case 3U: ret = SJA1105P_e_pseqState_WAIT_RX_DPLL; break;
		case 2U: ret = SJA1105P_e_pseqState_WAIT_RXCLK; break;
		case 1U: ret = SJA1105P_e_pseqState_WAIT_TXCLK; break;
		default: ret = SJA1105P_e_pseqState_WAIT_MPLL_ON; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_pause_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_pause_t
*/
SJA1105P_pause_t SJA1105P_convertToPause(uint32_t stdintValue)
{
	SJA1105P_pause_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_pause_MIXED_PAUSE; break;
		case 2U: ret = SJA1105P_e_pause_SYMM_PAUSE; break;
		case 1U: ret = SJA1105P_e_pause_ASYMM_PAUSE; break;
		default: ret = SJA1105P_e_pause_NO_PAUSE; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_oui_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_oui_t
*/
SJA1105P_oui_t SJA1105P_convertToOui(uint32_t stdintValue)
{
	SJA1105P_oui_t ret;
	switch (stdintValue)
	{
		case 24631U: ret = SJA1105P_e_oui_NXP; break;
		default: ret = SJA1105P_e_oui_UNKNOWN; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_remoteFault_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_remoteFault_t
*/
SJA1105P_remoteFault_t SJA1105P_convertToRemoteFault(uint32_t stdintValue)
{
	SJA1105P_remoteFault_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_remoteFault_AUTONEG_ERROR; break;
		case 2U: ret = SJA1105P_e_remoteFault_LINK_FAIL; break;
		case 1U: ret = SJA1105P_e_remoteFault_OFFLINE; break;
		default: ret = SJA1105P_e_remoteFault_NO_ERROR; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_autonegMode_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_autonegMode_t
*/
SJA1105P_autonegMode_t SJA1105P_convertToAutonegMode(uint32_t stdintValue)
{
	SJA1105P_autonegMode_t ret;
	switch (stdintValue)
	{
		case 2U: ret = SJA1105P_e_autonegMode_CL37_SGMII; break;
		default: ret = SJA1105P_e_autonegMode_INVALID; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_macWidth_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_macWidth_t
*/
SJA1105P_macWidth_t SJA1105P_convertToMacWidth(uint32_t stdintValue)
{
	SJA1105P_macWidth_t ret;
	switch (stdintValue)
	{
		case 1U: ret = SJA1105P_e_macWidth_8BIT; break;
		default: ret = SJA1105P_e_macWidth_4BIT; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_testPattern_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_testPattern_t
*/
SJA1105P_testPattern_t SJA1105P_convertToTestPattern(uint32_t stdintValue)
{
	SJA1105P_testPattern_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_testPattern_RESERVED; break;
		case 2U: ret = SJA1105P_e_testPattern_MIXED_FREQUENCY; break;
		case 1U: ret = SJA1105P_e_testPattern_LOW_FREQUENCY; break;
		default: ret = SJA1105P_e_testPattern_HIGH_FREQUENCY; break;
	}
	return ret;
}

/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_txEdgerate_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_txEdgerate_t
*/
SJA1105P_txEdgerate_t SJA1105P_convertToTxEdgerate(uint32_t stdintValue)
{
	SJA1105P_txEdgerate_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_txEdgerate_RESERVED; break;
		case 2U: ret = SJA1105P_e_txEdgerate_SLOW; break;
		case 1U: ret = SJA1105P_e_txEdgerate_MEDIUM; break;
		default: ret = SJA1105P_e_txEdgerate_FAST; break;
	}
	return ret;
}
