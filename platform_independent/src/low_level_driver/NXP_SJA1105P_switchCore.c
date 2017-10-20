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
* \file NXP_SJA1105P_switchCore.c
*
* \author NXP Semiconductors
*
* \version 0.3.0
*
* \date 2017-07-04
*
* \brief Status information and control registers of the switch
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define WORD0 0U
#define WORD1 1U
#define WORD2 2U
#define WORD3 3U
#define WORD4 4U
#define WORD5 5U
#define WORD6 6U
#define WORD7 7U
#define WORD8 8U
#define WORD9 9U
#define WORD10 10U
#define WORD11 11U
/* register addresses */

/* register category device_id */
#define D_ID_ADDR (0x0)  /**< Address of the deviceId register */

/* register category configuration_flags */
#define CF_ADDR (0x1)  /**< Address of the configurationFlags register */

/* register category general_status_l2 */
#define GS_L2_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusL2 register */
#define GS_L2_ADDR       (0x3)  /**< Address of the generalStatusL2 register */

/* register category general_status_hashconfs */
#define GSH_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusHashconfs register */
#define GSH_ADDR       (0x5)  /**< Address of the generalStatusHashconfs register */

/* register category general_status_vlan */
#define GS_VLAN_ADDR (0x7)  /**< Address of the generalStatusVlan register */

/* register category general_status_mem */
#define GS_MEM_0_ADDR (0x9)  /**< Address of the generalStatusMem0 register */
#define GS_MEM_1_ADDR (0xa)  /**< Address of the generalStatusMem1 register */

/* register category general_status_drop */
#define GS_DROP_ADDR (0xb)  /**< Address of the generalStatusDrop register */

/* register category general_status_ram_error */
#define GS_RAM_E_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusRamError register */
#define GS_RAM_E_ADDR       (0xc)  /**< Address of the generalStatusRamError register */

/* register category general_status_ram_init */
#define GS_RAM_INIT_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusRamInit register */
#define GS_RAM_INIT_ADDR       (0xe)  /**< Address of the generalStatusRamInit register */

/* register category ethernet_port_status_control */
#define E_PORT_SC_ADDR (0x10)  /**< Address of the ethernetPortStatusControl register */

/* register category transmission_control */
#define TC_ADDR (0x12)  /**< Address of the transmissionControl register */

/* register category ptp_control */
#define PTP_C_0_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl0 register */
#define PTP_C_0_ADDR       (0x15)  /**< Address of the ptpControl0 register */
#define PTP_C_1_ADDR       (0x17)  /**< Address of the ptpControl1 register */
#define PTP_C_2_ADDR       (0x18)  /**< Address of the ptpControl2 register */
#define PTP_C_3_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl3 register */
#define PTP_C_3_ADDR       (0x19)  /**< Address of the ptpControl3 register */
#define PTP_C_4_ADDR       (0x1b)  /**< Address of the ptpControl4 register */
#define PTP_C_5_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl5 register */
#define PTP_C_5_ADDR       (0x1c)  /**< Address of the ptpControl5 register */
#define PTP_C_6_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl6 register */
#define PTP_C_6_ADDR       (0x1f)  /**< Address of the ptpControl6 register */

/* register category l2_art_entry */
#define L2_ART_E_WORD_COUNT (5)     /**< Number of data words contained in the l2ArtEntry register */
#define L2_ART_E_ADDR       (0x24)  /**< Address of the l2ArtEntry register */

/* register category l2_art_locked_entry */
#define L2_ART_LE_WORD_COUNT (5)     /**< Number of data words contained in the l2ArtLockedEntry register */
#define L2_ART_LE_ADDR       (0x24)  /**< Address of the l2ArtLockedEntry register */

/* register category l2_address_lookup_table_control */
#define L2_ALTC_ADDR (0x29)  /**< Address of the l2AddressLookupTableControl register */

/* register category l2_forwarding_table_entry */
#define L2_FTE_WORD_COUNT (2)     /**< Number of data words contained in the l2ForwardingTableEntry register */
#define L2_FTE_ADDR       (0x2a)  /**< Address of the l2ForwardingTableEntry register */

/* register category l2_forwarding_table_control */
#define L2_FTC_ADDR (0x2c)  /**< Address of the l2ForwardingTableControl register */

/* register category vlan_lookup_table_entry */
#define VLAN_LTE_WORD_COUNT (2)     /**< Number of data words contained in the vlanLookupTableEntry register */
#define VLAN_LTE_ADDR       (0x2d)  /**< Address of the vlanLookupTableEntry register */

/* register category vlan_lookup_table_control */
#define VLAN_LTC_ADDR (0x30)  /**< Address of the vlanLookupTableControl register */

/* register category credit_based_shaping_entry */
#define CBSE_WORD_COUNT (5)     /**< Number of data words contained in the creditBasedShapingEntry register */
#define CBSE_ADDR       (0x32)  /**< Address of the creditBasedShapingEntry register */

/* register category credit_based_shaping_control */
#define CBSC_ADDR (0x37)  /**< Address of the creditBasedShapingControl register */

/* register category retagging_table_entry */
#define RTE_WORD_COUNT (2)     /**< Number of data words contained in the retaggingTableEntry register */
#define RTE_ADDR       (0x38)  /**< Address of the retaggingTableEntry register */

/* register category retagging_table_control */
#define RTC_ADDR (0x3a)  /**< Address of the retaggingTableControl register */

/* register category general_parameters_entry */
#define GPE_WORD_COUNT (11)    /**< Number of data words contained in the generalParametersEntry register */
#define GPE_ADDR       (0x3b)  /**< Address of the generalParametersEntry register */

/* register category general_parameters_control */
#define GPC_ADDR (0x46)  /**< Address of the generalParametersControl register */

/* register category mac_cfg_table_entry */
#define MAC_CFG_TE_WORD_COUNT (8)     /**< Number of data words contained in the macCfgTableEntry register */
#define MAC_CFG_TE_ADDR       (0x4b)  /**< Address of the macCfgTableEntry register */

/* register category mac_cfg_table_control */
#define MAC_CFG_TC_ADDR (0x53)  /**< Address of the macCfgTableControl register */

/* register category l2_lookup_parameters_entry */
#define L2_LPE_WORD_COUNT (4)     /**< Number of data words contained in the l2LookupParametersEntry register */
#define L2_LPE_ADDR       (0x54)  /**< Address of the l2LookupParametersEntry register */

/* register category l2_lookup_parameters_control */
#define L2_LPC_ADDR (0x58)  /**< Address of the l2LookupParametersControl register */

/* register category ptp_egress */
#define PTP_E_PORT_INC            (0x4)
#define PTP_E_TIMESTAMP_INDEX_INC (0x2)
#define PTP_E_0_ADDR              (0xc0)  /**< Address of the ptpEgress0 register */
#define PTP_E_1_ADDR              (0xc1)  /**< Address of the ptpEgress1 register */

/* register category l2_memory_partition_status */
#define L2_MPS_ADDR (0x100)  /**< Address of the l2MemoryPartitionStatus register */

/* register category mac_level_diagn_counters */
#define MAC_LDC_PORT_INC (0x2)
#define MAC_LDC_ADDR     (0x200)  /**< Address of the macLevelDiagnCounters register */

/* register category mac_level_diagn_flags */
#define MAC_LDF_PORT_INC (0x2)
#define MAC_LDF_ADDR     (0x201)  /**< Address of the macLevelDiagnFlags register */

/* register category hl_diagn_counters_1 */
#define HL_DC_1_PORT_INC     (0x10)
#define HL_DC_1_0_WORD_COUNT (2)      /**< Number of data words contained in the hlDiagnCounters10 register */
#define HL_DC_1_0_ADDR       (0x400)  /**< Address of the hlDiagnCounters10 register */
#define HL_DC_1_1_WORD_COUNT (2)      /**< Number of data words contained in the hlDiagnCounters11 register */
#define HL_DC_1_1_ADDR       (0x402)  /**< Address of the hlDiagnCounters11 register */
#define HL_DC_1_2_WORD_COUNT (2)      /**< Number of data words contained in the hlDiagnCounters12 register */
#define HL_DC_1_2_ADDR       (0x404)  /**< Address of the hlDiagnCounters12 register */
#define HL_DC_1_3_WORD_COUNT (2)      /**< Number of data words contained in the hlDiagnCounters13 register */
#define HL_DC_1_3_ADDR       (0x406)  /**< Address of the hlDiagnCounters13 register */
#define HL_DC_1_4_ADDR       (0x408)  /**< Address of the hlDiagnCounters14 register */
#define HL_DC_1_5_ADDR       (0x40b)  /**< Address of the hlDiagnCounters15 register */
#define HL_DC_1_6_ADDR       (0x40c)  /**< Address of the hlDiagnCounters16 register */
#define HL_DC_1_7_ADDR       (0x40e)  /**< Address of the hlDiagnCounters17 register */
#define HL_DC_1_8_ADDR       (0x40f)  /**< Address of the hlDiagnCounters18 register */

/* register category hl_diagn_counters_2 */
#define HL_DC_2_PORT_INC (0x10)
#define HL_DC_2_0_ADDR   (0x600)  /**< Address of the hlDiagnCounters20 register */
#define HL_DC_2_1_ADDR   (0x601)  /**< Address of the hlDiagnCounters21 register */
#define HL_DC_2_2_ADDR   (0x602)  /**< Address of the hlDiagnCounters22 register */
#define HL_DC_2_3_ADDR   (0x603)  /**< Address of the hlDiagnCounters23 register */

/* register category hl_diagn_counters_occupancy */
#define HL_DCO_PORT_INC (0x10)
#define HL_DCO_ADDR     (0x604)  /**< Address of the hlDiagnCountersOccupancy register */

/* register category l2_memory_partition_error_counters */
#define L2_MPEC_ADDR (0x1000)  /**< Address of the l2MemoryPartitionErrorCounters register */

/* register category eth_stat_counters */
#define ETH_STAT_C_PORT_INC (0x18)
#define ETH_STAT_C_00_ADDR  (0x1400)  /**< Address of the ethStatCounters00 register */
#define ETH_STAT_C_01_ADDR  (0x1401)  /**< Address of the ethStatCounters01 register */
#define ETH_STAT_C_02_ADDR  (0x1402)  /**< Address of the ethStatCounters02 register */
#define ETH_STAT_C_03_ADDR  (0x1403)  /**< Address of the ethStatCounters03 register */
#define ETH_STAT_C_04_ADDR  (0x1404)  /**< Address of the ethStatCounters04 register */
#define ETH_STAT_C_05_ADDR  (0x1405)  /**< Address of the ethStatCounters05 register */
#define ETH_STAT_C_06_ADDR  (0x1406)  /**< Address of the ethStatCounters06 register */
#define ETH_STAT_C_07_ADDR  (0x1407)  /**< Address of the ethStatCounters07 register */
#define ETH_STAT_C_08_ADDR  (0x1408)  /**< Address of the ethStatCounters08 register */
#define ETH_STAT_C_09_ADDR  (0x1409)  /**< Address of the ethStatCounters09 register */
#define ETH_STAT_C_10_ADDR  (0x140a)  /**< Address of the ethStatCounters10 register */
#define ETH_STAT_C_11_ADDR  (0x140b)  /**< Address of the ethStatCounters11 register */
#define ETH_STAT_C_12_ADDR  (0x140c)  /**< Address of the ethStatCounters12 register */
#define ETH_STAT_C_13_ADDR  (0x140d)  /**< Address of the ethStatCounters13 register */
#define ETH_STAT_C_14_ADDR  (0x140e)  /**< Address of the ethStatCounters14 register */
#define ETH_STAT_C_15_ADDR  (0x140f)  /**< Address of the ethStatCounters15 register */
#define ETH_STAT_C_16_ADDR  (0x1410)  /**< Address of the ethStatCounters16 register */
#define ETH_STAT_C_17_ADDR  (0x1411)  /**< Address of the ethStatCounters17 register */
#define ETH_STAT_C_18_ADDR  (0x1412)  /**< Address of the ethStatCounters18 register */
#define ETH_STAT_C_19_ADDR  (0x1413)  /**< Address of the ethStatCounters19 register */
#define ETH_STAT_C_20_ADDR  (0x1414)  /**< Address of the ethStatCounters20 register */
#define ETH_STAT_C_21_ADDR  (0x1415)  /**< Address of the ethStatCounters21 register */
#define ETH_STAT_C_22_ADDR  (0x1416)  /**< Address of the ethStatCounters22 register */

/* register category l2_forwarding_parameters */
#define L2_FP_WORD_COUNT (3)       /**< Number of data words contained in the l2ForwardingParameters register */
#define L2_FP_ADDR       (0x8000)  /**< Address of the l2ForwardingParameters register */

/* register category avb_parameters_entry */
#define AVB_PE_WORD_COUNT (4)       /**< Number of data words contained in the avbParametersEntry register */
#define AVB_PE_ADDR       (0x8003)  /**< Address of the avbParametersEntry register */

/* register category avb_parameters_control */
#define AVB_PC_ADDR (0x8007)  /**< Address of the avbParametersControl register */

/* variable masks */

/* register category configuration_flags */
#define CF_CONFIGS_SHIFT (31)           /**< Shift value for the CONFIGS field */
#define CF_CRCCHKL_SHIFT (30)           /**< Shift value for the CRCCHKL field */
#define CF_CRCCHKL_MASK  (0x40000000U)  /**< Bit mask for the CRCCHKL field */
#define CF_IDS_SHIFT     (29)           /**< Shift value for the IDS field */
#define CF_IDS_MASK      (0x20000000U)  /**< Bit mask for the IDS field */
#define CF_CRCCHKG_SHIFT (28)           /**< Shift value for the CRCCHKG field */
#define CF_CRCCHKG_MASK  (0x10000000U)  /**< Bit mask for the CRCCHKG field */
#define CF_NSLOT_MASK    (0x0000000FU)  /**< Bit mask for the NSLOT field */

/* register category general_status_l2 */
#define GS_L2_PORTENF_SHIFT     (8)            /**< Shift value for the PORTENF field */
#define GS_L2_FWDS_SHIFT        (4)            /**< Shift value for the FWDS field */
#define GS_L2_FWDS_MASK         (0x00000010U)  /**< Bit mask for the FWDS field */
#define GS_L2_MACFDS_SHIFT      (3)            /**< Shift value for the MACFDS field */
#define GS_L2_MACFDS_MASK       (0x00000008U)  /**< Bit mask for the MACFDS field */
#define GS_L2_ENFFDS_SHIFT      (2)            /**< Shift value for the ENFFDS field */
#define GS_L2_ENFFDS_MASK       (0x00000004U)  /**< Bit mask for the ENFFDS field */
#define GS_L2_L2BUSYFDS_SHIFT   (1)            /**< Shift value for the L2BUSYFDS field */
#define GS_L2_L2BUSYFDS_MASK    (0x00000002U)  /**< Bit mask for the L2BUSYFDS field */
#define GS_L2_L2BUSYS_MASK      (0x00000001U)  /**< Bit mask for the L2BUSYS field */
#define GS_L2_MACADD_SHIFT_0    (16)           /**< Shift value for the MACADD field */
#define GS_L2_MACADD_SHIFT_OVLP (16)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADD field */

/* register category general_status_hashconfs */
#define GSH_VLANIDHC_SHIFT      (4)            /**< Shift value for the VLANIDHC field */
#define GSH_VLANIDHC_MASK       (0x0000FFF0U)  /**< Bit mask for the VLANIDHC field */
#define GSH_HASHCONFS_MASK      (0x00000001U)  /**< Bit mask for the HASHCONFS field */
#define GSH_MACADDHC_SHIFT_0    (16)           /**< Shift value for the MACADDHC field */
#define GSH_MACADDHC_SHIFT_OVLP (16)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADDHC field */

/* register category general_status_vlan */
#define GS_VLAN_WPVLANID_SHIFT   (16)           /**< Shift value for the WPVLANID field */
#define GS_VLAN_PORT_SHIFT       (8)            /**< Shift value for the PORT field */
#define GS_VLAN_VLANBUSYS_SHIFT  (4)            /**< Shift value for the VLANBUSYS field */
#define GS_VLAN_VLANBUSYS_MASK   (0x00000010U)  /**< Bit mask for the VLANBUSYS field */
#define GS_VLAN_WRONGPORTS_SHIFT (3)            /**< Shift value for the WRONGPORTS field */
#define GS_VLAN_WRONGPORTS_MASK  (0x00000008U)  /**< Bit mask for the WRONGPORTS field */
#define GS_VLAN_VNOTFOUNDS_SHIFT (2)            /**< Shift value for the VNOTFOUNDS field */
#define GS_VLAN_VNOTFOUNDS_MASK  (0x00000004U)  /**< Bit mask for the VNOTFOUNDS field */

/* register category general_status_mem */
#define GS_MEM_0_EMPTYS_SHIFT               (31)           /**< Shift value for the EMPTYS field */
#define GS_MEM_0_BUFFERS_MASK               (0x7FFFFFFFU)  /**< Bit mask for the BUFFERS field */
#define GS_MEM_1_BUFFERS_LOW_WATERMARK_MASK (0x7FFFFFFFU)  /**< Bit mask for the BUFFERS_LOW_WATERMARK field */

/* register category general_status_drop */
#define GS_DROP_PORT_SHIFT (8)            /**< Shift value for the PORT field */
#define GS_DROP_FWDS_SHIFT (1)            /**< Shift value for the FWDS field */
#define GS_DROP_FWDS_MASK  (0x00000002U)  /**< Bit mask for the FWDS field */
#define GS_DROP_PARTS_MASK (0x00000001U)  /**< Bit mask for the PARTS field */

/* register category general_status_ram_error */
#define GS_RAM_E_RAMPARERR_MASK_0     (0x001FFFFFU)  /**< Bit mask for the RAMPARERR field */
#define GS_RAM_E_RAMPARERR_MASK_1     (0x0000001FU)  /**< Bit mask for the RAMPARERR field */
#define GS_RAM_E_RAMPARERR_SHIFT_OVLP (21)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping RAMPARERR field */

/* register category general_status_ram_init */
#define GS_RAM_INIT_RPARINIT_MASK_0     (0x007FFFFFU)  /**< Bit mask for the RPARINIT field */
#define GS_RAM_INIT_RPARINIT_MASK_1     (0x0000001FU)  /**< Bit mask for the RPARINIT field */
#define GS_RAM_INIT_RPARINIT_SHIFT_OVLP (23)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping RPARINIT field */

/* register category ethernet_port_status_control */
#define E_PORT_SC_CLEARPORT_MASK (0x0000001FU)  /**< Bit mask for the CLEARPORT field */

/* register category transmission_control */
#define TC_INHIBITTX_MASK (0x0000001FU)  /**< Bit mask for the INHIBITTX field */

