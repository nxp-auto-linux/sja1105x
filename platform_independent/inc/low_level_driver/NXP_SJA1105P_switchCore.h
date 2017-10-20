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
* \file NXP_SJA1105P_switchCore.h
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

#ifndef NXP_SJA1105P_SWITCHCORE_H
#define NXP_SJA1105P_SWITCHCORE_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_switchSpeed_10_MBPS  = 3,  /**< 10 Mbps */
	SJA1105P_e_switchSpeed_100_MBPS = 2,  /**< 100 Mbps */
	SJA1105P_e_switchSpeed_1_GBPS   = 1,  /**< 1 Gbps */
	SJA1105P_e_switchSpeed_DISABLED = 0   /**< Port disabled */
} SJA1105P_switchSpeed_t;

typedef enum 
{
	SJA1105P_e_hostCmd_WRITE      = 3,  /**< Host write */
	SJA1105P_e_hostCmd_READ       = 2,  /**< Host read */
	SJA1105P_e_hostCmd_SEARCH     = 1,  /**< Host search */
	SJA1105P_e_hostCmd_INVALIDATE = 4   /**< Host invalidate entry */
} SJA1105P_hostCmd_t;

typedef struct
{
	uint8_t configs;  /**< device configuration status: {0: invalid, 1: valid} */
	uint8_t crcchkl;  /**< local CRC check: {0: check ok, 1: check failed} */
	uint8_t ids;      /**< device identifier flag: {0: matching device identifier found, 1: matching device identifier not found} */
	uint8_t crcchkg;  /**< global CRC check: {0: check ok, 1: check failed} */
	uint8_t nslot;    /**< free-running 0 to 9 counter used for internal processing. It is intended to be used for debugging */
} SJA1105P_configurationFlagsArgument_t;

typedef struct
{
	uint8_t  portenf;    /**< Number to the port that triggered ENFFDS */
	uint8_t  fwds;       /**< Forwarding frame drop status: {0: no dropped frames; 1: frame dropped because input port was not set to Forwarding when frame received} */
	uint8_t  macfds;     /**< Standard MAC frame drop status: {0: no dropped frames; 1: frame dropped because it contained a filtered MAC address when the switch is not configured to forward such traffic} */
	uint8_t  enffds;     /**< Enforced frame drop status: {0: no dropped frames: frame received at PORTENF dropped because it carried a source MAC address declared to be enforced on a different port in the L2 Address Lookup table} */
	uint8_t  l2busyfds;  /**< L2 Address Lookup table busy/frame drop status: {0: not busy; no dropped frames; 1: frame received at PORTENF dropped because it was received while L2 Address Lookup table was being initialized} */
	uint8_t  l2busys;    /**< L2 Address Lookup table status: {0: L2 Address Lookup table initialization complete; 1: L2 Address Lookup is being initialized} */
	uint64_t macadd;     /**< Source MAC address that triggered ENFFDS */
} SJA1105P_generalStatusL2Argument_t;

typedef struct
{
	uint16_t vlanidhc;   /**< VLAN ID that triggered HASHCONFS */
	uint8_t  hashconfs;  /**< Hash conflict status: {0: No hash conflict encountered; 1: unresolved hash conflict in L2 Address Lookup table} */
	uint64_t macaddhc;   /**< source MAC address that triggered HASHCONFS */
} SJA1105P_generalStatusHashconfsArgument_t;

typedef struct
{
	uint16_t wpvlanid;    /**< VLAN ID that triggered WRONGPORTS or VNOTFOUNDS */
	uint8_t  port;        /**< Input port number that triggered WRONGPORTS or VNOTFOUNDS. Contains valid data when one of these flags is set. */
	uint8_t  vlanbusys;   /**< VLAN Lookup table status: {0: VLAN Lookup table initialization complete: 1: VLAN Lookup table is being initialized} */
	uint8_t  wrongports;  /**< Port status for VLAN frame: {0: Frame/port status OK; 1: Frame received at PORT dropped because the port is not configured for the VLAN ID in the VLAN Lookup table} */
	uint8_t  vnotfounds;  /**< Port/VLAN ID status: {0: Frame OK; 1: Frame received at port PORT containing the VLAN ID dropped because the VLAN ID is not configured in the VLAN Lookup table} */
} SJA1105P_generalStatusVlanArgument_t;

typedef struct
{
	uint8_t  emptys;   /**< dynamic memory status: {0: ok; 1: out of memory} */
	uint32_t buffers;  /**< Number of frame buffers available to dynamic memory management */
} SJA1105P_generalStatusMem0Argument_t;

typedef struct
{
	uint8_t port;   /**< Port number where frame dropped if FWDS or PARTS is set */
	uint8_t fwds;   /**< Port forwarding status: {0: No dropped frames, 1: Port identified by PORT sourced a fame that was dropped because the configured forwarding direction did not contain any ports} */
	uint8_t parts;  /**< Port forwarding status: {0: No dropped frames, 1: Port identified by PORT sourced a fame that was dropped because the configured forwarding direction did not contain any ports} */
} SJA1105P_generalStatusDropArgument_t;

