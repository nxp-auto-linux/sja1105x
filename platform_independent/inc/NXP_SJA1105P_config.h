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
* \file NXP_SJA1105P_config.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Static configuration of the switch
*
*****************************************************************************/

#ifndef NXP_SJA1105P_CONFIG_H
#define NXP_SJA1105P_CONFIG_H

#ifdef UNIT_TEST
#define static extern
#else
#define static static
#endif

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* Defines
*****************************************************************************/

/**** User Settings Start ****/
/* Default assumptions are made. User specific options can be selected through -D option */

#ifndef SJA1105P_ETHIF_ZEROCOPY
	/* default assumption: Zero copy is used */
	#define SJA1105P_ETHIF_ZEROCOPY 1U  /**< All operations within the Ethernet Interface are zero-copy. This requires that memory does not get reallocated while the ethIf is processing a specific frame */
#endif

#ifndef SJA1105P_N_SWITCHES
	#define SJA1105P_N_SWITCHES 1U   /**< Number of switches */
#endif

/**** User Settings End ****/

/**** Fixed Settings ****/

#define SJA1105P_N_LOGICAL_PORTS (SJA1105P_N_PORTS + ((uint16_t) (SJA1105P_N_SWITCHES - 1U) * 3U))  /**< Number of logical ports available. Initial switch brings 5 ports, every cascaded adds 3 */
#define SJA1105P_MASTER_SWITCH 0U  /**< index of the switch connected to the host processor. Should not be changed. Software requires the host to be connected to switch with ID 0 */

/* Hardware properties */
#define SJA1105P_N_PORTS          5U /**< Number of ports in each switch IC */
#define SJA1105P_N_VLAN_ENTRIES   4096U
#define SJA1105P_N_ARL_ENTRIES    1024U
#define SJA1105P_N_RETAG_ENTRIES  32U
#define SJA1105P_N_MGMT_ROUTES    4U
#define SJA1105P_N_EGR_TIMESTAMPS 2U
#define SJA1105P_N_SHAPER         10U
#define SJA1105P_N_MACFLTS        2U

/* Timestamp properties */
#define SJA1105P_TIMESTAMP_LENGTH 32U

/* Meta frame format */
#define SJA1105P_META_FRAME_ETH_TYPE          0x0008U
#define SJA1105P_META_FRAME_N_BYTES           64U
#define SJA1105P_META_FRAME_N_BYTES_TS        3U
#define SJA1105P_META_FRAME_N_BYTES_DSTMAC    2U
#define SJA1105P_META_FRAME_BYTE_PL_START     14U
#define SJA1105P_META_FRAME_BYTE_TS_START     (uint8_t) (SJA1105P_META_FRAME_BYTE_PL_START + 1U)
#define SJA1105P_META_FRAME_BYTE_DSTMAC_START (uint8_t) (SJA1105P_META_FRAME_BYTE_PL_START + 4U)
#define SJA1105P_META_FRAME_BYTE_SRC_PORT     (uint8_t) (SJA1105P_META_FRAME_BYTE_PL_START + 6U)
#define SJA1105P_META_FRAME_BYTE_SWITCH_ID    (uint8_t) (SJA1105P_META_FRAME_BYTE_PL_START + 7U)

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
	uint8_t  switchId[SJA1105P_N_SWITCHES];
	uint64_t macFltres[SJA1105P_N_MACFLTS];
	uint64_t macFlt[SJA1105P_N_MACFLTS];
	uint8_t  inclSrcpt[SJA1105P_N_MACFLTS];
	uint8_t  sendMeta[SJA1105P_N_MACFLTS];
	uint8_t  hostPort[SJA1105P_N_SWITCHES];
	uint8_t  cascPort[SJA1105P_N_SWITCHES];
	uint8_t  managementPriority;
	uint8_t  mirrorPort[SJA1105P_N_SWITCHES];
	uint16_t egressMirroringOuterTag;
	uint8_t  egressMirroringOuterPcp;
	uint8_t  egressMirroringOuterDei;
} SJA1105P_generalParameters_t;  /**< General Parameters Table */

typedef struct
{
	uint64_t srcMeta;
	uint64_t dstMeta;
	uint8_t  ptpMaster[SJA1105P_N_SWITCHES];
} SJA1105P_avbParameters_t;  /**< AVB Parameters Table */

typedef struct
{
	uint8_t physicalPort;
	uint8_t switchId;
} SJA1105P_port_t;  /**< Information on port number and the physical switch associated with it */

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

extern SJA1105P_generalParameters_t SJA1105P_g_generalParameters;
extern SJA1105P_avbParameters_t     SJA1105P_g_avbParameters;
extern uint8_t                      SJA1105P_g_ptpMasterSwitch;

/* MAC Configuration Table */
extern uint16_t SJA1105P_g_macConfigurationVlanid;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t  SJA1105P_initSwitch(void);
extern uint8_t  SJA1105P_loadConfig(uint8_t configIndex, uint8_t switchId);
extern uint8_t  SJA1105P_synchSwitchConfiguration(void);

extern void     SJA1105P_initAutoPortMapping(void);
extern uint8_t  SJA1105P_initManualPortMapping(const SJA1105P_port_t k_portMapping[SJA1105P_N_LOGICAL_PORTS]);

extern void     SJA1105P_setPhyPropagationDelay(uint16_t propagationDelay, uint8_t port, SJA1105P_direction_t direction);
extern uint16_t SJA1105P_getPhyPropagationDelay(uint8_t port, SJA1105P_direction_t direction);

extern uint8_t  SJA1105P_getPhysicalPort(uint8_t logicalPort, SJA1105P_port_t *p_physicalPort);
extern uint8_t  SJA1105P_getLogicalPort(uint8_t *p_logicalPort, const SJA1105P_port_t *kp_physicalPort);

extern void     SJA1105P_getPhysicalPortVector(uint16_t logicalPorts, uint8_t switchId, uint8_t *p_physicalPorts);
extern void     SJA1105P_getPortVectorFromPhysicalPortVector(uint8_t physicalPorts, uint8_t switchId, uint16_t *p_logicalPorts);
extern void     SJA1105P_getSwitchesFromPorts(uint16_t logicalPorts, uint8_t *p_switches);

#endif /* NXP_SJA1105P_CONFIG_H */
