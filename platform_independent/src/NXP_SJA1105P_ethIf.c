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
* \file NXP_SJA1105P_ethIf.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Handling of Ethernet traffic related to the switch
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_ethIf.h"
#include "NXP_SJA1105P_ptp.h"
#include "NXP_SJA1105P_mgmtRoutes.h"
#include "NXP_SJA1105P_utils.h"
#include "NXP_SJA1105P_addressResolutionTable.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define BYTE 8U

#define N_BYTES_MAX_SIZE_ETH_FRAME 1522U
#define N_BYTES_MIN_SIZE_ETH_FRAME 64U

#define SWITCH_RECV_QUEUE_MAX_FRAMES   (SJA1105P_SWITCH_RECV_QUEUE_MEMORY / N_BYTES_MIN_SIZE_ETH_FRAME)    /**< Maximum number of frames in switch receive queue */
#define ENDPOINT_RECV_QUEUE_MAX_FRAMES (SJA1105P_ENDPOINT_RECV_QUEUE_MEMORY / N_BYTES_MIN_SIZE_ETH_FRAME)  /**< Maximum number of frames in endpoint receive queue */

/* Ethernet frame layout */
#define N_BYTES_MAC_ADDR 6U
#define N_BYTES_VLAN_TAG 4U
#define N_BYTES_ETH_TYPE 2U
#define BYTE_DST_MAC_ADDR_START 0U
#define BYTE_SRC_PORT 2U
#define BYTE_SWITCH_ID 1U
#define BYTE_ORIG_DSTMAC1 (uint8_t) (BYTE_DST_MAC_ADDR_START + 3U)
#define BYTE_ORIG_DSTMAC2 (uint8_t) (BYTE_DST_MAC_ADDR_START + 4U)
#define BYTE_SRC_MAC_ADDR_START 6U
#define BYTE_VLAN_TAG_START 12U
#define BYTE_ETH_TYPE_START 16U
#define VLAN_TAG_TPID_LSB 16U
#define VLAN_TAG_TPID_TAGGED 0x8100U

/* Descriptor Flags */
#define DESC_FLAG_TAKE_TIME_STAMP_MASK 1U  /**< Tx only: Mask for the flag to take a timestamp */
#define DESC_FLAG_META_FRAME_MASK      1U  /**< Rx only: Mask for the flag indicating the frame is a meta frame */

#define L1_OVERHEAD 20U  /* L1 overhead in Bytes compared to L2 frame. Needed for timestamp correction at host port */

/******************************************************************************
* INTERNAL TYPE DEFINITIONS
*****************************************************************************/

typedef struct
{
	uint64_t dstMacAddress;  /**< Destination MAC Address */
	uint64_t srcMacAddress;  /**< Source MAC Address */
	uint32_t vlanTag;        /**< 802.1Q Tag */
	uint16_t ethType;        /**< Ethertype - Contains frame length of vlanFrame==0 */
	uint8_t  taggedFrame;    /**< Indicates that the frame is a VLAN-tagged Frame (1). */
} ethHeader_t;

typedef struct
{
	uint32_t timeStampL;                  /**< Contains the lower 24 bit of the ingress timestamp */
	uint16_t origDstMacAddressByte1And2;  /**< Byte 2 and 1 of the original source MAC Address */
	uint8_t  srcPort;                     /**< Port at which the frame was trapped */
	uint8_t  switchId;                    /**< ID of the switch where the frame was trapped */
} metaData_t;

typedef struct
{
	uint8_t  trapped;               /**< The frame was trapped */
	uint8_t  filterId;              /**< ID of the filter which triggered the trap */
	uint8_t  accepted;              /**< There is a valid subscription for the Ethertype of the trapped frame */
	uint8_t  inclSrcPort;           /**< The trapped frame has the port and switch ID embedded in the DST MAC Address */
	uint8_t  followedByMetaFrame;   /**< The trapped frame will be followed up by a meta frame */ 
	uint64_t srcMacAddress;         /**< SRC MAC Address of the trapped frame */
	uint16_t ethType;               /**< Ethertype of the trapped frame */
	uint64_t approximateTimeStamp;  /**< Timestamp read from SW. Can be combined with HW timestamp to have complete and accurate information */
} trapInformation_t;

typedef struct
{ 
	uint16_t tail;                  /**< Last occupied byte in the queue. Frames are popped from here. */
	uint16_t head;                  /**< First occupied byte in the queue */
	uint16_t queuedBytes;           /**< Number of bytes occupied in the queue */
	uint8_t  youngestElement;       /**< Index of the younges element in the queue (corresponds to head) */
	uint8_t  queuedElements;        /**< Number of elements buffered in the queue */
	const uint16_t k_nMemoryBytes;  /**< Number of total memory bytes in the queue */
	const uint8_t k_nElements;      /**< Number of total elements in the queue */
	uint8_t  *p_queue;              /**< Memory location of the queue */
	uint16_t *p_lenList;            /**< Memory location of a list containing the length of each element */
} queueMetaData_t;

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

/* Settings */
uint8_t static g_forwardMeta = 0U;