/* register category ptp_control */
#define PTP_C_0_PTPPINST_SHIFT_OVLP    (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPPINST field */
#define PTP_C_2_VALID_SHIFT            (31)           /**< Shift value for the VALID field */
#define PTP_C_2_STARTPTPCP_SHIFT       (28)           /**< Shift value for the STARTPTPCP field */
#define PTP_C_2_STARTPTPCP_MASK        (0x10000000U)  /**< Bit mask for the STARTPTPCP field */
#define PTP_C_2_STOPPTPCP_SHIFT        (27)           /**< Shift value for the STOPPTPCP field */
#define PTP_C_2_STOPPTPCP_MASK         (0x08000000U)  /**< Bit mask for the STOPPTPCP field */
#define PTP_C_2_SYNC_CASCADED_SHIFT    (26)           /**< Shift value for the SYNC_CASCADED field */
#define PTP_C_2_SYNC_CASCADED_MASK     (0x04000000U)  /**< Bit mask for the SYNC_CASCADED field */
#define PTP_C_2_RESPTP_SHIFT           (3)            /**< Shift value for the RESPTP field */
#define PTP_C_2_RESPTP_MASK            (0x00000008U)  /**< Bit mask for the RESPTP field */
#define PTP_C_2_CORRCLK4TS_SHIFT       (2)            /**< Shift value for the CORRCLK4TS field */
#define PTP_C_2_CORRCLK4TS_MASK        (0x00000004U)  /**< Bit mask for the CORRCLK4TS field */
#define PTP_C_2_PTPCLKSUB_SHIFT        (1)            /**< Shift value for the PTPCLKSUB field */
#define PTP_C_2_PTPCLKSUB_MASK         (0x00000002U)  /**< Bit mask for the PTPCLKSUB field */
#define PTP_C_2_PTPCLKADD_MASK         (0x00000001U)  /**< Bit mask for the PTPCLKADD field */
#define PTP_C_3_PTPCLKVAL_SHIFT_OVLP   (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPCLKVAL field */
#define PTP_C_5_PTPTSCLK_SHIFT_OVLP    (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPTSCLK field */
#define PTP_C_6_PTPTSSHADOW_SHIFT_OVLP (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPTSSHADOW field */

/* register category l2_art_entry */
#define L2_ART_E_INDEX_SHIFT            (6)            /**< Shift value for the INDEX field */
#define L2_ART_E_INDEX_MASK             (0x0000FFC0U)  /**< Bit mask for the INDEX field */
#define L2_ART_E_ENFPORT_SHIFT          (16)           /**< Shift value for the ENFPORT field */
#define L2_ART_E_ENFPORT_MASK           (0x00010000U)  /**< Bit mask for the ENFPORT field */
#define L2_ART_E_DESTPORTS_SHIFT        (17)           /**< Shift value for the DESTPORTS field */
#define L2_ART_E_DESTPORTS_MASK         (0x003E0000U)  /**< Bit mask for the DESTPORTS field */
#define L2_ART_E_MACADDR_SHIFT_0        (22)           /**< Shift value for the MACADDR field */
#define L2_ART_E_MACADDR_SHIFT_OVLP_1   (10)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADDR field */
#define L2_ART_E_MACADDR_MASK_2         (0x0000003FU)  /**< Bit mask for the MACADDR field */
#define L2_ART_E_MACADDR_SHIFT_OVLP_2   (42)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MACADDR field */
#define L2_ART_E_VLANID_SHIFT           (6)            /**< Shift value for the VLANID field */
#define L2_ART_E_VLANID_MASK            (0x0003FFC0U)  /**< Bit mask for the VLANID field */
#define L2_ART_E_INNER_OUTER_VLAN_SHIFT (18)           /**< Shift value for the INNER_OUTER_VLAN field */
#define L2_ART_E_INNER_OUTER_VLAN_MASK  (0x00040000U)  /**< Bit mask for the INNER_OUTER_VLAN field */
#define L2_ART_E_MASK_SHIFT_0           (19)           /**< Shift value for the MASK field */
#define L2_ART_E_MASK_SHIFT_OVLP_1      (13)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MASK field */
#define L2_ART_E_MASK_SHIFT_OVLP_2      (45)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MASK field */
#define L2_ART_E_AGE_SHIFT              (16)           /**< Shift value for the AGE field */
#define L2_ART_E_AGE_MASK               (0x7FFF0000U)  /**< Bit mask for the AGE field */
#define L2_ART_E_TOUCHED_SHIFT          (31)           /**< Shift value for the TOUCHED field */

/* register category l2_art_locked_entry */
#define L2_ART_LE_INDEX_SHIFT            (6)            /**< Shift value for the INDEX field */
#define L2_ART_LE_INDEX_MASK             (0x0000FFC0U)  /**< Bit mask for the INDEX field */
#define L2_ART_LE_ENFPORT_SHIFT          (16)           /**< Shift value for the ENFPORT field */
#define L2_ART_LE_ENFPORT_MASK           (0x00010000U)  /**< Bit mask for the ENFPORT field */
#define L2_ART_LE_DESTPORTS_SHIFT        (17)           /**< Shift value for the DESTPORTS field */
#define L2_ART_LE_DESTPORTS_MASK         (0x003E0000U)  /**< Bit mask for the DESTPORTS field */
#define L2_ART_LE_MACADDR_SHIFT_0        (22)           /**< Shift value for the MACADDR field */
#define L2_ART_LE_MACADDR_SHIFT_OVLP_1   (10)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADDR field */
#define L2_ART_LE_MACADDR_MASK_2         (0x0000003FU)  /**< Bit mask for the MACADDR field */
#define L2_ART_LE_MACADDR_SHIFT_OVLP_2   (42)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MACADDR field */
#define L2_ART_LE_VLANID_SHIFT           (6)            /**< Shift value for the VLANID field */
#define L2_ART_LE_VLANID_MASK            (0x0003FFC0U)  /**< Bit mask for the VLANID field */
#define L2_ART_LE_INNER_OUTER_VLAN_SHIFT (18)           /**< Shift value for the INNER_OUTER_VLAN field */
#define L2_ART_LE_INNER_OUTER_VLAN_MASK  (0x00040000U)  /**< Bit mask for the INNER_OUTER_VLAN field */
#define L2_ART_LE_MASK_SHIFT_0           (19)           /**< Shift value for the MASK field */
#define L2_ART_LE_MASK_SHIFT_OVLP_1      (13)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MASK field */
#define L2_ART_LE_MASK_SHIFT_OVLP_2      (45)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MASK field */
#define L2_ART_LE_RETAG_SHIFT            (16)           /**< Shift value for the RETAG field */
#define L2_ART_LE_RETAG_MASK             (0x00010000U)  /**< Bit mask for the RETAG field */
#define L2_ART_LE_MIRROR_SHIFT           (17)           /**< Shift value for the MIRROR field */
#define L2_ART_LE_MIRROR_MASK            (0x00020000U)  /**< Bit mask for the MIRROR field */
#define L2_ART_LE_TAKE_TS_SHIFT          (18)           /**< Shift value for the TAKE_TS field */
#define L2_ART_LE_TAKE_TS_MASK           (0x00040000U)  /**< Bit mask for the TAKE_TS field */
#define L2_ART_LE_MIRRORED_VLAN_SHIFT    (19)           /**< Shift value for the MIRRORED_VLAN field */
#define L2_ART_LE_MIRRORED_VLAN_MASK     (0x7FF80000U)  /**< Bit mask for the MIRRORED_VLAN field */
#define L2_ART_LE_TS_REG_SHIFT           (31)           /**< Shift value for the TS_REG field */

/* register category l2_address_lookup_table_control */
#define L2_ALTC_VALID_SHIFT     (31)           /**< Shift value for the VALID field */
#define L2_ALTC_RDWRSET_SHIFT   (30)           /**< Shift value for the RDWRSET field */
#define L2_ALTC_RDWRSET_MASK    (0x40000000U)  /**< Bit mask for the RDWRSET field */
#define L2_ALTC_ERRORS_SHIFT    (29)           /**< Shift value for the ERRORS field */
#define L2_ALTC_ERRORS_MASK     (0x20000000U)  /**< Bit mask for the ERRORS field */
#define L2_ALTC_LOCKEDS_SHIFT   (28)           /**< Shift value for the LOCKEDS field */
#define L2_ALTC_LOCKEDS_MASK    (0x10000000U)  /**< Bit mask for the LOCKEDS field */
#define L2_ALTC_VALIDENT_SHIFT  (27)           /**< Shift value for the VALIDENT field */
#define L2_ALTC_VALIDENT_MASK   (0x08000000U)  /**< Bit mask for the VALIDENT field */
#define L2_ALTC_MGMTROUTE_SHIFT (26)           /**< Shift value for the MGMTROUTE field */
#define L2_ALTC_MGMTROUTE_MASK  (0x04000000U)  /**< Bit mask for the MGMTROUTE field */
#define L2_ALTC_HOST_CMD_SHIFT  (23)           /**< Shift value for the HOST_CMD field */
#define L2_ALTC_HOST_CMD_MASK   (0x03800000U)  /**< Bit mask for the HOST_CMD field */

/* register category l2_forwarding_table_entry */
#define L2_FTE_VLAN_PMAP_0_SHIFT      (25)           /**< Shift value for the VLAN_PMAP_0 field */
#define L2_FTE_VLAN_PMAP_0_MASK       (0x0E000000U)  /**< Bit mask for the VLAN_PMAP_0 field */
#define L2_FTE_VLAN_PMAP_1_SHIFT      (28)           /**< Shift value for the VLAN_PMAP_1 field */
#define L2_FTE_VLAN_PMAP_1_MASK       (0x70000000U)  /**< Bit mask for the VLAN_PMAP_1 field */
#define L2_FTE_VLAN_PMAP_2_SHIFT_0    (31)           /**< Shift value for the VLAN_PMAP_2 field */
#define L2_FTE_VLAN_PMAP_2_MASK_1     (0x00000003U)  /**< Bit mask for the VLAN_PMAP_2 field */
#define L2_FTE_VLAN_PMAP_2_SHIFT_OVLP (1)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLAN_PMAP_2 field */
#define L2_FTE_VLAN_PMAP_3_SHIFT      (2)            /**< Shift value for the VLAN_PMAP_3 field */
#define L2_FTE_VLAN_PMAP_3_MASK       (0x0000001CU)  /**< Bit mask for the VLAN_PMAP_3 field */
#define L2_FTE_VLAN_PMAP_4_SHIFT      (5)            /**< Shift value for the VLAN_PMAP_4 field */
#define L2_FTE_VLAN_PMAP_4_MASK       (0x000000E0U)  /**< Bit mask for the VLAN_PMAP_4 field */
#define L2_FTE_VLAN_PMAP_5_SHIFT      (8)            /**< Shift value for the VLAN_PMAP_5 field */
#define L2_FTE_VLAN_PMAP_5_MASK       (0x00000700U)  /**< Bit mask for the VLAN_PMAP_5 field */
#define L2_FTE_VLAN_PMAP_6_SHIFT      (11)           /**< Shift value for the VLAN_PMAP_6 field */
#define L2_FTE_VLAN_PMAP_6_MASK       (0x00003800U)  /**< Bit mask for the VLAN_PMAP_6 field */
#define L2_FTE_VLAN_PMAP_7_SHIFT      (14)           /**< Shift value for the VLAN_PMAP_7 field */
#define L2_FTE_VLAN_PMAP_7_MASK       (0x0001C000U)  /**< Bit mask for the VLAN_PMAP_7 field */
#define L2_FTE_FL_DOMAIN_SHIFT        (17)           /**< Shift value for the FL_DOMAIN field */
#define L2_FTE_FL_DOMAIN_MASK         (0x003E0000U)  /**< Bit mask for the FL_DOMAIN field */
#define L2_FTE_REACH_PORT_SHIFT       (22)           /**< Shift value for the REACH_PORT field */
#define L2_FTE_REACH_PORT_MASK        (0x07C00000U)  /**< Bit mask for the REACH_PORT field */
#define L2_FTE_BC_DOMAIN_SHIFT        (27)           /**< Shift value for the BC_DOMAIN field */

/* register category l2_forwarding_table_control */
#define L2_FTC_VALID_SHIFT   (31)           /**< Shift value for the VALID field */
#define L2_FTC_ERRORS_SHIFT  (30)           /**< Shift value for the ERRORS field */
#define L2_FTC_ERRORS_MASK   (0x40000000U)  /**< Bit mask for the ERRORS field */
#define L2_FTC_RDWRSET_SHIFT (29)           /**< Shift value for the RDWRSET field */
#define L2_FTC_RDWRSET_MASK  (0x20000000U)  /**< Bit mask for the RDWRSET field */
#define L2_FTC_INDEX_MASK    (0x0000001FU)  /**< Bit mask for the INDEX field */

/* register category vlan_lookup_table_entry */
#define VLAN_LTE_VLANID_SHIFT_0    (27)           /**< Shift value for the VLANID field */
#define VLAN_LTE_VLANID_MASK_1     (0x0000007FU)  /**< Bit mask for the VLANID field */
#define VLAN_LTE_VLANID_SHIFT_OVLP (5)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLANID field */
#define VLAN_LTE_TAG_PORT_SHIFT    (7)            /**< Shift value for the TAG_PORT field */
#define VLAN_LTE_TAG_PORT_MASK     (0x00000F80U)  /**< Bit mask for the TAG_PORT field */
#define VLAN_LTE_VLAN_BC_SHIFT     (12)           /**< Shift value for the VLAN_BC field */
#define VLAN_LTE_VLAN_BC_MASK      (0x0001F000U)  /**< Bit mask for the VLAN_BC field */
#define VLAN_LTE_VMEMB_PORT_SHIFT  (17)           /**< Shift value for the VMEMB_PORT field */
#define VLAN_LTE_VMEMB_PORT_MASK   (0x003E0000U)  /**< Bit mask for the VMEMB_PORT field */
#define VLAN_LTE_VEGR_MIRR_SHIFT   (22)           /**< Shift value for the VEGR_MIRR field */
#define VLAN_LTE_VEGR_MIRR_MASK    (0x07C00000U)  /**< Bit mask for the VEGR_MIRR field */
#define VLAN_LTE_VING_MIRR_SHIFT   (27)           /**< Shift value for the VING_MIRR field */

/* register category vlan_lookup_table_control */
#define VLAN_LTC_VALID_SHIFT    (31)           /**< Shift value for the VALID field */
#define VLAN_LTC_RDWRSET_SHIFT  (30)           /**< Shift value for the RDWRSET field */
#define VLAN_LTC_RDWRSET_MASK   (0x40000000U)  /**< Bit mask for the RDWRSET field */
#define VLAN_LTC_VALIDENT_SHIFT (27)           /**< Shift value for the VALIDENT field */
#define VLAN_LTC_VALIDENT_MASK  (0x08000000U)  /**< Bit mask for the VALIDENT field */

/* register category credit_based_shaping_entry */
#define CBSE_IDLE_SLOPE_SHIFT_0    (26)           /**< Shift value for the IDLE_SLOPE field */
#define CBSE_IDLE_SLOPE_MASK_1     (0x03FFFFFFU)  /**< Bit mask for the IDLE_SLOPE field */
#define CBSE_IDLE_SLOPE_SHIFT_OVLP (6)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping IDLE_SLOPE field */
#define CBSE_SEND_SLOPE_SHIFT_0    (26)           /**< Shift value for the SEND_SLOPE field */
#define CBSE_SEND_SLOPE_MASK_1     (0x03FFFFFFU)  /**< Bit mask for the SEND_SLOPE field */
#define CBSE_SEND_SLOPE_SHIFT_OVLP (6)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping SEND_SLOPE field */
#define CBSE_CREDIT_HI_SHIFT_0     (26)           /**< Shift value for the CREDIT_HI field */
#define CBSE_CREDIT_HI_MASK_1      (0x03FFFFFFU)  /**< Bit mask for the CREDIT_HI field */
#define CBSE_CREDIT_HI_SHIFT_OVLP  (6)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping CREDIT_HI field */
#define CBSE_CREDIT_LO_SHIFT_0     (26)           /**< Shift value for the CREDIT_LO field */
#define CBSE_CREDIT_LO_MASK_1      (0x03FFFFFFU)  /**< Bit mask for the CREDIT_LO field */
#define CBSE_CREDIT_LO_SHIFT_OVLP  (6)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping CREDIT_LO field */
#define CBSE_CBS_PRIO_SHIFT        (26)           /**< Shift value for the CBS_PRIO field */
#define CBSE_CBS_PRIO_MASK         (0x1C000000U)  /**< Bit mask for the CBS_PRIO field */
#define CBSE_CBS_PORT_SHIFT        (29)           /**< Shift value for the CBS_PORT field */

/* register category credit_based_shaping_control */
#define CBSC_VALID_SHIFT    (31)           /**< Shift value for the VALID field */
#define CBSC_RDWRSET_SHIFT  (30)           /**< Shift value for the RDWRSET field */
#define CBSC_RDWRSET_MASK   (0x40000000U)  /**< Bit mask for the RDWRSET field */
#define CBSC_ERRORS_SHIFT   (29)           /**< Shift value for the ERRORS field */
#define CBSC_ERRORS_MASK    (0x20000000U)  /**< Bit mask for the ERRORS field */
#define CBSC_SHAPER_ID_MASK (0x0000000FU)  /**< Bit mask for the SHAPER_ID field */

/* register category retagging_table_entry */
#define RTE_DESTPORTS_SHIFT     (23)           /**< Shift value for the DESTPORTS field */
#define RTE_DESTPORTS_MASK      (0x0F800000U)  /**< Bit mask for the DESTPORTS field */
#define RTE_USE_DESTPORTS_SHIFT (28)           /**< Shift value for the USE_DESTPORTS field */
#define RTE_USE_DESTPORTS_MASK  (0x10000000U)  /**< Bit mask for the USE_DESTPORTS field */
#define RTE_DO_NOT_LEARN_SHIFT  (29)           /**< Shift value for the DO_NOT_LEARN field */
#define RTE_DO_NOT_LEARN_MASK   (0x20000000U)  /**< Bit mask for the DO_NOT_LEARN field */
#define RTE_VLAN_EGR_SHIFT_0    (30)           /**< Shift value for the VLAN_EGR field */
#define RTE_VLAN_EGR_MASK_1     (0x000003FFU)  /**< Bit mask for the VLAN_EGR field */
#define RTE_VLAN_EGR_SHIFT_OVLP (2)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLAN_EGR field */
#define RTE_VLAN_ING_SHIFT      (10)           /**< Shift value for the VLAN_ING field */
#define RTE_VLAN_ING_MASK       (0x003FFC00U)  /**< Bit mask for the VLAN_ING field */
#define RTE_ING_PORT_SHIFT      (22)           /**< Shift value for the ING_PORT field */
#define RTE_ING_PORT_MASK       (0x07C00000U)  /**< Bit mask for the ING_PORT field */
#define RTE_EGR_PORT_SHIFT      (27)           /**< Shift value for the EGR_PORT field */

/* register category retagging_table_control */
#define RTC_VALID_SHIFT    (31)           /**< Shift value for the VALID field */
#define RTC_ERRORS_SHIFT   (30)           /**< Shift value for the ERRORS field */
#define RTC_ERRORS_MASK    (0x40000000U)  /**< Bit mask for the ERRORS field */
#define RTC_VALIDENT_SHIFT (29)           /**< Shift value for the VALIDENT field */
#define RTC_VALIDENT_MASK  (0x20000000U)  /**< Bit mask for the VALIDENT field */
#define RTC_RDWRSET_SHIFT  (28)           /**< Shift value for the RDWRSET field */
#define RTC_RDWRSET_MASK   (0x10000000U)  /**< Bit mask for the RDWRSET field */
#define RTC_INDEX_MASK     (0x0000003FU)  /**< Bit mask for the INDEX field */