typedef struct
{
	uint8_t valid;         /**< Setting this flag in combination with STARTPTPCP or STOPPTPCP dynamically changes the behavior of the schedule running based on the PTP time and the external PTP_CLIK pin */
	uint8_t startptpcp;    /**< Setting this flag in combination with VALID triggers the switch to begin toggling the external PTP_CLK pin at a rate of PTPPINDUR when the synchcronized PTP clock exceeds the value of PTPPINST */
	uint8_t stopptpcp;     /**< Setting this flag in combination with VALID triggers the switch to stop toggling the external PTP_CLK pin. */
	uint8_t syncCascaded;
	uint8_t resptp;        /**< Asserting this flag in combination with VALID will cause PTPCLK, PTPTSCLK and PTPCLKRATE to be re-set to their power-on defaults. */
	uint8_t corrclk4ts;    /**< Asserting this flag in combination with VALID will cause subsequent timestamps on ingress and egress management frames to be taken based on PTPCLK. If the flag is de-asserted, timestamps will be taken based on PTPTSCLK */
	uint8_t ptpclksub;     /**< Asserting this flag in combination with VALID will cause subsequent writes to PTPCLKVAL to be subtracted from the clock rather than setting a new value. */
	uint8_t ptpclkadd;     /**< Asserting this flag in combination with VALID will cause subsequent writes to PTPCLKVAL to be added to the clock rather than setting a new value. */
} SJA1105P_ptpControl2Argument_t;

typedef struct
{
	uint16_t index;           /**< Contains the address within the physical memory that this entry is to be stored in. */
	uint8_t  enfport;         /**< If this flag is set, MACADDR will be enforced as the source MAC address on ports having their flag set in DESTPORTS */
	uint8_t  destports;       /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
	uint64_t macaddr;         /**< The MAC address associated with this entry */
	uint16_t vlanid;          /**< The VLAN ID associated with this entry. VLANID is only included in the lookup process if SHARED_LEARN in Table 7 is cleared, otherwise this parameter is ignored. */
	uint8_t  innerOuterVlan;  /**< Specifies if the VLAN ID used in the search/learn key is a inner VLAN or an outer VLAN. */
	uint64_t mask;            /**< Mask used by the TCAM memory for the search operation. */
	uint16_t age;             /**< When aging is performed and the counter is equal to a configured maximum age value, the entry is invalidated. */
	uint8_t  touched;         /**< If it is zero it means that the AGE counter is not incremented when aging is performed. */
} SJA1105P_l2ArtEntryArgument_t;

typedef struct
{
	uint16_t index;           /**< Contains the address within the physical memory that this entry is to be stored in. */
	uint8_t  enfport;         /**< If this flag is set, MACADDR will be enforced as the source MAC address on ports having their flag set in DESTPORTS */
	uint8_t  destports;       /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
	uint64_t macaddr;         /**< The MAC address associated with this entry */
	uint16_t vlanid;          /**< The VLAN ID associated with this entry. VLANID is only included in the lookup process if SHARED_LEARN in Table 7 is cleared, otherwise this parameter is ignored. */
	uint8_t  innerOuterVlan;  /**< Specifies if the VLAN ID used in the search/learn key is a inner VLAN or an outer VLAN. */
	uint64_t mask;            /**< Mask used by the TCAM memory for the search operation. */
	uint8_t  retag;           /**< If a destination or source MAC address of a received frame produces a match in the TCAM memory and this flag is found asserted in any of the matched TCAM entries, than the frame is retagged using the mirrored VLAN ID as the new tag. */
	uint8_t  mirror;          /**< If the destination or source MAC address of a received frame produces a match in the TCAM memory and this flag is found asserted in any (or both) of the matched TCAM entries, a copy of the received frame is sent to the MIRROR_PORT */
	uint8_t  takeTs;          /**< If a destination MAC address of a received frame produces a match in the TCAM memory and this flag and bLocked flag are both found asserted then an egress timestamp is captured for this frame for each transmitted destination port */
	uint16_t mirroredVlan;  
	uint8_t  tsReg;           /**< Selects the register in which the egress timestamp will be stored. */
} SJA1105P_l2ArtLockedEntryArgument_t;

