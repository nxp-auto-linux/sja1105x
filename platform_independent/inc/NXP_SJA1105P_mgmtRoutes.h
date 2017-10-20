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
* \file NXP_SJA1105P_mgmtRoutes.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
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
extern void    SJA1105P_flushAllMgmtRoutes(void);

#endif /* NXP_SJA1105P_MGMT_ROUTES_H */
