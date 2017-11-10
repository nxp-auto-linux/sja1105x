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
* \file NXP_SJA1105_switchCore.h
*
* \author NXP Semiconductors
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief Status information and control registers of the switch
*
*****************************************************************************/

#ifndef NXP_SJA1105_SWITCHCORE_H
#define NXP_SJA1105_SWITCHCORE_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category general_status_device */
#define SJA1105_GSD_0_ADDR (0x0)  /**< Address of the generalStatusDevice0 register */
#define SJA1105_GSD_1_ADDR (0x1)  /**< Address of the generalStatusDevice1 register */

/* register category general_status_l2 */
#define SJA1105_GS_L2_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusL2 register */
#define SJA1105_GS_L2_ADDR       (0x3)  /**< Address of the generalStatusL2 register */

/* register category general_status_hashconfs */
#define SJA1105_GSH_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusHashconfs register */
#define SJA1105_GSH_ADDR       (0x5)  /**< Address of the generalStatusHashconfs register */

/* register category general_status_vlan */
#define SJA1105_GS_VLAN_ADDR (0x7)  /**< Address of the generalStatusVlan register */

/* register category general_status_memory */
#define SJA1105_GSM_ADDR (0x9)  /**< Address of the generalStatusMemory register */

/* register category general_status_drop */
#define SJA1105_GS_DROP_ADDR (0xa)  /**< Address of the generalStatusDrop register */

/* register category general_status_ram_error */
#define SJA1105_GS_RAM_E_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusRamError register */
#define SJA1105_GS_RAM_E_ADDR       (0xb)  /**< Address of the generalStatusRamError register */

/* register category general_status_ram_init */
#define SJA1105_GS_RAM_INIT_WORD_COUNT (2)    /**< Number of data words contained in the generalStatusRamInit register */
#define SJA1105_GS_RAM_INIT_ADDR       (0xd)  /**< Address of the generalStatusRamInit register */

/* register category ethernet_port_status_control_reset */
#define SJA1105_E_PORT_SCR_ADDR (0xf)  /**< Address of the ethernetPortStatusControlReset register */

/* register category ethernet_port_status_control_transmission */
#define SJA1105_E_PORT_SCT_ADDR (0x11)  /**< Address of the ethernetPortStatusControlTransmission register */

/* register category ptp_control */
#define SJA1105_PTP_C_0_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl0 register */
#define SJA1105_PTP_C_0_ADDR       (0x14)  /**< Address of the ptpControl0 register */
#define SJA1105_PTP_C_1_ADDR       (0x16)  /**< Address of the ptpControl1 register */
#define SJA1105_PTP_C_2_ADDR       (0x17)  /**< Address of the ptpControl2 register */
#define SJA1105_PTP_C_3_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl3 register */
#define SJA1105_PTP_C_3_ADDR       (0x18)  /**< Address of the ptpControl3 register */
#define SJA1105_PTP_C_4_ADDR       (0x1a)  /**< Address of the ptpControl4 register */
#define SJA1105_PTP_C_5_WORD_COUNT (2)     /**< Number of data words contained in the ptpControl5 register */
#define SJA1105_PTP_C_5_ADDR       (0x1b)  /**< Address of the ptpControl5 register */

/* register category l2_address_lookup_table_reconfiguration_entry */
#define SJA1105_L2_ALTRE_WORD_COUNT (3)     /**< Number of data words contained in the l2AddressLookupTableReconfigurationEntry register */
#define SJA1105_L2_ALTRE_ADDR       (0x20)  /**< Address of the l2AddressLookupTableReconfigurationEntry register */

/* register category l2_address_lookup_table_reconfiguration_control */
#define SJA1105_L2_ALTRC_ADDR (0x23)  /**< Address of the l2AddressLookupTableReconfigurationControl register */

/* register category l2_forwarding_table_reconfiguration_entry */
#define SJA1105_L2_FTRE_WORD_COUNT (2)     /**< Number of data words contained in the l2ForwardingTableReconfigurationEntry register */
#define SJA1105_L2_FTRE_ADDR       (0x24)  /**< Address of the l2ForwardingTableReconfigurationEntry register */

/* register category l2_forwarding_table_reconfiguration_control */
#define SJA1105_L2_FTRC_ADDR (0x26)  /**< Address of the l2ForwardingTableReconfigurationControl register */

/* register category vlan_lookup_table_reconfiguration_entry */
#define SJA1105_VLAN_LTRE_WORD_COUNT (2)     /**< Number of data words contained in the vlanLookupTableReconfigurationEntry register */
#define SJA1105_VLAN_LTRE_ADDR       (0x27)  /**< Address of the vlanLookupTableReconfigurationEntry register */

/* register category vlan_lookup_table_reconfiguration_control */
#define SJA1105_VLAN_LTRC_ADDR (0x2a)  /**< Address of the vlanLookupTableReconfigurationControl register */

/* register category credit_based_shaping_block */
#define SJA1105_CBSB_0_ADDR (0x2c)  /**< Address of the creditBasedShapingBlock0 register */
#define SJA1105_CBSB_1_ADDR (0x2d)  /**< Address of the creditBasedShapingBlock1 register */
#define SJA1105_CBSB_2_ADDR (0x2e)  /**< Address of the creditBasedShapingBlock2 register */
#define SJA1105_CBSB_3_ADDR (0x2f)  /**< Address of the creditBasedShapingBlock3 register */
#define SJA1105_CBSB_4_ADDR (0x30)  /**< Address of the creditBasedShapingBlock4 register */

/* register category retagging_table_reconfiguration_entry */
#define SJA1105_RTRE_WORD_COUNT (2)     /**< Number of data words contained in the retaggingTableReconfigurationEntry register */
#define SJA1105_RTRE_ADDR       (0x31)  /**< Address of the retaggingTableReconfigurationEntry register */

/* register category retagging_table_reconfiguration_control */
#define SJA1105_RTRC_ADDR (0x33)  /**< Address of the retaggingTableReconfigurationControl register */

/* register category general_parameters_block_reconfiguration */
#define SJA1105_GPBR_ADDR (0x34)  /**< Address of the generalParametersBlockReconfiguration register */

/* register category mac_configuration_table_reconfiguration */
#define SJA1105_MAC_CTR_0_ADDR (0x36)  /**< Address of the macConfigurationTableReconfiguration0 register */
#define SJA1105_MAC_CTR_1_ADDR (0x37)  /**< Address of the macConfigurationTableReconfiguration1 register */

/* register category l2_lookup_parameters_control */
#define SJA1105_L2_LPC_ADDR (0x38)  /**< Address of the l2LookupParametersControl register */

/* register category general_status_ptp_egress */
#define SJA1105_GS_PTP_E_PORT_INC (0x2) 
#define SJA1105_GS_PTP_E_ADDR     (0xc0)  /**< Address of the generalStatusPtpEgress register */

/* register category l2_memory_partition_status */
#define SJA1105_L2_MPS_ADDR (0x100)  /**< Address of the l2MemoryPartitionStatus register */

/* register category mac_level_diagnostic_counters */
#define SJA1105_MAC_LDC_PORT_INC (0x2)  
#define SJA1105_MAC_LDC_ADDR     (0x200)  /**< Address of the macLevelDiagnosticCounters register */

/* register category mac_level_diagnostic_flags */
#define SJA1105_MAC_LDF_PORT_INC (0x2)  
#define SJA1105_MAC_LDF_ADDR     (0x201)  /**< Address of the macLevelDiagnosticFlags register */

