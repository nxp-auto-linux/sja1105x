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
* \file NXP_SJA1105P_diagnostics.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
*
* \brief Obtain diagnostic information on the switch
*
*****************************************************************************/

#ifndef NXP_SJA1105P_DIAGNOSTICS_H
#define NXP_SJA1105P_DIAGNOSTICS_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	SJA1105P_e_etherStatDirection_INGRESS,
	SJA1105P_e_etherStatDirection_EGRESS,
	SJA1105P_e_etherStatDirection_BOTH
} SJA1105P_etherStatDirection_t;  /**< 32-bit Ethernet statistic counters */

typedef enum 
{
	/* unidirectional */
	SJA1105P_e_etherStat32_N_NOT_REACH,     /**< number of frames that produced a match in the L2 Lookup table since power on or reset, but were not routed to the port this counter is assigned to because the port is not reachable for the respective ingress port as per REACH_PORT in the L2 Forwarding table */
	SJA1105P_e_etherStat32_N_EGR_DISABLED,  /**< number of frames that were not routed to the port this counter is assigned to, since power-on or reset, because the port was down */
	SJA1105P_e_etherStat32_N_PART_DROP,     /**< number of frames that were dropped on ingress because the respective memory partition of the port (as defined per PARTITION of the L2 Policing table) had no space left after power-on or reset */
	SJA1105P_e_etherStat32_N_QFULL,         /**< number of frames that were dropped on egress because the respective priority queue of the destination port (as defined per VLAN_PMAP of the L2 Forwarding table) did not have any space left since power on or reset */
	SJA1105P_e_etherStat32_N_POLERR,        /**< number of frames that were dropped based on the L2 policing operation since power on or reset */
	SJA1105P_e_etherStat32_N_CRCERR,        /**< number of frames that had a receive-side CRC error on this port since power on or reset */
	SJA1105P_e_etherStat32_N_SIZEERR,       /**< number of frames received since power on or reset with an invalid length (2 K bytes or more or the length contained in the Type/Length field of the frame did not match the actual length) as well as frames received while ingress traffic was disabled */
	SJA1105P_e_etherStat32_N_VLANERR,       /**< number of frames that were dropped since power on or reset because the VLAN ID was either not found in the VLAN Lookup table, the respective port is not listed in the VMEMB_PORT vector of the configured VLANID, or a legal or illegal double-tagged frame was received while double-tagged traffic was not allowed */
	SJA1105P_e_etherStat32_N_N664ERR,       /**< number of frames dropped since power-on or reset because they had an EtherType field other than 800h while the DRPNONA664 flag was set for the respective port in the MAC Configuration table, they were not tagged while untagged traffic was not allowed (flag at position 4 of PORTST of the MAC Configuration Table), or that were not routed to any destination (because destination ports were down) */

	SJA1105P_e_etherStat32_N_ADDR_NOT_LEARNED_DROP,
	SJA1105P_e_etherStat32_N_EMPTY_ROUTE_DROP,
	SJA1105P_e_etherStat32_N_ILLEGAL_DOUBLE_DROP,
	SJA1105P_e_etherStat32_N_DOUBLE_TAGGED_DROP,
	SJA1105P_e_etherStat32_N_SINGLE_OUTER_DROP,
	SJA1105P_e_etherStat32_N_SINGLE_INNER_DROP,
	SJA1105P_e_etherStat32_N_UNTAGGED_DROP,

	/* ingress + egress */
	SJA1105P_e_etherStat32_N_PKTS_1024_TO_1518_OCTETS,
	SJA1105P_e_etherStat32_N_PKTS_512_TO_1023_OCTETS,
	SJA1105P_e_etherStat32_N_PKTS_256_TO_511_OCTETS,
	SJA1105P_e_etherStat32_N_PKTS_128_TO_255_OCTETS,
	SJA1105P_e_etherStat32_N_PKTS_65_TO_127_OCTETS,
	SJA1105P_e_etherStat32_N_PKTS_64_OCTETS,
	SJA1105P_e_etherStat32_N_MULTICAST_PKTS,
	SJA1105P_e_etherStat32_N_BROADCAST_PKTS
} SJA1105P_etherStat32_t;  /**< 32-bit Ethernet statistic counters */

typedef enum
{
	SJA1105P_e_etherStat64_N_OCTETS,
	SJA1105P_e_etherStat64_N_PKTS
} SJA1105P_etherStat64_t;  /**< 64-bit Ethernet statistic counters */

typedef struct
{
	uint32_t nRunt;      /**< Number of frames that do not have a SOF, alignment or MII error, but are shorter than 64 bytes. The counter will not wrap. */
	uint32_t nSoferr;    /**< Number of frames that have a SOF error. The field will not wrap. */
	uint32_t nAlignerr;  /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but whose length is not a multiple of eight bits. The field will not wrap. */
	uint32_t nMiierr;    /**< This field counts the number of frames that started with a valid start sequence (preamble plus SOF delimiter byte) but terminated with the MII error input being asserted. The field will not wrap. */
} SJA1105P_macLevelErrors_t;

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_get64bitEtherStatCounter(SJA1105P_etherStat64_t counterName, uint64_t *p_counterValue, uint8_t port, SJA1105P_etherStatDirection_t direction);
extern uint8_t SJA1105P_get32bitEtherStatCounter(SJA1105P_etherStat32_t counterName, uint32_t *p_counterValue, uint8_t port, SJA1105P_etherStatDirection_t direction);

extern uint8_t SJA1105P_getMacErrors(SJA1105P_macLevelErrors_t *p_macLevelErrors, uint8_t port);

extern uint8_t SJA1105P_getTemperature(SJA1105P_tempThreshold_t a_temperature[SJA1105P_N_SWITCHES]);

#endif /* NXP_SJA1105P_DIAGNOSTICS_H */