/* register category general_parameters_entry */
#define GPE_EGR_MIRR_OUTER_DEI_SHIFT          (25)           /**< Shift value for the EGR_MIRR_OUTER_DEI field */
#define GPE_EGR_MIRR_OUTER_DEI_MASK           (0x02000000U)  /**< Bit mask for the EGR_MIRR_OUTER_DEI field */
#define GPE_EGR_MIRR_OUTER_PCP_SHIFT          (26)           /**< Shift value for the EGR_MIRR_OUTER_PCP field */
#define GPE_EGR_MIRR_OUTER_PCP_MASK           (0x1C000000U)  /**< Bit mask for the EGR_MIRR_OUTER_PCP field */
#define GPE_EGR_MIRR_OUTER_TAG_SHIFT_0        (29)           /**< Shift value for the EGR_MIRR_OUTER_TAG field */
#define GPE_EGR_MIRR_OUTER_TAG_MASK_1         (0x000001FFU)  /**< Bit mask for the EGR_MIRR_OUTER_TAG field */
#define GPE_EGR_MIRR_OUTER_TAG_SHIFT_OVLP     (3)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping EGR_MIRR_OUTER_TAG field */
#define GPE_TAKE_EGR_TS_ON_HOST_PORT_SHIFT    (9)            /**< Shift value for the TAKE_EGR_TS_ON_HOST_PORT field */
#define GPE_TAKE_EGR_TS_ON_HOST_PORT_MASK     (0x00000200U)  /**< Bit mask for the TAKE_EGR_TS_ON_HOST_PORT field */
#define GPE_TPID2_SHIFT                       (10)           /**< Shift value for the TPID2 field */
#define GPE_IGNORE_2_STEP_FLAG_SHIFT          (26)           /**< Shift value for the IGNORE_2_STEP_FLAG field */
#define GPE_IGNORE_2_STEP_FLAG_MASK           (0x04000000U)  /**< Bit mask for the IGNORE_2_STEP_FLAG field */
#define GPE_TPID_SHIFT_0                      (27)           /**< Shift value for the TPID field */
#define GPE_TPID_MASK_1                       (0x000007FFU)  /**< Bit mask for the TPID field */
#define GPE_TPID_SHIFT_OVLP                   (5)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping TPID field */
#define GPE_VL_MASK_SHIFT_0                   (11)           /**< Shift value for the VL_MASK field */
#define GPE_VL_MASK_MASK_1                    (0x000007FFU)  /**< Bit mask for the VL_MASK field */
#define GPE_VL_MASK_SHIFT_OVLP                (21)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VL_MASK field */
#define GPE_VL_MARKER_SHIFT_0                 (11)           /**< Shift value for the VL_MARKER field */
#define GPE_VL_MARKER_MASK_1                  (0x000007FFU)  /**< Bit mask for the VL_MARKER field */
#define GPE_VL_MARKER_SHIFT_OVLP              (21)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VL_MARKER field */
#define GPE_MIRRORP_SHIFT                     (11)           /**< Shift value for the MIRRORP field */
#define GPE_MIRRORP_MASK                      (0x00003800U)  /**< Bit mask for the MIRRORP field */
#define GPE_HOST_PORT_SHIFT                   (14)           /**< Shift value for the HOST_PORT field */
#define GPE_HOST_PORT_MASK                    (0x0001C000U)  /**< Bit mask for the HOST_PORT field */
#define GPE_CASCADING_PORT_SHIFT              (17)           /**< Shift value for the CASCADING_PORT field */
#define GPE_CASCADING_PORT_MASK               (0x000E0000U)  /**< Bit mask for the CASCADING_PORT field */
#define GPE_SEND_FOLLOW_UP_TS_0_SHIFT         (20)           /**< Shift value for the SEND_FOLLOW_UP_TS_0 field */
#define GPE_SEND_FOLLOW_UP_TS_0_MASK          (0x00100000U)  /**< Bit mask for the SEND_FOLLOW_UP_TS_0 field */
#define GPE_SEND_FOLLOW_UP_TS_1_SHIFT         (21)           /**< Shift value for the SEND_FOLLOW_UP_TS_1 field */
#define GPE_SEND_FOLLOW_UP_TS_1_MASK          (0x00200000U)  /**< Bit mask for the SEND_FOLLOW_UP_TS_1 field */
#define GPE_INCLUDE_SRC_PORT_0_SHIFT          (22)           /**< Shift value for the INCLUDE_SRC_PORT_0 field */
#define GPE_INCLUDE_SRC_PORT_0_MASK           (0x00400000U)  /**< Bit mask for the INCLUDE_SRC_PORT_0 field */
#define GPE_INCLUDE_SRC_PORT_1_SHIFT          (23)           /**< Shift value for the INCLUDE_SRC_PORT_1 field */
#define GPE_INCLUDE_SRC_PORT_1_MASK           (0x00800000U)  /**< Bit mask for the INCLUDE_SRC_PORT_1 field */
#define GPE_MAC_FILTER_0_SHIFT_0              (24)           /**< Shift value for the MAC_FILTER_0 field */
#define GPE_MAC_FILTER_0_SHIFT_OVLP_1         (8)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAC_FILTER_0 field */
#define GPE_MAC_FILTER_0_SHIFT_OVLP_2         (40)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MAC_FILTER_0 field */
#define GPE_MAC_FILTER_1_SHIFT_0              (8)            /**< Shift value for the MAC_FILTER_1 field */
#define GPE_MAC_FILTER_1_MASK_1               (0x00FFFFFFU)  /**< Bit mask for the MAC_FILTER_1 field */
#define GPE_MAC_FILTER_1_SHIFT_OVLP           (24)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAC_FILTER_1 field */
#define GPE_MAC_FILTER_RESULT_0_SHIFT_0       (24)           /**< Shift value for the MAC_FILTER_RESULT_0 field */
#define GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_1  (8)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAC_FILTER_RESULT_0 field */
#define GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_2  (40)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping MAC_FILTER_RESULT_0 field */
#define GPE_MAC_FILTER_RESULT_1_SHIFT_0       (8)            /**< Shift value for the MAC_FILTER_RESULT_1 field */
#define GPE_MAC_FILTER_RESULT_1_MASK_1        (0x00FFFFFFU)  /**< Bit mask for the MAC_FILTER_RESULT_1 field */
#define GPE_MAC_FILTER_RESULT_1_SHIFT_OVLP    (24)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAC_FILTER_RESULT_1 field */
#define GPE_MGMT_PRIO_SHIFT                   (24)           /**< Shift value for the MGMT_PRIO field */
#define GPE_MGMT_PRIO_MASK                    (0x07000000U)  /**< Bit mask for the MGMT_PRIO field */
#define GPE_ID_SHIFT                          (27)           /**< Shift value for the ID field */
#define GPE_ID_MASK                           (0x38000000U)  /**< Bit mask for the ID field */
#define GPE_MIRROR_PORT_RECONFIG_ENABLE_SHIFT (30)           /**< Shift value for the MIRROR_PORT_RECONFIG_ENABLE field */
#define GPE_MIRROR_PORT_RECONFIG_ENABLE_MASK  (0x40000000U)  /**< Bit mask for the MIRROR_PORT_RECONFIG_ENABLE field */
#define GPE_ARINC_SHIFT                       (31)           /**< Shift value for the ARINC field */

/* register category general_parameters_control */
#define GPC_VALID_SHIFT                    (31)           /**< Shift value for the VALID field */
#define GPC_ERRORS_SHIFT                   (30)           /**< Shift value for the ERRORS field */
#define GPC_ERRORS_MASK                    (0x40000000U)  /**< Bit mask for the ERRORS field */
#define GPC_MIRR_PORT_RECONFIG_ERROR_SHIFT (29)           /**< Shift value for the MIRR_PORT_RECONFIG_ERROR field */
#define GPC_MIRR_PORT_RECONFIG_ERROR_MASK  (0x20000000U)  /**< Bit mask for the MIRR_PORT_RECONFIG_ERROR field */
#define GPC_RDWRSET_SHIFT                  (28)           /**< Shift value for the RDWRSET field */
#define GPC_RDWRSET_MASK                   (0x10000000U)  /**< Bit mask for the RDWRSET field */

/* register category mac_cfg_table_entry */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_SHIFT       (13)           /**< Shift value for the INGRESS_MIRRORING_OUTER_DEI field */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_MASK        (0x00002000U)  /**< Bit mask for the INGRESS_MIRRORING_OUTER_DEI field */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_SHIFT       (14)           /**< Shift value for the INGRESS_MIRRORING_OUTER_PCP field */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_MASK        (0x0001C000U)  /**< Bit mask for the INGRESS_MIRRORING_OUTER_PCP field */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_SHIFT       (17)           /**< Shift value for the INGRESS_MIRRORING_OUTER_TAG field */
#define MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_MASK        (0x1FFE0000U)  /**< Bit mask for the INGRESS_MIRRORING_OUTER_TAG field */
#define MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_SHIFT     (29)           /**< Shift value for the MIRROR_CONFLICT_ON_EGRESS_TAG field */
#define MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_MASK      (0x20000000U)  /**< Bit mask for the MIRROR_CONFLICT_ON_EGRESS_TAG field */
#define MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_SHIFT (30)           /**< Shift value for the MIRROR_CONFLICT_ON_INGRESS_EGRESS field */
#define MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_MASK  (0x40000000U)  /**< Bit mask for the MIRROR_CONFLICT_ON_INGRESS_EGRESS field */
#define MAC_CFG_TE_INGRESS_SHIFT                           (31)           /**< Shift value for the INGRESS field */
#define MAC_CFG_TE_EGRESS_MASK                             (0x00000001U)  /**< Bit mask for the EGRESS field */
#define MAC_CFG_TE_DYN_LEARN_SHIFT                         (1)            /**< Shift value for the DYN_LEARN field */
#define MAC_CFG_TE_DYN_LEARN_MASK                          (0x00000002U)  /**< Bit mask for the DYN_LEARN field */
#define MAC_CFG_TE_RETAG_SHIFT                             (2)            /**< Shift value for the RETAG field */
#define MAC_CFG_TE_RETAG_MASK                              (0x00000004U)  /**< Bit mask for the RETAG field */
#define MAC_CFG_TE_DRPUNTAG_SHIFT                          (3)            /**< Shift value for the DRPUNTAG field */
#define MAC_CFG_TE_DRPUNTAG_MASK                           (0x00000008U)  /**< Bit mask for the DRPUNTAG field */
#define MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_SHIFT          (4)            /**< Shift value for the DROP_SINGLE_INNER_TAGGED field */
#define MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_MASK           (0x00000010U)  /**< Bit mask for the DROP_SINGLE_INNER_TAGGED field */
#define MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_SHIFT          (5)            /**< Shift value for the DROP_SINGLE_OUTER_TAGGED field */
#define MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_MASK           (0x00000020U)  /**< Bit mask for the DROP_SINGLE_OUTER_TAGGED field */
#define MAC_CFG_TE_DROP_DOUBLE_TAGGED_SHIFT                (6)            /**< Shift value for the DROP_DOUBLE_TAGGED field */
#define MAC_CFG_TE_DROP_DOUBLE_TAGGED_MASK                 (0x00000040U)  /**< Bit mask for the DROP_DOUBLE_TAGGED field */
#define MAC_CFG_TE_DROP_NON_A664_SHIFT                     (7)            /**< Shift value for the DROP_NON_A664 field */
#define MAC_CFG_TE_DROP_NON_A664_MASK                      (0x00000080U)  /**< Bit mask for the DROP_NON_A664 field */
#define MAC_CFG_TE_EGRMIRR_SHIFT                           (8)            /**< Shift value for the EGRMIRR field */
#define MAC_CFG_TE_EGRMIRR_MASK                            (0x00000100U)  /**< Bit mask for the EGRMIRR field */
#define MAC_CFG_TE_INGMIRR_SHIFT                           (9)            /**< Shift value for the INGMIRR field */
#define MAC_CFG_TE_INGMIRR_MASK                            (0x00000200U)  /**< Bit mask for the INGMIRR field */
#define MAC_CFG_TE_VLANID_SHIFT                            (10)           /**< Shift value for the VLANID field */
#define MAC_CFG_TE_VLANID_MASK                             (0x003FFC00U)  /**< Bit mask for the VLANID field */
#define MAC_CFG_TE_VLAN_PRIO_SHIFT                         (22)           /**< Shift value for the VLAN_PRIO field */
#define MAC_CFG_TE_VLAN_PRIO_MASK                          (0x01C00000U)  /**< Bit mask for the VLAN_PRIO field */
#define MAC_CFG_TE_MAX_AGE_SHIFT_0                         (25)           /**< Shift value for the MAX_AGE field */
#define MAC_CFG_TE_MAX_AGE_MASK_1                          (0x00000001U)  /**< Bit mask for the MAX_AGE field */
#define MAC_CFG_TE_MAX_AGE_SHIFT_OVLP                      (7)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAX_AGE field */
#define MAC_CFG_TE_TPDELOUT_SHIFT                          (1)            /**< Shift value for the TPDELOUT field */
#define MAC_CFG_TE_TPDELIN_SHIFT_0                         (17)           /**< Shift value for the TPDELIN field */
#define MAC_CFG_TE_TPDELIN_MASK_1                          (0x00000001U)  /**< Bit mask for the TPDELIN field */
#define MAC_CFG_TE_TPDELIN_SHIFT_OVLP                      (15)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping TPDELIN field */
#define MAC_CFG_TE_SPEED_SHIFT                             (1)            /**< Shift value for the SPEED field */
#define MAC_CFG_TE_SPEED_MASK                              (0x00000006U)  /**< Bit mask for the SPEED field */
#define MAC_CFG_TE_IFG_SHIFT                               (3)            /**< Shift value for the IFG field */
#define MAC_CFG_TE_IFG_MASK                                (0x000000F8U)  /**< Bit mask for the IFG field */
#define MAC_CFG_TE_ENABLED_0_SHIFT                         (8)            /**< Shift value for the ENABLED_0 field */
#define MAC_CFG_TE_ENABLED_0_MASK                          (0x00000100U)  /**< Bit mask for the ENABLED_0 field */
#define MAC_CFG_TE_BASE_0_SHIFT                            (9)            /**< Shift value for the BASE_0 field */
#define MAC_CFG_TE_BASE_0_MASK                             (0x0003FE00U)  /**< Bit mask for the BASE_0 field */
#define MAC_CFG_TE_TOP_0_SHIFT                             (18)           /**< Shift value for the TOP_0 field */
#define MAC_CFG_TE_TOP_0_MASK                              (0x07FC0000U)  /**< Bit mask for the TOP_0 field */
#define MAC_CFG_TE_ENABLED_1_SHIFT                         (27)           /**< Shift value for the ENABLED_1 field */
#define MAC_CFG_TE_ENABLED_1_MASK                          (0x08000000U)  /**< Bit mask for the ENABLED_1 field */
#define MAC_CFG_TE_BASE_1_SHIFT_0                          (28)           /**< Shift value for the BASE_1 field */
#define MAC_CFG_TE_BASE_1_MASK_1                           (0x0000001FU)  /**< Bit mask for the BASE_1 field */
#define MAC_CFG_TE_BASE_1_SHIFT_OVLP                       (4)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping BASE_1 field */
#define MAC_CFG_TE_TOP_1_SHIFT                             (5)            /**< Shift value for the TOP_1 field */
#define MAC_CFG_TE_TOP_1_MASK                              (0x00003FE0U)  /**< Bit mask for the TOP_1 field */
#define MAC_CFG_TE_ENABLED_2_SHIFT                         (14)           /**< Shift value for the ENABLED_2 field */
#define MAC_CFG_TE_ENABLED_2_MASK                          (0x00004000U)  /**< Bit mask for the ENABLED_2 field */
#define MAC_CFG_TE_BASE_2_SHIFT                            (15)           /**< Shift value for the BASE_2 field */
#define MAC_CFG_TE_BASE_2_MASK                             (0x00FF8000U)  /**< Bit mask for the BASE_2 field */
#define MAC_CFG_TE_TOP_2_SHIFT_0                           (24)           /**< Shift value for the TOP_2 field */
#define MAC_CFG_TE_TOP_2_MASK_1                            (0x00000001U)  /**< Bit mask for the TOP_2 field */
#define MAC_CFG_TE_TOP_2_SHIFT_OVLP                        (8)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping TOP_2 field */
#define MAC_CFG_TE_ENABLED_3_SHIFT                         (1)            /**< Shift value for the ENABLED_3 field */
#define MAC_CFG_TE_ENABLED_3_MASK                          (0x00000002U)  /**< Bit mask for the ENABLED_3 field */
#define MAC_CFG_TE_BASE_3_SHIFT                            (2)            /**< Shift value for the BASE_3 field */
#define MAC_CFG_TE_BASE_3_MASK                             (0x000007FCU)  /**< Bit mask for the BASE_3 field */
#define MAC_CFG_TE_TOP_3_SHIFT                             (11)           /**< Shift value for the TOP_3 field */
#define MAC_CFG_TE_TOP_3_MASK                              (0x000FF800U)  /**< Bit mask for the TOP_3 field */
#define MAC_CFG_TE_ENABLED_4_SHIFT                         (20)           /**< Shift value for the ENABLED_4 field */
#define MAC_CFG_TE_ENABLED_4_MASK                          (0x00100000U)  /**< Bit mask for the ENABLED_4 field */
#define MAC_CFG_TE_BASE_4_SHIFT                            (21)           /**< Shift value for the BASE_4 field */
#define MAC_CFG_TE_BASE_4_MASK                             (0x3FE00000U)  /**< Bit mask for the BASE_4 field */
#define MAC_CFG_TE_TOP_4_SHIFT_0                           (30)           /**< Shift value for the TOP_4 field */
#define MAC_CFG_TE_TOP_4_MASK_1                            (0x0000007FU)  /**< Bit mask for the TOP_4 field */
#define MAC_CFG_TE_TOP_4_SHIFT_OVLP                        (2)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping TOP_4 field */
#define MAC_CFG_TE_ENABLED_5_SHIFT                         (7)            /**< Shift value for the ENABLED_5 field */
#define MAC_CFG_TE_ENABLED_5_MASK                          (0x00000080U)  /**< Bit mask for the ENABLED_5 field */
#define MAC_CFG_TE_BASE_5_SHIFT                            (8)            /**< Shift value for the BASE_5 field */
#define MAC_CFG_TE_BASE_5_MASK                             (0x0001FF00U)  /**< Bit mask for the BASE_5 field */
#define MAC_CFG_TE_TOP_5_SHIFT                             (17)           /**< Shift value for the TOP_5 field */
#define MAC_CFG_TE_TOP_5_MASK                              (0x03FE0000U)  /**< Bit mask for the TOP_5 field */
#define MAC_CFG_TE_ENABLED_6_SHIFT                         (26)           /**< Shift value for the ENABLED_6 field */
#define MAC_CFG_TE_ENABLED_6_MASK                          (0x04000000U)  /**< Bit mask for the ENABLED_6 field */
#define MAC_CFG_TE_BASE_6_SHIFT_0                          (27)           /**< Shift value for the BASE_6 field */
#define MAC_CFG_TE_BASE_6_MASK_1                           (0x0000000FU)  /**< Bit mask for the BASE_6 field */
#define MAC_CFG_TE_BASE_6_SHIFT_OVLP                       (5)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping BASE_6 field */
#define MAC_CFG_TE_TOP_6_SHIFT                             (4)            /**< Shift value for the TOP_6 field */
#define MAC_CFG_TE_TOP_6_MASK                              (0x00001FF0U)  /**< Bit mask for the TOP_6 field */
#define MAC_CFG_TE_ENABLED_7_SHIFT                         (13)           /**< Shift value for the ENABLED_7 field */
#define MAC_CFG_TE_ENABLED_7_MASK                          (0x00002000U)  /**< Bit mask for the ENABLED_7 field */
#define MAC_CFG_TE_BASE_7_SHIFT                            (14)           /**< Shift value for the BASE_7 field */
#define MAC_CFG_TE_BASE_7_MASK                             (0x007FC000U)  /**< Bit mask for the BASE_7 field */
#define MAC_CFG_TE_TOP_7_SHIFT                             (23)           /**< Shift value for the TOP_7 field */

/* register category mac_cfg_table_control */
#define MAC_CFG_TC_VALID_SHIFT   (31)           /**< Shift value for the VALID field */
#define MAC_CFG_TC_ERRORS_SHIFT  (30)           /**< Shift value for the ERRORS field */
#define MAC_CFG_TC_ERRORS_MASK   (0x40000000U)  /**< Bit mask for the ERRORS field */
#define MAC_CFG_TC_RDWRSET_SHIFT (29)           /**< Shift value for the RDWRSET field */
#define MAC_CFG_TC_RDWRSET_MASK  (0x20000000U)  /**< Bit mask for the RDWRSET field */
#define MAC_CFG_TC_PORT_MASK     (0x0000001FU)  /**< Bit mask for the PORT field */