/* Internal ring buffers */
/* Physical memory of the queues */
static uint8_t g_switchRecvQueue[SJA1105P_SWITCH_RECV_QUEUE_MEMORY];
static uint8_t g_endPointRecvQueue[SJA1105P_ENDPOINT_RECV_QUEUE_MEMORY];
/* Auxiliary queues */
static uint16_t g_switchLenList[SWITCH_RECV_QUEUE_MAX_FRAMES];
static SJA1105P_frameDescriptor_t g_switchRecvDescriptorQueue[SWITCH_RECV_QUEUE_MAX_FRAMES];
static uint16_t g_endPointLenList[ENDPOINT_RECV_QUEUE_MAX_FRAMES];
static SJA1105P_frameDescriptor_t g_endPointRecvDescriptorQueue[ENDPOINT_RECV_QUEUE_MAX_FRAMES];
/* Meta information of the queues */
static queueMetaData_t g_switchRecvQueueMetaData =
{
	/*.tail =*/ 0,
	/*.head =*/ (uint16_t) (SJA1105P_SWITCH_RECV_QUEUE_MEMORY - 1U),
	/*.queuedBytes =*/ 0,
	/*.youngestElement =*/ (uint8_t) (SWITCH_RECV_QUEUE_MAX_FRAMES - 1U),
	/*.queuedElements =*/ 0,
	/*.k_nMemoryBytes =*/ (uint16_t) SJA1105P_SWITCH_RECV_QUEUE_MEMORY,
	/*.k_nElements =*/ (uint8_t) SWITCH_RECV_QUEUE_MAX_FRAMES,
	/*.p_queue =*/ g_switchRecvQueue,
	/*.p_lenList =*/ g_switchLenList 
};
static queueMetaData_t g_endPointRecvQueueMetaData =
{
	/*.tail =*/ 0,
	/*.head =*/ (uint16_t) (SJA1105P_ENDPOINT_RECV_QUEUE_MEMORY - 1U),
	/*.queuedBytes =*/ 0,
	/*.youngestElement =*/ (uint8_t) (ENDPOINT_RECV_QUEUE_MAX_FRAMES - 1U),
	/*.queuedElements =*/ 0,
	/*.k_nMemoryBytes =*/ (uint16_t) SJA1105P_ENDPOINT_RECV_QUEUE_MEMORY,
	/*.k_nElements =*/ (uint8_t) ENDPOINT_RECV_QUEUE_MAX_FRAMES,
	/*.p_queue =*/ g_endPointRecvQueue,
	/*.p_lenList =*/ g_endPointLenList 
};

/* Switch Ethernet interface */
static SJA1105P_recvFrameHandler_cb_t gpf_switchRecvFrameHandler = NULL; /**< Function through which frames can be dispatched to the switch */
static uint8_t  g_switchNFrames = 0;
static uint16_t g_switchEthTypeFilter[SJA1105P_N_ETH_TYPE_FILTERS_SWITCH]        = {0};
static uint16_t g_switchEthTypeFilterMask[SJA1105P_N_ETH_TYPE_FILTERS_SWITCH]    = {0};
static uint8_t  g_switchEthTypeFilterEnabled[SJA1105P_N_ETH_TYPE_FILTERS_SWITCH] = {0};

/* Endpoint Ethernet interface */
static SJA1105P_recvFrameHandler_cb_t gpf_endPointRecvFrameHandler = NULL; /**< Function through which frames can be dispatched to the endpoint */
static uint8_t  g_endPointIfActive = 0;    /**< Indicates that the endpoint interface is initialized */
static uint64_t g_endPointMacAddress = 0;  /**< MAC Address of the endpoint */
static uint8_t  g_endPointNFrames = 0;

/* Pysical Ethernet interface function pointers */
static SJA1105P_sendFrame_cb_t gpf_sendFrame_cb = NULL;  /**< Pointer to the function used for sending Ethernet frames to the network */
static SJA1105P_recvFrame_cb_t gpf_recvFrame_cb = NULL;  /**< Pointer to the function used for receiving Ethernet frames to the network */

static uint8_t g_nPendingMetaFrames;

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

/* General Util Functions */
static void decodeEthFrame(const uint8_t *kp_data, ethHeader_t *p_ethHeader);
static void decodeMetaFrame(const uint8_t *kp_data, metaData_t *p_metaData);
static void extractInclMetaData(uint64_t dstMacAddress, metaData_t *p_metaData);
static void correctDstMac(uint16_t origDstMacAddressByte1And2, uint8_t *p_frameBuf);
static void extractFromArray(const uint8_t *kp_data, uint64_t *p_extractedData, uint8_t nBytes, uint8_t startIndex);
static uint8_t checkIfMacFiltered(uint64_t dstMacAddress, uint8_t *p_filterId);

/* Queue Functions */
static uint8_t pushToQueue(queueMetaData_t *p_queueMetaData, uint16_t len, const uint8_t *kp_data);
static uint8_t popFromQueue(queueMetaData_t *p_queueMetaData, uint16_t *p_len, const uint8_t **pkp_data);

/*  Dispatch Functions */
static uint8_t dispatchRecvSwitchFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, const uint8_t *kp_data);
static uint8_t dispatchRecvEndPointFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, const uint8_t *kp_data);

/* Internal traffic handling */
static uint8_t forwardTrappedFrame(const metaData_t *kp_metaData, trapInformation_t *p_trapInformation, SJA1105P_frameDescriptor_t *p_frameDescriptor, uint8_t *p_frameBuf);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Tick function of the Ethernet Interface
*
* This function has to called periodically to enable autonomous operation of ethIf.
*
*/
extern uint8_t SJA1105P_ethIfTick(void)
{
	uint8_t ret;

	ret = SJA1105P_forwardRecvFrames();
	ret += SJA1105P_pollAndDispatchEgressTimeStampsTick();

	return ret;
}

/**
* \brief Register a callback function used to send Ethernet frames
*
* \param[in]  pf_sendFrame_cb Function pointer to the send function
*/
extern void SJA1105P_registerFrameSendCB(SJA1105P_sendFrame_cb_t pf_sendFrame_cb)
{
	gpf_sendFrame_cb = pf_sendFrame_cb;
}

