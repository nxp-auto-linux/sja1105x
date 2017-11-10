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
* \file NXP_SJA1105P_mgmtRoutes.h
*
* \author NXP Semiconductors
*
* \date 2017-03-02
*
* \brief Functions configuring Management Routes
*
*****************************************************************************/

#ifndef NXP_SJA1105P_MGMT_ROUTES_H
#define NXP_SJA1105P_MGMT_ROUTES_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct 
{
	uint64_t macaddr;    /**< Destination MAC address */
	uint16_t destports;  /**< Defines the ports (one bit per each port) to which frames carrying MACADDR as destination MAC address will be forwarded */
} SJA1105P_mgmtRoute_t;

typedef void (*SJA1105P_egressTimeStampHandler_cb_t)(uint64_t timeStamp, uint8_t port, uint8_t timeStampIndex);  /**< Type of a function called to deliver an egress timestamp */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_setupMgmtRoute(const SJA1105P_mgmtRoute_t *kp_mgmtRoute, uint8_t takeTimeStamp, uint8_t *p_timeStampIndex);

extern uint8_t SJA1105P_pollAndDispatchEgressTimeStampsTick(void);
extern void    SJA1105P_registerEgressTimeStampHandler(SJA1105P_egressTimeStampHandler_cb_t pf_egressTimeStampHandler);
extern uint8_t SJA1105P_getEgressTimeStamp(uint64_t *p_timeStamp, uint8_t port, uint8_t timeStampIndex);

#endif /* NXP_SJA1105P_MGMT_ROUTES_H */