/* register category l2_lookup_parameters_entry */
#define L2_LPE_ONE_SHOT_LEARNING_SHIFT                          (22)           /**< Shift value for the ONE_SHOT_LEARNING field */
#define L2_LPE_ONE_SHOT_LEARNING_MASK                           (0x00400000U)  /**< Bit mask for the ONE_SHOT_LEARNING field */
#define L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_SHIFT                 (23)           /**< Shift value for the OVERWRITE_DYN_IF_TCAM_FULL field */
#define L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_MASK                  (0x00800000U)  /**< Bit mask for the OVERWRITE_DYN_IF_TCAM_FULL field */
#define L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_SHIFT         (24)           /**< Shift value for the USE_FREE_STATIC_SPACE_FOR_LEARNING field */
#define L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_MASK          (0x01000000U)  /**< Bit mask for the USE_FREE_STATIC_SPACE_FOR_LEARNING field */
#define L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_SHIFT              (25)           /**< Shift value for the IGNORE_MGMT_ADDR_AT_HOST_PORT field */
#define L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_MASK               (0x02000000U)  /**< Bit mask for the IGNORE_MGMT_ADDR_AT_HOST_PORT field */
#define L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_SHIFT         (26)           /**< Shift value for the SKIP_PORT_ENFORCEMENT_AT_HOST_PORT field */
#define L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_MASK          (0x04000000U)  /**< Bit mask for the SKIP_PORT_ENFORCEMENT_AT_HOST_PORT field */
#define L2_LPE_SHARED_LEARNING_SHIFT                            (27)           /**< Shift value for the SHARED_LEARNING field */
#define L2_LPE_SHARED_LEARNING_MASK                             (0x08000000U)  /**< Bit mask for the SHARED_LEARNING field */
#define L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_0                     (28)           /**< Shift value for the DROP_IF_CANNOT_LEARN field */
#define L2_LPE_DROP_IF_CANNOT_LEARN_MASK_1                      (0x00000001U)  /**< Bit mask for the DROP_IF_CANNOT_LEARN field */
#define L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_OVLP                  (4)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping DROP_IF_CANNOT_LEARN field */
#define L2_LPE_START_DYN_SPACE_ADDR_SHIFT                       (1)            /**< Shift value for the START_DYN_SPACE_ADDR field */
#define L2_LPE_START_DYN_SPACE_ADDR_MASK                        (0x000007FEU)  /**< Bit mask for the START_DYN_SPACE_ADDR field */
#define L2_LPE_MAX_AGE_SHIFT                                    (11)           /**< Shift value for the MAX_AGE field */
#define L2_LPE_MAX_AGE_MASK                                     (0x03FFF800U)  /**< Bit mask for the MAX_AGE field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_0        (26)           /**< Shift value for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_MASK_1         (0x0000001FU)  /**< Bit mask for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_OVLP     (6)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_SHIFT          (5)            /**< Shift value for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_MASK           (0x0000FFE0U)  /**< Bit mask for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_SHIFT          (16)           /**< Shift value for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_MASK           (0x07FF0000U)  /**< Bit mask for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_0        (27)           /**< Shift value for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_MASK_1         (0x0000003FU)  /**< Bit mask for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_OVLP     (5)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_SHIFT          (6)            /**< Shift value for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4 field */
#define L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_MASK           (0x0001FFC0U)  /**< Bit mask for the MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4 field */
#define L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_SHIFT   (17)           /**< Shift value for the ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST field */
#define L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_MASK    (0x003E0000U)  /**< Bit mask for the ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST field */
#define L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_SHIFT (22)           /**< Shift value for the ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST field */
#define L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_MASK  (0x07C00000U)  /**< Bit mask for the ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST field */
#define L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_BROADCAST_SHIFT (27)           /**< Shift value for the ENABLE_SOURCE_PORT_ENFORCING_FOR_BROADCAST field */

/* register category l2_lookup_parameters_control */
#define L2_LPC_VALID_SHIFT   (31)           /**< Shift value for the VALID field */
#define L2_LPC_RDWRSET_SHIFT (30)           /**< Shift value for the RDWRSET field */
#define L2_LPC_RDWRSET_MASK  (0x40000000U)  /**< Bit mask for the RDWRSET field */

/* register category ptp_egress */
#define PTP_E_0_UPDATE_MASK (0x00000001U)  /**< Bit mask for the UPDATE field */

/* register category l2_memory_partition_status */
#define L2_MPS_L2PARTS_SHIFT (31)           /**< Shift value for the L2PARTS field */
#define L2_MPS_N_L2PSPC_MASK (0x7FFFFFFFU)  /**< Bit mask for the N_L2PSPC field */

/* register category mac_level_diagn_counters */
#define MAC_LDC_N_RUNT_SHIFT     (24)  /**< Shift value for the N_RUNT field */
#define MAC_LDC_N_SOFERR_SHIFT   (16)  /**< Shift value for the N_SOFERR field */
#define MAC_LDC_N_ALIGNERR_SHIFT (8)   /**< Shift value for the N_ALIGNERR field */

/* register category mac_level_diagn_flags */
#define MAC_LDF_SPCPRIOR_SHIFT   (12)           /**< Shift value for the SPCPRIOR field */
#define MAC_LDF_SPCPRIOR_MASK    (0x0000F000U)  /**< Bit mask for the SPCPRIOR field */
#define MAC_LDF_PORTDROPS_SHIFT  (6)            /**< Shift value for the PORTDROPS field */
#define MAC_LDF_PORTDROPS_MASK   (0x00000040U)  /**< Bit mask for the PORTDROPS field */
#define MAC_LDF_LENDROPS_SHIFT   (5)            /**< Shift value for the LENDROPS field */
#define MAC_LDF_LENDROPS_MASK    (0x00000020U)  /**< Bit mask for the LENDROPS field */
#define MAC_LDF_BWDROPS_SHIFT    (4)            /**< Shift value for the BWDROPS field */
#define MAC_LDF_BWDROPS_MASK     (0x00000010U)  /**< Bit mask for the BWDROPS field */
#define MAC_LDF_DRN664ERRS_SHIFT (2)            /**< Shift value for the DRN664ERRS field */
#define MAC_LDF_DRN664ERRS_MASK  (0x00000004U)  /**< Bit mask for the DRN664ERRS field */
#define MAC_LDF_SPCERRS_SHIFT    (1)            /**< Shift value for the SPCERRS field */
#define MAC_LDF_SPCERRS_MASK     (0x00000002U)  /**< Bit mask for the SPCERRS field */

/* register category hl_diagn_counters_1 */
#define HL_DC_1_0_N_TXBYTE_SHIFT_OVLP (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_TXBYTE field */
#define HL_DC_1_1_N_TXFRM_SHIFT_OVLP  (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_TXFRM field */
#define HL_DC_1_2_N_RXBYTE_SHIFT_OVLP (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_RXBYTE field */
#define HL_DC_1_3_N_RXFRM_SHIFT_OVLP  (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_RXFRM field */

/* register category hl_diagn_counters_occupancy */
#define HL_DCO_Q_OCCUPANCY_HIGH_WATT_SHIFT (16)  /**< Shift value for the Q_OCCUPANCY_HIGH_WATT field */

/* register category l2_forwarding_parameters */
#define L2_FP_PARTITION_SPACE_7_SHIFT      (13)           /**< Shift value for the PARTITION_SPACE_7 field */
#define L2_FP_PARTITION_SPACE_7_MASK       (0x007FE000U)  /**< Bit mask for the PARTITION_SPACE_7 field */
#define L2_FP_PARTITION_SPACE_6_SHIFT_0    (23)           /**< Shift value for the PARTITION_SPACE_6 field */
#define L2_FP_PARTITION_SPACE_6_MASK_1     (0x00000001U)  /**< Bit mask for the PARTITION_SPACE_6 field */
#define L2_FP_PARTITION_SPACE_6_SHIFT_OVLP (9)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PARTITION_SPACE_6 field */
#define L2_FP_PARTITION_SPACE_5_SHIFT      (1)            /**< Shift value for the PARTITION_SPACE_5 field */
#define L2_FP_PARTITION_SPACE_5_MASK       (0x000007FEU)  /**< Bit mask for the PARTITION_SPACE_5 field */
#define L2_FP_PARTITION_SPACE_4_SHIFT      (11)           /**< Shift value for the PARTITION_SPACE_4 field */
#define L2_FP_PARTITION_SPACE_4_MASK       (0x001FF800U)  /**< Bit mask for the PARTITION_SPACE_4 field */
#define L2_FP_PARTITION_SPACE_3_SHIFT      (21)           /**< Shift value for the PARTITION_SPACE_3 field */
#define L2_FP_PARTITION_SPACE_3_MASK       (0x7FE00000U)  /**< Bit mask for the PARTITION_SPACE_3 field */
#define L2_FP_PARTITION_SPACE_2_SHIFT_0    (31)           /**< Shift value for the PARTITION_SPACE_2 field */
#define L2_FP_PARTITION_SPACE_2_MASK_1     (0x000001FFU)  /**< Bit mask for the PARTITION_SPACE_2 field */
#define L2_FP_PARTITION_SPACE_2_SHIFT_OVLP (1)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PARTITION_SPACE_2 field */
#define L2_FP_PARTITION_SPACE_1_SHIFT      (9)            /**< Shift value for the PARTITION_SPACE_1 field */
#define L2_FP_PARTITION_SPACE_1_MASK       (0x0007FE00U)  /**< Bit mask for the PARTITION_SPACE_1 field */
#define L2_FP_PARTITION_SPACE_0_SHIFT      (19)           /**< Shift value for the PARTITION_SPACE_0 field */
#define L2_FP_PARTITION_SPACE_0_MASK       (0x1FF80000U)  /**< Bit mask for the PARTITION_SPACE_0 field */
#define L2_FP_MAX_DYN_PRIO_SHIFT           (29)           /**< Shift value for the MAX_DYN_PRIO field */

/* register category avb_parameters_entry */
#define AVB_PE_SRC_META_SHIFT_0               (30)           /**< Shift value for the SRC_META field */
#define AVB_PE_SRC_META_SHIFT_OVLP_1          (2)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping SRC_META field */
#define AVB_PE_SRC_META_MASK_2                (0x00003FFFU)  /**< Bit mask for the SRC_META field */
#define AVB_PE_SRC_META_SHIFT_OVLP_2          (34)           /**< Shift value for the to compensate for the alignment of the 3. part of the overlapping SRC_META field */
#define AVB_PE_DST_META_SHIFT_0               (14)           /**< Shift value for the DST_META field */
#define AVB_PE_DST_META_MASK_1                (0x3FFFFFFFU)  /**< Bit mask for the DST_META field */
#define AVB_PE_DST_META_SHIFT_OVLP            (18)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping DST_META field */
#define AVB_PE_PTP_MASTER_SHIFT               (30)           /**< Shift value for the PTP_MASTER field */
#define AVB_PE_PTP_MASTER_MASK                (0x40000000U)  /**< Bit mask for the PTP_MASTER field */
#define AVB_PE_USE_L2_DATA_RATE_FOR_CBS_SHIFT (31)           /**< Shift value for the USE_L2_DATA_RATE_FOR_CBS field */

/* register category avb_parameters_control */
#define AVB_PC_VALID_SHIFT   (31)           /**< Shift value for the VALID field */
#define AVB_PC_ERRORS_SHIFT  (30)           /**< Shift value for the ERRORS field */
#define AVB_PC_ERRORS_MASK   (0x40000000U)  /**< Bit mask for the ERRORS field */
#define AVB_PC_RDWRSET_SHIFT (29)           /**< Shift value for the RDWRSET field */
#define AVB_PC_RDWRSET_MASK  (0x20000000U)  /**< Bit mask for the RDWRSET field */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to GET data of the device_id
*
* \param[out] p_id Device identification code
* \return uint8_t
*/
uint8_t SJA1105P_getDeviceId(uint32_t *p_id, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) D_ID_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_id = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the configuration_flags
*
* \param[out] p_configurationFlags Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_configurationFlagsArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getConfigurationFlags(SJA1105P_configurationFlagsArgument_t *p_configurationFlags, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) CF_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (CF_CONFIGS_SHIFT);  /* shift to right aligned position */
	p_configurationFlags->configs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CF_CRCCHKL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CF_CRCCHKL_SHIFT);  /* shift to right aligned position */
	p_configurationFlags->crcchkl = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CF_IDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CF_IDS_SHIFT);  /* shift to right aligned position */
	p_configurationFlags->ids = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CF_CRCCHKG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CF_CRCCHKG_SHIFT);  /* shift to right aligned position */
	p_configurationFlags->crcchkg = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CF_NSLOT_MASK);   /* mask desired bits */
	p_configurationFlags->nslot = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_l2
*
* \param[out] p_generalStatusL2 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalStatusL2Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusL2(SJA1105P_generalStatusL2Argument_t *p_generalStatusL2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[GS_L2_WORD_COUNT] = {0};
	uint64_t macaddTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, GS_L2_WORD_COUNT, (uint32_t) GS_L2_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (GS_L2_PORTENF_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->portenf = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_L2_FWDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_L2_FWDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->fwds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_L2_MACFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_L2_MACFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->macfds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_L2_ENFFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_L2_ENFFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->enffds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_L2_L2BUSYFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_L2_L2BUSYFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->l2busyfds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_L2_L2BUSYS_MASK);   /* mask desired bits */
	p_generalStatusL2->l2busys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (GS_L2_MACADD_SHIFT_0);  /* shift to right aligned position */
	macaddTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GS_L2_MACADD_SHIFT_OVLP);
	p_generalStatusL2->macadd = (uint64_t) macaddTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_hashconfs
*
* \param[out] p_generalStatusHashconfs Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalStatusHashconfsArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusHashconfs(SJA1105P_generalStatusHashconfsArgument_t *p_generalStatusHashconfs, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[GSH_WORD_COUNT] = {0};
	uint64_t macaddhcTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, GSH_WORD_COUNT, (uint32_t) GSH_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GSH_VLANIDHC_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GSH_VLANIDHC_SHIFT);  /* shift to right aligned position */
	p_generalStatusHashconfs->vlanidhc = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GSH_HASHCONFS_MASK);   /* mask desired bits */
	p_generalStatusHashconfs->hashconfs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (GSH_MACADDHC_SHIFT_0);  /* shift to right aligned position */
	macaddhcTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddhcTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GSH_MACADDHC_SHIFT_OVLP);
	p_generalStatusHashconfs->macaddhc = (uint64_t) macaddhcTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_vlan
*
* \param[out] p_generalStatusVlan Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalStatusVlanArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusVlan(SJA1105P_generalStatusVlanArgument_t *p_generalStatusVlan, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) GS_VLAN_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (GS_VLAN_WPVLANID_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->wpvlanid = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (GS_VLAN_PORT_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->port = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_VLAN_VLANBUSYS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_VLAN_VLANBUSYS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->vlanbusys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_VLAN_WRONGPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_VLAN_WRONGPORTS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->wrongports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_VLAN_VNOTFOUNDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_VLAN_VNOTFOUNDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->vnotfounds = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_mem_0
*
* \param[out] p_generalStatusMem0 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalStatusMem0Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusMem0(SJA1105P_generalStatusMem0Argument_t *p_generalStatusMem0, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) GS_MEM_0_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (GS_MEM_0_EMPTYS_SHIFT);  /* shift to right aligned position */
	p_generalStatusMem0->emptys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_MEM_0_BUFFERS_MASK);   /* mask desired bits */
	p_generalStatusMem0->buffers = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_mem_1
*
* \param[out] p_buffersLowWatermark Minimum value of the total number of frame buffers available to dynamic memory management since the last read
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusMem1(uint32_t *p_buffersLowWatermark, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) GS_MEM_1_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_MEM_1_BUFFERS_LOW_WATERMARK_MASK);   /* mask desired bits */
	*p_buffersLowWatermark = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_drop
*
* \param[out] p_generalStatusDrop Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalStatusDropArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusDrop(SJA1105P_generalStatusDropArgument_t *p_generalStatusDrop, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) GS_DROP_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (GS_DROP_PORT_SHIFT);  /* shift to right aligned position */
	p_generalStatusDrop->port = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_DROP_FWDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GS_DROP_FWDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusDrop->fwds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GS_DROP_PARTS_MASK);   /* mask desired bits */
	p_generalStatusDrop->parts = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_ram_error
*
* \param[out] p_ramparerr Any set bit indicates a parity error in a memory block.
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusRamError(uint32_t *p_ramparerr, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[GS_RAM_E_WORD_COUNT] = {0};
	uint32_t ramparerrTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, GS_RAM_E_WORD_COUNT, (uint32_t) GS_RAM_E_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_RAM_E_RAMPARERR_MASK_0);   /* mask desired bits */
	ramparerrTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (GS_RAM_E_RAMPARERR_MASK_1);   /* mask desired bits */
	ramparerrTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) GS_RAM_E_RAMPARERR_SHIFT_OVLP);
	*p_ramparerr = (uint32_t) ramparerrTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the general_status_ram_init
*
* \param[in]  rparinit Parity flag initialization pattern
* \return uint8_t
*/
uint8_t SJA1105P_setGeneralStatusRamInit(uint32_t rparinit, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[GS_RAM_INIT_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((rparinit <= 0xFFFFFFFU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) GS_RAM_INIT_RPARINIT_MASK_0) & rparinit;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) rparinit >> (uint8_t) GS_RAM_INIT_RPARINIT_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GS_RAM_INIT_RPARINIT_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, GS_RAM_INIT_WORD_COUNT, (uint32_t) GS_RAM_INIT_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_ram_init
*
* \param[out] p_rparinit Parity flag initialization pattern
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralStatusRamInit(uint32_t *p_rparinit, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[GS_RAM_INIT_WORD_COUNT] = {0};
	uint32_t rparinitTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, GS_RAM_INIT_WORD_COUNT, (uint32_t) GS_RAM_INIT_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GS_RAM_INIT_RPARINIT_MASK_0);   /* mask desired bits */
	rparinitTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (GS_RAM_INIT_RPARINIT_MASK_1);   /* mask desired bits */
	rparinitTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) GS_RAM_INIT_RPARINIT_SHIFT_OVLP);
	*p_rparinit = (uint32_t) rparinitTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ethernet_port_status_control