/* register category high_level_diagnostic_counters_1 */
#define SJA1105_HIGH_LDC_1_PORT_INC     (0x10) 
#define SJA1105_HIGH_LDC_1_0_WORD_COUNT (2)      /**< Number of data words contained in the highLevelDiagnosticCounters10 register */
#define SJA1105_HIGH_LDC_1_0_ADDR       (0x400)  /**< Address of the highLevelDiagnosticCounters10 register */
#define SJA1105_HIGH_LDC_1_1_WORD_COUNT (2)      /**< Number of data words contained in the highLevelDiagnosticCounters11 register */
#define SJA1105_HIGH_LDC_1_1_ADDR       (0x402)  /**< Address of the highLevelDiagnosticCounters11 register */
#define SJA1105_HIGH_LDC_1_2_WORD_COUNT (2)      /**< Number of data words contained in the highLevelDiagnosticCounters12 register */
#define SJA1105_HIGH_LDC_1_2_ADDR       (0x404)  /**< Address of the highLevelDiagnosticCounters12 register */
#define SJA1105_HIGH_LDC_1_3_WORD_COUNT (2)      /**< Number of data words contained in the highLevelDiagnosticCounters13 register */
#define SJA1105_HIGH_LDC_1_3_ADDR       (0x406)  /**< Address of the highLevelDiagnosticCounters13 register */
#define SJA1105_HIGH_LDC_1_4_ADDR       (0x408)  /**< Address of the highLevelDiagnosticCounters14 register */
#define SJA1105_HIGH_LDC_1_5_ADDR       (0x40b)  /**< Address of the highLevelDiagnosticCounters15 register */
#define SJA1105_HIGH_LDC_1_6_ADDR       (0x40c)  /**< Address of the highLevelDiagnosticCounters16 register */
#define SJA1105_HIGH_LDC_1_7_ADDR       (0x40e)  /**< Address of the highLevelDiagnosticCounters17 register */
#define SJA1105_HIGH_LDC_1_8_ADDR       (0x40f)  /**< Address of the highLevelDiagnosticCounters18 register */

/* register category high_level_diagnostic_counters_2 */
#define SJA1105_HIGH_LDC_2_PORT_INC (0x10) 
#define SJA1105_HIGH_LDC_2_0_ADDR   (0x600)  /**< Address of the highLevelDiagnosticCounters20 register */
#define SJA1105_HIGH_LDC_2_1_ADDR   (0x601)  /**< Address of the highLevelDiagnosticCounters21 register */
#define SJA1105_HIGH_LDC_2_2_ADDR   (0x602)  /**< Address of the highLevelDiagnosticCounters22 register */
#define SJA1105_HIGH_LDC_2_3_ADDR   (0x603)  /**< Address of the highLevelDiagnosticCounters23 register */

/* register category l2_memory_partition_error_counters */
#define SJA1105_L2_MPEC_ADDR (0x1000)  /**< Address of the l2MemoryPartitionErrorCounters register */

/* variable masks */

/* register category general_status_device */
#define SJA1105_GSD_1_CONFIGS_SHIFT (31)           /**< Shift value for the CONFIGS field */
#define SJA1105_GSD_1_CRCCHKL_SHIFT (30)           /**< Shift value for the CRCCHKL field */
#define SJA1105_GSD_1_CRCCHKL_MASK  (0x40000000U)  /**< Bit mask for the CRCCHKL field */
#define SJA1105_GSD_1_IDS_SHIFT     (29)           /**< Shift value for the IDS field */
#define SJA1105_GSD_1_IDS_MASK      (0x20000000U)  /**< Bit mask for the IDS field */
#define SJA1105_GSD_1_CRCCHKG_SHIFT (28)           /**< Shift value for the CRCCHKG field */
#define SJA1105_GSD_1_CRCCHKG_MASK  (0x10000000U)  /**< Bit mask for the CRCCHKG field */
#define SJA1105_GSD_1_NSLOT_MASK    (0x0000000FU)  /**< Bit mask for the NSLOT field */

/* register category general_status_l2 */
#define SJA1105_GS_L2_PORTENF_SHIFT     (8)            /**< Shift value for the PORTENF field */
#define SJA1105_GS_L2_FWDS_SHIFT        (4)            /**< Shift value for the FWDS field */
#define SJA1105_GS_L2_FWDS_MASK         (0x00000010U)  /**< Bit mask for the FWDS field */
#define SJA1105_GS_L2_MACFDS_SHIFT      (3)            /**< Shift value for the MACFDS field */
#define SJA1105_GS_L2_MACFDS_MASK       (0x00000008U)  /**< Bit mask for the MACFDS field */
#define SJA1105_GS_L2_ENFFDS_SHIFT      (2)            /**< Shift value for the ENFFDS field */
#define SJA1105_GS_L2_ENFFDS_MASK       (0x00000004U)  /**< Bit mask for the ENFFDS field */
#define SJA1105_GS_L2_L2BUSYFDS_SHIFT   (1)            /**< Shift value for the L2BUSYFDS field */
#define SJA1105_GS_L2_L2BUSYFDS_MASK    (0x00000002U)  /**< Bit mask for the L2BUSYFDS field */
#define SJA1105_GS_L2_L2BUSYS_MASK      (0x00000001U)  /**< Bit mask for the L2BUSYS field */
#define SJA1105_GS_L2_MACADD_SHIFT_0    (16)           /**< Shift value for the MACADD field */
#define SJA1105_GS_L2_MACADD_SHIFT_OVLP (16)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADD field */

/* register category general_status_hashconfs */
#define SJA1105_GSH_VLANIDHC_SHIFT      (4)            /**< Shift value for the VLANIDHC field */
#define SJA1105_GSH_VLANIDHC_MASK       (0x0000FFF0U)  /**< Bit mask for the VLANIDHC field */
#define SJA1105_GSH_HASHCONFS_MASK      (0x00000001U)  /**< Bit mask for the HASHCONFS field */
#define SJA1105_GSH_MACADDHC_SHIFT_0    (16)           /**< Shift value for the MACADDHC field */
#define SJA1105_GSH_MACADDHC_SHIFT_OVLP (16)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADDHC field */

/* register category general_status_vlan */
#define SJA1105_GS_VLAN_WPVLANID_SHIFT   (16)           /**< Shift value for the WPVLANID field */
#define SJA1105_GS_VLAN_PORT_SHIFT       (8)            /**< Shift value for the PORT field */
#define SJA1105_GS_VLAN_VLANBUSYS_SHIFT  (4)            /**< Shift value for the VLANBUSYS field */
#define SJA1105_GS_VLAN_VLANBUSYS_MASK   (0x00000010U)  /**< Bit mask for the VLANBUSYS field */
#define SJA1105_GS_VLAN_WRONGPORTS_SHIFT (3)            /**< Shift value for the WRONGPORTS field */
#define SJA1105_GS_VLAN_WRONGPORTS_MASK  (0x00000008U)  /**< Bit mask for the WRONGPORTS field */
#define SJA1105_GS_VLAN_VNOTFOUNDS_SHIFT (2)            /**< Shift value for the VNOTFOUNDS field */
#define SJA1105_GS_VLAN_VNOTFOUNDS_MASK  (0x00000004U)  /**< Bit mask for the VNOTFOUNDS field */

/* register category general_status_memory */
#define SJA1105_GSM_EMPTYS_SHIFT (31)           /**< Shift value for the EMPTYS field */
#define SJA1105_GSM_BUFFERS_MASK (0x7FFFFFFFU)  /**< Bit mask for the BUFFERS field */

/* register category general_status_drop */
#define SJA1105_GS_DROP_PORT_SHIFT (8)            /**< Shift value for the PORT field */
#define SJA1105_GS_DROP_FWDS_SHIFT (1)            /**< Shift value for the FWDS field */
#define SJA1105_GS_DROP_FWDS_MASK  (0x00000002U)  /**< Bit mask for the FWDS field */
#define SJA1105_GS_DROP_PARTS_MASK (0x00000001U)  /**< Bit mask for the PARTS field */

/* register category general_status_ram_error */
#define SJA1105_GS_RAM_E_RAMPARERR_MASK_0     (0x001FFFFFU)  /**< Bit mask for the RAMPARERR field */
#define SJA1105_GS_RAM_E_RAMPARERR_MASK_1     (0x0000001FU)  /**< Bit mask for the RAMPARERR field */
#define SJA1105_GS_RAM_E_RAMPARERR_SHIFT_OVLP (21)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping RAMPARERR field */

/* register category general_status_ram_init */
#define SJA1105_GS_RAM_INIT_RPARINIT_MASK_0     (0x001FFFFFU)  /**< Bit mask for the RPARINIT field */
#define SJA1105_GS_RAM_INIT_RPARINIT_MASK_1     (0x0000001FU)  /**< Bit mask for the RPARINIT field */
#define SJA1105_GS_RAM_INIT_RPARINIT_SHIFT_OVLP (21)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping RPARINIT field */