typedef struct
{
	uint8_t            valid;      /**< This flag to triggers a dynamic change in the contents of the L2 Address Lookup table (if RDWRSET is set) or a read access (when RDWRSET is cleared) */
	uint8_t            rdwrset;    /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t            lockeds;    /**< This flag should be read cleared when the VALID flag is set. The flag will also be found cleared in response to a read having the MGMTROUTE flag set. In response to a read with the MGMTROUTE flag cleared, the flag be set if the most recent access operated on an entry that was either loaded by configuration or through dynamic reconfiguration */
	uint8_t            valident;   /**< This flag determines if the respective entry should be marked valid. Marking an entry as invalid (clearing VALIDENT) has the effect, that the entry at the respective position will be available for address learning again. Not applicable for management routes. */
	uint8_t            mgmtroute;  /**< This flag to indicates that the request is targeted for a management route entry. */
	SJA1105P_hostCmd_t hostCmd;    /**< On write, the host will request one of the following commands: host search (001), host read (010), host write (011), host invalidate entry (other values) */
} SJA1105P_l2AddressLookupTableControlSetArgument_t;

typedef struct
{
	uint8_t            valid;      /**< This flag to triggers a dynamic change in the contents of the L2 Address Lookup table (if RDWRSET is set) or a read access (when RDWRSET is cleared) */
	uint8_t            errors;     /**< This flag should be read cleared when the VALID flag is set. If it is found to be set while the VALID flag is set, the most recent access resulted in an error. */
	uint8_t            lockeds;    /**< This flag should be read cleared when the VALID flag is set. The flag will also be found cleared in response to a read having the MGMTROUTE flag set. In response to a read with the MGMTROUTE flag cleared, the flag be set if the most recent access operated on an entry that was either loaded by configuration or through dynamic reconfiguration */
	uint8_t            valident;   /**< This flag determines if the respective entry should be marked valid. Marking an entry as invalid (clearing VALIDENT) has the effect, that the entry at the respective position will be available for address learning again. Not applicable for management routes. */
	uint8_t            mgmtroute;  /**< This flag to indicates that the request is targeted for a management route entry. */
	SJA1105P_hostCmd_t hostCmd;    /**< On write, the host will request one of the following commands: host search (001), host read (010), host write (011), host invalidate entry (other values) */
} SJA1105P_l2AddressLookupTableControlGetArgument_t;

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
} SJA1105P_l2ForwardingTableEntryArgument_t;

typedef struct
{
	uint8_t valid;    /**< The host sets this flag to trigger a dynamic change in the entry indicated by INDEX */
	uint8_t rdwrset;  /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t index;    /**< Contains the index of the entry being dynamically reconfigured. */
} SJA1105P_l2ForwardingTableControlArgument_t;

typedef struct
{
	uint16_t vlanid;     /**< The VLAN ID associated with this entry */
	uint8_t  tagPort;    /**< Defines the set of ports on which a frame tagged with the respective VLAN ID may be received. All bits must be set in order to deactivate VLAN-based ingress port policing. */
	uint8_t  vlanBc;     /**< This field restricts the broadcast domain of the specific VLAN. If a bit is cleared, the respective port cannot be reached by a frame tagged with the specific VLAN ID. All bits have to be set to de-activate reachability limitations for certain VLANs. */
	uint8_t  vmembPort;  /**< Defines if a frame associated with the respective VLAN ID shall be sent un-tagged. */
	uint8_t  vegrMirr;   /**< All traffic tagged with VLANID and forwarded to any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
	uint8_t  vingMirr;   /**< All traffic tagged with VLANID and received on any of the ports having its flag asserted in this field will be forwarded to the mirror port as defined by the MIRR_PORT field of the General Parameters configuration block. */
} SJA1105P_vlanLookupTableEntryArgument_t;

typedef struct
{
	uint8_t valid;     /**< The host sets this flag to trigger a dynamic change in the entry associated with the VLANID field. */
	uint8_t rdwrset;   /**< The host sets this flag to trigger a dynamic change in the entry indicated by INDEX */
	uint8_t valident;  /**< In case of a write access, this flag determines if the respective entry should be marked valid */
} SJA1105P_vlanLookupTableControlArgument_t;

typedef struct
{
	uint32_t idleSlope;  /**< Value at which the credit counter gets increased at a rate of bytes per second times link speed */
	uint32_t sendSlope;  /**< Value at which the credit counter gets decreased at a rate of bytes per second times link speed */
	uint32_t creditHi;   /**< Value at which the credit counter positively saturates upon transmission of a frame */
	uint32_t creditLo;   /**< Value at which the credit counter negatively saturates upon transmission of a frame */
	uint8_t  cbsPrio;    /**< Priority queue to which the credit-based shaper is assigned */
	uint8_t  cbsPort;    /**< Port to which the credit-based shaper is assigned */
} SJA1105P_creditBasedShapingEntryArgument_t;

typedef struct
{
	uint8_t valid;     /**< By writing a value that has this flag set, the host triggers an update. */
	uint8_t rdwrset;   /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t shaperId;  /**< Index of the credit-based shaper which is subject to dynamic reconfiguration */
} SJA1105P_creditBasedShapingControlArgument_t;