/**
* \brief Register a callback function used to receive Ethernet frames
*
* \param[in]  pf_sendFrame_cb Function pointer to the receive function
*/
extern void SJA1105P_registerFrameRecvCB(SJA1105P_recvFrame_cb_t pf_recvFrame_cb)
{
	gpf_recvFrame_cb = pf_recvFrame_cb;
}

/**
* \brief Check for received Ethernet frames and forward them
*
* This function should be called to trigger the SJA1105P EthIf to forward received
* frames to the higher software layers. It is ideally called every time the
* Ethernet MAC has received a frame.
*
* \return uint8_t: {0: success, else: error}
*/
extern uint8_t SJA1105P_forwardRecvFrames(void)
{
	uint8_t ret = 1;
	uint16_t len;
	uint8_t *p_frameBuf = NULL;
	SJA1105P_frameDescriptor_t *p_recvFrameDescriptor = NULL;
	#if SJA1105P_ETHIF_ZEROCOPY == 0U
		static uint8_t frameBufWaitingMeta[SJA1105P_N_SWITCHES][N_BYTES_MAX_SIZE_ETH_FRAME];
		static SJA1105P_frameDescriptor_t frameDescriptorWaitingMeta[SJA1105P_N_SWITCHES];
	#else  /* Zero-copy */
		static uint8_t *p_frameBufWaitingMeta[SJA1105P_N_SWITCHES];
		static SJA1105P_frameDescriptor_t *p_frameDescriptorWaitingMeta[SJA1105P_N_SWITCHES];
	#endif
	static trapInformation_t trapInformation;
	static trapInformation_t trapInformationWaitingMeta[SJA1105P_N_SWITCHES];
	uint8_t currentFrameIsMetaFrame;
	ethHeader_t ethHeader;
	metaData_t metaData;

	do
	{
		len = gpf_recvFrame_cb((const SJA1105P_frameDescriptor_t **) &p_recvFrameDescriptor, (const uint8_t **) &p_frameBuf);
		if (len == 0U) 
		{
			/* no more frames buffered */
			break;
		}
		/* Frame successfully received */
		ret = 0;
		decodeEthFrame(p_frameBuf, &ethHeader);
		currentFrameIsMetaFrame = 0;
		if (g_nPendingMetaFrames > 0U)
		{  /* check if frame is a meta frame */
			if ((ethHeader.dstMacAddress == SJA1105P_g_avbParameters.dstMeta)
				&& (ethHeader.srcMacAddress == SJA1105P_g_avbParameters.srcMeta)
				&& (ethHeader.taggedFrame == 0U)
				&& (ethHeader.ethType == SJA1105P_META_FRAME_ETH_TYPE))
			{
				currentFrameIsMetaFrame = 1;
			}
		}

		if (currentFrameIsMetaFrame == 0U)
		{  /* This is a regular frame */
			/* determine if frame was trapped */
			trapInformation.trapped = checkIfMacFiltered(ethHeader.dstMacAddress, &trapInformation.filterId);

			if (trapInformation.trapped == 1U)
			{
				trapInformation.ethType = ethHeader.ethType;
				/* Check if meta frame follows and whether the source port was included in the trapped frame */
				trapInformation.followedByMetaFrame = 0;
				if (SJA1105P_g_generalParameters.sendMeta[trapInformation.filterId] == 1U)
				{
					trapInformation.followedByMetaFrame = 1;
					/* a meta frame will follow */
					trapInformation.inclSrcPort = SJA1105P_g_generalParameters.inclSrcpt[trapInformation.filterId];
					trapInformation.srcMacAddress = ethHeader.srcMacAddress;
					/* immediately read out the switch timestamp to regenerate the complete timestamp with the meta frame later on */
					ret = SJA1105P_getPtpClk(&(trapInformation.approximateTimeStamp));
				}
				else
				{
					trapInformation.approximateTimeStamp = 0;  /* No timestamp was recorded */
				}

				/* Forward trapped frame */
				if (trapInformation.followedByMetaFrame == 1U)
				{
					if (g_nPendingMetaFrames < SJA1105P_N_SWITCHES)
					{  /* remember the frame while waiting for the meta frame */

						#if SJA1105P_ETHIF_ZEROCOPY == 0U
							for (i = 0; i < p_recvFrameDescriptor->len; i++)
							{
								frameBufWaitingMeta[g_nPendingMetaFrames][i] = p_frameBuf[i];
							}
							frameDescriptorWaitingMeta[g_nPendingMetaFrames] = *p_recvFrameDescriptor;
						#else  /* No copy, just maintain the pointer */
							p_frameBufWaitingMeta[g_nPendingMetaFrames] = p_frameBuf;
							p_frameDescriptorWaitingMeta[g_nPendingMetaFrames] = p_recvFrameDescriptor;
						#endif
						trapInformationWaitingMeta[g_nPendingMetaFrames] = trapInformation;
						/* No immediate forwarding, will be forwarded once meta frame arrives */
						g_nPendingMetaFrames++;
					}
					else
					{
						/* More more trapped frames can be buffered
						 * Either a meta frame was lost, or a trapped
						 * spurious frame was decoded as trapped frame */
						 ret = 1; /* return with failure */
					}
				}
				else
				{  /* No meta frame will follow. Frame can directly be forwarded */
					if (trapInformation.inclSrcPort == 1U)
					{
						extractInclMetaData(ethHeader.dstMacAddress, &metaData);
					}
					ret += forwardTrappedFrame(&metaData, &trapInformation, p_recvFrameDescriptor, p_frameBuf);
				}
			}
			else
			{
				/* Frame reached the host port without trapping. Forward to endpoint interface */
				ret = dispatchRecvEndPointFrame(p_recvFrameDescriptor, p_frameBuf);
			}
		}
		else
		{  /* this frame is a meta frame */
			/* Decode meta frame and retrieve timestamp */
			decodeMetaFrame(p_frameBuf, &metaData);
			g_nPendingMetaFrames--; /* meta frame received - no longer waiting for this one */

			#if SJA1105P_ETHIF_ZEROCOPY == 0U			
				ret += forwardTrappedFrame(&metaData, &trapInformationWaitingMeta[g_nPendingMetaFrames], &frameDescriptorWaitingMeta[g_nPendingMetaFrames], frameBufWaitingMeta[g_nPendingMetaFrames]);
			#else
				ret += forwardTrappedFrame(&metaData, &trapInformationWaitingMeta[g_nPendingMetaFrames], p_frameDescriptorWaitingMeta[g_nPendingMetaFrames], p_frameBufWaitingMeta[g_nPendingMetaFrames]);
			#endif
			if (g_forwardMeta == 1U)
			{  /* meta frame should be forwarded */
				p_recvFrameDescriptor->flags = DESC_FLAG_META_FRAME_MASK;  /* Mark frame as meta frame */
				ret += dispatchRecvSwitchFrame(p_recvFrameDescriptor, p_frameBuf);
			}
		}
	}
	while (ret == 0U);  /* continue as long as no errors occur */
	return ret;
}


