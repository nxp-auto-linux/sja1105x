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
* \file NXP_SJA1105_diagnostics.h
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Obtain diagnostic information on the switch
*
*****************************************************************************/

#ifndef NXP_SJA1105_DIAGNOSTICS_H
#define NXP_SJA1105_DIAGNOSTICS_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_config.h"
#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef enum 
{
	e_etherStat32_N_NOT_REACH,     /**< number of frames that produced a match in the L2 Lookup table since power on or reset, but were not routed to the port this counter is assigned to because the port is not reachable for the respective ingress port as per REACH_PORT in the L2 Forwarding table */
	e_etherStat32_N_EGR_DISABLED,  /**< number of frames that were not routed to the port this counter is assigned to, since power-on or reset, because the port was down */
	e_etherStat32_N_PART_DROP,     /**< number of frames dropped on ingress because the respective memory partition of the port (as defined per PARTITION of the L2 Policing table) had no space left after power-on or reset */
	e_etherStat32_N_QFULL,         /**< number of frames dropped on egress because the respective priority queue of the destination port (as defined per VLAN_PMAP of the L2 Forwarding table) did not have any space left since power on or reset */
	e_etherStat32_N_POLERR,        /**< number of frames dropped based on the L2 policing operation since power on or reset */
	e_etherStat32_N_CRCERR,        /**< number of frames that had a receive-side CRC error on this port since power on or reset */
	e_etherStat32_N_SIZEERR,       /**< number of frames received since power on or reset with an invalid length (2 K bytes or more or the length contained in the Type/Length field of the frame did not match the actual length) as well as frames received while ingress traffic was disabled */
	e_etherStat32_N_VLANERR,       /**< number of frames dropped since power on or reset because the VLAN ID was either not found in the VLAN Lookup table, the respective port is not listed in the VMEMB_PORT vector of the configured VLANID, or a legal or illegal double-tagged frame was received while double-tagged traffic was not allowed */
	e_etherStat32_N_N664ERR        /**< number of frames dropped since power-on or reset because they had an EtherType field other than 800h while the DRPNONA664 flag was set for the respective port in the MAC Configuration table, they were not tagged while untagged traffic was not allowed (flag at position 4 of PORTST of the MAC Configuration Table), or that were not routed to any destination (because destination ports were down) */
} SJA1105_etherStat32_t;  /**< 32-bit Ethernet statistic counters */

typedef enum 
{
	e_etherStat64_N_TXBYTE,  /**< number of octets transmitted */
	e_etherStat64_N_TXFRM,   /**< number of frames transmitted */
	e_etherStat64_N_RXBYTE,  /**< number of octets received */
	e_etherStat64_N_RXFRM    /**< number of frames received */
} SJA1105_etherStat64_t;  /**< 64-bit Ethernet statistic counters */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105_get64bitEtherStatCounter(SJA1105_etherStat64_t counterName, uint64_t *p_counterValue, uint8_t port);
extern uint8_t SJA1105_get32bitEtherStatCounter(SJA1105_etherStat32_t counterName, uint32_t *p_counterValue, uint8_t port);

extern uint8_t SJA1105_getTemperature(SJA1105_tempThreshold_t a_temperature[SJA1105_N_SWITCHES]);

#endif /* NXP_SJA1105_DIAGNOSTICS_H */