*
* \param[in]  clearport This field is used to reset the MAC-level diagnostics counters and flags for each port.
* \return uint8_t
*/
uint8_t SJA1105P_setEthernetPortStatusControl(uint8_t clearport, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((clearport <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) E_PORT_SC_CLEARPORT_MASK) & clearport;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) E_PORT_SC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the transmission_control
*
* \param[in]  inhibittx This vector represents the set of ports on which the transmission is inhibited. A port will inhibit transmission if and only if the respective bit is set to 1.
* \return uint8_t
*/
uint8_t SJA1105P_setTransmissionControl(uint8_t inhibittx, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((inhibittx <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) TC_INHIBITTX_MASK) & inhibittx;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) TC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_0
*
* \param[in]  ptppinst Synchronized PTP clock value at which the switch shall start toggling the external PTP_CLK pin
* \return uint8_t
*/
uint8_t SJA1105P_setPtpControl0(uint64_t ptppinst, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[PTP_C_0_WORD_COUNT];
	uint32_t tmp;

	cData = (uint32_t) ptppinst;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (ptppinst >> (uint8_t) PTP_C_0_PTPPINST_SHIFT_OVLP);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, PTP_C_0_WORD_COUNT, (uint32_t) PTP_C_0_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_1
*
* \param[in]  ptppindur This field specifies the interval between two edges of the external clock on pin PTP_CLK
* \return uint8_t
*/
uint8_t SJA1105P_setPtpControl1(uint32_t ptppindur, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) ptppindur;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) PTP_C_1_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_2
*
* \param[in]  ptpControl2 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_ptpControl2Argument_t
* \return uint8_t
*/
uint8_t SJA1105P_setPtpControl2(const SJA1105P_ptpControl2Argument_t *pk_ptpControl2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_ptpControl2->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->startptpcp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->stopptpcp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->syncCascaded <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->resptp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->corrclk4ts <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->ptpclksub <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->ptpclkadd <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_ptpControl2->valid) << ((uint8_t) PTP_C_2_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_ptpControl2->startptpcp) << ((uint8_t) PTP_C_2_STARTPTPCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_STARTPTPCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->stopptpcp) << ((uint8_t) PTP_C_2_STOPPTPCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_STOPPTPCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->syncCascaded) << ((uint8_t) PTP_C_2_SYNC_CASCADED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_SYNC_CASCADED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->resptp) << ((uint8_t) PTP_C_2_RESPTP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_RESPTP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->corrclk4ts) << ((uint8_t) PTP_C_2_CORRCLK4TS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_CORRCLK4TS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->ptpclksub) << ((uint8_t) PTP_C_2_PTPCLKSUB_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) PTP_C_2_PTPCLKSUB_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) PTP_C_2_PTPCLKADD_MASK) & pk_ptpControl2->ptpclkadd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) PTP_C_2_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_2
*
* \param[out] p_syncCascaded
* \return uint8_t
*/
uint8_t SJA1105P_getPtpControl2(uint8_t *p_syncCascaded, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) PTP_C_2_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PTP_C_2_SYNC_CASCADED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (PTP_C_2_SYNC_CASCADED_SHIFT);  /* shift to right aligned position */
	*p_syncCascaded = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_3
*
* \param[in]  ptpclkval PTP clock
* \return uint8_t
*/
uint8_t SJA1105P_setPtpControl3(uint64_t ptpclkval, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[PTP_C_3_WORD_COUNT];
	uint32_t tmp;

	cData = (uint32_t) ptpclkval;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (ptpclkval >> (uint8_t) PTP_C_3_PTPCLKVAL_SHIFT_OVLP);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, PTP_C_3_WORD_COUNT, (uint32_t) PTP_C_3_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_3
*
* \param[out] p_ptpclkval PTP clock
* \return uint8_t
*/
uint8_t SJA1105P_getPtpControl3(uint64_t *p_ptpclkval, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[PTP_C_3_WORD_COUNT] = {0};
	uint64_t ptpclkvalTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, PTP_C_3_WORD_COUNT, (uint32_t) PTP_C_3_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	ptpclkvalTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	ptpclkvalTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) PTP_C_3_PTPCLKVAL_SHIFT_OVLP);
	*p_ptpclkval = (uint64_t) ptpclkvalTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_4
*
* \param[in]  ptpclkrate This field determines the speed of PTPCLK. It implements a fixed-point clock rate value with a single-bit integer part and a 31-bit fractional part. PTPCLKVAL will tick at the rate of PTPTSCLK multiplied by this field.
* \return uint8_t
*/
uint8_t SJA1105P_setPtpControl4(uint32_t ptpclkrate, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) ptpclkrate;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) PTP_C_4_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_5
*
* \param[out] p_ptptsclk PTP timestamp clock that is used to timestamp MAC management frames on ingress and egress
* \return uint8_t
*/
uint8_t SJA1105P_getPtpControl5(uint64_t *p_ptptsclk, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[PTP_C_5_WORD_COUNT] = {0};
	uint64_t ptptsclkTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, PTP_C_5_WORD_COUNT, (uint32_t) PTP_C_5_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	ptptsclkTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	ptptsclkTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) PTP_C_5_PTPTSCLK_SHIFT_OVLP);
	*p_ptptsclk = (uint64_t) ptptsclkTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_6
*
* \param[out] p_ptptsshadow shadow register used to obtain the PTP CLK offset between a switch PTP clock master and the PTP clock slaves
* \return uint8_t
*/
uint8_t SJA1105P_getPtpControl6(uint64_t *p_ptptsshadow, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[PTP_C_6_WORD_COUNT] = {0};
	uint64_t ptptsshadowTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, PTP_C_6_WORD_COUNT, (uint32_t) PTP_C_6_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	ptptsshadowTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	ptptsshadowTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) PTP_C_6_PTPTSSHADOW_SHIFT_OVLP);
	*p_ptptsshadow = (uint64_t) ptptsshadowTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_art_entry
*
* \param[in]  l2ArtEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ArtEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2ArtEntry(const SJA1105P_l2ArtEntryArgument_t *pk_l2ArtEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[L2_ART_E_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ArtEntry->index <= 0x3FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->enfport <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->destports <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->macaddr <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->innerOuterVlan <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->mask <= 0x1FFFFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->age <= 0x7FFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtEntry->touched <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ArtEntry->index) << ((uint8_t) L2_ART_E_INDEX_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_INDEX_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtEntry->enfport) << ((uint8_t) L2_ART_E_ENFPORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_ENFPORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtEntry->destports) << ((uint8_t) L2_ART_E_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtEntry->macaddr) << ((uint8_t) L2_ART_E_MACADDR_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (pk_l2ArtEntry->macaddr >> (uint8_t) L2_ART_E_MACADDR_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) (pk_l2ArtEntry->macaddr >> (uint8_t) L2_ART_E_MACADDR_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_ART_E_MACADDR_MASK_2) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtEntry->vlanid) << ((uint8_t) L2_ART_E_VLANID_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_VLANID_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtEntry->innerOuterVlan) << ((uint8_t) L2_ART_E_INNER_OUTER_VLAN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_INNER_OUTER_VLAN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtEntry->mask) << ((uint8_t) L2_ART_E_MASK_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) (pk_l2ArtEntry->mask >> (uint8_t) L2_ART_E_MASK_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD3] = cData;
	tmp = (uint32_t) (pk_l2ArtEntry->mask >> (uint8_t) L2_ART_E_MASK_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtEntry->age) << ((uint8_t) L2_ART_E_AGE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_E_AGE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtEntry->touched) << ((uint8_t) L2_ART_E_TOUCHED_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD4] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, L2_ART_E_WORD_COUNT, (uint32_t) L2_ART_E_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_art_entry
*
* \param[out] p_l2ArtEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ArtEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2ArtEntry(SJA1105P_l2ArtEntryArgument_t *p_l2ArtEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[L2_ART_E_WORD_COUNT] = {0};
	uint64_t macaddrTmp;
	uint64_t maskTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, L2_ART_E_WORD_COUNT, (uint32_t) L2_ART_E_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_E_INDEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_INDEX_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->index = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_E_ENFPORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_ENFPORT_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->enfport = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_E_DESTPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_DESTPORTS_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->destports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (L2_ART_E_MACADDR_SHIFT_0);  /* shift to right aligned position */
	macaddrTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddrTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_E_MACADDR_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_E_MACADDR_MASK_2);   /* mask desired bits */
	macaddrTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_E_MACADDR_SHIFT_OVLP_2);
	p_l2ArtEntry->macaddr = (uint64_t) macaddrTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_E_VLANID_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_VLANID_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->vlanid = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_E_INNER_OUTER_VLAN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_INNER_OUTER_VLAN_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->innerOuterVlan = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (L2_ART_E_MASK_SHIFT_0);  /* shift to right aligned position */
	maskTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD3];
	maskTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_E_MASK_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD4];
	maskTmp |= (uint64_t) ((uint64_t) ((uint16_t) cResult) << (uint8_t) L2_ART_E_MASK_SHIFT_OVLP_2);
	p_l2ArtEntry->mask = (uint64_t) maskTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (L2_ART_E_AGE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_E_AGE_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->age = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult >>= (uint32_t) (L2_ART_E_TOUCHED_SHIFT);  /* shift to right aligned position */
	p_l2ArtEntry->touched = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_art_locked_entry
*
* \param[in]  l2ArtLockedEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ArtLockedEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2ArtLockedEntry(const SJA1105P_l2ArtLockedEntryArgument_t *pk_l2ArtLockedEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[L2_ART_LE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ArtLockedEntry->index <= 0x3FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->enfport <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->destports <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->macaddr <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->innerOuterVlan <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->mask <= 0x1FFFFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->retag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->mirror <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->takeTs <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->mirroredVlan <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ArtLockedEntry->tsReg <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ArtLockedEntry->index) << ((uint8_t) L2_ART_LE_INDEX_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_INDEX_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->enfport) << ((uint8_t) L2_ART_LE_ENFPORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_ENFPORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->destports) << ((uint8_t) L2_ART_LE_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->macaddr) << ((uint8_t) L2_ART_LE_MACADDR_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (pk_l2ArtLockedEntry->macaddr >> (uint8_t) L2_ART_LE_MACADDR_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) (pk_l2ArtLockedEntry->macaddr >> (uint8_t) L2_ART_LE_MACADDR_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_ART_LE_MACADDR_MASK_2) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->vlanid) << ((uint8_t) L2_ART_LE_VLANID_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_VLANID_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->innerOuterVlan) << ((uint8_t) L2_ART_LE_INNER_OUTER_VLAN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_INNER_OUTER_VLAN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->mask) << ((uint8_t) L2_ART_LE_MASK_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) (pk_l2ArtLockedEntry->mask >> (uint8_t) L2_ART_LE_MASK_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD3] = cData;
	tmp = (uint32_t) (pk_l2ArtLockedEntry->mask >> (uint8_t) L2_ART_LE_MASK_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->retag) << ((uint8_t) L2_ART_LE_RETAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_RETAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->mirror) << ((uint8_t) L2_ART_LE_MIRROR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_MIRROR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->takeTs) << ((uint8_t) L2_ART_LE_TAKE_TS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_TAKE_TS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->mirroredVlan) << ((uint8_t) L2_ART_LE_MIRRORED_VLAN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ART_LE_MIRRORED_VLAN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ArtLockedEntry->tsReg) << ((uint8_t) L2_ART_LE_TS_REG_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD4] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, L2_ART_LE_WORD_COUNT, (uint32_t) L2_ART_LE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_art_locked_entry
*
* \param[out] p_l2ArtLockedEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ArtLockedEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2ArtLockedEntry(SJA1105P_l2ArtLockedEntryArgument_t *p_l2ArtLockedEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[L2_ART_LE_WORD_COUNT] = {0};
	uint64_t macaddrTmp;
	uint64_t maskTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, L2_ART_LE_WORD_COUNT, (uint32_t) L2_ART_LE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_LE_INDEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_INDEX_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->index = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_LE_ENFPORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_ENFPORT_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->enfport = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_ART_LE_DESTPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_DESTPORTS_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->destports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (L2_ART_LE_MACADDR_SHIFT_0);  /* shift to right aligned position */
	macaddrTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddrTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_LE_MACADDR_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_LE_MACADDR_MASK_2);   /* mask desired bits */
	macaddrTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_LE_MACADDR_SHIFT_OVLP_2);
	p_l2ArtLockedEntry->macaddr = (uint64_t) macaddrTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_LE_VLANID_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_VLANID_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->vlanid = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_ART_LE_INNER_OUTER_VLAN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_INNER_OUTER_VLAN_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->innerOuterVlan = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (L2_ART_LE_MASK_SHIFT_0);  /* shift to right aligned position */
	maskTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD3];
	maskTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) L2_ART_LE_MASK_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD4];
	maskTmp |= (uint64_t) ((uint64_t) ((uint16_t) cResult) << (uint8_t) L2_ART_LE_MASK_SHIFT_OVLP_2);
	p_l2ArtLockedEntry->mask = (uint64_t) maskTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (L2_ART_LE_RETAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_RETAG_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->retag = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (L2_ART_LE_MIRROR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_MIRROR_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->mirror = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (L2_ART_LE_TAKE_TS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_TAKE_TS_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->takeTs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (L2_ART_LE_MIRRORED_VLAN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ART_LE_MIRRORED_VLAN_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->mirroredVlan = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult >>= (uint32_t) (L2_ART_LE_TS_REG_SHIFT);  /* shift to right aligned position */
	p_l2ArtLockedEntry->tsReg = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_address_lookup_table_control
*
* \param[in]  l2AddressLookupTableControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2AddressLookupTableControlSetArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2AddressLookupTableControl(const SJA1105P_l2AddressLookupTableControlSetArgument_t *pk_l2AddressLookupTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2AddressLookupTableControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableControl->lockeds <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableControl->valident <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableControl->mgmtroute <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((((uint8_t) pk_l2AddressLookupTableControl->hostCmd) <= 0x7U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2AddressLookupTableControl->valid) << ((uint8_t) L2_ALTC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableControl->rdwrset) << ((uint8_t) L2_ALTC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ALTC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableControl->lockeds) << ((uint8_t) L2_ALTC_LOCKEDS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ALTC_LOCKEDS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableControl->valident) << ((uint8_t) L2_ALTC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ALTC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableControl->mgmtroute) << ((uint8_t) L2_ALTC_MGMTROUTE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ALTC_MGMTROUTE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) ((uint8_t) pk_l2AddressLookupTableControl->hostCmd)) << ((uint8_t) L2_ALTC_HOST_CMD_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_ALTC_HOST_CMD_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) L2_ALTC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_address_lookup_table_control
*
* \param[out] p_l2AddressLookupTableControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2AddressLookupTableControlGetArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2AddressLookupTableControl(SJA1105P_l2AddressLookupTableControlGetArgument_t *p_l2AddressLookupTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) L2_ALTC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (L2_ALTC_VALID_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->valid = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_ALTC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ALTC_ERRORS_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->errors = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_ALTC_LOCKEDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ALTC_LOCKEDS_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->lockeds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_ALTC_VALIDENT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ALTC_VALIDENT_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->valident = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_ALTC_MGMTROUTE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ALTC_MGMTROUTE_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->mgmtroute = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_ALTC_HOST_CMD_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_ALTC_HOST_CMD_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableControl->hostCmd = SJA1105P_convertToHostCmd(cResult);  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_forwarding_table_entry
*
* \param[in]  l2ForwardingTableEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ForwardingTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2ForwardingTableEntry(const SJA1105P_l2ForwardingTableEntryArgument_t *pk_l2ForwardingTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[L2_FTE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ForwardingTableEntry->vlanPmap0 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap1 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap2 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap3 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap4 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap5 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap6 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->vlanPmap7 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->flDomain <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->reachPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableEntry->bcDomain <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap0) << ((uint8_t) L2_FTE_VLAN_PMAP_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_0_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap1) << ((uint8_t) L2_FTE_VLAN_PMAP_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap2) << ((uint8_t) L2_FTE_VLAN_PMAP_2_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap2 >> (uint8_t) L2_FTE_VLAN_PMAP_2_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_2_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap3) << ((uint8_t) L2_FTE_VLAN_PMAP_3_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_3_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap4) << ((uint8_t) L2_FTE_VLAN_PMAP_4_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_4_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap5) << ((uint8_t) L2_FTE_VLAN_PMAP_5_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_5_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap6) << ((uint8_t) L2_FTE_VLAN_PMAP_6_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_6_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->vlanPmap7) << ((uint8_t) L2_FTE_VLAN_PMAP_7_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_VLAN_PMAP_7_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->flDomain) << ((uint8_t) L2_FTE_FL_DOMAIN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_FL_DOMAIN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->reachPort) << ((uint8_t) L2_FTE_REACH_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTE_REACH_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableEntry->bcDomain) << ((uint8_t) L2_FTE_BC_DOMAIN_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, L2_FTE_WORD_COUNT, (uint32_t) L2_FTE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_forwarding_table_entry
*
* \param[out] p_l2ForwardingTableEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ForwardingTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2ForwardingTableEntry(SJA1105P_l2ForwardingTableEntryArgument_t *p_l2ForwardingTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[L2_FTE_WORD_COUNT] = {0};
	uint8_t vlanPmap2Tmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, L2_FTE_WORD_COUNT, (uint32_t) L2_FTE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_0_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap0 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_1_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap1 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_2_SHIFT_0);  /* shift to right aligned position */
	vlanPmap2Tmp  = (uint8_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_2_MASK_1);   /* mask desired bits */
	vlanPmap2Tmp |= (uint8_t) ((uint8_t) cResult << (uint8_t) L2_FTE_VLAN_PMAP_2_SHIFT_OVLP);
	p_l2ForwardingTableEntry->vlanPmap2 = (uint8_t) vlanPmap2Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_3_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_3_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap3 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_4_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap4 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_5_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_5_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap5 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_6_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_6_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap6 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_VLAN_PMAP_7_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_VLAN_PMAP_7_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->vlanPmap7 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_FL_DOMAIN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_FL_DOMAIN_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->flDomain = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FTE_REACH_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTE_REACH_PORT_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->reachPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (L2_FTE_BC_DOMAIN_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingTableEntry->bcDomain = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_forwarding_table_control
*
* \param[in]  l2ForwardingTableControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ForwardingTableControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2ForwardingTableControl(const SJA1105P_l2ForwardingTableControlArgument_t *pk_l2ForwardingTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ForwardingTableControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableControl->index <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ForwardingTableControl->valid) << ((uint8_t) L2_FTC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ForwardingTableControl->rdwrset) << ((uint8_t) L2_FTC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_FTC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) L2_FTC_INDEX_MASK) & pk_l2ForwardingTableControl->index;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) L2_FTC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_forwarding_table_control
*
* \param[out] p_errors This value indicates whether a dynamic reconfiguration attempt was successful.
* \return uint8_t
*/
uint8_t SJA1105P_getL2ForwardingTableControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) L2_FTC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_FTC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FTC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the vlan_lookup_table_entry
*
* \param[in]  vlanLookupTableEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_vlanLookupTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setVlanLookupTableEntry(const SJA1105P_vlanLookupTableEntryArgument_t *pk_vlanLookupTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[VLAN_LTE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_vlanLookupTableEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableEntry->tagPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableEntry->vlanBc <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableEntry->vmembPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableEntry->vegrMirr <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableEntry->vingMirr <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_vlanLookupTableEntry->vlanid) << ((uint8_t) VLAN_LTE_VLANID_SHIFT_0);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_vlanLookupTableEntry->vlanid >> (uint8_t) VLAN_LTE_VLANID_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) VLAN_LTE_VLANID_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_vlanLookupTableEntry->tagPort) << ((uint8_t) VLAN_LTE_TAG_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTE_TAG_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableEntry->vlanBc) << ((uint8_t) VLAN_LTE_VLAN_BC_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTE_VLAN_BC_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableEntry->vmembPort) << ((uint8_t) VLAN_LTE_VMEMB_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTE_VMEMB_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableEntry->vegrMirr) << ((uint8_t) VLAN_LTE_VEGR_MIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTE_VEGR_MIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableEntry->vingMirr) << ((uint8_t) VLAN_LTE_VING_MIRR_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, VLAN_LTE_WORD_COUNT, (uint32_t) VLAN_LTE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the vlan_lookup_table_entry
*
* \param[out] p_vlanLookupTableEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_vlanLookupTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getVlanLookupTableEntry(SJA1105P_vlanLookupTableEntryArgument_t *p_vlanLookupTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[VLAN_LTE_WORD_COUNT] = {0};
	uint16_t vlanidTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, VLAN_LTE_WORD_COUNT, (uint32_t) VLAN_LTE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (VLAN_LTE_VLANID_SHIFT_0);  /* shift to right aligned position */
	vlanidTmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (VLAN_LTE_VLANID_MASK_1);   /* mask desired bits */
	vlanidTmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) VLAN_LTE_VLANID_SHIFT_OVLP);
	p_vlanLookupTableEntry->vlanid = (uint16_t) vlanidTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (VLAN_LTE_TAG_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (VLAN_LTE_TAG_PORT_SHIFT);  /* shift to right aligned position */
	p_vlanLookupTableEntry->tagPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (VLAN_LTE_VLAN_BC_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (VLAN_LTE_VLAN_BC_SHIFT);  /* shift to right aligned position */
	p_vlanLookupTableEntry->vlanBc = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (VLAN_LTE_VMEMB_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (VLAN_LTE_VMEMB_PORT_SHIFT);  /* shift to right aligned position */
	p_vlanLookupTableEntry->vmembPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (VLAN_LTE_VEGR_MIRR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (VLAN_LTE_VEGR_MIRR_SHIFT);  /* shift to right aligned position */
	p_vlanLookupTableEntry->vegrMirr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (VLAN_LTE_VING_MIRR_SHIFT);  /* shift to right aligned position */
	p_vlanLookupTableEntry->vingMirr = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the vlan_lookup_table_control
*
* \param[in]  vlanLookupTableControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_vlanLookupTableControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setVlanLookupTableControl(const SJA1105P_vlanLookupTableControlArgument_t *pk_vlanLookupTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_vlanLookupTableControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableControl->valident <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_vlanLookupTableControl->valid) << ((uint8_t) VLAN_LTC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_vlanLookupTableControl->rdwrset) << ((uint8_t) VLAN_LTC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableControl->valident) << ((uint8_t) VLAN_LTC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) VLAN_LTC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) VLAN_LTC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the vlan_lookup_table_control
*
* \param[out] p_valident In case of a write access, this flag determines if the respective entry should be marked valid
* \return uint8_t
*/
uint8_t SJA1105P_getVlanLookupTableControl(uint8_t *p_valident, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) VLAN_LTC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (VLAN_LTC_VALIDENT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (VLAN_LTC_VALIDENT_SHIFT);  /* shift to right aligned position */
	*p_valident = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_entry
*
* \param[in]  creditBasedShapingEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_creditBasedShapingEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setCreditBasedShapingEntry(const SJA1105P_creditBasedShapingEntryArgument_t *pk_creditBasedShapingEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[CBSE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_creditBasedShapingEntry->cbsPrio <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingEntry->cbsPort <= 0x7U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_creditBasedShapingEntry->idleSlope) << ((uint8_t) CBSE_IDLE_SLOPE_SHIFT_0);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (pk_creditBasedShapingEntry->idleSlope >> (uint8_t) CBSE_IDLE_SLOPE_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) CBSE_IDLE_SLOPE_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingEntry->sendSlope) << ((uint8_t) CBSE_SEND_SLOPE_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) (pk_creditBasedShapingEntry->sendSlope >> (uint8_t) CBSE_SEND_SLOPE_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) CBSE_SEND_SLOPE_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingEntry->creditHi) << ((uint8_t) CBSE_CREDIT_HI_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) (pk_creditBasedShapingEntry->creditHi >> (uint8_t) CBSE_CREDIT_HI_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) CBSE_CREDIT_HI_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingEntry->creditLo) << ((uint8_t) CBSE_CREDIT_LO_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD3] = cData;
	tmp = (uint32_t) (pk_creditBasedShapingEntry->creditLo >> (uint8_t) CBSE_CREDIT_LO_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) CBSE_CREDIT_LO_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingEntry->cbsPrio) << ((uint8_t) CBSE_CBS_PRIO_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CBSE_CBS_PRIO_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_creditBasedShapingEntry->cbsPort) << ((uint8_t) CBSE_CBS_PORT_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD4] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, CBSE_WORD_COUNT, (uint32_t) CBSE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the credit_based_shaping_entry
*
* \param[out] p_creditBasedShapingEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_creditBasedShapingEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getCreditBasedShapingEntry(SJA1105P_creditBasedShapingEntryArgument_t *p_creditBasedShapingEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[CBSE_WORD_COUNT] = {0};
	uint32_t idleSlopeTmp;
	uint32_t sendSlopeTmp;
	uint32_t creditHiTmp;
	uint32_t creditLoTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, CBSE_WORD_COUNT, (uint32_t) CBSE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (CBSE_IDLE_SLOPE_SHIFT_0);  /* shift to right aligned position */
	idleSlopeTmp  = (uint32_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (CBSE_IDLE_SLOPE_MASK_1);   /* mask desired bits */
	idleSlopeTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) CBSE_IDLE_SLOPE_SHIFT_OVLP);
	p_creditBasedShapingEntry->idleSlope = (uint32_t) idleSlopeTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (CBSE_SEND_SLOPE_SHIFT_0);  /* shift to right aligned position */
	sendSlopeTmp  = (uint32_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (CBSE_SEND_SLOPE_MASK_1);   /* mask desired bits */
	sendSlopeTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) CBSE_SEND_SLOPE_SHIFT_OVLP);
	p_creditBasedShapingEntry->sendSlope = (uint32_t) sendSlopeTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (CBSE_CREDIT_HI_SHIFT_0);  /* shift to right aligned position */
	creditHiTmp  = (uint32_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (CBSE_CREDIT_HI_MASK_1);   /* mask desired bits */
	creditHiTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) CBSE_CREDIT_HI_SHIFT_OVLP);
	p_creditBasedShapingEntry->creditHi = (uint32_t) creditHiTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult >>= (uint32_t) (CBSE_CREDIT_LO_SHIFT_0);  /* shift to right aligned position */
	creditLoTmp  = (uint32_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (CBSE_CREDIT_LO_MASK_1);   /* mask desired bits */
	creditLoTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) CBSE_CREDIT_LO_SHIFT_OVLP);
	p_creditBasedShapingEntry->creditLo = (uint32_t) creditLoTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (CBSE_CBS_PRIO_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CBSE_CBS_PRIO_SHIFT);  /* shift to right aligned position */
	p_creditBasedShapingEntry->cbsPrio = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult >>= (uint32_t) (CBSE_CBS_PORT_SHIFT);  /* shift to right aligned position */
	p_creditBasedShapingEntry->cbsPort = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_control
*
* \param[in]  creditBasedShapingControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_creditBasedShapingControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setCreditBasedShapingControl(const SJA1105P_creditBasedShapingControlArgument_t *pk_creditBasedShapingControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_creditBasedShapingControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingControl->shaperId <= 0xFU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_creditBasedShapingControl->valid) << ((uint8_t) CBSC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingControl->rdwrset) << ((uint8_t) CBSC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) CBSC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) CBSC_SHAPER_ID_MASK) & pk_creditBasedShapingControl->shaperId;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) CBSC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the credit_based_shaping_control
*
* \param[out] p_errors Error
* \return uint8_t
*/
uint8_t SJA1105P_getCreditBasedShapingControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) CBSC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (CBSC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (CBSC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the retagging_table_entry
*
* \param[in]  retaggingTableEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_retaggingTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setRetaggingTableEntry(const SJA1105P_retaggingTableEntryArgument_t *pk_retaggingTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[RTE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_retaggingTableEntry->destports <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->useDestports <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->doNotLearn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->vlanEgr <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->vlanIng <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->ingPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableEntry->egrPort <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_retaggingTableEntry->destports) << ((uint8_t) RTE_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTE_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->useDestports) << ((uint8_t) RTE_USE_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTE_USE_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->doNotLearn) << ((uint8_t) RTE_DO_NOT_LEARN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTE_DO_NOT_LEARN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->vlanEgr) << ((uint8_t) RTE_VLAN_EGR_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_retaggingTableEntry->vlanEgr >> (uint8_t) RTE_VLAN_EGR_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) RTE_VLAN_EGR_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->vlanIng) << ((uint8_t) RTE_VLAN_ING_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTE_VLAN_ING_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->ingPort) << ((uint8_t) RTE_ING_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTE_ING_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableEntry->egrPort) << ((uint8_t) RTE_EGR_PORT_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, RTE_WORD_COUNT, (uint32_t) RTE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the retagging_table_entry
*
* \param[out] p_retaggingTableEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_retaggingTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getRetaggingTableEntry(SJA1105P_retaggingTableEntryArgument_t *p_retaggingTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[RTE_WORD_COUNT] = {0};
	uint16_t vlanEgrTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, RTE_WORD_COUNT, (uint32_t) RTE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (RTE_DESTPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTE_DESTPORTS_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->destports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (RTE_USE_DESTPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTE_USE_DESTPORTS_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->useDestports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (RTE_DO_NOT_LEARN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTE_DO_NOT_LEARN_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->doNotLearn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (RTE_VLAN_EGR_SHIFT_0);  /* shift to right aligned position */
	vlanEgrTmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (RTE_VLAN_EGR_MASK_1);   /* mask desired bits */
	vlanEgrTmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) RTE_VLAN_EGR_SHIFT_OVLP);
	p_retaggingTableEntry->vlanEgr = (uint16_t) vlanEgrTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (RTE_VLAN_ING_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTE_VLAN_ING_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->vlanIng = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (RTE_ING_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTE_ING_PORT_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->ingPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (RTE_EGR_PORT_SHIFT);  /* shift to right aligned position */
	p_retaggingTableEntry->egrPort = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the retagging_table_control
*
* \param[in]  retaggingTableControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_retaggingTableControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setRetaggingTableControl(const SJA1105P_retaggingTableControlArgument_t *pk_retaggingTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_retaggingTableControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableControl->valident <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableControl->index <= 0x3FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_retaggingTableControl->valid) << ((uint8_t) RTC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableControl->valident) << ((uint8_t) RTC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableControl->rdwrset) << ((uint8_t) RTC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) RTC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) RTC_INDEX_MASK) & pk_retaggingTableControl->index;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) RTC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the retagging_table_control
*
* \param[out] p_errors If found set on read, the most recent access resulted in an error because it was issued prior to completing the configuration load procedure.
* \return uint8_t
*/
uint8_t SJA1105P_getRetaggingTableControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) RTC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (RTC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (RTC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the general_parameters_entry
*
* \param[in]  generalParametersEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setGeneralParametersEntry(const SJA1105P_generalParametersEntryArgument_t *pk_generalParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[GPE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_generalParametersEntry->egrMirrOuterDei <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->egrMirrOuterPcp <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->egrMirrOuterTag <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->takeEgrTsOnHostPort <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->ignore2StepFlag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->mirrorp <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->hostPort <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->cascadingPort <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->sendFollowUpTs0 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->sendFollowUpTs1 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->includeSrcPort0 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->includeSrcPort1 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->macFilter0 <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->macFilter1 <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->macFilterResult0 <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->macFilterResult1 <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->mgmtPrio <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->id <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->mirrorPortReconfigEnable <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersEntry->arinc <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_generalParametersEntry->egrMirrOuterDei) << ((uint8_t) GPE_EGR_MIRR_OUTER_DEI_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_EGR_MIRR_OUTER_DEI_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->egrMirrOuterPcp) << ((uint8_t) GPE_EGR_MIRR_OUTER_PCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_EGR_MIRR_OUTER_PCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->egrMirrOuterTag) << ((uint8_t) GPE_EGR_MIRR_OUTER_TAG_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_generalParametersEntry->egrMirrOuterTag >> (uint8_t) GPE_EGR_MIRR_OUTER_TAG_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_EGR_MIRR_OUTER_TAG_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->takeEgrTsOnHostPort) << ((uint8_t) GPE_TAKE_EGR_TS_ON_HOST_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_TAKE_EGR_TS_ON_HOST_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->tpid2) << ((uint8_t) GPE_TPID2_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->ignore2StepFlag) << ((uint8_t) GPE_IGNORE_2_STEP_FLAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_IGNORE_2_STEP_FLAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->tpid) << ((uint8_t) GPE_TPID_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) ((uint32_t) pk_generalParametersEntry->tpid >> (uint8_t) GPE_TPID_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_TPID_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->vlMask) << ((uint8_t) GPE_VL_MASK_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->vlMask >> (uint8_t) GPE_VL_MASK_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_VL_MASK_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->vlMarker) << ((uint8_t) GPE_VL_MARKER_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD3] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->vlMarker >> (uint8_t) GPE_VL_MARKER_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_VL_MARKER_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->mirrorp) << ((uint8_t) GPE_MIRRORP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_MIRRORP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->hostPort) << ((uint8_t) GPE_HOST_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_HOST_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->cascadingPort) << ((uint8_t) GPE_CASCADING_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_CASCADING_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->sendFollowUpTs0) << ((uint8_t) GPE_SEND_FOLLOW_UP_TS_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_SEND_FOLLOW_UP_TS_0_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->sendFollowUpTs1) << ((uint8_t) GPE_SEND_FOLLOW_UP_TS_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_SEND_FOLLOW_UP_TS_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->includeSrcPort0) << ((uint8_t) GPE_INCLUDE_SRC_PORT_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_INCLUDE_SRC_PORT_0_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->includeSrcPort1) << ((uint8_t) GPE_INCLUDE_SRC_PORT_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_INCLUDE_SRC_PORT_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->macFilter0) << ((uint8_t) GPE_MAC_FILTER_0_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD4] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilter0 >> (uint8_t) GPE_MAC_FILTER_0_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD5] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilter0 >> (uint8_t) GPE_MAC_FILTER_0_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->macFilter1) << ((uint8_t) GPE_MAC_FILTER_1_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD6] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilter1 >> (uint8_t) GPE_MAC_FILTER_1_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_MAC_FILTER_1_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->macFilterResult0) << ((uint8_t) GPE_MAC_FILTER_RESULT_0_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD7] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilterResult0 >> (uint8_t) GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD8] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilterResult0 >> (uint8_t) GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->macFilterResult1) << ((uint8_t) GPE_MAC_FILTER_RESULT_1_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD9] = cData;
	tmp = (uint32_t) (pk_generalParametersEntry->macFilterResult1 >> (uint8_t) GPE_MAC_FILTER_RESULT_1_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) GPE_MAC_FILTER_RESULT_1_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersEntry->mgmtPrio) << ((uint8_t) GPE_MGMT_PRIO_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_MGMT_PRIO_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->id) << ((uint8_t) GPE_ID_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_ID_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->mirrorPortReconfigEnable) << ((uint8_t) GPE_MIRROR_PORT_RECONFIG_ENABLE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPE_MIRROR_PORT_RECONFIG_ENABLE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_generalParametersEntry->arinc) << ((uint8_t) GPE_ARINC_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD10] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, GPE_WORD_COUNT, (uint32_t) GPE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the general_parameters_entry