/* Switch Ethernet Interface */

/**
* \brief Initialize the Ethernet Interface of the switch
*
* \param [in]  kp_switchEthIfConfig Configuration parameters
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105P_initSwitchEthIf(const SJA1105P_switchEthIfConfig_t *kp_switchEthIfConfig)
{
	g_forwardMeta = kp_switchEthIfConfig->forwardMeta;
	return 0;
}

/**
* \brief Set up a filter to subscribe to traffic based on the Eth Type
*
* The filter consists of an Eth Type value and a bit mask to allow don't cares
*
* \param[in]  ethType Eth Type
* \param[in]  ethTypeMask Eth Type mask (0s represent don't cares)
* \param[out] p_filterId ID of the filter used
*
* \return uint8_t: {0: subscription successfully set up, else: no more filters can be configured}
*/
extern uint8_t  SJA1105P_subscribeEthTypeForSwitchIf(uint16_t ethType, uint16_t ethTypeMask, uint8_t *p_filterId)
{
	uint8_t ret = 1;
	uint8_t i;

	/* find filter disabled filter */
	for (i=0; i<SJA1105P_N_ETH_TYPE_FILTERS_SWITCH; i++)
	{
		if (g_switchEthTypeFilterEnabled[i] == 0U)
		{  /* filter found - setup end return */
			g_switchEthTypeFilter[i] = ethType;
			g_switchEthTypeFilterMask[i] = ethTypeMask;
			g_switchEthTypeFilterEnabled[i] =  1;
			*p_filterId = i;
			ret = 0;
			break;
		}
	}
	return ret;
}

/**
* \brief Receive N Ethernet frames from switch ports using a frame handler function
*
* This function provides means to read a frame that was captured in the switch.
* If configured, the frame is accompanied by an ingress timestamp.
*
* \param[in]  nFrames Number of frames that will be delivered. If 0, the loop will not break and frames are delivered indefinitely.
* \param[in]  pf_frameHandler Callback function to which the frames will be delivered
*/
extern void SJA1105P_recvSwitchFrameLoop(uint8_t nFrames, SJA1105P_recvFrameHandler_cb_t pf_frameHandler)
{
	uint16_t len;
	uint8_t poppedElement;
	const uint8_t *kp_data;

	gpf_switchRecvFrameHandler = pf_frameHandler;
	g_switchNFrames = nFrames;

	/* If frames are buffered, dispatch these to the frame handler */
	while ((gpf_switchRecvFrameHandler != NULL) && (g_switchRecvQueueMetaData.queuedElements > 0U))
	{
		poppedElement = popFromQueue(&g_switchRecvQueueMetaData, &len, &kp_data);
		if (dispatchRecvSwitchFrame(&g_switchRecvDescriptorQueue[poppedElement], kp_data) != 0U)
		{  /* something went wrong, stop the receive loop */
			break;
		}
	}
	/* Either nFrames have been returned or no more frames are buffered */
}

/**
* \brief Receive Ethernet frame from switch ports
*
* This function provides means to read a frame that was captured in the switch.
* If configured, the frame is accompanied by an ingress timestamp.
*
* \param[inout] pkp_frameDescriptor Double pointer to a descriptor containing meta information
* \param[inout] pkp_data Double pointer to the memory location of the received frame
*
* \return uint16_t: Returns the length of the received frame in bytes. On error, 0
*/
extern uint16_t SJA1105P_recvSwitchFrame(const SJA1105P_frameDescriptor_t **pkp_frameDescriptor, const uint8_t **pkp_data)
{
	uint8_t element;
	uint16_t len = 0;
	
	if (g_switchRecvQueueMetaData.queuedElements > 0U)
	{
		element = popFromQueue(&g_switchRecvQueueMetaData, &len, pkp_data);
		*pkp_frameDescriptor = &g_switchRecvDescriptorQueue[element];
	}
	return len;
}