typedef struct
{
	uint8_t  destports;     /**< This field provides a dedicated forwarding for all frames carrying a VLAN ID that matches VLAN_EGR of the entry */
	uint8_t  useDestports;  /**< If this flag is asserted, DESTPORTS of the entry will be used to forward all frames carrying a VLAN ID that matches VLAN_EGR of the entry. */
	uint8_t  doNotLearn;    /**< If this flag is asserted, address learning will be disabled for all frames carrying a VLAN ID that matches VLAN_EGR of the entry. */
	uint16_t vlanEgr;       /**< This VLAN ID will replace the VLAN ID of the original frame at egress. */
	uint16_t vlanIng;       /**< The VLAN ID of the ingress frame */
	uint8_t  ingPort;       /**< If a frame with VLAN ID VLAN_ING is received on any port having its flag set in this field, a copy with VLAN ID VLAN_EGR will be generated */
	uint8_t  egrPort;       /**< If a frame with VLAN ID VLAN_ING is forwarded to any port having its flag set in this field, a copy with VLAN ID VLAN_EGR will be generated. */
} SJA1105P_retaggingTableEntryArgument_t;

typedef struct
{
	uint8_t valid;     /**< The host sets this flag to trigger a dynamic change to the entry with index INDEX. If this flag is found set on read, the switch is still busy processing the most recent update request. */
	uint8_t valident;  /**< Indicates that the entry at position INDEX should be enabled. */
	uint8_t rdwrset;   /**< The host sets this flag to trigger a dynamic change in the entry indicated by INDEX */
	uint8_t index;     /**< Contains the index of the entry being dynamically reconfigured. */
} SJA1105P_retaggingTableControlArgument_t;

typedef struct
{
	uint8_t  egrMirrOuterDei;         
	uint8_t  egrMirrOuterPcp;         
	uint16_t egrMirrOuterTag;         
	uint8_t  takeEgrTsOnHostPort;     
	uint16_t tpid2;                   
	uint8_t  ignore2StepFlag;         
	uint16_t tpid;                    
	uint32_t vlMask;                  
	uint32_t vlMarker;                
	uint8_t  mirrorp;                 
	uint8_t  hostPort;                
	uint8_t  cascadingPort;           
	uint8_t  sendFollowUpTs0;         
	uint8_t  sendFollowUpTs1;         
	uint8_t  includeSrcPort0;         
	uint8_t  includeSrcPort1;         
	uint64_t macFilter0;              
	uint64_t macFilter1;              
	uint64_t macFilterResult0;        
	uint64_t macFilterResult1;        
	uint8_t  mgmtPrio;                
	uint8_t  id;                      
	uint8_t  mirrorPortReconfigEnable;
	uint8_t  arinc;                   
} SJA1105P_generalParametersEntryArgument_t;

typedef struct
{
	uint8_t valid;    /**< By writing a value that has this flag set the host triggers an update. The flag will always be found cleared on read. */
	uint8_t rdwrset;  /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
} SJA1105P_generalParametersControlSetArgument_t;

typedef struct
{
	uint8_t errors;                 /**< Error */
	uint8_t mirrPortReconfigError;  /**< MirrorPortReconfigError */
} SJA1105P_generalParametersControlGetArgument_t;

typedef struct
{
	uint8_t  ingressMirroringOuterDei;     
	uint8_t  ingressMirroringOuterPcp;     
	uint16_t ingressMirroringOuterTag;     
	uint8_t  mirrorConflictOnEgressTag;    
	uint8_t  mirrorConflictOnIngressEgress;
	uint8_t  ingress;                      
	uint8_t  egress;                       
	uint8_t  dynLearn;                     
	uint8_t  retag;                        
	uint8_t  drpuntag;                     
	uint8_t  dropSingleInnerTagged;        
	uint8_t  dropSingleOuterTagged;        
	uint8_t  dropDoubleTagged;             
	uint8_t  dropNonA664;                  
	uint8_t  egrmirr;                      
	uint8_t  ingmirr;                      
	uint16_t vlanid;                       
	uint8_t  vlanPrio;                     
	uint8_t  maxAge;                       
	uint16_t tpdelout;                     
	uint16_t tpdelin;                      
	uint8_t  speed;                        
	uint8_t  ifg;                          
	uint8_t  enabled0;                     
	uint16_t base0;                        
	uint16_t top0;                         
	uint8_t  enabled1;                     
	uint16_t base1;                        
	uint16_t top1;                         
	uint8_t  enabled2;                     
	uint16_t base2;                        
	uint16_t top2;                         
	uint8_t  enabled3;                     
	uint16_t base3;                        
	uint16_t top3;                         
	uint8_t  enabled4;                     
	uint16_t base4;                        
	uint16_t top4;                         
	uint8_t  enabled5;                     
	uint16_t base5;                        
	uint16_t top5;                         
	uint8_t  enabled6;                     
	uint16_t base6;                        
	uint16_t top6;                         
	uint8_t  enabled7;                     
	uint16_t base7;                        
	uint16_t top7;                         
} SJA1105P_macCfgTableEntryArgument_t;

