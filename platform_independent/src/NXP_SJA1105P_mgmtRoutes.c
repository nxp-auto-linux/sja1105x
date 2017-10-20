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
* \file NXP_SJA1105P_mgmtRoutes.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Functions configuring Management Routes
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_mgmtRoutes.h"
#include "NXP_SJA1105P_ptp.h"
#include "NXP_SJA1105P_utils.h"
#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_switchCore.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define BYTE 8U
#define MICRO_TO_8NS 125U
#define L1_OVERHEAD 20U  /**< L1 overhead in Bytes compared to L2 frame. Needed for timestamp correction at host port */
#define TRAPPED_FRAME_LENGTH 64U  /**< Used to correct egress timestamps taken at the host port. Valid for gPTP frames */

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static SJA1105P_egressTimeStampHandler_cb_t gpf_egressTimeStampHandler = NULL;
static uint16_t g_egressTimeStampsAllocated[SJA1105P_N_EGR_TIMESTAMPS]  = {0};  /**< each bit specifies if an egress timestamp is allocated or not */
static uint8_t  g_nEgressTimeStampsAllocated[SJA1105P_N_EGR_TIMESTAMPS] = {0};  /**< Number of allocated timestamps for a specific timestamp Index. Corresponds to the sum of bits set in g_egressTimeStampsAllocated */

static uint8_t  g_mgmtRouteActive[SJA1105P_N_SWITCHES] = {0};  /**< each bit specifies if the corresponding mgmt route is currently used */

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t allocateMgmtRoute(uint64_t macaddr, uint8_t lastSwitch, uint8_t *p_mgmtRoutes);
static void    deallocateMgmtRoute(uint8_t mgmtRouteIndex, uint8_t switchId);
static uint8_t allocateTimeStamp(uint16_t destports);
static void    deallocateTimeStamp(uint8_t port, uint8_t timeStampIndex);
static uint8_t syncMgmtRoutes(void);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Setup a Management Route
*
* \param[in]  kp_mgmtRoute 
* \param[in]  takeTimeStamp  
* \param[out] p_timeStampIndex Index of the timestamps which are used for the route
*
* \return uint8_t: {0: successful, else: failed}
*/
extern uint8_t SJA1105P_setupMgmtRoute(const SJA1105P_mgmtRoute_t *kp_mgmtRoute, uint8_t takeTimeStamp, uint8_t *p_timeStampIndex)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t switches;
	uint8_t lastSwitch = 0;
	uint8_t timeStampIndex = 0;
	uint8_t physicalDestPorts;
	uint8_t mgmtRouteIndeces[SJA1105P_N_SWITCHES];

	SJA1105P_l2ArtEntryArgument_t entry = {0};
	SJA1105P_l2AddressLookupTableControlSetArgument_t control;

	SJA1105P_getSwitchesFromPorts(kp_mgmtRoute->destports, &switches);
	/* find the last switch in the cascade which is part of the route */
	for  (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		if (((switches >> switchId) & 1U) == 1U)
		{
			lastSwitch = switchId;
		}
	}

	if (allocateMgmtRoute(kp_mgmtRoute->macaddr, lastSwitch, mgmtRouteIndeces) == 0U)
	{  /* Management Route allocated */
		if (takeTimeStamp == 1U)
		{
			timeStampIndex = allocateTimeStamp(kp_mgmtRoute->destports);
			*p_timeStampIndex = timeStampIndex;
			if (timeStampIndex >= SJA1105P_N_EGR_TIMESTAMPS)
			{  /* timestamp resources could not be allocated */
				ret = 1;
				for (switchId = 0; switchId <= lastSwitch; switchId++)
				{
					deallocateMgmtRoute(mgmtRouteIndeces[switchId], switchId);  /* No longer needed */
				}
			}
		}	
	}
	else
	{  /* No resources for management route available */
		ret = 1;
	}
	if (ret == 0U)
	{  /* All resources available. Setup management route */
		control.valid     = 1;
		control.rdwrset   = 1;
		control.hostCmd   = SJA1105P_e_hostCmd_WRITE;
		control.valident  = 1;
		control.mgmtroute = 1;
		control.lockeds = 0;  /* not relevant for management routes */

		entry.macaddr = kp_mgmtRoute->macaddr;
		entry.enfport = 1;
		entry.vlanid  = (uint16_t) ((uint8_t) ((uint16_t) timeStampIndex << 1U));
		entry.vlanid |= (uint16_t) (1U & (uint16_t) takeTimeStamp);
		for (switchId = 0; switchId <= lastSwitch; switchId++)
		{
			if (mgmtRouteIndeces[switchId] != SJA1105P_N_MGMT_ROUTES)
			{
				SJA1105P_getPhysicalPortVector(kp_mgmtRoute->destports, switchId, &physicalDestPorts);
				entry.destports = physicalDestPorts;
				entry.index     = mgmtRouteIndeces[switchId];
				if (switchId < lastSwitch)
				{  /* management route has to be extended to cascaded switch */
					entry.destports |= (uint8_t) (((uint8_t) 1) << SJA1105P_g_generalParameters.cascPort[switchId]);
				}

				ret += SJA1105P_setL2ArtEntry(&entry, switchId);
				ret += SJA1105P_setL2AddressLookupTableControl(&control, switchId);
			}
		}
	}
	return ret;
}