/**
* \brief Transmit a frame through a specified list of switch ports.
*
* Optionally, an egress timestamp can be recorded. With the returned timestamp
* index, the timestamp can be read after transmission.
*
* \param[in]  kp_frameDescriptor Descriptor containing meta information
* \param[in]  p_data Memory location of the frame to be transmitted
* \param[out] p_timeStampIndex Index of the timestamps which are used on the egress port
*
* \return uint8_t: {0: send successful, else: send failed}
*/
extern uint8_t SJA1105P_sendSwitchFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, uint8_t *p_data, uint8_t *p_timeStampIndex)
{
	uint8_t ret = ~((uint8_t) 0);
	SJA1105P_mgmtRoute_t mgmtRoute;
	ethHeader_t ethHeader;

	/* set up managment route */
	decodeEthFrame(p_data, &ethHeader);
	mgmtRoute.macaddr   = ethHeader.dstMacAddress;
	mgmtRoute.destports = kp_frameDescriptor->ports;
	if (SJA1105P_setupMgmtRoute(&mgmtRoute, (uint8_t) (kp_frameDescriptor->flags & DESC_FLAG_TAKE_TIME_STAMP_MASK), p_timeStampIndex) == 0U)
	{  /* Management Route setup successfully */
		while (gpf_sendFrame_cb(kp_frameDescriptor, p_data) != 0U){};
		/* TODO: solution without loop, requires destruction of mgmt route when sendFrame fails */
		ret = 0;
	}
	return ret;
}

/* Endpoint Ethernet Interface */

/**
* \brief Initialize endpoint Ethernet interface
*
* After initialization, Ethernet frames with a destination address
* corresponding to macAddress will be received at the endpoint
* interface. 
*
* \param[in]  macAddress MAC Address of the endpoint
*
* \return uint8_t: {0: successfully initialized, else: error}
*/
extern uint8_t SJA1105P_initEndPointEthIf(uint64_t macAddress)
{
	uint8_t ret = 0;
	SJA1105P_addressResolutionTableEntry_t arlEntry;
	SJA1105P_port_t physicalPort;
	uint8_t hostPort;  /* logical index of the host port */

	if (g_endPointIfActive == 0U)
	{
		g_endPointMacAddress = macAddress;
		g_endPointIfActive = 1U;

		physicalPort.physicalPort = SJA1105P_g_generalParameters.hostPort[SJA1105P_MASTER_SWITCH];
		physicalPort.switchId     = SJA1105P_MASTER_SWITCH;
		SJA1105P_getLogicalPort(&hostPort, &physicalPort);
		arlEntry.ports = ((uint16_t) 1) << hostPort;
		arlEntry.enforcePorts = 1;
		arlEntry.dstMacAddress = macAddress;
		arlEntry.vlanId = SJA1105P_g_macConfigurationVlanid;
		
		ret += SJA1105P_addArlTableEntry(&arlEntry);
	}
	else
	{
		ret = 1;
	}
	return ret;
}

/**
* \brief Receive N Ethernet frames for the endpoint using a frame handler function\n
*
* This function provides means to read a frame that was directed to the endpoint.
*
* \param[in]  nFrames Number of frames that will be delivered. If 0, the loop will not break and frames are delivered indefinitely.
* \param[in]  pf_frameHandler Callback function to which the frames will be delivered
*/
extern void SJA1105P_recvEndPointFrameLoop(uint8_t nFrames, SJA1105P_recvFrameHandler_cb_t pf_frameHandler)
{
	uint16_t len;
	uint8_t poppedElement;
	const uint8_t *kp_data;

	gpf_endPointRecvFrameHandler = pf_frameHandler;
	g_endPointNFrames = nFrames;

	/* If frames are buffered, dispatch these to the frame handler */
	while ((gpf_endPointRecvFrameHandler != NULL) && (g_endPointRecvQueueMetaData.queuedElements > 0U))
	{
		poppedElement = popFromQueue(&g_endPointRecvQueueMetaData, &len, &kp_data);
		if (dispatchRecvEndPointFrame(&g_endPointRecvDescriptorQueue[poppedElement], kp_data) != 0U)
		{  /* something went wrong, stop the receive loop */
			break;
		}
	}
	/* Either nFrames have been returned or no more frames are buffered */
}

/**
* \brief Receive an endpoint Ethernet Frame
*
* \param[inout] pkp_frameDescriptor Double pointer to a descriptor containing meta information
* \param[inout] pkp_data Double pointer to the memory location of the received frame
*
* \return uint16_t: Returns the length of the received frame in bytes. On error, returns 0
*/
extern uint16_t SJA1105P_recvEndPointFrame(const SJA1105P_frameDescriptor_t **pkp_frameDescriptor, const uint8_t **pkp_data)
{
	uint16_t len = 0;
	uint8_t poppedElement;

	if (g_endPointRecvQueueMetaData.queuedElements > 0U)
	{
		poppedElement = popFromQueue(&g_endPointRecvQueueMetaData, &len, pkp_data);
		*pkp_frameDescriptor = &g_endPointRecvDescriptorQueue[poppedElement];
	}
	return len;
}

/**
* \brief Transmit an endpoint Ethernet frame
*
* \param[in]  kp_frameDescriptor Pointer to a descriptor containing meta data of the frame
* \param[in]  p_data Memory location of the frame to be transmitted
*
* \return uint8_t: {0: send successful, else: failed}
*/
extern uint8_t SJA1105P_sendEndPointFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, uint8_t *p_data)
{
	uint8_t ret = 0;
	SJA1105P_mgmtRoute_t mgmtRoute;
	SJA1105P_port_t physicalPort;
	uint8_t hostPort;  /* logical index of the host port */
	ethHeader_t ethHeader;
	
	decodeEthFrame(p_data, &ethHeader);
	/* check if the frame should be trapped by the switch */
	if (checkIfMacFiltered(ethHeader.dstMacAddress, NULL) == 1U)
	{
		mgmtRoute.macaddr   = ethHeader.dstMacAddress;
		physicalPort.physicalPort = SJA1105P_g_generalParameters.hostPort[SJA1105P_MASTER_SWITCH];
		physicalPort.switchId     = SJA1105P_MASTER_SWITCH;
		ret = SJA1105P_getLogicalPort(&hostPort, &physicalPort);
		mgmtRoute.destports = (uint16_t) (((uint16_t) 1) << hostPort);
		ret += SJA1105P_setupMgmtRoute(&mgmtRoute, 0, NULL);		
	}

	if (ret == 0U)
	{  /* No errors so far, proceed to send frame */
		while (gpf_sendFrame_cb(kp_frameDescriptor, p_data) != 0U){};
	}
	return ret;
}