/* register category ethernet_port_status_control_reset */
#define SJA1105_E_PORT_SCR_CLEARPORT_MASK (0x0000001FU)  /**< Bit mask for the CLEARPORT field */

/* register category ethernet_port_status_control_transmission */
#define SJA1105_E_PORT_SCT_INHIBITTX_MASK (0x0000001FU)  /**< Bit mask for the INHIBITTX field */

/* register category ptp_control */
#define SJA1105_PTP_C_0_PTPPINST_SHIFT_OVLP  (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPPINST field */
#define SJA1105_PTP_C_2_VALID_SHIFT          (31)           /**< Shift value for the VALID field */
#define SJA1105_PTP_C_2_STARTPTPCP_SHIFT     (28)           /**< Shift value for the STARTPTPCP field */
#define SJA1105_PTP_C_2_STARTPTPCP_MASK      (0x10000000U)  /**< Bit mask for the STARTPTPCP field */
#define SJA1105_PTP_C_2_STOPPTPCP_SHIFT      (27)           /**< Shift value for the STOPPTPCP field */
#define SJA1105_PTP_C_2_STOPPTPCP_MASK       (0x08000000U)  /**< Bit mask for the STOPPTPCP field */
#define SJA1105_PTP_C_2_RESPTP_SHIFT         (2)            /**< Shift value for the RESPTP field */
#define SJA1105_PTP_C_2_RESPTP_MASK          (0x00000004U)  /**< Bit mask for the RESPTP field */
#define SJA1105_PTP_C_2_CORRCLK4TS_SHIFT     (1)            /**< Shift value for the CORRCLK4TS field */
#define SJA1105_PTP_C_2_CORRCLK4TS_MASK      (0x00000002U)  /**< Bit mask for the CORRCLK4TS field */
#define SJA1105_PTP_C_2_PTPCLKADD_MASK       (0x00000001U)  /**< Bit mask for the PTPCLKADD field */
#define SJA1105_PTP_C_3_PTPCLKVAL_SHIFT_OVLP (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPCLKVAL field */
#define SJA1105_PTP_C_5_PTPTSCLK_SHIFT_OVLP  (32)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping PTPTSCLK field */

/* register category l2_address_lookup_table_reconfiguration_entry */
#define SJA1105_L2_ALTRE_INDEX_SHIFT          (20)           /**< Shift value for the INDEX field */
#define SJA1105_L2_ALTRE_INDEX_MASK           (0x3FF00000U)  /**< Bit mask for the INDEX field */
#define SJA1105_L2_ALTRE_ENFPORT_SHIFT        (30)           /**< Shift value for the ENFPORT field */
#define SJA1105_L2_ALTRE_ENFPORT_MASK         (0x40000000U)  /**< Bit mask for the ENFPORT field */
#define SJA1105_L2_ALTRE_DESTPORTS_SHIFT_0    (31)           /**< Shift value for the DESTPORTS field */
#define SJA1105_L2_ALTRE_DESTPORTS_MASK_1     (0x0000000FU)  /**< Bit mask for the DESTPORTS field */
#define SJA1105_L2_ALTRE_DESTPORTS_SHIFT_OVLP (1)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping DESTPORTS field */
#define SJA1105_L2_ALTRE_MACADDR_SHIFT_0      (4)            /**< Shift value for the MACADDR field */
#define SJA1105_L2_ALTRE_MACADDR_MASK_1       (0x000FFFFFU)  /**< Bit mask for the MACADDR field */
#define SJA1105_L2_ALTRE_MACADDR_SHIFT_OVLP   (28)           /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping MACADDR field */
#define SJA1105_L2_ALTRE_VLANID_SHIFT         (20)           /**< Shift value for the VLANID field */

/* register category l2_address_lookup_table_reconfiguration_control */
#define SJA1105_L2_ALTRC_VALID_SHIFT     (31)           /**< Shift value for the VALID field */
#define SJA1105_L2_ALTRC_RDWRSET_SHIFT   (30)           /**< Shift value for the RDWRSET field */
#define SJA1105_L2_ALTRC_RDWRSET_MASK    (0x40000000U)  /**< Bit mask for the RDWRSET field */
#define SJA1105_L2_ALTRC_ERRORS_SHIFT    (29)           /**< Shift value for the ERRORS field */
#define SJA1105_L2_ALTRC_ERRORS_MASK     (0x20000000U)  /**< Bit mask for the ERRORS field */
#define SJA1105_L2_ALTRC_LOCKEDS_SHIFT   (28)           /**< Shift value for the LOCKEDS field */
#define SJA1105_L2_ALTRC_LOCKEDS_MASK    (0x10000000U)  /**< Bit mask for the LOCKEDS field */
#define SJA1105_L2_ALTRC_VALIDENT_SHIFT  (27)           /**< Shift value for the VALIDENT field */
#define SJA1105_L2_ALTRC_VALIDENT_MASK   (0x08000000U)  /**< Bit mask for the VALIDENT field */
#define SJA1105_L2_ALTRC_MGMTROUTE_SHIFT (26)           /**< Shift value for the MGMTROUTE field */
#define SJA1105_L2_ALTRC_MGMTROUTE_MASK  (0x04000000U)  /**< Bit mask for the MGMTROUTE field */

/* register category l2_forwarding_table_reconfiguration_entry */
#define SJA1105_L2_FTRE_VLAN_PMAP_0_SHIFT      (25)           /**< Shift value for the VLAN_PMAP_0 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_0_MASK       (0x0E000000U)  /**< Bit mask for the VLAN_PMAP_0 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_1_SHIFT      (28)           /**< Shift value for the VLAN_PMAP_1 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_1_MASK       (0x70000000U)  /**< Bit mask for the VLAN_PMAP_1 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_2_SHIFT_0    (31)           /**< Shift value for the VLAN_PMAP_2 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_2_MASK_1     (0x00000003U)  /**< Bit mask for the VLAN_PMAP_2 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_2_SHIFT_OVLP (1)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLAN_PMAP_2 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_3_SHIFT      (2)            /**< Shift value for the VLAN_PMAP_3 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_3_MASK       (0x0000001CU)  /**< Bit mask for the VLAN_PMAP_3 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_4_SHIFT      (5)            /**< Shift value for the VLAN_PMAP_4 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_4_MASK       (0x000000E0U)  /**< Bit mask for the VLAN_PMAP_4 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_5_SHIFT      (8)            /**< Shift value for the VLAN_PMAP_5 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_5_MASK       (0x00000700U)  /**< Bit mask for the VLAN_PMAP_5 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_6_SHIFT      (11)           /**< Shift value for the VLAN_PMAP_6 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_6_MASK       (0x00003800U)  /**< Bit mask for the VLAN_PMAP_6 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_7_SHIFT      (14)           /**< Shift value for the VLAN_PMAP_7 field */
#define SJA1105_L2_FTRE_VLAN_PMAP_7_MASK       (0x0001C000U)  /**< Bit mask for the VLAN_PMAP_7 field */
#define SJA1105_L2_FTRE_FL_DOMAIN_SHIFT        (17)           /**< Shift value for the FL_DOMAIN field */
#define SJA1105_L2_FTRE_FL_DOMAIN_MASK         (0x003E0000U)  /**< Bit mask for the FL_DOMAIN field */
#define SJA1105_L2_FTRE_REACH_PORT_SHIFT       (22)           /**< Shift value for the REACH_PORT field */
#define SJA1105_L2_FTRE_REACH_PORT_MASK        (0x07C00000U)  /**< Bit mask for the REACH_PORT field */
#define SJA1105_L2_FTRE_BC_DOMAIN_SHIFT        (27)           /**< Shift value for the BC_DOMAIN field */

/* register category l2_forwarding_table_reconfiguration_control */
#define SJA1105_L2_FTRC_VALID_SHIFT  (31)           /**< Shift value for the VALID field */
#define SJA1105_L2_FTRC_ERRORS_SHIFT (30)           /**< Shift value for the ERRORS field */
#define SJA1105_L2_FTRC_ERRORS_MASK  (0x40000000U)  /**< Bit mask for the ERRORS field */
#define SJA1105_L2_FTRC_INDEX_MASK   (0x0000001FU)  /**< Bit mask for the INDEX field */