/**
* \brief Check for recorded egress timestamps and dispatch them
*
* Egress timestamps have to be polled. Therefore, this function should be called cyclic.
*
* \param[in]  pf_egressTimeStampHandler Callback function for handling egress timestamps
*/
extern uint8_t SJA1105P_pollAndDispatchEgressTimeStampsTick(void)
{
	uint8_t  timeStampStatus;
	uint8_t  ret = 0;
	uint8_t  port;
	uint8_t  timeStampIndex;
	uint64_t timeStamp;

	if (gpf_egressTimeStampHandler != NULL)
	{
		for (timeStampIndex = 0; timeStampIndex < SJA1105P_N_EGR_TIMESTAMPS; timeStampIndex++)
		{
			if (g_nEgressTimeStampsAllocated[timeStampIndex] > 0U)
			{
				for (port = 0; port < SJA1105P_N_PORTS; port++)
				{
					if ((g_egressTimeStampsAllocated[timeStampIndex] & ((uint16_t) (((uint16_t) 1U) << port))) != 0U)
					{
						timeStampStatus = SJA1105P_getEgressTimeStamp(&timeStamp, port, timeStampIndex);
						if (timeStampStatus == 0U)
						{  /* timestamp was read - dispatch it */
							gpf_egressTimeStampHandler(timeStamp, port, timeStampIndex);
						}
						else
						{
							if ((uint8_t) (~timeStampStatus) != 0U)
							{  /* an error occured while trying to read the timestamp */
								ret++;
							}
						}
					}
				}
			}
		}
	}
	else
	{  /* no callback has been registered yet to retrieve the timestamp */
		ret = 1;
	}
	return ret;
}

/**
* \brief Register a callback function that is used to dispatch egress timestamps
*
* \param[in]  pf_egressTimeStampHandler Callback function for handling egress timestamps
*/
extern void SJA1105P_registerEgressTimeStampHandler(SJA1105P_egressTimeStampHandler_cb_t pf_egressTimeStampHandler)
{
	gpf_egressTimeStampHandler = pf_egressTimeStampHandler;
}

/**
* \brief Get the egress timestamp specified by timestamp index at one port
*
* \param[out] p_timeStamp Memory location of the returned timestamp defined in multiples of 8 ns
* \param[in]  port Port at which the timestamp should be read
* \param[in]  timeStampIndex Index of the timestamp to be read
*
* \return uint8_t: Returns 0 upon success. Else failed. If no error occurred, but the timestamp was not updated, returns a bit vector of ones
*/
extern uint8_t SJA1105P_getEgressTimeStamp(uint64_t *p_timeStamp, uint8_t port, uint8_t timeStampIndex)
{
	uint8_t ret;
	uint64_t timeStampTmp;
	uint32_t timeStampL;
	uint8_t  updated;
	uint32_t timestampCorrection;
	SJA1105P_port_t physicalPort;
	SJA1105P_portStatusMiixArgument_t portStatus;
	uint16_t speed;  /* [Mbps] speed of the port */

	ret  = SJA1105P_getPhysicalPort(port, &physicalPort);
	ret += SJA1105P_getPtpEgress0(&updated, physicalPort.physicalPort, timeStampIndex, physicalPort.switchId);
	if (ret == 0U)
	{  /* PTP status successfully read */
		if (updated == 1U)
		{  /* timestamp can be read */
			ret =  SJA1105P_getPtpClk(&timeStampTmp);
			ret += SJA1105P_getPtpEgress1(&timeStampL, physicalPort.physicalPort, timeStampIndex, physicalPort.switchId);
			SJA1105P_reconstructTimeStamp(timeStampL, &timeStampTmp);
			timeStampTmp += (uint64_t) SJA1105P_getPhyPropagationDelay(port, SJA1105P_e_direction_TX);  /* Tx PHY propagation delay compensation */

			/* Apply correction to faulty timestamps taken at egress to the host port */
			if ((physicalPort.switchId == SJA1105P_MASTER_SWITCH) &&
			    (physicalPort.physicalPort == SJA1105P_g_generalParameters.hostPort[physicalPort.switchId]))
			{
				ret += SJA1105P_getPortStatusMiix(&portStatus, physicalPort.physicalPort, physicalPort.switchId);
				switch (portStatus.speed)
				{
					case SJA1105P_e_speed_1_GBPS:
						speed = 1000;
						break;
					case SJA1105P_e_speed_100_MBPS:
						speed = 100;
						break;
					default:
						speed = 10;
						break;
				}
				timestampCorrection = (L1_OVERHEAD + TRAPPED_FRAME_LENGTH) * BYTE * MICRO_TO_8NS / ((uint32_t) speed);
				timeStampTmp = timeStampTmp - (uint64_t) timestampCorrection;
			}
			
			*p_timeStamp  = timeStampTmp;
			deallocateTimeStamp(port, timeStampIndex);
		}
		else
		{  /* timestamp not yet captured */
			ret = ~ret;
		}
	}
	return ret;
}