/**
* \brief Flush pedning frames buffered by the HAL
*
*/
extern void SJA1105P_flushEthItf(void)
{
	SJA1105P_flushAllMgmtRoutes();
	g_nPendingMetaFrames = 0;
}


/* General Util Functions */

/**
* \brief Decode the header of an Ethernet frame
*
* \param[in]  kp_data Memory location of the frame to be decoded
* \param[out] p_ethHeader Memory location where the Ethernet header will be returned
*
*/
static void decodeEthFrame(const uint8_t *kp_data, ethHeader_t *p_ethHeader)
{
	uint64_t extractedData;
	uint16_t tpid;

	extractFromArray(kp_data, &extractedData, N_BYTES_MAC_ADDR, BYTE_DST_MAC_ADDR_START);
	p_ethHeader->dstMacAddress = (uint64_t) extractedData;

	extractFromArray(kp_data, &extractedData, N_BYTES_MAC_ADDR, BYTE_SRC_MAC_ADDR_START);
	p_ethHeader->srcMacAddress = (uint64_t) extractedData;

	extractFromArray(kp_data, &extractedData, N_BYTES_VLAN_TAG, BYTE_VLAN_TAG_START);
	p_ethHeader->vlanTag = (uint32_t) extractedData;

	extractFromArray(kp_data, &extractedData, N_BYTES_ETH_TYPE, BYTE_ETH_TYPE_START);
	p_ethHeader->ethType = (uint16_t) extractedData;

	tpid = (uint16_t) (p_ethHeader->vlanTag >> (uint8_t) VLAN_TAG_TPID_LSB);
	if (tpid == (uint16_t) VLAN_TAG_TPID_TAGGED)
	{
		p_ethHeader->taggedFrame = 1;
	}
	else
	{
		p_ethHeader->taggedFrame = 0;
		p_ethHeader->ethType     = tpid;
	}
}

/**
* \brief Decode the meta data encapsulated in a meta frame
*
* \param[in]  kp_data Memory location of the meta frame to be decoded
* \param[out] p_metaData Memory location where the meta data will be returned
*
*/
static void decodeMetaFrame(const uint8_t *kp_data, metaData_t *p_metaData)
{
	uint64_t extractedData;

	extractFromArray(kp_data, &extractedData, SJA1105P_META_FRAME_N_BYTES_TS, SJA1105P_META_FRAME_BYTE_TS_START);
	p_metaData->timeStampL = (uint32_t) extractedData;

	extractFromArray(kp_data, &extractedData, SJA1105P_META_FRAME_N_BYTES_DSTMAC, SJA1105P_META_FRAME_BYTE_DSTMAC_START);
	p_metaData->origDstMacAddressByte1And2 = (uint16_t) extractedData;	

	p_metaData->srcPort  = kp_data[SJA1105P_META_FRAME_BYTE_SRC_PORT];
	p_metaData->switchId = kp_data[SJA1105P_META_FRAME_BYTE_SWITCH_ID];
}

/**
* \brief Extract information from a frame (not meta-frame) that had the source port and switch ID embedded
*
* \param[in]  dstMacAddress MAC Address of the received frame
* \param[out] p_metaData Memory location where the meta data will be returned
*/
static void extractInclMetaData(uint64_t dstMacAddress, metaData_t *p_metaData)
{
	p_metaData->srcPort  = (uint8_t) (dstMacAddress >> (BYTE_SRC_PORT * BYTE));
	p_metaData->switchId = (uint8_t) (dstMacAddress >> (BYTE_SWITCH_ID * BYTE));
	p_metaData->timeStampL = 0;  /* No timestamp information available here */
	p_metaData->origDstMacAddressByte1And2 = 0;
}

/**
* \brief Correct the destination MAC address in a trapped frame
* In trapped frames, the source port and device ID is embedded in the destination MAC address.
* The overwritten values are contained in the meta frame and can be reconstructed.
*
* \param[in]     origDstMacAddressByte1And2 Byte 2 and 1 of the original source MAC Address
* \param[inout]  p_frameBuf Memory location of the frame with the DST address to be corrected
*/
static void correctDstMac(uint16_t origDstMacAddressByte1And2, uint8_t *p_frameBuf)
{ 
	p_frameBuf[BYTE_ORIG_DSTMAC1]  = (uint8_t) (origDstMacAddressByte1And2 >> BYTE);
	p_frameBuf[BYTE_ORIG_DSTMAC2]  = (uint8_t) (origDstMacAddressByte1And2);
}

/**
* \brief Check whether the dstMacAddress is covered by one of the filtering rules
*
* \param[in]  dstMacAddress MAC address to be checked
* \param[out] p_filterId 
*
* \return uint8_t Returns 1 if corresponding filter was found, else 0
*/
static uint8_t checkIfMacFiltered(uint64_t dstMacAddress, uint8_t *p_filterId)
{
	uint8_t trapped = 0;
	uint8_t i;

	for (i = 0; i < SJA1105P_N_MACFLTS; i++)
	{
		if ((dstMacAddress & SJA1105P_g_generalParameters.macFlt[i]) == SJA1105P_g_generalParameters.macFltres[i])
		{  /* frame was trapped in the switch */
			trapped  = 1;
			if (p_filterId != NULL)
			{
				*p_filterId = i;
			}
			break;
		}
	}
	return trapped;
}