/* register category vlan_lookup_table_reconfiguration_entry */
#define SJA1105_VLAN_LTRE_VLANID_SHIFT_0    (27)           /**< Shift value for the VLANID field */
#define SJA1105_VLAN_LTRE_VLANID_MASK_1     (0x0000007FU)  /**< Bit mask for the VLANID field */
#define SJA1105_VLAN_LTRE_VLANID_SHIFT_OVLP (5)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLANID field */
#define SJA1105_VLAN_LTRE_TAG_PORT_SHIFT    (7)            /**< Shift value for the TAG_PORT field */
#define SJA1105_VLAN_LTRE_TAG_PORT_MASK     (0x00000F80U)  /**< Bit mask for the TAG_PORT field */
#define SJA1105_VLAN_LTRE_VLAN_BC_SHIFT     (12)           /**< Shift value for the VLAN_BC field */
#define SJA1105_VLAN_LTRE_VLAN_BC_MASK      (0x0001F000U)  /**< Bit mask for the VLAN_BC field */
#define SJA1105_VLAN_LTRE_VMEMB_PORT_SHIFT  (17)           /**< Shift value for the VMEMB_PORT field */
#define SJA1105_VLAN_LTRE_VMEMB_PORT_MASK   (0x003E0000U)  /**< Bit mask for the VMEMB_PORT field */
#define SJA1105_VLAN_LTRE_VEGR_MIRR_SHIFT   (22)           /**< Shift value for the VEGR_MIRR field */
#define SJA1105_VLAN_LTRE_VEGR_MIRR_MASK    (0x07C00000U)  /**< Bit mask for the VEGR_MIRR field */
#define SJA1105_VLAN_LTRE_VING_MIRR_SHIFT   (27)           /**< Shift value for the VING_MIRR field */

/* register category vlan_lookup_table_reconfiguration_control */
#define SJA1105_VLAN_LTRC_VALID_SHIFT    (31)           /**< Shift value for the VALID field */
#define SJA1105_VLAN_LTRC_VALIDENT_SHIFT (27)           /**< Shift value for the VALIDENT field */
#define SJA1105_VLAN_LTRC_VALIDENT_MASK  (0x08000000U)  /**< Bit mask for the VALIDENT field */

/* register category credit_based_shaping_block */
#define SJA1105_CBSB_4_VALID_SHIFT     (31)           /**< Shift value for the VALID field */
#define SJA1105_CBSB_4_SHAPER_ID_SHIFT (16)           /**< Shift value for the SHAPER_ID field */
#define SJA1105_CBSB_4_SHAPER_ID_MASK  (0x000F0000U)  /**< Bit mask for the SHAPER_ID field */
#define SJA1105_CBSB_4_CBS_PORT_SHIFT  (3)            /**< Shift value for the CBS_PORT field */
#define SJA1105_CBSB_4_CBS_PORT_MASK   (0x00000038U)  /**< Bit mask for the CBS_PORT field */
#define SJA1105_CBSB_4_CBS_PRIO_MASK   (0x00000007U)  /**< Bit mask for the CBS_PRIO field */

/* register category retagging_table_reconfiguration_entry */
#define SJA1105_RTRE_DESTPORTS_SHIFT     (23)           /**< Shift value for the DESTPORTS field */
#define SJA1105_RTRE_DESTPORTS_MASK      (0x0F800000U)  /**< Bit mask for the DESTPORTS field */
#define SJA1105_RTRE_USE_DESTPORTS_SHIFT (28)           /**< Shift value for the USE_DESTPORTS field */
#define SJA1105_RTRE_USE_DESTPORTS_MASK  (0x10000000U)  /**< Bit mask for the USE_DESTPORTS field */
#define SJA1105_RTRE_DO_NOT_LEARN_SHIFT  (29)           /**< Shift value for the DO_NOT_LEARN field */
#define SJA1105_RTRE_DO_NOT_LEARN_MASK   (0x20000000U)  /**< Bit mask for the DO_NOT_LEARN field */
#define SJA1105_RTRE_VLAN_EGR_SHIFT_0    (30)           /**< Shift value for the VLAN_EGR field */
#define SJA1105_RTRE_VLAN_EGR_MASK_1     (0x000003FFU)  /**< Bit mask for the VLAN_EGR field */
#define SJA1105_RTRE_VLAN_EGR_SHIFT_OVLP (2)            /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping VLAN_EGR field */
#define SJA1105_RTRE_VLAN_ING_SHIFT      (10)           /**< Shift value for the VLAN_ING field */
#define SJA1105_RTRE_VLAN_ING_MASK       (0x003FFC00U)  /**< Bit mask for the VLAN_ING field */
#define SJA1105_RTRE_ING_PORT_SHIFT      (22)           /**< Shift value for the ING_PORT field */
#define SJA1105_RTRE_ING_PORT_MASK       (0x07C00000U)  /**< Bit mask for the ING_PORT field */
#define SJA1105_RTRE_EGR_PORT_SHIFT      (27)           /**< Shift value for the EGR_PORT field */

/* register category retagging_table_reconfiguration_control */
#define SJA1105_RTRC_VALID_SHIFT    (31)           /**< Shift value for the VALID field */
#define SJA1105_RTRC_ERRORS_SHIFT   (30)           /**< Shift value for the ERRORS field */
#define SJA1105_RTRC_ERRORS_MASK    (0x40000000U)  /**< Bit mask for the ERRORS field */
#define SJA1105_RTRC_VALIDENT_SHIFT (29)           /**< Shift value for the VALIDENT field */
#define SJA1105_RTRC_VALIDENT_MASK  (0x20000000U)  /**< Bit mask for the VALIDENT field */
#define SJA1105_RTRC_INDEX_MASK     (0x0000003FU)  /**< Bit mask for the INDEX field */

/* register category general_parameters_block_reconfiguration */
#define SJA1105_GPBR_VALID_SHIFT  (31)           /**< Shift value for the VALID field */
#define SJA1105_GPBR_ERRORS_SHIFT (30)           /**< Shift value for the ERRORS field */
#define SJA1105_GPBR_ERRORS_MASK  (0x40000000U)  /**< Bit mask for the ERRORS field */
#define SJA1105_GPBR_MIRRORP_MASK (0x00000007U)  /**< Bit mask for the MIRRORP field */

