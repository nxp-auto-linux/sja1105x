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
* \file NXP_SJA1105P_ethIf.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Handling of Ethernet traffic related to the switch
*
*****************************************************************************/

#ifndef NXP_SJA1105P_ETHIF_H
#define NXP_SJA1105P_ETHIF_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

/******************************************************************************
* Defines
*****************************************************************************/

#define SJA1105P_SWITCH_RECV_QUEUE_MEMORY   1522U  /**< (B) Memory of the switch receive queue in bytes */
#define SJA1105P_ENDPOINT_RECV_QUEUE_MEMORY 1522U  /**< (B) Memory of the endpoint receive queue in bytes */

#define SJA1105P_N_ETH_TYPE_FILTERS_SWITCH  8U

/******************************************************************************
* TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
	uint8_t forwardMeta;  /**< If 1, meta frames will be forwarded to the upper layer. If 0, meta frames are dropped in the ethIf. */
} SJA1105P_switchEthIfConfig_t;

typedef struct
{
	uint64_t rxTimeStampTxPrivate;  /**< Rx: (8 ns) Receive timestamp of the packet defined in multiples of 8 ns -- Tx: Private data which can be passed through the ethIf */
	uint8_t  port;                  /**< Rx (switchIf): Port at which the frame was received -- Tx: reserved */
	uint16_t ports;                 /**< Rx: reserved -- Tx (switchIf): Defines the ports (one bit per each port) to which frames are forwarded */
	uint16_t len;                   /**< (B) total length of the frame in bytes */
	uint8_t  flags;                 /**< Tx (switchIf): bit0: take timestamp -- Rx: bit0: frame is a meta frame, bit1-7: reserved */
} SJA1105P_frameDescriptor_t;  /**< descriptor used within ethIf. Note that some of the field have different meaning depending on the direction. Also, some fields are only available with the switch interface */

typedef uint8_t  (*SJA1105P_sendFrame_cb_t)(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, uint8_t *p_data);             /**< Type of the function called for sending an Ethernet frame */
typedef uint16_t (*SJA1105P_recvFrame_cb_t)(const SJA1105P_frameDescriptor_t **pkp_frameDescriptor, const uint8_t **pkp_data);  /**< Type of the function called for receiving an Ethernet frame */

typedef void (*SJA1105P_recvFrameHandler_cb_t)(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, const uint8_t *kp_data);    /**< Type of a function called on reception of a switch frame */

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_ethIfTick(void);
extern uint8_t SJA1105P_forwardRecvFrames(void);
extern void SJA1105P_flushEthItf(void);

/* Physical Ethernet Interface */
extern void SJA1105P_registerFrameSendCB(SJA1105P_sendFrame_cb_t pf_sendFrame_cb);
extern void SJA1105P_registerFrameRecvCB(SJA1105P_recvFrame_cb_t pf_recvFrame_cb);

/* Switch Ethernet Interface */
extern uint8_t SJA1105P_initSwitchEthIf(const SJA1105P_switchEthIfConfig_t *kp_switchEthIfConfig);

extern uint8_t  SJA1105P_subscribeEthTypeForSwitchIf(uint16_t ethType, uint16_t ethTypeMask, uint8_t *p_filterId);

extern uint16_t SJA1105P_recvSwitchFrame(const SJA1105P_frameDescriptor_t **pkp_frameDescriptor, const uint8_t **pkp_data);
extern void     SJA1105P_recvSwitchFrameLoop(uint8_t nFrames, SJA1105P_recvFrameHandler_cb_t pf_frameHandler);
extern uint8_t  SJA1105P_sendSwitchFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, uint8_t *p_data, uint8_t *p_timeStampIndex);

/* Endpoint Ethernet Interface */
extern uint8_t  SJA1105P_initEndPointEthIf(uint64_t macAddress);

extern uint16_t SJA1105P_recvEndPointFrame(const SJA1105P_frameDescriptor_t **pkp_frameDescriptor, const uint8_t **pkp_data);
extern void     SJA1105P_recvEndPointFrameLoop(uint8_t nFrames, SJA1105P_recvFrameHandler_cb_t pf_frameHandler);
extern uint8_t  SJA1105P_sendEndPointFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, uint8_t *p_data);

#endif /* NXP_SJA1105P_ETHIF_H */