/**
* \brief Add an element to the queue
*
* \param[in]     len Number of bytes to be added to the queue
* \param[in]     kp_data Memory location of data to be added to the queue
* \param[inout]  p_queueMetaData Memory location of the queue meta data
*
* \return Index of the element added
*/
static uint8_t pushToQueue(queueMetaData_t *p_queueMetaData, uint16_t len, const uint8_t *kp_data)
{
	uint8_t ret = ~((uint8_t) 0);
	uint16_t i;
	uint8_t element;
	/* check if sufficient memory is available */
	if ((uint16_t) (p_queueMetaData->k_nMemoryBytes - (p_queueMetaData->queuedBytes)) >= len)
	{
		if ((p_queueMetaData->queuedElements) < (p_queueMetaData->k_nElements))
		{
			/* Sufficient memory available. Store frame in queue */
			for (i = 0; i < len; i++)
			{
				p_queueMetaData->p_queue[(p_queueMetaData->head + 1U + i) % (p_queueMetaData->k_nMemoryBytes)] = kp_data[i];
			}
			element = (p_queueMetaData->youngestElement + 1U) % (p_queueMetaData->k_nElements);  /* element number of the frame just queued */

			p_queueMetaData->head = (p_queueMetaData->head + len) % (p_queueMetaData->k_nMemoryBytes);
			p_queueMetaData->queuedBytes = (uint16_t) (p_queueMetaData->queuedBytes + len);
			p_queueMetaData->p_lenList[element] = len;

			p_queueMetaData->youngestElement = element;
			p_queueMetaData->queuedElements++;

			ret = element;
		}
	}
	return ret;
}

/**
* \brief Retrieve an element from the queue
*
* \param[out]    p_len Memory location to which the number of bytes popped will be written
* \param[in]     p_data Memory location to which the data will be written
* \param[inout]  p_queueMetaData Memory location of the queue meta data
*
* \return Index of the element popped
*/
static uint8_t popFromQueue(queueMetaData_t *p_queueMetaData, uint16_t *p_len, const uint8_t **pkp_data)
{
	uint8_t elementToBePopped = ~((uint8_t) 0);
	uint16_t len;

	/* check if an element is stored */
	if (p_queueMetaData->queuedElements != 0U)
	{
		/* pop oldest element in the queue */
		elementToBePopped = (p_queueMetaData->youngestElement - p_queueMetaData->queuedElements + 1U) % (p_queueMetaData->k_nElements);
		len = p_queueMetaData->p_lenList[elementToBePopped];
		*p_len = len;
		*pkp_data = &p_queueMetaData->p_queue[(p_queueMetaData->tail) % (p_queueMetaData->k_nMemoryBytes)];
		p_queueMetaData->tail = (p_queueMetaData->tail + len) % p_queueMetaData->k_nMemoryBytes;
		p_queueMetaData->queuedBytes -= len;

		p_queueMetaData->queuedElements--;
	}
	return elementToBePopped;
}

/**
* \brief Dispatch a received frame towards the Switch Receive Ethernet Interface
* 
* If a valid frame handler is in place, the frame will be directly dispatched to it.
* If not, the frame will be dispatched to the internal queue for later forwarding
* 
* \param[in]  kp_frameDescriptor Pointer to the descriptor of the frame to be dispatched
* \param[in]  kp_data Pointer to the data of the frame to be dispatched
* 
* \return uint8_t Return 0 when dispatch is successful. Else, failed, e.g. because buffer is full
*/
static uint8_t dispatchRecvSwitchFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, const uint8_t *kp_data)
{	
	uint8_t ret = 0;
	uint8_t element;

	if (gpf_switchRecvFrameHandler != NULL)
	{  /* dispatch directly to the frame handler */
		gpf_switchRecvFrameHandler(kp_frameDescriptor, kp_data);
		if (g_switchNFrames == 1U)
		{  /* that was the last frame to be returned */
			gpf_switchRecvFrameHandler = NULL;
		}
		if (g_switchNFrames > 0U)
		{
			g_switchNFrames--;
		}
	}
	else
	{  /* push frame to internal queue */
		element = pushToQueue(&g_switchRecvQueueMetaData, kp_frameDescriptor->len, kp_data);
		if (element < g_switchRecvQueueMetaData.k_nElements)
		{
			g_switchRecvDescriptorQueue[element] = *kp_frameDescriptor;
		}
		else
		{  /* no more space available, frame will be dropped */
			ret = 1;
		}
	}
	return ret;
}