/* register category mac_configuration_table_reconfiguration */
#define SJA1105_MAC_CTR_0_TPDELIN_SHIFT   (16)           /**< Shift value for the TPDELIN field */
#define SJA1105_MAC_CTR_1_VALID_SHIFT     (31)           /**< Shift value for the VALID field */
#define SJA1105_MAC_CTR_1_SPEED_SHIFT     (29)           /**< Shift value for the SPEED field */
#define SJA1105_MAC_CTR_1_SPEED_MASK      (0x60000000U)  /**< Bit mask for the SPEED field */
#define SJA1105_MAC_CTR_1_PORT_SHIFT      (24)           /**< Shift value for the PORT field */
#define SJA1105_MAC_CTR_1_PORT_MASK       (0x07000000U)  /**< Bit mask for the PORT field */
#define SJA1105_MAC_CTR_1_DRPDTAG_SHIFT   (23)           /**< Shift value for the DRPDTAG field */
#define SJA1105_MAC_CTR_1_DRPDTAG_MASK    (0x00800000U)  /**< Bit mask for the DRPDTAG field */
#define SJA1105_MAC_CTR_1_DRPUNTAG_SHIFT  (22)           /**< Shift value for the DRPUNTAG field */
#define SJA1105_MAC_CTR_1_DRPUNTAG_MASK   (0x00400000U)  /**< Bit mask for the DRPUNTAG field */
#define SJA1105_MAC_CTR_1_RETAG_SHIFT     (21)           /**< Shift value for the RETAG field */
#define SJA1105_MAC_CTR_1_RETAG_MASK      (0x00200000U)  /**< Bit mask for the RETAG field */
#define SJA1105_MAC_CTR_1_DYN_LEARN_SHIFT (20)           /**< Shift value for the DYN_LEARN field */
#define SJA1105_MAC_CTR_1_DYN_LEARN_MASK  (0x00100000U)  /**< Bit mask for the DYN_LEARN field */
#define SJA1105_MAC_CTR_1_EGRESS_SHIFT    (19)           /**< Shift value for the EGRESS field */
#define SJA1105_MAC_CTR_1_EGRESS_MASK     (0x00080000U)  /**< Bit mask for the EGRESS field */
#define SJA1105_MAC_CTR_1_INGRESS_SHIFT   (18)           /**< Shift value for the INGRESS field */
#define SJA1105_MAC_CTR_1_INGRESS_MASK    (0x00040000U)  /**< Bit mask for the INGRESS field */
#define SJA1105_MAC_CTR_1_INGMIRR_SHIFT   (17)           /**< Shift value for the INGMIRR field */
#define SJA1105_MAC_CTR_1_INGMIRR_MASK    (0x00020000U)  /**< Bit mask for the INGMIRR field */
#define SJA1105_MAC_CTR_1_EGRMIRR_SHIFT   (16)           /**< Shift value for the EGRMIRR field */
#define SJA1105_MAC_CTR_1_EGRMIRR_MASK    (0x00010000U)  /**< Bit mask for the EGRMIRR field */
#define SJA1105_MAC_CTR_1_VLANPRIO_SHIFT  (12)           /**< Shift value for the VLANPRIO field */
#define SJA1105_MAC_CTR_1_VLANPRIO_MASK   (0x00007000U)  /**< Bit mask for the VLANPRIO field */
#define SJA1105_MAC_CTR_1_VLANID_MASK     (0x00000FFFU)  /**< Bit mask for the VLANID field */

/* register category l2_lookup_parameters_control */
#define SJA1105_L2_LPC_VALID_SHIFT (31)  /**< Shift value for the VALID field */

/* register category general_status_ptp_egress */
#define SJA1105_GS_PTP_E_PTPEGR_TS_SHIFT (8)            /**< Shift value for the PTPEGR_TS field */
#define SJA1105_GS_PTP_E_UPDATE_MASK     (0x00000001U)  /**< Bit mask for the UPDATE field */

/* register category l2_memory_partition_status */
#define SJA1105_L2_MPS_L2PARTS_SHIFT (31)           /**< Shift value for the L2PARTS field */
#define SJA1105_L2_MPS_N_L2PSPC_MASK (0x7FFFFFFFU)  /**< Bit mask for the N_L2PSPC field */

/* register category mac_level_diagnostic_counters */
#define SJA1105_MAC_LDC_N_RUNT_SHIFT     (24)  /**< Shift value for the N_RUNT field */
#define SJA1105_MAC_LDC_N_SOFERR_SHIFT   (16)  /**< Shift value for the N_SOFERR field */
#define SJA1105_MAC_LDC_N_ALIGNERR_SHIFT (8)   /**< Shift value for the N_ALIGNERR field */

/* register category mac_level_diagnostic_flags */
#define SJA1105_MAC_LDF_SPCPRIOR_SHIFT   (12)           /**< Shift value for the SPCPRIOR field */
#define SJA1105_MAC_LDF_SPCPRIOR_MASK    (0x0000F000U)  /**< Bit mask for the SPCPRIOR field */
#define SJA1105_MAC_LDF_PORTDROPS_SHIFT  (6)            /**< Shift value for the PORTDROPS field */
#define SJA1105_MAC_LDF_PORTDROPS_MASK   (0x00000040U)  /**< Bit mask for the PORTDROPS field */
#define SJA1105_MAC_LDF_LENDROPS_SHIFT   (5)            /**< Shift value for the LENDROPS field */
#define SJA1105_MAC_LDF_LENDROPS_MASK    (0x00000020U)  /**< Bit mask for the LENDROPS field */
#define SJA1105_MAC_LDF_BWDROPS_SHIFT    (4)            /**< Shift value for the BWDROPS field */
#define SJA1105_MAC_LDF_BWDROPS_MASK     (0x00000010U)  /**< Bit mask for the BWDROPS field */
#define SJA1105_MAC_LDF_DRN664ERRS_SHIFT (2)            /**< Shift value for the DRN664ERRS field */
#define SJA1105_MAC_LDF_DRN664ERRS_MASK  (0x00000004U)  /**< Bit mask for the DRN664ERRS field */
#define SJA1105_MAC_LDF_SPCERRS_SHIFT    (1)            /**< Shift value for the SPCERRS field */
#define SJA1105_MAC_LDF_SPCERRS_MASK     (0x00000002U)  /**< Bit mask for the SPCERRS field */

/* register category high_level_diagnostic_counters_1 */
#define SJA1105_HIGH_LDC_1_0_N_TXBYTE_SHIFT_OVLP (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_TXBYTE field */
#define SJA1105_HIGH_LDC_1_1_N_TXFRM_SHIFT_OVLP  (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_TXFRM field */
#define SJA1105_HIGH_LDC_1_2_N_RXBYTE_SHIFT_OVLP (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_RXBYTE field */
#define SJA1105_HIGH_LDC_1_3_N_RXFRM_SHIFT_OVLP  (32)  /**< Shift value for the to compensate for the alignment of the 2. part of the overlapping N_RXFRM field */

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	e_switchSpeed_10_MBPS  = 3,  /**< 10 Mbps */
	e_switchSpeed_100_MBPS = 2,  /**< 100 Mbps */
	e_switchSpeed_1_GBPS   = 1,  /**< 1 Gbps */
	e_switchSpeed_DISABLED = 0   /**< Port disabled */
} SJA1105_switchSpeed_t;

typedef struct
{
	uint8_t configs;  /**< device configuration status: {0: invalid, 1: valid} */
	uint8_t crcchkl;  /**< local CRC check: {0: check ok, 1: check failed} */
	uint8_t ids;      /**< device identifier flag: {0: matching device identifier found, 1: matching device identifier not found} */
	uint8_t crcchkg;  /**< global CRC check: {0: check ok, 1: check failed} */
	uint8_t nslot;    /**< free-running 0 to 9 counter used for internal processing. It is intended to be used for debugging */
} SJA1105_generalStatusDevice1Argument_t;

typedef struct
{
	uint8_t  portenf;    /**< Number to the port that triggered ENFFDS */
	uint8_t  fwds;       /**< Forwarding frame drop status: {0: no dropped frames; 1: frame dropped because input port was not set to Forwarding when frame received} */
	uint8_t  macfds;     /**< Standard MAC frame drop status: {0: no dropped frames; 1: frame dropped because it contained a filtered MAC address when the switch is not configured to forward such traffic} */
	uint8_t  enffds;     /**< Enforced frame drop status: {0: no dropped frames: frame received at PORTENF dropped because it carried a source MAC address declared to be enforced on a different port in the L2 Address Lookup table} */
	uint8_t  l2busyfds;  /**< L2 Address Lookup table busy/frame drop status: {0: not busy; no dropped frames; 1: frame received at PORTENF dropped because it was received while L2 Address Lookup table was being initialized} */
	uint8_t  l2busys;    /**< L2 Address Lookup table status: {0: L2 Address Lookup table initialization complete; 1: L2 Address Lookup is being initialized} */
	uint64_t macadd;     /**< Source MAC address that triggered ENFFDS */
} SJA1105_generalStatusL2Argument_t;

typedef struct
{
	uint16_t vlanidhc;   /**< VLAN ID that triggered HASHCONFS */
	uint8_t  hashconfs;  /**< Hash conflict status: {0: No hash conflict encountered; 1: unresolved hash conflict in L2 Address Lookup table} */
	uint64_t macaddhc;   /**< source MAC address that triggered HASHCONFS */
} SJA1105_generalStatusHashconfsArgument_t;