*
* \param[out] p_generalParametersEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralParametersEntry(SJA1105P_generalParametersEntryArgument_t *p_generalParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[GPE_WORD_COUNT] = {0};
	uint16_t egrMirrOuterTagTmp;
	uint16_t tpidTmp;
	uint32_t vlMaskTmp;
	uint32_t vlMarkerTmp;
	uint64_t macFilter0Tmp;
	uint64_t macFilter1Tmp;
	uint64_t macFilterResult0Tmp;
	uint64_t macFilterResult1Tmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, GPE_WORD_COUNT, (uint32_t) GPE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GPE_EGR_MIRR_OUTER_DEI_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_EGR_MIRR_OUTER_DEI_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->egrMirrOuterDei = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (GPE_EGR_MIRR_OUTER_PCP_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_EGR_MIRR_OUTER_PCP_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->egrMirrOuterPcp = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (GPE_EGR_MIRR_OUTER_TAG_SHIFT_0);  /* shift to right aligned position */
	egrMirrOuterTagTmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (GPE_EGR_MIRR_OUTER_TAG_MASK_1);   /* mask desired bits */
	egrMirrOuterTagTmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) GPE_EGR_MIRR_OUTER_TAG_SHIFT_OVLP);
	p_generalParametersEntry->egrMirrOuterTag = (uint16_t) egrMirrOuterTagTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (GPE_TAKE_EGR_TS_ON_HOST_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_TAKE_EGR_TS_ON_HOST_PORT_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->takeEgrTsOnHostPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (GPE_TPID2_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->tpid2 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (GPE_IGNORE_2_STEP_FLAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_IGNORE_2_STEP_FLAG_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->ignore2StepFlag = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (GPE_TPID_SHIFT_0);  /* shift to right aligned position */
	tpidTmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (GPE_TPID_MASK_1);   /* mask desired bits */
	tpidTmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) GPE_TPID_SHIFT_OVLP);
	p_generalParametersEntry->tpid = (uint16_t) tpidTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (GPE_VL_MASK_SHIFT_0);  /* shift to right aligned position */
	vlMaskTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (GPE_VL_MASK_MASK_1);   /* mask desired bits */
	vlMaskTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) GPE_VL_MASK_SHIFT_OVLP);
	p_generalParametersEntry->vlMask = (uint32_t) vlMaskTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult >>= (uint32_t) (GPE_VL_MARKER_SHIFT_0);  /* shift to right aligned position */
	vlMarkerTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_VL_MARKER_MASK_1);   /* mask desired bits */
	vlMarkerTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) GPE_VL_MARKER_SHIFT_OVLP);
	p_generalParametersEntry->vlMarker = (uint32_t) vlMarkerTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_MIRRORP_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_MIRRORP_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->mirrorp = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_HOST_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_HOST_PORT_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->hostPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_CASCADING_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_CASCADING_PORT_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->cascadingPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_SEND_FOLLOW_UP_TS_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_SEND_FOLLOW_UP_TS_0_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->sendFollowUpTs0 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_SEND_FOLLOW_UP_TS_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_SEND_FOLLOW_UP_TS_1_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->sendFollowUpTs1 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_INCLUDE_SRC_PORT_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_INCLUDE_SRC_PORT_0_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->includeSrcPort0 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (GPE_INCLUDE_SRC_PORT_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_INCLUDE_SRC_PORT_1_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->includeSrcPort1 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult >>= (uint32_t) (GPE_MAC_FILTER_0_SHIFT_0);  /* shift to right aligned position */
	macFilter0Tmp  = (uint64_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD5];
	macFilter0Tmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GPE_MAC_FILTER_0_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD6];
	macFilter0Tmp |= (uint64_t) ((uint64_t) ((uint8_t) cResult) << (uint8_t) GPE_MAC_FILTER_0_SHIFT_OVLP_2);
	p_generalParametersEntry->macFilter0 = (uint64_t) macFilter0Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult >>= (uint32_t) (GPE_MAC_FILTER_1_SHIFT_0);  /* shift to right aligned position */
	macFilter1Tmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD7];
	cResult  &= (uint32_t) (GPE_MAC_FILTER_1_MASK_1);   /* mask desired bits */
	macFilter1Tmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GPE_MAC_FILTER_1_SHIFT_OVLP);
	p_generalParametersEntry->macFilter1 = (uint64_t) macFilter1Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD7];
	cResult >>= (uint32_t) (GPE_MAC_FILTER_RESULT_0_SHIFT_0);  /* shift to right aligned position */
	macFilterResult0Tmp  = (uint64_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD8];
	macFilterResult0Tmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD9];
	macFilterResult0Tmp |= (uint64_t) ((uint64_t) ((uint8_t) cResult) << (uint8_t) GPE_MAC_FILTER_RESULT_0_SHIFT_OVLP_2);
	p_generalParametersEntry->macFilterResult0 = (uint64_t) macFilterResult0Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD9];
	cResult >>= (uint32_t) (GPE_MAC_FILTER_RESULT_1_SHIFT_0);  /* shift to right aligned position */
	macFilterResult1Tmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD10];
	cResult  &= (uint32_t) (GPE_MAC_FILTER_RESULT_1_MASK_1);   /* mask desired bits */
	macFilterResult1Tmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) GPE_MAC_FILTER_RESULT_1_SHIFT_OVLP);
	p_generalParametersEntry->macFilterResult1 = (uint64_t) macFilterResult1Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD10];
	cResult  &= (uint32_t) (GPE_MGMT_PRIO_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_MGMT_PRIO_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->mgmtPrio = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD10];
	cResult  &= (uint32_t) (GPE_ID_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_ID_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->id = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD10];
	cResult  &= (uint32_t) (GPE_MIRROR_PORT_RECONFIG_ENABLE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPE_MIRROR_PORT_RECONFIG_ENABLE_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->mirrorPortReconfigEnable = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD10];
	cResult >>= (uint32_t) (GPE_ARINC_SHIFT);  /* shift to right aligned position */
	p_generalParametersEntry->arinc = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the general_parameters_control