/**
* \brief Dispatch a received frame towards the Endpoint Receive Ethernet Interface
* 
* If a valid frame handler is in place, the frame will be directly dispatched to it.
* If not, the frame will be dispatched to the internal queue for later forwarding
* 
* \param[in]  kp_frameDescriptor Pointer to the descriptor of the frame to be dispatched
* \param[in]  kp_data Pointer to the data of the frame to be dispatched
* 
* \return uint8_t Return 0 when dispatch is successful. Else, failed, e.g. because buffer is full
*/
static uint8_t dispatchRecvEndPointFrame(const SJA1105P_frameDescriptor_t *kp_frameDescriptor, const uint8_t *kp_data)
{	
	uint8_t ret = 0;
	uint8_t element;

	if (gpf_endPointRecvFrameHandler != NULL)
	{  /* dispatch directly to the frame handler */
		gpf_endPointRecvFrameHandler(kp_frameDescriptor, kp_data);
		if (g_endPointNFrames == 1U)
		{  /* that was the last frame to be returned */
			gpf_endPointRecvFrameHandler = NULL;
		}
		if (g_endPointNFrames > 0U)
		{
			g_endPointNFrames--;
		}
	}
	else
	{  /* push frame to internal queue */
		element = pushToQueue(&g_endPointRecvQueueMetaData, kp_frameDescriptor->len, kp_data);
		if (element < g_endPointRecvQueueMetaData.k_nElements)
		{
			g_endPointRecvDescriptorQueue[element] = *kp_frameDescriptor;
		}
		else
		{  /* no more space available, frame will be dropped */
			ret = 1;
		}
	}
	return ret;
}

/**
* \brief Handling of a frame that was trapped within the switch
* 
* Decision can be taken to forward the frame to switchIf, endpointIf, or to loop it back to the network
* 
* \param[in]  kp_metaData meta information of the trapped frame
* \param[in]  p_trapInformation Contains information on why the specific frame was trapped
* \param[in]  p_frameDescriptor Pointer to the descriptor of the trapped frame to be forwarded
* \param[in]  p_frameBuf Pointer to the data of the trapped frame to be forwarded
* 
* \return uint8_t Return 0 when dispatch is successful. Else, failed, e.g. because buffer is full
*/
static uint8_t forwardTrappedFrame(const metaData_t *kp_metaData, trapInformation_t *p_trapInformation, SJA1105P_frameDescriptor_t *p_frameDescriptor, uint8_t *p_frameBuf)
{
	uint8_t ret = 0;
	uint8_t i;

	if (p_trapInformation->inclSrcPort == 1U)
	{  /* The DST MAC has to be corrected */
		correctDstMac(kp_metaData->origDstMacAddressByte1And2, p_frameBuf);
	}

	/* Forward frame to destination */
	if (((kp_metaData->srcPort == SJA1105P_g_generalParameters.hostPort[SJA1105P_MASTER_SWITCH])     /* If received on host port */
	    && (kp_metaData->switchId == SJA1105P_g_generalParameters.switchId[SJA1105P_MASTER_SWITCH])  /* of the switch connected to the host */
	    && (p_trapInformation->srcMacAddress == g_endPointMacAddress)  /* and source is endpoint */
	    && ( g_endPointIfActive == 1U))  /* -> forward to switch */
	    || (kp_metaData->srcPort != SJA1105P_g_generalParameters.hostPort[SJA1105P_MASTER_SWITCH]))  /* If received on non-host port -> forward to switch */
	{  /* This is a frame intended for the switch interface */
		/* check if a valid subscription exists for the frame */
		for (i = 0; i < SJA1105P_N_ETH_TYPE_FILTERS_SWITCH; i++)
		{
			p_trapInformation->accepted = 0;
			if ((g_switchEthTypeFilterEnabled[i] == 1U)
			     && (   (p_trapInformation->ethType & g_switchEthTypeFilterMask[i])
			         == (g_switchEthTypeFilter[i] & g_switchEthTypeFilterMask[i])))
			{
				p_trapInformation->accepted = 1;
				break;
			}
		}
		if (p_trapInformation->accepted == 1U)
		{  /* This frame passed the Eth Type filtering */
			SJA1105P_reconstructTimeStamp(kp_metaData->timeStampL, &(p_trapInformation->approximateTimeStamp));
			p_trapInformation->approximateTimeStamp -= (uint64_t) SJA1105P_getPhyPropagationDelay(kp_metaData->srcPort, SJA1105P_e_direction_RX);  /* compensate for ingress propagation delay in PHY */
			p_frameDescriptor->rxTimeStampTxPrivate = p_trapInformation->approximateTimeStamp;
			p_frameDescriptor->port = kp_metaData->srcPort;
			ret += dispatchRecvSwitchFrame(p_frameDescriptor, p_frameBuf);
		}
		else
		{  /* This frame did not pass the Eth Type filtering. It has to be looped back. */
			correctDstMac(kp_metaData->origDstMacAddressByte1And2, p_frameBuf);
			p_frameDescriptor->ports = (uint16_t) ~((uint16_t) (((uint16_t) 1U) << kp_metaData->srcPort));  /* forward to all ports except the one where the frame was trapped */
			p_frameDescriptor->flags = 0;
			p_frameDescriptor->rxTimeStampTxPrivate = 0;
			ret += SJA1105P_sendSwitchFrame(p_frameDescriptor, p_frameBuf, NULL);
		}
	}
	else
	{  /* This is a frame intended for the endpoint interface */
		ret += dispatchRecvEndPointFrame(p_frameDescriptor, p_frameBuf);
	}

	return ret;
}

/**
* \brief Extract a value from a byte array.\n
* 
* \param[in]  kp_data Pointer to the data from which the value will be extracted. Is assumed to be Big Endian.
* \param[out] p_extractedData 64-bit unsigned integer containing the value. It is LSb aligned and padded with zeros.
* \param[in]  nBytes Number of Bytes to be extracted
* \param[in]  startIndex Index of the first Byte (MSB) to be extracted
*/
static void extractFromArray(const uint8_t *kp_data, uint64_t *p_extractedData, uint8_t nBytes, uint8_t startIndex)
{
	uint8_t i;

	*p_extractedData = 0;
	for (i = 0; i < nBytes; i++)
	{
		*p_extractedData |= ((uint64_t) kp_data[startIndex + i]) << ((nBytes - i - 1U) * BYTE);
	}
}