typedef struct
{
	uint16_t wpvlanid;    /**< VLAN ID that triggered WRONGPORTS or VNOTFOUNDS */
	uint8_t  port;        /**< Input port number that triggered WRONGPORTS or VNOTFOUNDS. Contains valid data when one of these flags is set. */
	uint8_t  vlanbusys;   /**< VLAN Lookup table status: {0: VLAN Lookup table initialization complete: 1: VLAN Lookup table is being initialized} */
	uint8_t  wrongports;  /**< Port status for VLAN frame: {0: Frame/port status OK; 1: Frame received at PORT dropped because the port is not configured for the VLAN ID in the VLAN Lookup table} */
	uint8_t  vnotfounds;  /**< Port/VLAN ID status: {0: Frame OK; 1: Frame received at port PORT containing the VLAN ID dropped because the VLAN ID is not configured in the VLAN Lookup table} */
} SJA1105_generalStatusVlanArgument_t;

typedef struct
{
	uint8_t  emptys;   /**< dynamic memory status: {0: ok; 1: out of memory} */
	uint32_t buffers;  /**< Number of frame buffers available to dynamic memory management */
} SJA1105_generalStatusMemoryArgument_t;

typedef struct
{
	uint8_t port;   /**< Port number where frame dropped if FWDS or PARTS is set */
	uint8_t fwds;   /**< Port forwarding status: {0: No dropped frames, 1: Port identified by PORT sourced a fame that was dropped because the configured forwarding direction did not contain any ports} */
	uint8_t parts;  /**< Port forwarding status: {0: No dropped frames, 1: Port identified by PORT sourced a fame that was dropped because the configured forwarding direction did not contain any ports} */
} SJA1105_generalStatusDropArgument_t;

typedef struct
{
	uint8_t valid;       /**< Setting this flag in combination with STARTPTPCP or STOPPTPCP dynamically changes the behavior of the schedule running based on the PTP time and the external PTP_CLIK pin */
	uint8_t startptpcp;  /**< Setting this flag in combination with VALID triggers the switch to begin toggling the external PTP_CLK pin at a rate of PTPPINDUR when the synchcronized PTP clock exceeds the value of PTPPINST */
	uint8_t stopptpcp;   /**< Setting this flag in combination with VALID triggers the switch to stop toggling the external PTP_CLK pin. */
	uint8_t resptp;      /**< Asserting this flag in combination with VALID will cause PTPCLK, PTPTSCLK and PTPCLKRATE to be re-set to their power-on defaults. */
	uint8_t corrclk4ts;  /**< Asserting this flag in combination with VALID will cause subsequent timestamps on ingress and egress management frames to be taken based on PTPCLK. If the flag is de-asserted, timestamps will be taken based on PTPTSCLK */
	uint8_t ptpclkadd;   /**< Asserting this flag in combination with VALID will cause subsequent writes to PTPCLKVAL to be added to the clock rather than setting a new value. */
} SJA1105_ptpControl2Argument_t;

typedef struct
{
	uint16_t index;      /**< Contains the address in physical memory where this entry will be stored */
	uint8_t  enfport;    /**< If this flag is set, MACADDR will be enforced as the source MAC address on ports having their flag set in DESTPORTS */
	uint8_t  destports;  /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
	uint64_t macaddr;    /**< The MAC address associated with this entry */
	uint16_t vlanid;     /**< The VLAN ID associated with this entry. VLANID is only included in the lookup process if SHARED_LEARN in Table 7 is cleared, otherwise this parameter is ignored. */
} SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t;

typedef struct
{
	uint8_t valid;      /**< This flag to triggers a dynamic change in the contents of the L2 Address Lookup table (if RDWRSET is set) or a read access (when RDWRSET is cleared) */
	uint8_t rdwrset;    /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t valident;   /**< This flag determines if the respective entry should be marked valid. Marking an entry as invalid (clearing VALIDENT) has the effect, that the entry at the respective position will be available for address learning again. Not applicable for management routes. */
	uint8_t mgmtroute;  /**< This flag to indicates that the request is targeted for a management route entry. */
} SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t;

typedef struct
{
	uint8_t valid;      /**< This flag to triggers a dynamic change in the contents of the L2 Address Lookup table (if RDWRSET is set) or a read access (when RDWRSET is cleared) */
	uint8_t rdwrset;    /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t errors;     /**< This flag should be read cleared when the VALID flag is set. If it is found to be set while the VALID flag is set, the most recent access resulted in an error. */
	uint8_t lockeds;    /**< This flag should be read cleared when the VALID flag is set. The flag will also be found cleared in response to a read having the MGMTROUTE flag set. In response to a read with the MGMTROUTE flag cleared, the flag be set if the most recent access operated on an entry that was either loaded by configuration or through dynamic reconfiguration */
	uint8_t valident;   /**< This flag determines if the respective entry should be marked valid. Marking an entry as invalid (clearing VALIDENT) has the effect, that the entry at the respective position will be available for address learning again. Not applicable for management routes. */
	uint8_t mgmtroute;  /**< This flag to indicates that the request is targeted for a management route entry. */
} SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t;

typedef struct
{
	uint8_t vlanPmap0;
	uint8_t vlanPmap1;
	uint8_t vlanPmap2;
	uint8_t vlanPmap3;
	uint8_t vlanPmap4;
	uint8_t vlanPmap5;
	uint8_t vlanPmap6;
	uint8_t vlanPmap7;
	uint8_t flDomain;   /**< Only valid for the first five entries in the table. Defines the destination ports of unknown traffic at the port associated with this entry. Each port is assigned a bit in this field with the LSB mapping to port 0. */
	uint8_t reachPort;  /**< Only valid for the first five entries in the table. Defines the ports that can be reached by traffic received on the port associated with the entry. Each port is assigned a bit in this field with the LSB mapping to port 0. */
	uint8_t bcDomain;   /**< Only valid for the first five entries in the table. Defines the broadcast domain of the port associated with the entry. Each port is assigned a bit in this field with the LSB mapping to port 0. */
} SJA1105_l2ForwardingTableReconfigurationEntryArgument_t;

typedef struct
{
	uint8_t valid;  /**< The host sets this flag to trigger a dynamic change in the entry indicated by INDEX */
	uint8_t index;  /**< Contains the index of the entry being dynamically reconfigured. */
} SJA1105_l2ForwardingTableReconfigurationControlArgument_t;

typedef struct
{
	uint16_t vlanid;     /**< The VLAN ID associated with this entry */
	uint8_t  tagPort;    /**< Defines the set of ports on which a frame tagged with the respective VLAN ID may be received. All bits must be set in order to deactivate VLAN-based ingress port policing. */
	uint8_t  vlanBc;     /**< This field restricts the broadcast domain of the specific VLAN. If a bit is cleared, the respective port cannot be reached by a frame tagged with the specific VLAN ID. All bits have to be set to de-activate reachability limitations for certain VLANs. */
	uint8_t  vmembPort;  /**< Defines if a frame associated with the respective VLAN ID shall be sent un-tagged. */
	uint8_t  vegrMirr;   /**< All traffic tagged with VLANID and forwarded to any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
	uint8_t  vingMirr;   /**< All traffic tagged with VLANID and received on any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
} SJA1105_vlanLookupTableReconfigurationEntryArgument_t;

typedef struct
{
	uint8_t valid;     /**< The host sets this flag to trigger a dynamic change in the entry associated with the VLANID field. */
	uint8_t valident;  /**< In case of a write access, this flag determines if the respective entry should be marked valid */
} SJA1105_vlanLookupTableReconfigurationControlArgument_t;

typedef struct
{
	uint8_t valid;     /**< When this flag is set, the host triggers a dynamic change of the entry with index SHAPER_ID. */
	uint8_t shaperId;  /**< Index of the credit-based shaper which is subject to dynamic reconfiguration */
	uint8_t cbsPort;   /**< Port to which the credit-based shaper is assigned */
	uint8_t cbsPrio;   /**< Priority queue to which the credit-based shaper is assigned */
} SJA1105_creditBasedShapingBlock4Argument_t;

