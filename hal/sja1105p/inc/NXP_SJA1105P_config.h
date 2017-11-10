/******************************************************************************
* (c) NXP B.V. 2016 - 2017. All rights reserved.
*
* Disclaimer
* 1. The NXP Software/Source Code is provided to Licensee "AS IS" without any
* warranties of any kind. NXP makes no warranties to Licensee and shall not
* indemnify Licensee or hold it harmless for any reason related to the NXP
* Software/Source Code or otherwise be liable to the NXP customer. The NXP
* customer acknowledges and agrees that the NXP Software/Source Code is
* provided AS-IS and accepts all risks of utilizing the NXP Software under the
* conditions set forth according to this disclaimer.
*
* 2. NXP EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING, BUT
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS. NXP
* SHALL HAVE NO LIABILITY TO THE NXP CUSTOMER, OR ITS SUBSIDIARIES, AFFILIATES,
* OR ANY OTHER THIRD PARTY FOR ANY DAMAGES, INCLUDING WITHOUT LIMITATION,
* DAMAGES RESULTING OR ALLEGDED TO HAVE RESULTED FROM ANY DEFECT, ERROR OR
* OMMISSION IN THE NXP SOFTWARE/SOURCE CODE, THIRD PARTY APPLICATION SOFTWARE
* AND/OR DOCUMENTATION, OR AS A RESULT OF ANY INFRINGEMENT OF ANY INTELLECTUAL
* PROPERTY RIGHT OF ANY THIRD PARTY. IN NO EVENT SHALL NXP BE LIABLE FOR ANY
* INCIDENTAL, INDIRECT, SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL DAMAGES
* (INCLUDING LOST PROFITS) SUFFERED BY NXP CUSTOMER OR ITS SUBSIDIARIES,
* AFFILIATES, OR ANY OTHER THIRD PARTY ARISING OUT OF OR RELATED TO THE NXP
* SOFTWARE/SOURCE CODE EVEN IF NXP HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGES.
*
* 3. NXP reserves the right to make changes to the NXP Software/Sourcecode any
* time, also without informing customer.
*
* 4. Licensee agrees to indemnify and hold harmless NXP and its affiliated
* companies from and against any claims, suits, losses, damages, liabilities,
* costs and expenses (including reasonable attorney's fees) resulting from
* Licensee's and/or Licensee customer's/licensee's use of the NXP
* Software/Source Code.
*****************************************************************************/

/**
*
* \file NXP_SJA1105P_config.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
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

#ifdef SJA1105P_REFBOARD
	#define SJA1105P_N_SWITCHES 2U   /**< Number of switches cascaded in the reference board */
#else
	#define SJA1105P_N_SWITCHES 1U   /**< Only one switch is used on the board */
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