/**
* \brief Flush all pending route and egress timestamp entries
*
*/
extern void SJA1105P_flushAllMgmtRoutes(void)
{
	int8_t  mgmtRouteIndex;
	uint8_t switchId;
	uint8_t port;
	uint8_t timeStampIndex;

	for (mgmtRouteIndex = 0; mgmtRouteIndex < SJA1105P_N_MGMT_ROUTES; mgmtRouteIndex++)
	{
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			if ((((uint8_t) (g_mgmtRouteActive[switchId] >> mgmtRouteIndex)) & 1U) == 1U)
			{  /* management route is currently allocated */
				deallocateMgmtRoute(mgmtRouteIndex, switchId);
			}
		}
	}

	for (timeStampIndex = 0; timeStampIndex < SJA1105P_N_EGR_TIMESTAMPS; timeStampIndex++)
	{
		for (port = 0; port < SJA1105P_N_PORTS; port++)
		{
			deallocateTimeStamp(port, timeStampIndex);
		}
	}
}

/**
* \brief Allocate Management Route
* 
* This algorithm is used to allocate a management route.
* It updates, which routes are still active.
* It find the route with the lowest possible index for each requested switch.
* It allocates the resources if it is possible to setup the route in all requested switches.
*
* \param[in]  macaddr MAC address for which the route should be allocated
* \param[in]  lastSwitch Bit vector indicating the switches in which a route is required
* \param[out] p_mgmtRoutes Pointer to list of management routes registered in each switch
*
* \return uint8_t: Returns the 0 if successful, else failed
*/
static uint8_t allocateMgmtRoute(uint64_t macaddr, uint8_t lastSwitch, uint8_t *p_mgmtRoutes)
{
	int8_t  mgmtRouteIndex;
	uint8_t switchId;
	uint8_t ret;
	static uint64_t mgmtRouteMacaddr[SJA1105P_N_SWITCHES][SJA1105P_N_MGMT_ROUTES];

	uint8_t bestMgmtRouteIndex[SJA1105P_N_SWITCHES];  /* set to invalid index */
	uint8_t switchesPossible = 0;  /* Bit vector indicating the switches in which a route is possible */

	/* Update list of active management routes */
	ret = syncMgmtRoutes();

	/* Find optimal management route in each switch */
	for (switchId = 0; switchId <= lastSwitch; switchId++)
	{
		bestMgmtRouteIndex[switchId] = SJA1105P_N_MGMT_ROUTES;

		for (mgmtRouteIndex = ((int8_t) SJA1105P_N_MGMT_ROUTES - 1); mgmtRouteIndex >= 0; mgmtRouteIndex--)
		{
			if ((((uint8_t) (g_mgmtRouteActive[switchId] >> (uint8_t) mgmtRouteIndex)) & 1U) == 0U)
			{  /* management route is available */
				bestMgmtRouteIndex[switchId] = (uint8_t) mgmtRouteIndex;
				switchesPossible |= (uint8_t) (((uint8_t) 1U) << switchId);
			}
			else
			{
				if (mgmtRouteMacaddr[switchId][mgmtRouteIndex] == macaddr)
				{  /* There is an active management route with the same MAC address */
					/* A lower index management route is not possible */
					break;
				}	
			}					
		}
		if (bestMgmtRouteIndex[switchId] == SJA1105P_N_MGMT_ROUTES)
		{  /* no management route could be found */
			/* abort, operation is not possible */
			ret = 1;
			break;
		}

	}

	/* Allocate the resources */
	if (ret == 0U)
	{ /* All Management Routes can be allocated */
		for (switchId = 0; switchId <= lastSwitch; switchId++)
		{
			p_mgmtRoutes[switchId] = bestMgmtRouteIndex[switchId];
			g_mgmtRouteActive[switchId] |= (uint8_t) (((uint8_t) 1) << bestMgmtRouteIndex[switchId]);  /* set corresponding bit high */
			mgmtRouteMacaddr[switchId][bestMgmtRouteIndex[switchId]] = macaddr;
		}
	}

	return ret;
}