typedef struct
{
	uint8_t  destports;     /**< This field provides a dedicated forwarding for all frames carrying a VLAN ID that matches VLAN_EGR of the entry */
	uint8_t  useDestports;  /**< If this flag is asserted, DESTPORTS of the entry will be used to forward all frames carrying a VLAN ID that matches VLAN_EGR of the entry. */
	uint8_t  doNotLearn;    /**< If this flag is asserted, address learning will be disabled for all frames carrying a VLAN ID that matches VLAN_EGR of the entry. */
	uint16_t vlanEgr;       /**< This VLAN ID will replace the VLAN ID of the original frame at egress. */
	uint16_t vlanIng;       /**< The VLAN ID of the ingress frame */
	uint8_t  ingPort;       /**< If a frame with VLAN ID VLAN_ING is received on any port having its flag set in this field, a copy with VLAN ID VLAN_EGR will be generated */
	uint8_t  egrPort;       /**< If a frame with VLAN ID VLAN_ING is forwarded to any port having its flag set in this field, a copy with VLAN ID VLAN_EGR will be generated. */
} SJA1105_retaggingTableReconfigurationEntryArgument_t;

typedef struct
{
	uint8_t valid;     /**< The host sets this flag to trigger a dynamic change to the entry with index INDEX. If this flag is found set on read, the switch is still busy processing the most recent update request. */
	uint8_t valident;  /**< Indicates that the entry at position INDEX should be enabled. */
	uint8_t index;     /**< Contains the index of the entry being dynamically reconfigured. */
} SJA1105_retaggingTableReconfigurationControlArgument_t;

typedef struct
{
	uint8_t valid;    /**< Triggers a reconfiguration of the general parameters block */
	uint8_t mirrorp;  /**< Contains the current mirror port setting. A value greater than 4 will turn mirroring off. */
} SJA1105_generalParametersBlockReconfigurationArgument_t;

typedef struct
{
	uint16_t tpdelin;   /**< Used to set a correction for updating the transparent clock of IEEE 1588v2 1-step event messages at the input port in multiples of 8 ns */
	uint16_t tpdelout;  /**< Used to set a correction for updating the transparent clock of IEEE 1588v2 1-step event messages at the output port in multiples of 8 ns */
} SJA1105_macConfigurationTableReconfiguration0Argument_t;

typedef struct
{
	uint8_t               valid;     /**< Flag to trigger a dynamic change to the contents of the MAC configuration table */
	SJA1105_switchSpeed_t speed;     /**< Used to set the port speed {11: 10 Mbps, 10: 100 Mbps, 01: 1 Gbps, 00 disabled} */
	uint8_t               port;      /**< Specifies the port that is affected by this dynamic reconfiguration {0:4} */
	uint8_t               drpdtag;   /**< If this flag is set, double-tagged ingress traffic will be dropped at the respective port. */
	uint8_t               drpuntag;  /**< If this flag is set, untagged ingress traffic will be dropped at the respective port. */
	uint8_t               retag;     /**< Enables retagging (using VLANID configured for the respective port but maintaining the priority value) of priority-tagged input on the respective port. */
	uint8_t               dynLearn;  /**< Enables address learning at the respective port when set. Note that learning is independent of whether input traffic is enabled */
	uint8_t               egress;    /**< Enables output on the respective port when set */
	uint8_t               ingress;   /**< Enables input on the respective port when set */
	uint8_t               ingmirr;   /**< If this flag is set, all traffic received on this port will be forwarded to the mirror port */
	uint8_t               egrmirr;   /**< If this flag is set, all traffic forwarded to this port except for locally generated PCFs willbe forwarded to the mirror port */
	uint8_t               vlanprio;  /**< Defines the IEEE 802.1Q VLAN priority level that is used for tagging un-tagged incoming frames */
	uint16_t              vlanid;    /**< Contains the IEEE 802.1Q VLAN ID that is used for tagging un-tagged incoming frames */
} SJA1105_macConfigurationTableReconfiguration1Argument_t;

typedef struct
{
	uint8_t valid;  /**< By writing a value that has this flag set, the host triggers an update. */
	uint8_t poly;   /**< On write contains the new value for CRC polynomial. Will return the polynomial currently used if VALID is found deasserted */
} SJA1105_l2LookupParametersControlArgument_t;

typedef struct
{
	uint32_t ptpegrTs;  /**< PTP egress timestamp: contains the PTP egress timestamp of the most recent management frame for which the user specified an egress timestamp to be captured */
	uint8_t  update;    /**< Set if value PTPEGR_TS has changed */
} SJA1105_generalStatusPtpEgressArgument_t;

typedef struct
{
	uint8_t  l2parts;  /**< switch memory partition status for Ethernet traffic: {0: No memory error; 1: The respective memory partition suffered at least one memory error after the previous read access to this field} */
	uint32_t nL2pspc;  /**< Number of frames left for the respective L2 memory partition at the time of the read access. */
} SJA1105_l2MemoryPartitionStatusArgument_t;

typedef struct
{
	uint8_t nRunt;      /**< Number of frames that do not have a SOF, alignment or MII error, but are shorter than 64 bytes. The counter will not wrap. */
	uint8_t nSoferr;    /**< Number of frames that have a SOF error. The field will not wrap. */
	uint8_t nAlignerr;  /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but whose length is not a multiple of eight bits. The field will not wrap. */
	uint8_t nMiierr;    /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but terminated with the MII error input being asserted. The field will not wrap. */
} SJA1105_macLevelDiagnosticCountersArgument_t;

typedef struct
{
	uint8_t spcprior;    /**< Index of the priority queue that hosted the frame dropped due to a space error. */
	uint8_t portdrops;   /**< Indicates that the respective port had a frame dropped because the port has not been enabled for traffic in the L2 Policing table. */
	uint8_t lendrops;    /**< Indicates that the respective port had a frame dropped because the frame was longer than defined in the L2 Policing table. */
	uint8_t bwdrops;     /**< Indicates that the respective port had a frame dropped because there was no bandwidth left on the port as defined in the L2 Policing table. */
	uint8_t drn664errs;  /**< Indicates that a frame was dropped at the respective port because its EtherType field contained a value other than 0x800 while the DRPNONA664 flag of the MAC Configuration block is set. */
	uint8_t spcerrs;     /**< Indicates that a frame was dropped at the respective port because the respective priority queue as defined by the BASE and TOP parameters did not have any space left or is de-activated. If the flag is set, SPCPRIOR will contain the index of the priority queue that hosted the dropped frame. */
} SJA1105_macLevelDiagnosticFlagsArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category general_status_device */
extern uint8_t SJA1105_getGeneralStatusDevice0(uint32_t *p_id, uint8_t deviceSelect);
extern uint8_t SJA1105_getGeneralStatusDevice1(SJA1105_generalStatusDevice1Argument_t *p_generalStatusDevice1, uint8_t deviceSelect);

/* register category general_status_l2 */
extern uint8_t SJA1105_getGeneralStatusL2(SJA1105_generalStatusL2Argument_t *p_generalStatusL2, uint8_t deviceSelect);

/* register category general_status_hashconfs */
extern uint8_t SJA1105_getGeneralStatusHashconfs(SJA1105_generalStatusHashconfsArgument_t *p_generalStatusHashconfs, uint8_t deviceSelect);

/* register category general_status_vlan */
extern uint8_t SJA1105_getGeneralStatusVlan(SJA1105_generalStatusVlanArgument_t *p_generalStatusVlan, uint8_t deviceSelect);

/* register category general_status_memory */
extern uint8_t SJA1105_getGeneralStatusMemory(SJA1105_generalStatusMemoryArgument_t *p_generalStatusMemory, uint8_t deviceSelect);

/* register category general_status_drop */
extern uint8_t SJA1105_getGeneralStatusDrop(SJA1105_generalStatusDropArgument_t *p_generalStatusDrop, uint8_t deviceSelect);

/* register category general_status_ram_error */
extern uint8_t SJA1105_getGeneralStatusRamError(uint32_t *p_ramparerr, uint8_t deviceSelect);

/* register category general_status_ram_init */
extern uint8_t SJA1105_setGeneralStatusRamInit(uint32_t rparinit, uint8_t deviceSelect);
extern uint8_t SJA1105_getGeneralStatusRamInit(uint32_t *p_rparinit, uint8_t deviceSelect);

/* register category ethernet_port_status_control_reset */
extern uint8_t SJA1105_setEthernetPortStatusControlReset(uint8_t clearport, uint8_t deviceSelect);