typedef struct
{
	uint8_t valid;    /**< The host sets this flag to trigger a dynamic change to the entry with index INDEX. If this flag is found set on read, the switch is still busy processing the most recent update request. */
	uint8_t rdwrset;  /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
	uint8_t port;     /**< Specifies the port number which is affected by this dynamic reconfiguration */
} SJA1105P_macCfgTableControlArgument_t;

typedef struct
{
	uint8_t  oneShotLearning;                      
	uint8_t  overwriteDynIfTcamFull;               
	uint8_t  useFreeStaticSpaceForLearning;        
	uint8_t  ignoreMgmtAddrAtHostPort;             
	uint8_t  skipPortEnforcementAtHostPort;        
	uint8_t  sharedLearning;                       
	uint8_t  dropIfCannotLearn;                    
	uint16_t startDynSpaceAddr;                    
	uint16_t maxAge;                               
	uint16_t maxNoOfAddrAllowedPerPort0;           
	uint16_t maxNoOfAddrAllowedPerPort1;           
	uint16_t maxNoOfAddrAllowedPerPort2;           
	uint16_t maxNoOfAddrAllowedPerPort3;           
	uint16_t maxNoOfAddrAllowedPerPort4;           
	uint8_t  enableSourcePortEnforcingForUnicast;  
	uint8_t  enableSourcePortEnforcingForMulticast;
	uint8_t  enableSourcePortEnforcingForBroadcast;
} SJA1105P_l2LookupParametersEntryArgument_t;

typedef struct
{
	uint8_t valid;    /**< By writing a value that has this flag set, the host triggers an update. */
	uint8_t rdwrset;  /**< Determines if an access is a read access (flag is cleared) or a write access (flag is set) */
} SJA1105P_l2LookupParametersControlArgument_t;

typedef struct
{
	uint8_t  l2parts;  /**< switch memory partition status for Ethernet traffic: {0: No memory error; 1: The respective memory partition suffered at least one memory error after the previous read access to this field} */
	uint32_t nL2pspc;  /**< Number of frames left for the respective L2 memory partition at the time of the read access. */
} SJA1105P_l2MemoryPartitionStatusArgument_t;

typedef struct
{
	uint8_t nRunt;      /**< Number of frames that do not have a SOF, alignment or MII error, but are shorter than 64 bytes. The counter will not wrap. */
	uint8_t nSoferr;    /**< Number of frames that have a SOF error. The field will not wrap. */
	uint8_t nAlignerr;  /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but whose length is not a multiple of eight bits. The field will not wrap. */
	uint8_t nMiierr;    /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but terminated with the MII error input being asserted. The field will not wrap. */
} SJA1105P_macLevelDiagnCountersArgument_t;

typedef struct
{
	uint8_t spcprior;    /**< Index of the priority queue that hosted the frame dropped due to a space error. */
	uint8_t portdrops;   /**< Indicates that the respective port had a frame dropped because the port has not been enabled for traffic in the L2 Policing table. */
	uint8_t lendrops;    /**< Indicates that the respective port had a frame dropped because the frame was longer than defined in the L2 Policing table. */
	uint8_t bwdrops;     /**< Indicates that the respective port had a frame dropped because there was no bandwidth left on the port as defined in the L2 Policing table. */
	uint8_t drn664errs;  /**< Indicates that a frame was dropped at the respective port because its EtherType field contained a value other than 0x800 while the DRPNONA664 flag of the MAC Configuration block is set. */
	uint8_t spcerrs;     /**< Indicates that a frame was dropped at the respective port because the respective priority queue as defined by the BASE and TOP parameters did not have any space left or is de-activated. If the flag is set, SPCPRIOR will contain the index of the priority queue that hosted the dropped frame. */
} SJA1105P_macLevelDiagnFlagsArgument_t;

typedef struct
{
	uint8_t qOccupancyHighWatt;  /**< Highest watermark for the queue occupancy of a TX MAC output priority queue */
	uint8_t qOccupancy;          /**< Number of frames stored in each TXMAC output priority queue */
} SJA1105P_hlDiagnCountersOccupancyArgument_t;

typedef struct
{
	uint16_t partitionSpace7;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace6;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace5;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace4;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace3;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace2;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace1;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint16_t partitionSpace0;  /**< Maximum amount of frame memory that may be used by the L2 memory partition */
	uint8_t  maxDynPrio;       /**< Maximum priority value that will be accepted by dynamic updates */
} SJA1105P_l2ForwardingParametersArgument_t;