/**
* \brief Deallocate Management Route
*
* \param[in]  mgmtRouteIndex
*
*/
static void deallocateMgmtRoute(uint8_t mgmtRouteIndex, uint8_t switchId)
{
	g_mgmtRouteActive[switchId] &= (uint8_t) ~((uint8_t) (((uint8_t) 1) << mgmtRouteIndex));  /* set corresponding bit low */
}

/**
* \brief Allocate resources for egress timestamping
*
* \param[in]  destports Defines the ports (one bit per each port) at which egress timestamps will be taken
*
* \return uint8_t: Returns the timestamp index if successful. If failed, return invalid index.
*/
static uint8_t allocateTimeStamp(uint16_t destports)
{
	uint8_t i;
	uint8_t timeStampIndex = SJA1105P_N_EGR_TIMESTAMPS;  /* Init with invalid timestamp index */
	uint8_t nPorts = 0;

	for (i = 0; i < SJA1105P_N_EGR_TIMESTAMPS; i++)
	{
		if ((g_egressTimeStampsAllocated[i] & destports) == 0U)
		{  /* timestamps available for all destination ports */
			if (timeStampIndex == SJA1105P_N_EGR_TIMESTAMPS)
			{  /* no time stamp was previously found. Take this one. */
				timeStampIndex = i;
			}
			else
			{
				if (g_nEgressTimeStampsAllocated[i] > g_nEgressTimeStampsAllocated[timeStampIndex])
				{
					/* Another available timestamp was already found.
					* This one is chosen, because more timestamps at other ports are allocated here.
					* This reduced fragmentation of the timestamps and increases the chance for timestamp availability. */
					timeStampIndex = i;
				}
			}
		}
	}

	/* determine at how many ports the timestamp will be allocated */
	for (i = 0; i < SJA1105P_N_PORTS; i++)
	{
		if (((destports >> i) & 1U) == 1U)
		{
			nPorts++;
		}
	}

	if (timeStampIndex < SJA1105P_N_EGR_TIMESTAMPS)
	{  /* time stamp can be allocated */
		g_egressTimeStampsAllocated[timeStampIndex] |= destports;
		g_nEgressTimeStampsAllocated[timeStampIndex] += nPorts;
	}

	return timeStampIndex;
}

/**
* \brief Deallocate resources for egress timestamping
*
* \param[in]  timeStampIndex
* \param[in]  port
*/
static void deallocateTimeStamp(uint8_t port, uint8_t timeStampIndex)
{
	g_nEgressTimeStampsAllocated[timeStampIndex]--;
	g_egressTimeStampsAllocated[timeStampIndex] &= (uint16_t) ~((uint16_t) (((uint16_t) 1) << port));  /* set bit corresponding to the port number low */
}

static uint8_t syncMgmtRoutes(void)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t mgmtRouteIndex;
	SJA1105P_l2ArtEntryArgument_t entry = {0};
	SJA1105P_l2AddressLookupTableControlSetArgument_t control = {0};

	control.valid     = 1;
	control.rdwrset   = 0;  /* read operation */
	control.hostCmd   = SJA1105P_e_hostCmd_READ;
	control.mgmtroute = 1;
	for (mgmtRouteIndex = 0; mgmtRouteIndex < SJA1105P_N_MGMT_ROUTES; mgmtRouteIndex++)
	{
		entry.index = mgmtRouteIndex;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			if ((((uint8_t) (g_mgmtRouteActive[switchId] >> mgmtRouteIndex)) & 1U) == 1U)
			{  /* management route is currently allocated */
			   /* check if still active */

				ret += SJA1105P_setL2ArtEntry(&entry, switchId);
				ret += SJA1105P_setL2AddressLookupTableControl(&control, switchId);
				ret += SJA1105P_getL2ArtEntry(&entry, switchId);
				if (entry.enfport == 0U)
				{  /* Management route is no longer active */
					deallocateMgmtRoute(mgmtRouteIndex, switchId);
				}
			}
		}
	}	
	return ret;
}