*
* \param[in]  generalParametersControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalParametersControlSetArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setGeneralParametersControl(const SJA1105P_generalParametersControlSetArgument_t *pk_generalParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_generalParametersControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersControl->rdwrset <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_generalParametersControl->valid) << ((uint8_t) GPC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_generalParametersControl->rdwrset) << ((uint8_t) GPC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) GPC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) GPC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the general_parameters_control
*
* \param[out] p_generalParametersControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_generalParametersControlGetArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getGeneralParametersControl(SJA1105P_generalParametersControlGetArgument_t *p_generalParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) GPC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GPC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPC_ERRORS_SHIFT);  /* shift to right aligned position */
	p_generalParametersControl->errors = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (GPC_MIRR_PORT_RECONFIG_ERROR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (GPC_MIRR_PORT_RECONFIG_ERROR_SHIFT);  /* shift to right aligned position */
	p_generalParametersControl->mirrPortReconfigError = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the mac_cfg_table_entry
*
* \param[in]  macCfgTableEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_macCfgTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setMacCfgTableEntry(const SJA1105P_macCfgTableEntryArgument_t *pk_macCfgTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[MAC_CFG_TE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_macCfgTableEntry->ingressMirroringOuterDei <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->ingressMirroringOuterPcp <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->ingressMirroringOuterTag <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->mirrorConflictOnEgressTag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->mirrorConflictOnIngressEgress <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->ingress <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->egress <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->dynLearn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->retag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->drpuntag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->dropSingleInnerTagged <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->dropSingleOuterTagged <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->dropDoubleTagged <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->dropNonA664 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->egrmirr <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->ingmirr <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->vlanPrio <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->speed <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->ifg <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled0 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base0 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top0 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled1 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base1 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top1 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled2 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base2 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top2 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled3 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base3 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top3 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled4 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base4 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top4 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled5 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base5 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top5 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled6 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base6 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top6 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->enabled7 <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->base7 <= 0x1FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableEntry->top7 <= 0x1FFU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_macCfgTableEntry->ingressMirroringOuterDei) << ((uint8_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->ingressMirroringOuterPcp) << ((uint8_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->ingressMirroringOuterTag) << ((uint8_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->mirrorConflictOnEgressTag) << ((uint8_t) MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->mirrorConflictOnIngressEgress) << ((uint8_t) MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->ingress) << ((uint8_t) MAC_CFG_TE_INGRESS_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = ((uint32_t) MAC_CFG_TE_EGRESS_MASK) & pk_macCfgTableEntry->egress;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->dynLearn) << ((uint8_t) MAC_CFG_TE_DYN_LEARN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DYN_LEARN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->retag) << ((uint8_t) MAC_CFG_TE_RETAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_RETAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->drpuntag) << ((uint8_t) MAC_CFG_TE_DRPUNTAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DRPUNTAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->dropSingleInnerTagged) << ((uint8_t) MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->dropSingleOuterTagged) << ((uint8_t) MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->dropDoubleTagged) << ((uint8_t) MAC_CFG_TE_DROP_DOUBLE_TAGGED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DROP_DOUBLE_TAGGED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->dropNonA664) << ((uint8_t) MAC_CFG_TE_DROP_NON_A664_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_DROP_NON_A664_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->egrmirr) << ((uint8_t) MAC_CFG_TE_EGRMIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_EGRMIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->ingmirr) << ((uint8_t) MAC_CFG_TE_INGMIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_INGMIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->vlanid) << ((uint8_t) MAC_CFG_TE_VLANID_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_VLANID_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->vlanPrio) << ((uint8_t) MAC_CFG_TE_VLAN_PRIO_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_VLAN_PRIO_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->maxAge) << ((uint8_t) MAC_CFG_TE_MAX_AGE_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->maxAge >> (uint8_t) MAC_CFG_TE_MAX_AGE_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_MAX_AGE_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->tpdelout) << ((uint8_t) MAC_CFG_TE_TPDELOUT_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->tpdelin) << ((uint8_t) MAC_CFG_TE_TPDELIN_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->tpdelin >> (uint8_t) MAC_CFG_TE_TPDELIN_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_TPDELIN_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->speed) << ((uint8_t) MAC_CFG_TE_SPEED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_SPEED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->ifg) << ((uint8_t) MAC_CFG_TE_IFG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_IFG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled0) << ((uint8_t) MAC_CFG_TE_ENABLED_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_0_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base0) << ((uint8_t) MAC_CFG_TE_BASE_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_0_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top0) << ((uint8_t) MAC_CFG_TE_TOP_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_0_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled1) << ((uint8_t) MAC_CFG_TE_ENABLED_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base1) << ((uint8_t) MAC_CFG_TE_BASE_1_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD3] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->base1 >> (uint8_t) MAC_CFG_TE_BASE_1_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_1_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top1) << ((uint8_t) MAC_CFG_TE_TOP_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled2) << ((uint8_t) MAC_CFG_TE_ENABLED_2_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_2_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base2) << ((uint8_t) MAC_CFG_TE_BASE_2_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_2_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top2) << ((uint8_t) MAC_CFG_TE_TOP_2_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD4] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->top2 >> (uint8_t) MAC_CFG_TE_TOP_2_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_2_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled3) << ((uint8_t) MAC_CFG_TE_ENABLED_3_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_3_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base3) << ((uint8_t) MAC_CFG_TE_BASE_3_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_3_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top3) << ((uint8_t) MAC_CFG_TE_TOP_3_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_3_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled4) << ((uint8_t) MAC_CFG_TE_ENABLED_4_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_4_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base4) << ((uint8_t) MAC_CFG_TE_BASE_4_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_4_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top4) << ((uint8_t) MAC_CFG_TE_TOP_4_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD5] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->top4 >> (uint8_t) MAC_CFG_TE_TOP_4_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_4_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled5) << ((uint8_t) MAC_CFG_TE_ENABLED_5_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_5_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base5) << ((uint8_t) MAC_CFG_TE_BASE_5_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_5_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top5) << ((uint8_t) MAC_CFG_TE_TOP_5_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_5_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled6) << ((uint8_t) MAC_CFG_TE_ENABLED_6_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_6_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base6) << ((uint8_t) MAC_CFG_TE_BASE_6_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD6] = cData;
	tmp = (uint32_t) ((uint32_t) pk_macCfgTableEntry->base6 >> (uint8_t) MAC_CFG_TE_BASE_6_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_6_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top6) << ((uint8_t) MAC_CFG_TE_TOP_6_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_TOP_6_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->enabled7) << ((uint8_t) MAC_CFG_TE_ENABLED_7_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_ENABLED_7_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->base7) << ((uint8_t) MAC_CFG_TE_BASE_7_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TE_BASE_7_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macCfgTableEntry->top7) << ((uint8_t) MAC_CFG_TE_TOP_7_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD7] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, MAC_CFG_TE_WORD_COUNT, (uint32_t) MAC_CFG_TE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_cfg_table_entry
*
* \param[out] p_macCfgTableEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_macCfgTableEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getMacCfgTableEntry(SJA1105P_macCfgTableEntryArgument_t *p_macCfgTableEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[MAC_CFG_TE_WORD_COUNT] = {0};
	uint8_t maxAgeTmp;
	uint16_t tpdelinTmp;
	uint16_t base1Tmp;
	uint16_t top2Tmp;
	uint16_t top4Tmp;
	uint16_t base6Tmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, MAC_CFG_TE_WORD_COUNT, (uint32_t) MAC_CFG_TE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_DEI_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ingressMirroringOuterDei = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_PCP_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ingressMirroringOuterPcp = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_INGRESS_MIRRORING_OUTER_TAG_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ingressMirroringOuterTag = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_MIRROR_CONFLICT_ON_EGRESS_TAG_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->mirrorConflictOnEgressTag = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_MIRROR_CONFLICT_ON_INGRESS_EGRESS_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->mirrorConflictOnIngressEgress = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (MAC_CFG_TE_INGRESS_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ingress = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_EGRESS_MASK);   /* mask desired bits */
	p_macCfgTableEntry->egress = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DYN_LEARN_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DYN_LEARN_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->dynLearn = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_RETAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_RETAG_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->retag = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DRPUNTAG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DRPUNTAG_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->drpuntag = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DROP_SINGLE_INNER_TAGGED_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->dropSingleInnerTagged = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DROP_SINGLE_OUTER_TAGGED_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->dropSingleOuterTagged = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DROP_DOUBLE_TAGGED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DROP_DOUBLE_TAGGED_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->dropDoubleTagged = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_DROP_NON_A664_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_DROP_NON_A664_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->dropNonA664 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_EGRMIRR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_EGRMIRR_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->egrmirr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_INGMIRR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_INGMIRR_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ingmirr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_VLANID_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_VLANID_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->vlanid = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (MAC_CFG_TE_VLAN_PRIO_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_VLAN_PRIO_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->vlanPrio = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (MAC_CFG_TE_MAX_AGE_SHIFT_0);  /* shift to right aligned position */
	maxAgeTmp  = (uint8_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (MAC_CFG_TE_MAX_AGE_MASK_1);   /* mask desired bits */
	maxAgeTmp |= (uint8_t) ((uint8_t) cResult << (uint8_t) MAC_CFG_TE_MAX_AGE_SHIFT_OVLP);
	p_macCfgTableEntry->maxAge = (uint8_t) maxAgeTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (MAC_CFG_TE_TPDELOUT_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->tpdelout = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (MAC_CFG_TE_TPDELIN_SHIFT_0);  /* shift to right aligned position */
	tpdelinTmp  = (uint16_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_TPDELIN_MASK_1);   /* mask desired bits */
	tpdelinTmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) MAC_CFG_TE_TPDELIN_SHIFT_OVLP);
	p_macCfgTableEntry->tpdelin = (uint16_t) tpdelinTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_SPEED_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_SPEED_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->speed = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_IFG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_IFG_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->ifg = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_0_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled0 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_0_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base0 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_0_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top0 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_1_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled1 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_1_SHIFT_0);  /* shift to right aligned position */
	base1Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_1_MASK_1);   /* mask desired bits */
	base1Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) MAC_CFG_TE_BASE_1_SHIFT_OVLP);
	p_macCfgTableEntry->base1 = (uint16_t) base1Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_1_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top1 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_2_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_2_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled2 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_2_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_2_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base2 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD4];
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_2_SHIFT_0);  /* shift to right aligned position */
	top2Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_2_MASK_1);   /* mask desired bits */
	top2Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) MAC_CFG_TE_TOP_2_SHIFT_OVLP);
	p_macCfgTableEntry->top2 = (uint16_t) top2Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_3_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_3_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled3 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_3_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_3_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base3 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_3_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_3_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top3 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_4_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled4 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_4_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base4 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD5];
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_4_SHIFT_0);  /* shift to right aligned position */
	top4Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD6];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_4_MASK_1);   /* mask desired bits */
	top4Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) MAC_CFG_TE_TOP_4_SHIFT_OVLP);
	p_macCfgTableEntry->top4 = (uint16_t) top4Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_5_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_5_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled5 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_5_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_5_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base5 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_5_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_5_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top5 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_6_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_6_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled6 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD6];
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_6_SHIFT_0);  /* shift to right aligned position */
	base6Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD7];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_6_MASK_1);   /* mask desired bits */
	base6Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) MAC_CFG_TE_BASE_6_SHIFT_OVLP);
	p_macCfgTableEntry->base6 = (uint16_t) base6Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD7];
	cResult  &= (uint32_t) (MAC_CFG_TE_TOP_6_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_6_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top6 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD7];
	cResult  &= (uint32_t) (MAC_CFG_TE_ENABLED_7_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_ENABLED_7_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->enabled7 = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD7];
	cResult  &= (uint32_t) (MAC_CFG_TE_BASE_7_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TE_BASE_7_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->base7 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD7];
	cResult >>= (uint32_t) (MAC_CFG_TE_TOP_7_SHIFT);  /* shift to right aligned position */
	p_macCfgTableEntry->top7 = (uint16_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the mac_cfg_table_control
*
* \param[in]  macCfgTableControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_macCfgTableControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setMacCfgTableControl(const SJA1105P_macCfgTableControlArgument_t *pk_macCfgTableControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_macCfgTableControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macCfgTableControl->port <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_macCfgTableControl->valid) << ((uint8_t) MAC_CFG_TC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macCfgTableControl->rdwrset) << ((uint8_t) MAC_CFG_TC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) MAC_CFG_TC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) MAC_CFG_TC_PORT_MASK) & pk_macCfgTableControl->port;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) MAC_CFG_TC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_cfg_table_control
*
* \param[out] p_errors Error
* \return uint8_t
*/
uint8_t SJA1105P_getMacCfgTableControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) MAC_CFG_TC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_CFG_TC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_CFG_TC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_lookup_parameters_entry
*
* \param[in]  l2LookupParametersEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2LookupParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2LookupParametersEntry(const SJA1105P_l2LookupParametersEntryArgument_t *pk_l2LookupParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[L2_LPE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2LookupParametersEntry->oneShotLearning <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->overwriteDynIfTcamFull <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->useFreeStaticSpaceForLearning <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->ignoreMgmtAddrAtHostPort <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->skipPortEnforcementAtHostPort <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->sharedLearning <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->dropIfCannotLearn <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->startDynSpaceAddr <= 0x3FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxAge <= 0x7FFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort0 <= 0x7FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort1 <= 0x7FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort2 <= 0x7FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort3 <= 0x7FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort4 <= 0x7FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->enableSourcePortEnforcingForUnicast <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->enableSourcePortEnforcingForMulticast <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersEntry->enableSourcePortEnforcingForBroadcast <= 0x1FU) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2LookupParametersEntry->oneShotLearning) << ((uint8_t) L2_LPE_ONE_SHOT_LEARNING_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_ONE_SHOT_LEARNING_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->overwriteDynIfTcamFull) << ((uint8_t) L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->useFreeStaticSpaceForLearning) << ((uint8_t) L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->ignoreMgmtAddrAtHostPort) << ((uint8_t) L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->skipPortEnforcementAtHostPort) << ((uint8_t) L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->sharedLearning) << ((uint8_t) L2_LPE_SHARED_LEARNING_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_SHARED_LEARNING_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->dropIfCannotLearn) << ((uint8_t) L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2LookupParametersEntry->dropIfCannotLearn >> (uint8_t) L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_LPE_DROP_IF_CANNOT_LEARN_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->startDynSpaceAddr) << ((uint8_t) L2_LPE_START_DYN_SPACE_ADDR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_START_DYN_SPACE_ADDR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxAge) << ((uint8_t) L2_LPE_MAX_AGE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_MAX_AGE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort0) << ((uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort0 >> (uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort1) << ((uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort2) << ((uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort3) << ((uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort3 >> (uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort4) << ((uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->enableSourcePortEnforcingForUnicast) << ((uint8_t) L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->enableSourcePortEnforcingForMulticast) << ((uint8_t) L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2LookupParametersEntry->enableSourcePortEnforcingForBroadcast) << ((uint8_t) L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_BROADCAST_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD3] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, L2_LPE_WORD_COUNT, (uint32_t) L2_LPE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_lookup_parameters_entry
*
* \param[out] p_l2LookupParametersEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2LookupParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2LookupParametersEntry(SJA1105P_l2LookupParametersEntryArgument_t *p_l2LookupParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[L2_LPE_WORD_COUNT] = {0};
	uint8_t dropIfCannotLearnTmp;
	uint16_t maxNoOfAddrAllowedPerPort0Tmp;
	uint16_t maxNoOfAddrAllowedPerPort3Tmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, L2_LPE_WORD_COUNT, (uint32_t) L2_LPE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_ONE_SHOT_LEARNING_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_ONE_SHOT_LEARNING_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->oneShotLearning = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_OVERWRITE_DYN_IF_TCAM_FULL_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->overwriteDynIfTcamFull = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_USE_FREE_STATIC_SPACE_FOR_LEARNING_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->useFreeStaticSpaceForLearning = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_IGNORE_MGMT_ADDR_AT_HOST_PORT_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->ignoreMgmtAddrAtHostPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_SKIP_PORT_ENFORCEMENT_AT_HOST_PORT_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->skipPortEnforcementAtHostPort = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_LPE_SHARED_LEARNING_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_SHARED_LEARNING_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->sharedLearning = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_0);  /* shift to right aligned position */
	dropIfCannotLearnTmp  = (uint8_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_LPE_DROP_IF_CANNOT_LEARN_MASK_1);   /* mask desired bits */
	dropIfCannotLearnTmp |= (uint8_t) ((uint8_t) cResult << (uint8_t) L2_LPE_DROP_IF_CANNOT_LEARN_SHIFT_OVLP);
	p_l2LookupParametersEntry->dropIfCannotLearn = (uint8_t) dropIfCannotLearnTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_LPE_START_DYN_SPACE_ADDR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_START_DYN_SPACE_ADDR_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->startDynSpaceAddr = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_LPE_MAX_AGE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_MAX_AGE_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->maxAge = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_0);  /* shift to right aligned position */
	maxNoOfAddrAllowedPerPort0Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_MASK_1);   /* mask desired bits */
	maxNoOfAddrAllowedPerPort0Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_0_SHIFT_OVLP);
	p_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort0 = (uint16_t) maxNoOfAddrAllowedPerPort0Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_1_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort1 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_2_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort2 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_0);  /* shift to right aligned position */
	maxNoOfAddrAllowedPerPort3Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_MASK_1);   /* mask desired bits */
	maxNoOfAddrAllowedPerPort3Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_3_SHIFT_OVLP);
	p_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort3 = (uint16_t) maxNoOfAddrAllowedPerPort3Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_MAX_NO_OF_ADDR_ALLOWED_PER_PORT_4_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->maxNoOfAddrAllowedPerPort4 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_UNICAST_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->enableSourcePortEnforcingForUnicast = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_MULTICAST_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->enableSourcePortEnforcingForMulticast = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult >>= (uint32_t) (L2_LPE_ENABLE_SOURCE_PORT_ENFORCING_FOR_BROADCAST_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersEntry->enableSourcePortEnforcingForBroadcast = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_lookup_parameters_control
*
* \param[in]  l2LookupParametersControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2LookupParametersControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setL2LookupParametersControl(const SJA1105P_l2LookupParametersControlArgument_t *pk_l2LookupParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2LookupParametersControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2LookupParametersControl->rdwrset <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2LookupParametersControl->valid) << ((uint8_t) L2_LPC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2LookupParametersControl->rdwrset) << ((uint8_t) L2_LPC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) L2_LPC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) L2_LPC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_lookup_parameters_control
*
* \param[out] p_valid By writing a value that has this flag set, the host triggers an update.
* \return uint8_t
*/
uint8_t SJA1105P_getL2LookupParametersControl(uint8_t *p_valid, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) L2_LPC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (L2_LPC_VALID_SHIFT);  /* shift to right aligned position */
	*p_valid = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_egress_0
*
* \param[out] p_update Set if value PTPEGR_TS has changed
* \param[in]  port Ethernet port number {0:4}
* \param[in]  timestampIndex Egress timestamp number number {0:1}
* \return uint8_t
*/
uint8_t SJA1105P_getPtpEgress0(uint8_t *p_update, uint8_t port, uint8_t timestampIndex, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) PTP_E_0_ADDR + ((uint32_t) PTP_E_PORT_INC * port) + ((uint32_t) PTP_E_TIMESTAMP_INDEX_INC * timestampIndex)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (PTP_E_0_UPDATE_MASK);   /* mask desired bits */
	*p_update = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_egress_1
*
* \param[out] p_ptpegrTs PTP egress timestamp: contains the PTP egress timestamp of the most recent management frame for which the user specified an egress timestamp to be captured
* \param[in]  port Ethernet port number {0:4}
* \param[in]  timestampIndex Egress timestamp number number {0:1}
* \return uint8_t
*/
uint8_t SJA1105P_getPtpEgress1(uint32_t *p_ptpegrTs, uint8_t port, uint8_t timestampIndex, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) PTP_E_1_ADDR + ((uint32_t) PTP_E_PORT_INC * port) + ((uint32_t) PTP_E_TIMESTAMP_INDEX_INC * timestampIndex)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ptpegrTs = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_memory_partition_status
*
* \param[out] p_l2MemoryPartitionStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2MemoryPartitionStatusArgument_t
* \param[in]  partition Memory partition number {0:7}
* \return uint8_t
*/
uint8_t SJA1105P_getL2MemoryPartitionStatus(SJA1105P_l2MemoryPartitionStatusArgument_t *p_l2MemoryPartitionStatus, uint8_t partition, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) L2_MPS_ADDR + ((uint32_t) partition)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (L2_MPS_L2PARTS_SHIFT);  /* shift to right aligned position */
	p_l2MemoryPartitionStatus->l2parts = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (L2_MPS_N_L2PSPC_MASK);   /* mask desired bits */
	p_l2MemoryPartitionStatus->nL2pspc = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_level_diagn_counters
*
* \param[out] p_macLevelDiagnCounters Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_macLevelDiagnCountersArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getMacLevelDiagnCounters(SJA1105P_macLevelDiagnCountersArgument_t *p_macLevelDiagnCounters, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) MAC_LDC_ADDR + ((uint32_t) MAC_LDC_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (MAC_LDC_N_RUNT_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnCounters->nRunt = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (MAC_LDC_N_SOFERR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnCounters->nSoferr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (MAC_LDC_N_ALIGNERR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnCounters->nAlignerr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	p_macLevelDiagnCounters->nMiierr = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_level_diagn_flags
*
* \param[out] p_macLevelDiagnFlags Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_macLevelDiagnFlagsArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getMacLevelDiagnFlags(SJA1105P_macLevelDiagnFlagsArgument_t *p_macLevelDiagnFlags, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) MAC_LDF_ADDR + ((uint32_t) MAC_LDF_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_SPCPRIOR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_SPCPRIOR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->spcprior = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_PORTDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_PORTDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->portdrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_LENDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_LENDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->lendrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_BWDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_BWDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->bwdrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_DRN664ERRS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_DRN664ERRS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->drn664errs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (MAC_LDF_SPCERRS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (MAC_LDF_SPCERRS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnFlags->spcerrs = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_0
*
* \param[out] p_nTxbyte Number of bytes transmitted to the respective port since power-on or reset. The counter will wrap
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters10(uint64_t *p_nTxbyte, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[HL_DC_1_0_WORD_COUNT] = {0};
	uint64_t nTxbyteTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, HL_DC_1_0_WORD_COUNT, (uint32_t) ((uint32_t) HL_DC_1_0_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nTxbyteTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nTxbyteTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) HL_DC_1_0_N_TXBYTE_SHIFT_OVLP);
	*p_nTxbyte = (uint64_t) nTxbyteTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_1
*
* \param[out] p_nTxfrm Number of frames transmitted to the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters11(uint64_t *p_nTxfrm, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[HL_DC_1_1_WORD_COUNT] = {0};
	uint64_t nTxfrmTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, HL_DC_1_1_WORD_COUNT, (uint32_t) ((uint32_t) HL_DC_1_1_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nTxfrmTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nTxfrmTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) HL_DC_1_1_N_TXFRM_SHIFT_OVLP);
	*p_nTxfrm = (uint64_t) nTxfrmTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_2
*
* \param[out] p_nRxbyte Number of bytes received on the respective port in MAC-level correct frames. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters12(uint64_t *p_nRxbyte, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[HL_DC_1_2_WORD_COUNT] = {0};
	uint64_t nRxbyteTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, HL_DC_1_2_WORD_COUNT, (uint32_t) ((uint32_t) HL_DC_1_2_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nRxbyteTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nRxbyteTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) HL_DC_1_2_N_RXBYTE_SHIFT_OVLP);
	*p_nRxbyte = (uint64_t) nRxbyteTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_3
*
* \param[out] p_nRxfrm Number of MAC-level correct frames received on the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters13(uint64_t *p_nRxfrm, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[HL_DC_1_3_WORD_COUNT] = {0};
	uint64_t nRxfrmTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, HL_DC_1_3_WORD_COUNT, (uint32_t) ((uint32_t) HL_DC_1_3_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nRxfrmTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nRxfrmTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) HL_DC_1_3_N_RXFRM_SHIFT_OVLP);
	*p_nRxfrm = (uint64_t) nRxfrmTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_4
*
* \param[out] p_nPolerr Number of frames that were dropped based on the policing operation. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters14(uint32_t *p_nPolerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_1_4_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nPolerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_5
*
* \param[out] p_nCrcerr Number of frames that had a receive-side CRC error on this port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters15(uint32_t *p_nCrcerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_1_5_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nCrcerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_6
*
* \param[out] p_nSizeerr Number of frames with an invalid length on this port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters16(uint32_t *p_nSizeerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_1_6_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nSizeerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_7
*
* \param[out] p_nVlanerr Number of frames that were dropped because the VLAN ID was either not found in the VLAN Lookup table or the respective port is not listed in the VMEMB_PORT vector of the configured VLANID. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters17(uint32_t *p_nVlanerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_1_7_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nVlanerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_1_8
*
* \param[out] p_nN664err Number of frames dropped because they had an Ether-Type field other than 0x800 while the DRPNONA664 flag is set for the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters18(uint32_t *p_nN664err, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_1_8_ADDR + ((uint32_t) HL_DC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nN664err = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_2_0
*
* \param[out] p_notReach Number of frames that produced a match in the L2 Lookup Table but were not forwarded to the port this counter is assigned to because the port is not reachable for the respective ingress port as per REACH_PORTS of the L2 Forwarding Table. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters20(uint32_t *p_notReach, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_2_0_ADDR + ((uint32_t) HL_DC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_notReach = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_2_1
*
* \param[out] p_egrDisabled This field counts the number of frames that were not routed to the port this counter is assigned to because the port was down as per the flag at position 1 of PORTST. The counter will wrap
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters21(uint32_t *p_egrDisabled, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_2_1_ADDR + ((uint32_t) HL_DC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrDisabled = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_2_2
*
* \param[out] p_partDrop Number of frames that were dropped on ingress because the respective memory partition of the port as defined per PARTITION of the L2 Policing Table received at this port did not have any space left. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters22(uint32_t *p_partDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_2_2_ADDR + ((uint32_t) HL_DC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_partDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_2_3
*
* \param[out] p_qfull Number of frames that were dropped on egress because the respective priority queue of the destination port as defined per VLAN_PMAP of the L2 Forwarding Table did not have any space left. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCounters23(uint32_t *p_qfull, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DC_2_3_ADDR + ((uint32_t) HL_DC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_qfull = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the hl_diagn_counters_occupancy
*
* \param[out] p_hlDiagnCountersOccupancy Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_hlDiagnCountersOccupancyArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  queue Output priority queue number {0:7}
* \return uint8_t
*/
uint8_t SJA1105P_getHlDiagnCountersOccupancy(SJA1105P_hlDiagnCountersOccupancyArgument_t *p_hlDiagnCountersOccupancy, uint8_t port, uint8_t queue, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) HL_DCO_ADDR + ((uint32_t) HL_DCO_PORT_INC * port) + ((uint32_t) queue)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (HL_DCO_Q_OCCUPANCY_HIGH_WATT_SHIFT);  /* shift to right aligned position */
	p_hlDiagnCountersOccupancy->qOccupancyHighWatt = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	p_hlDiagnCountersOccupancy->qOccupancy = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_memory_partition_error_counters
*
* \param[out] p_nL2pspcdrn Number of frames dropped due to lack of L2 memory partition space. The counter will wrap.
* \param[in]  partition Memory partition number {0:7}
* \return uint8_t
*/
uint8_t SJA1105P_getL2MemoryPartitionErrorCounters(uint32_t *p_nL2pspcdrn, uint8_t partition, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) L2_MPEC_ADDR + ((uint32_t) partition)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nL2pspcdrn = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_00
*
* \param[out] p_ingBroadcastPkts Number of correct received broadcast frames
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters00(uint32_t *p_ingBroadcastPkts, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_00_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingBroadcastPkts = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_01
*
* \param[out] p_ingMulticastPkts Number of correct received multicast frames
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters01(uint32_t *p_ingMulticastPkts, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_01_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingMulticastPkts = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_02
*
* \param[out] p_ingPkts64Octets Number of correct received frames with length = 64
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters02(uint32_t *p_ingPkts64Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_02_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts64Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_03
*
* \param[out] p_ingPkts65To127Octets Number of correct received frames with length >= 65 and <= 127
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters03(uint32_t *p_ingPkts65To127Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_03_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts65To127Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_04
*
* \param[out] p_ingPkts128To255Octets Number of correct received frames with length >= 128 and <= 255
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters04(uint32_t *p_ingPkts128To255Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_04_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts128To255Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_05
*
* \param[out] p_ingPkts256To511Octets Number of correct received frames with length >= 256 and <= 511
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters05(uint32_t *p_ingPkts256To511Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_05_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts256To511Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_06
*
* \param[out] p_ingPkts512To1023Octets Number of correct received frames with length >= 512 and <= 1023
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters06(uint32_t *p_ingPkts512To1023Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_06_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts512To1023Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_07
*
* \param[out] p_ingPkts1024To1518Octets Number of correct received frames with length >= 1024 and <= 1518
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters07(uint32_t *p_ingPkts1024To1518Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_07_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingPkts1024To1518Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_08
*
* \param[out] p_egrBroadcastPkts Number of transmitted broadcast frames
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters08(uint32_t *p_egrBroadcastPkts, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_08_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrBroadcastPkts = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_09
*
* \param[out] p_egrMulticastPkts Number of transmitted multicast frames
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters09(uint32_t *p_egrMulticastPkts, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_09_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrMulticastPkts = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_10
*
* \param[out] p_egrPkts64Octets Number of transmitted frames with length = 64
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters10(uint32_t *p_egrPkts64Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_10_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts64Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_11
*
* \param[out] p_egrPkts65To127Octets Number of transmitted frames with length >= 65 and <= 127
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters11(uint32_t *p_egrPkts65To127Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_11_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts65To127Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_12
*
* \param[out] p_egrPkts128To255Octets Number of transmitted frames with length >= 128 and <= 255
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters12(uint32_t *p_egrPkts128To255Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_12_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts128To255Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_13
*
* \param[out] p_egrPkts256To511Octets Number of transmitted frames with length >= 256 and <= 511
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters13(uint32_t *p_egrPkts256To511Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_13_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts256To511Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_14
*
* \param[out] p_egrPkts512To1023Octets Number of transmitted frames with length >= 512 and <= 1023
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters14(uint32_t *p_egrPkts512To1023Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_14_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts512To1023Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_15
*
* \param[out] p_egrPkts1024To1518Octets Number of transmitted frames with length >= 1024 and <= 1518
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters15(uint32_t *p_egrPkts1024To1518Octets, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_15_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrPkts1024To1518Octets = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_16
*
* \param[out] p_ingUntaggedDrop Number of received untagged frames that were dropped
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters16(uint32_t *p_ingUntaggedDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_16_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingUntaggedDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_17
*
* \param[out] p_ingSingleInnerDrop Number of received single inner tagged frames that were dropped
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters17(uint32_t *p_ingSingleInnerDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_17_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingSingleInnerDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_18
*
* \param[out] p_ingSingleOuterDrop Number of received single outer tagged frames that were dropped
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters18(uint32_t *p_ingSingleOuterDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_18_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingSingleOuterDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_19
*
* \param[out] p_ingDoubleTaggedDrop Number of received double tagged frames that were dropped
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters19(uint32_t *p_ingDoubleTaggedDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_19_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingDoubleTaggedDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_20
*
* \param[out] p_ingIllegalDoubleTaggedDrop Number of received frames that are dropped because they are classified as illegal double tagged traffic
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters20(uint32_t *p_ingIllegalDoubleTaggedDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_20_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingIllegalDoubleTaggedDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_21
*
* \param[out] p_ingEmptyRouteDrop Number of received frames that are dropped because their destination route is empty
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters21(uint32_t *p_ingEmptyRouteDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_21_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingEmptyRouteDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the eth_stat_counters_22
*
* \param[out] p_ingAddrNotLearnedDrop Number of received frames that are dropped because their source MAC address cannot be learned and the switch is configured to drop this traffic
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105P_getEthStatCounters22(uint32_t *p_ingAddrNotLearnedDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) ETH_STAT_C_22_ADDR + ((uint32_t) ETH_STAT_C_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_ingAddrNotLearnedDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_forwarding_parameters
*
* \param[out] p_l2ForwardingParameters Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_l2ForwardingParametersArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getL2ForwardingParameters(SJA1105P_l2ForwardingParametersArgument_t *p_l2ForwardingParameters, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[L2_FP_WORD_COUNT] = {0};
	uint16_t partitionSpace6Tmp;
	uint16_t partitionSpace2Tmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, L2_FP_WORD_COUNT, (uint32_t) L2_FP_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_7_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_7_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace7 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_6_SHIFT_0);  /* shift to right aligned position */
	partitionSpace6Tmp  = (uint16_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_6_MASK_1);   /* mask desired bits */
	partitionSpace6Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) L2_FP_PARTITION_SPACE_6_SHIFT_OVLP);
	p_l2ForwardingParameters->partitionSpace6 = (uint16_t) partitionSpace6Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_5_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_5_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace5 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_4_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_4_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace4 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_3_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_3_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace3 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_2_SHIFT_0);  /* shift to right aligned position */
	partitionSpace2Tmp  = (uint16_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_2_MASK_1);   /* mask desired bits */
	partitionSpace2Tmp |= (uint16_t) ((uint16_t) cResult << (uint8_t) L2_FP_PARTITION_SPACE_2_SHIFT_OVLP);
	p_l2ForwardingParameters->partitionSpace2 = (uint16_t) partitionSpace2Tmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_1_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_1_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace1 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (L2_FP_PARTITION_SPACE_0_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (L2_FP_PARTITION_SPACE_0_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->partitionSpace0 = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (L2_FP_MAX_DYN_PRIO_SHIFT);  /* shift to right aligned position */
	p_l2ForwardingParameters->maxDynPrio = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the avb_parameters_entry
*
* \param[in]  avbParametersEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_avbParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setAvbParametersEntry(const SJA1105P_avbParametersEntryArgument_t *pk_avbParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[AVB_PE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_avbParametersEntry->srcMeta <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_avbParametersEntry->dstMeta <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_avbParametersEntry->ptpMaster <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_avbParametersEntry->useL2DataRateForCbs <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_avbParametersEntry->srcMeta) << ((uint8_t) AVB_PE_SRC_META_SHIFT_0);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (pk_avbParametersEntry->srcMeta >> (uint8_t) AVB_PE_SRC_META_SHIFT_OVLP_1);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) (pk_avbParametersEntry->srcMeta >> (uint8_t) AVB_PE_SRC_META_SHIFT_OVLP_2);  /* shift to right align subfield with register */
	tmp = ((uint32_t) AVB_PE_SRC_META_MASK_2) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_avbParametersEntry->dstMeta) << ((uint8_t) AVB_PE_DST_META_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	tmp = (uint32_t) (pk_avbParametersEntry->dstMeta >> (uint8_t) AVB_PE_DST_META_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) AVB_PE_DST_META_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_avbParametersEntry->ptpMaster) << ((uint8_t) AVB_PE_PTP_MASTER_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AVB_PE_PTP_MASTER_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_avbParametersEntry->useL2DataRateForCbs) << ((uint8_t) AVB_PE_USE_L2_DATA_RATE_FOR_CBS_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD3] = cData;
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, AVB_PE_WORD_COUNT, (uint32_t) AVB_PE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the avb_parameters_entry
*
* \param[out] p_avbParametersEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105P_avbParametersEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_getAvbParametersEntry(SJA1105P_avbParametersEntryArgument_t *p_avbParametersEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[AVB_PE_WORD_COUNT] = {0};
	uint64_t srcMetaTmp;
	uint64_t dstMetaTmp;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, AVB_PE_WORD_COUNT, (uint32_t) AVB_PE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (AVB_PE_SRC_META_SHIFT_0);  /* shift to right aligned position */
	srcMetaTmp  = (uint64_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	srcMetaTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) AVB_PE_SRC_META_SHIFT_OVLP_1);

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (AVB_PE_SRC_META_MASK_2);   /* mask desired bits */
	srcMetaTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) AVB_PE_SRC_META_SHIFT_OVLP_2);
	p_avbParametersEntry->srcMeta = (uint64_t) srcMetaTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (AVB_PE_DST_META_SHIFT_0);  /* shift to right aligned position */
	dstMetaTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (AVB_PE_DST_META_MASK_1);   /* mask desired bits */
	dstMetaTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) AVB_PE_DST_META_SHIFT_OVLP);
	p_avbParametersEntry->dstMeta = (uint64_t) dstMetaTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult  &= (uint32_t) (AVB_PE_PTP_MASTER_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AVB_PE_PTP_MASTER_SHIFT);  /* shift to right aligned position */
	p_avbParametersEntry->ptpMaster = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD3];
	cResult >>= (uint32_t) (AVB_PE_USE_L2_DATA_RATE_FOR_CBS_SHIFT);  /* shift to right aligned position */
	p_avbParametersEntry->useL2DataRateForCbs = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the avb_parameters_control
*
* \param[in]  avbParametersControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105P_avbParametersControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105P_setAvbParametersControl(const SJA1105P_avbParametersControlArgument_t *pk_avbParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;

		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_avbParametersControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_avbParametersControl->rdwrset <= 0x1U) ? 0U : 1U);

		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_avbParametersControl->valid) << ((uint8_t) AVB_PC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_avbParametersControl->rdwrset) << ((uint8_t) AVB_PC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) AVB_PC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) AVB_PC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the avb_parameters_control
*
* \param[out] p_errors The host sets this flag to trigger a dynamic change in the entry indicated by INDEX
* \return uint8_t
*/
uint8_t SJA1105P_getAvbParametersControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105P_gpf_spiRead32(deviceSelect, 1U, (uint32_t) AVB_PC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (AVB_PC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (AVB_PC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}


/**
* \brief Convert an unsigned stdint uint32_t to an enum of type ::SJA1105P_hostCmd_t
*
* \param[in]  stdintValue Value to be converted
*
* \return SJA1105P_hostCmd_t
*/
SJA1105P_hostCmd_t SJA1105P_convertToHostCmd(uint32_t stdintValue)
{
	SJA1105P_hostCmd_t ret;
	switch (stdintValue)
	{
		case 3U: ret = SJA1105P_e_hostCmd_WRITE; break;
		case 2U: ret = SJA1105P_e_hostCmd_READ; break;
		case 1U: ret = SJA1105P_e_hostCmd_SEARCH; break;
		default: ret = SJA1105P_e_hostCmd_INVALIDATE; break;
	}
	return ret;
}