typedef struct
{
	uint64_t srcMeta;              /**< Source MAC address used for meta data follow up frames */
	uint64_t dstMeta;              /**< Destination MAC address used for meta data follow up frames */
	uint8_t  ptpMaster;            /**< Specifies if the switch is behaving as a PTP CLK master (1) or a PTP CLK slave (0) relative to the PTP CLK inout pin */
	uint8_t  useL2DataRateForCbs;  /**< Specifies if the CBS blocks use L1 (preamble, SFD, IFG, length) or L2 (length) data rate into account when they shape the output traffic */
} SJA1105P_avbParametersEntryArgument_t;

typedef struct
{
	uint8_t valid;    /**< The host sets this flag to trigger a dynamic change in the entry associated with the VLANID field. */
	uint8_t rdwrset;  /**< The host sets this flag to trigger a dynamic change in the entry indicated by INDEX */
} SJA1105P_avbParametersControlArgument_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

/* register category device_id */
extern uint8_t SJA1105P_getDeviceId(uint32_t *p_id, uint8_t deviceSelect);

/* register category configuration_flags */
extern uint8_t SJA1105P_getConfigurationFlags(SJA1105P_configurationFlagsArgument_t *p_configurationFlags, uint8_t deviceSelect);

/* register category general_status_l2 */
extern uint8_t SJA1105P_getGeneralStatusL2(SJA1105P_generalStatusL2Argument_t *p_generalStatusL2, uint8_t deviceSelect);

/* register category general_status_hashconfs */
extern uint8_t SJA1105P_getGeneralStatusHashconfs(SJA1105P_generalStatusHashconfsArgument_t *p_generalStatusHashconfs, uint8_t deviceSelect);

/* register category general_status_vlan */
extern uint8_t SJA1105P_getGeneralStatusVlan(SJA1105P_generalStatusVlanArgument_t *p_generalStatusVlan, uint8_t deviceSelect);

/* register category general_status_mem */
extern uint8_t SJA1105P_getGeneralStatusMem0(SJA1105P_generalStatusMem0Argument_t *p_generalStatusMem0, uint8_t deviceSelect);
extern uint8_t SJA1105P_getGeneralStatusMem1(uint32_t *p_buffersLowWatermark, uint8_t deviceSelect);

/* register category general_status_drop */
extern uint8_t SJA1105P_getGeneralStatusDrop(SJA1105P_generalStatusDropArgument_t *p_generalStatusDrop, uint8_t deviceSelect);

/* register category general_status_ram_error */
extern uint8_t SJA1105P_getGeneralStatusRamError(uint32_t *p_ramparerr, uint8_t deviceSelect);

/* register category general_status_ram_init */
extern uint8_t SJA1105P_setGeneralStatusRamInit(uint32_t rparinit, uint8_t deviceSelect);
extern uint8_t SJA1105P_getGeneralStatusRamInit(uint32_t *p_rparinit, uint8_t deviceSelect);

/* register category ethernet_port_status_control */
extern uint8_t SJA1105P_setEthernetPortStatusControl(uint8_t clearport, uint8_t deviceSelect);

/* register category transmission_control */
extern uint8_t SJA1105P_setTransmissionControl(uint8_t inhibittx, uint8_t deviceSelect);