/* register category ethernet_port_status_control_transmission */
extern uint8_t SJA1105_setEthernetPortStatusControlTransmission(uint8_t inhibittx, uint8_t deviceSelect);

/* register category ptp_control */
extern uint8_t SJA1105_setPtpControl0(uint64_t ptppinst, uint8_t deviceSelect);
extern uint8_t SJA1105_setPtpControl1(uint32_t ptppindur, uint8_t deviceSelect);
extern uint8_t SJA1105_setPtpControl2(const SJA1105_ptpControl2Argument_t *pk_ptpControl2, uint8_t deviceSelect);
extern uint8_t SJA1105_setPtpControl3(uint64_t ptpclkval, uint8_t deviceSelect);
extern uint8_t SJA1105_getPtpControl3(uint64_t *p_ptpclkval, uint8_t deviceSelect);
extern uint8_t SJA1105_setPtpControl4(uint32_t ptpclkrate, uint8_t deviceSelect);
extern uint8_t SJA1105_getPtpControl5(uint64_t *p_ptptsclk, uint8_t deviceSelect);

/* register category l2_address_lookup_table_reconfiguration_entry */
extern uint8_t SJA1105_setL2AddressLookupTableReconfigurationEntry(const SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *pk_l2AddressLookupTableReconfigurationEntry, uint8_t deviceSelect);
extern uint8_t SJA1105_getL2AddressLookupTableReconfigurationEntry(SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *p_l2AddressLookupTableReconfigurationEntry, uint8_t deviceSelect);

/* register category l2_address_lookup_table_reconfiguration_control */
extern uint8_t SJA1105_setL2AddressLookupTableReconfigurationControl(const SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t *pk_l2AddressLookupTableReconfigurationControl, uint8_t deviceSelect);
extern uint8_t SJA1105_getL2AddressLookupTableReconfigurationControl(SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t *p_l2AddressLookupTableReconfigurationControl, uint8_t deviceSelect);

/* register category l2_forwarding_table_reconfiguration_entry */
extern uint8_t SJA1105_setL2ForwardingTableReconfigurationEntry(const SJA1105_l2ForwardingTableReconfigurationEntryArgument_t *pk_l2ForwardingTableReconfigurationEntry, uint8_t deviceSelect);

/* register category l2_forwarding_table_reconfiguration_control */
extern uint8_t SJA1105_setL2ForwardingTableReconfigurationControl(const SJA1105_l2ForwardingTableReconfigurationControlArgument_t *pk_l2ForwardingTableReconfigurationControl, uint8_t deviceSelect);
extern uint8_t SJA1105_getL2ForwardingTableReconfigurationControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category vlan_lookup_table_reconfiguration_entry */
extern uint8_t SJA1105_setVlanLookupTableReconfigurationEntry(const SJA1105_vlanLookupTableReconfigurationEntryArgument_t *pk_vlanLookupTableReconfigurationEntry, uint8_t deviceSelect);

/* register category vlan_lookup_table_reconfiguration_control */
extern uint8_t SJA1105_setVlanLookupTableReconfigurationControl(const SJA1105_vlanLookupTableReconfigurationControlArgument_t *pk_vlanLookupTableReconfigurationControl, uint8_t deviceSelect);

/* register category credit_based_shaping_block */
extern uint8_t SJA1105_setCreditBasedShapingBlock0(uint32_t idleSlope, uint8_t deviceSelect);
extern uint8_t SJA1105_setCreditBasedShapingBlock1(uint32_t sendSlope, uint8_t deviceSelect);
extern uint8_t SJA1105_setCreditBasedShapingBlock2(uint32_t creditHi, uint8_t deviceSelect);
extern uint8_t SJA1105_setCreditBasedShapingBlock3(uint32_t creditLo, uint8_t deviceSelect);
extern uint8_t SJA1105_setCreditBasedShapingBlock4(const SJA1105_creditBasedShapingBlock4Argument_t *pk_creditBasedShapingBlock4, uint8_t deviceSelect);

/* register category retagging_table_reconfiguration_entry */
extern uint8_t SJA1105_setRetaggingTableReconfigurationEntry(const SJA1105_retaggingTableReconfigurationEntryArgument_t *pk_retaggingTableReconfigurationEntry, uint8_t deviceSelect);

/* register category retagging_table_reconfiguration_control */
extern uint8_t SJA1105_setRetaggingTableReconfigurationControl(const SJA1105_retaggingTableReconfigurationControlArgument_t *pk_retaggingTableReconfigurationControl, uint8_t deviceSelect);
extern uint8_t SJA1105_getRetaggingTableReconfigurationControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category general_parameters_block_reconfiguration */
extern uint8_t SJA1105_setGeneralParametersBlockReconfiguration(const SJA1105_generalParametersBlockReconfigurationArgument_t *pk_generalParametersBlockReconfiguration, uint8_t deviceSelect);
extern uint8_t SJA1105_getGeneralParametersBlockReconfiguration(uint8_t *p_errors, uint8_t deviceSelect);

/* register category mac_configuration_table_reconfiguration */
extern uint8_t SJA1105_setMacConfigurationTableReconfiguration0(const SJA1105_macConfigurationTableReconfiguration0Argument_t *pk_macConfigurationTableReconfiguration0, uint8_t deviceSelect);
extern uint8_t SJA1105_setMacConfigurationTableReconfiguration1(const SJA1105_macConfigurationTableReconfiguration1Argument_t *pk_macConfigurationTableReconfiguration1, uint8_t deviceSelect);

/* register category l2_lookup_parameters_control */
extern uint8_t SJA1105_setL2LookupParametersControl(const SJA1105_l2LookupParametersControlArgument_t *pk_l2LookupParametersControl, uint8_t deviceSelect);
extern uint8_t SJA1105_getL2LookupParametersControl(SJA1105_l2LookupParametersControlArgument_t *p_l2LookupParametersControl, uint8_t deviceSelect);

/* register category general_status_ptp_egress */
extern uint8_t SJA1105_getGeneralStatusPtpEgress(SJA1105_generalStatusPtpEgressArgument_t *p_generalStatusPtpEgress, uint8_t port, uint8_t timestamp, uint8_t deviceSelect);

/* register category l2_memory_partition_status */
extern uint8_t SJA1105_getL2MemoryPartitionStatus(SJA1105_l2MemoryPartitionStatusArgument_t *p_l2MemoryPartitionStatus, uint8_t partition, uint8_t deviceSelect);

/* register category mac_level_diagnostic_counters */
extern uint8_t SJA1105_getMacLevelDiagnosticCounters(SJA1105_macLevelDiagnosticCountersArgument_t *p_macLevelDiagnosticCounters, uint8_t port, uint8_t deviceSelect);

/* register category mac_level_diagnostic_flags */
extern uint8_t SJA1105_getMacLevelDiagnosticFlags(SJA1105_macLevelDiagnosticFlagsArgument_t *p_macLevelDiagnosticFlags, uint8_t port, uint8_t deviceSelect);

/* register category high_level_diagnostic_counters_1 */
extern uint8_t SJA1105_getHighLevelDiagnosticCounters10(uint64_t *p_nTxbyte, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters11(uint64_t *p_nTxfrm, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters12(uint64_t *p_nRxbyte, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters13(uint64_t *p_nRxfrm, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters14(uint32_t *p_nPolerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters15(uint32_t *p_nCrcerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters16(uint32_t *p_nSizeerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters17(uint32_t *p_nVlanerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters18(uint32_t *p_nN664err, uint8_t port, uint8_t deviceSelect);

/* register category high_level_diagnostic_counters_2 */
extern uint8_t SJA1105_getHighLevelDiagnosticCounters20(uint32_t *p_notReach, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters21(uint32_t *p_egrDisabled, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters22(uint32_t *p_partDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105_getHighLevelDiagnosticCounters23(uint32_t *p_qfull, uint8_t port, uint8_t deviceSelect);

/* register category l2_memory_partition_error_counters */
extern uint8_t SJA1105_getL2MemoryPartitionErrorCounters(uint32_t *p_nL2pspcdrn, uint8_t partition, uint8_t deviceSelect);

#endif  /* NXP_SJA1105_SWITCHCORE_H */