/* register category ptp_control */
extern uint8_t SJA1105P_setPtpControl0(uint64_t ptppinst, uint8_t deviceSelect);
extern uint8_t SJA1105P_setPtpControl1(uint32_t ptppindur, uint8_t deviceSelect);
extern uint8_t SJA1105P_setPtpControl2(const SJA1105P_ptpControl2Argument_t *pk_ptpControl2, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPtpControl2(uint8_t *p_syncCascaded, uint8_t deviceSelect);
extern uint8_t SJA1105P_setPtpControl3(uint64_t ptpclkval, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPtpControl3(uint64_t *p_ptpclkval, uint8_t deviceSelect);
extern uint8_t SJA1105P_setPtpControl4(uint32_t ptpclkrate, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPtpControl5(uint64_t *p_ptptsclk, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPtpControl6(uint64_t *p_ptptsshadow, uint8_t deviceSelect);

/* register category l2_art_entry */
extern uint8_t SJA1105P_setL2ArtEntry(const SJA1105P_l2ArtEntryArgument_t *pk_l2ArtEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2ArtEntry(SJA1105P_l2ArtEntryArgument_t *p_l2ArtEntry, uint8_t deviceSelect);

/* register category l2_art_locked_entry */
extern uint8_t SJA1105P_setL2ArtLockedEntry(const SJA1105P_l2ArtLockedEntryArgument_t *pk_l2ArtLockedEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2ArtLockedEntry(SJA1105P_l2ArtLockedEntryArgument_t *p_l2ArtLockedEntry, uint8_t deviceSelect);

/* register category l2_address_lookup_table_control */
extern uint8_t SJA1105P_setL2AddressLookupTableControl(const SJA1105P_l2AddressLookupTableControlSetArgument_t *pk_l2AddressLookupTableControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2AddressLookupTableControl(SJA1105P_l2AddressLookupTableControlGetArgument_t *p_l2AddressLookupTableControl, uint8_t deviceSelect);

/* register category l2_forwarding_table_entry */
extern uint8_t SJA1105P_setL2ForwardingTableEntry(const SJA1105P_l2ForwardingTableEntryArgument_t *pk_l2ForwardingTableEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2ForwardingTableEntry(SJA1105P_l2ForwardingTableEntryArgument_t *p_l2ForwardingTableEntry, uint8_t deviceSelect);

/* register category l2_forwarding_table_control */
extern uint8_t SJA1105P_setL2ForwardingTableControl(const SJA1105P_l2ForwardingTableControlArgument_t *pk_l2ForwardingTableControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2ForwardingTableControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category vlan_lookup_table_entry */
extern uint8_t SJA1105P_setVlanLookupTableEntry(const SJA1105P_vlanLookupTableEntryArgument_t *pk_vlanLookupTableEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getVlanLookupTableEntry(SJA1105P_vlanLookupTableEntryArgument_t *p_vlanLookupTableEntry, uint8_t deviceSelect);

/* register category vlan_lookup_table_control */
extern uint8_t SJA1105P_setVlanLookupTableControl(const SJA1105P_vlanLookupTableControlArgument_t *pk_vlanLookupTableControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getVlanLookupTableControl(uint8_t *p_valident, uint8_t deviceSelect);

/* register category credit_based_shaping_entry */
extern uint8_t SJA1105P_setCreditBasedShapingEntry(const SJA1105P_creditBasedShapingEntryArgument_t *pk_creditBasedShapingEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCreditBasedShapingEntry(SJA1105P_creditBasedShapingEntryArgument_t *p_creditBasedShapingEntry, uint8_t deviceSelect);

/* register category credit_based_shaping_control */
extern uint8_t SJA1105P_setCreditBasedShapingControl(const SJA1105P_creditBasedShapingControlArgument_t *pk_creditBasedShapingControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getCreditBasedShapingControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category retagging_table_entry */
extern uint8_t SJA1105P_setRetaggingTableEntry(const SJA1105P_retaggingTableEntryArgument_t *pk_retaggingTableEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getRetaggingTableEntry(SJA1105P_retaggingTableEntryArgument_t *p_retaggingTableEntry, uint8_t deviceSelect);

/* register category retagging_table_control */
extern uint8_t SJA1105P_setRetaggingTableControl(const SJA1105P_retaggingTableControlArgument_t *pk_retaggingTableControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getRetaggingTableControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category general_parameters_entry */
extern uint8_t SJA1105P_setGeneralParametersEntry(const SJA1105P_generalParametersEntryArgument_t *pk_generalParametersEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getGeneralParametersEntry(SJA1105P_generalParametersEntryArgument_t *p_generalParametersEntry, uint8_t deviceSelect);

/* register category general_parameters_control */
extern uint8_t SJA1105P_setGeneralParametersControl(const SJA1105P_generalParametersControlSetArgument_t *pk_generalParametersControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getGeneralParametersControl(SJA1105P_generalParametersControlGetArgument_t *p_generalParametersControl, uint8_t deviceSelect);

/* register category mac_cfg_table_entry */
extern uint8_t SJA1105P_setMacCfgTableEntry(const SJA1105P_macCfgTableEntryArgument_t *pk_macCfgTableEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getMacCfgTableEntry(SJA1105P_macCfgTableEntryArgument_t *p_macCfgTableEntry, uint8_t deviceSelect);

/* register category mac_cfg_table_control */
extern uint8_t SJA1105P_setMacCfgTableControl(const SJA1105P_macCfgTableControlArgument_t *pk_macCfgTableControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getMacCfgTableControl(uint8_t *p_errors, uint8_t deviceSelect);

/* register category l2_lookup_parameters_entry */
extern uint8_t SJA1105P_setL2LookupParametersEntry(const SJA1105P_l2LookupParametersEntryArgument_t *pk_l2LookupParametersEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2LookupParametersEntry(SJA1105P_l2LookupParametersEntryArgument_t *p_l2LookupParametersEntry, uint8_t deviceSelect);

/* register category l2_lookup_parameters_control */
extern uint8_t SJA1105P_setL2LookupParametersControl(const SJA1105P_l2LookupParametersControlArgument_t *pk_l2LookupParametersControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getL2LookupParametersControl(uint8_t *p_valid, uint8_t deviceSelect);

/* register category ptp_egress */
extern uint8_t SJA1105P_getPtpEgress0(uint8_t *p_update, uint8_t port, uint8_t timestampIndex, uint8_t deviceSelect);
extern uint8_t SJA1105P_getPtpEgress1(uint32_t *p_ptpegrTs, uint8_t port, uint8_t timestampIndex, uint8_t deviceSelect);

/* register category l2_memory_partition_status */
extern uint8_t SJA1105P_getL2MemoryPartitionStatus(SJA1105P_l2MemoryPartitionStatusArgument_t *p_l2MemoryPartitionStatus, uint8_t partition, uint8_t deviceSelect);

/* register category mac_level_diagn_counters */
extern uint8_t SJA1105P_getMacLevelDiagnCounters(SJA1105P_macLevelDiagnCountersArgument_t *p_macLevelDiagnCounters, uint8_t port, uint8_t deviceSelect);

/* register category mac_level_diagn_flags */
extern uint8_t SJA1105P_getMacLevelDiagnFlags(SJA1105P_macLevelDiagnFlagsArgument_t *p_macLevelDiagnFlags, uint8_t port, uint8_t deviceSelect);

/* register category hl_diagn_counters_1 */
extern uint8_t SJA1105P_getHlDiagnCounters10(uint64_t *p_nTxbyte, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters11(uint64_t *p_nTxfrm, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters12(uint64_t *p_nRxbyte, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters13(uint64_t *p_nRxfrm, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters14(uint32_t *p_nPolerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters15(uint32_t *p_nCrcerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters16(uint32_t *p_nSizeerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters17(uint32_t *p_nVlanerr, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters18(uint32_t *p_nN664err, uint8_t port, uint8_t deviceSelect);

/* register category hl_diagn_counters_2 */
extern uint8_t SJA1105P_getHlDiagnCounters20(uint32_t *p_notReach, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters21(uint32_t *p_egrDisabled, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters22(uint32_t *p_partDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getHlDiagnCounters23(uint32_t *p_qfull, uint8_t port, uint8_t deviceSelect);

/* register category hl_diagn_counters_occupancy */
extern uint8_t SJA1105P_getHlDiagnCountersOccupancy(SJA1105P_hlDiagnCountersOccupancyArgument_t *p_hlDiagnCountersOccupancy, uint8_t port, uint8_t queue, uint8_t deviceSelect);

/* register category l2_memory_partition_error_counters */
extern uint8_t SJA1105P_getL2MemoryPartitionErrorCounters(uint32_t *p_nL2pspcdrn, uint8_t partition, uint8_t deviceSelect);

/* register category eth_stat_counters */
extern uint8_t SJA1105P_getEthStatCounters00(uint32_t *p_ingBroadcastPkts, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters01(uint32_t *p_ingMulticastPkts, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters02(uint32_t *p_ingPkts64Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters03(uint32_t *p_ingPkts65To127Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters04(uint32_t *p_ingPkts128To255Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters05(uint32_t *p_ingPkts256To511Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters06(uint32_t *p_ingPkts512To1023Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters07(uint32_t *p_ingPkts1024To1518Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters08(uint32_t *p_egrBroadcastPkts, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters09(uint32_t *p_egrMulticastPkts, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters10(uint32_t *p_egrPkts64Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters11(uint32_t *p_egrPkts65To127Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters12(uint32_t *p_egrPkts128To255Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters13(uint32_t *p_egrPkts256To511Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters14(uint32_t *p_egrPkts512To1023Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters15(uint32_t *p_egrPkts1024To1518Octets, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters16(uint32_t *p_ingUntaggedDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters17(uint32_t *p_ingSingleInnerDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters18(uint32_t *p_ingSingleOuterDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters19(uint32_t *p_ingDoubleTaggedDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters20(uint32_t *p_ingIllegalDoubleTaggedDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters21(uint32_t *p_ingEmptyRouteDrop, uint8_t port, uint8_t deviceSelect);
extern uint8_t SJA1105P_getEthStatCounters22(uint32_t *p_ingAddrNotLearnedDrop, uint8_t port, uint8_t deviceSelect);

/* register category l2_forwarding_parameters */
extern uint8_t SJA1105P_getL2ForwardingParameters(SJA1105P_l2ForwardingParametersArgument_t *p_l2ForwardingParameters, uint8_t deviceSelect);

/* register category avb_parameters_entry */
extern uint8_t SJA1105P_setAvbParametersEntry(const SJA1105P_avbParametersEntryArgument_t *pk_avbParametersEntry, uint8_t deviceSelect);
extern uint8_t SJA1105P_getAvbParametersEntry(SJA1105P_avbParametersEntryArgument_t *p_avbParametersEntry, uint8_t deviceSelect);

/* register category avb_parameters_control */
extern uint8_t SJA1105P_setAvbParametersControl(const SJA1105P_avbParametersControlArgument_t *pk_avbParametersControl, uint8_t deviceSelect);
extern uint8_t SJA1105P_getAvbParametersControl(uint8_t *p_errors, uint8_t deviceSelect);

extern SJA1105P_hostCmd_t SJA1105P_convertToHostCmd(uint32_t stdintValue);

#endif  /* NXP_SJA1105P_SWITCHCORE_H */
