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
* \file NXP_SJA1105_cbs.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Configuration of the Credit Based Shaper (CBS) blocks
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105_cbs.h"
#include "NXP_SJA1105_config.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define MAX_CREDIT 0x7FFFFFFFU  /* maximum 32-bit value represented in 2s complement */
#define MIN_CREDIT 0x80000000U  /* minimum 32-bit value represented in 2s complement */

#define MEDIA_SPECIFIC_FRAMING_OVERHEAD 22U  /* Assumption: 14-octet IEEE 802.3 header, 4-octet IEEE 802.1Q priority/VID Tag, 4-octet CRC - Preamble and IFG are not accounted for in this CBS implementation */

#define MEGA 1000000U
#define BYTE 8U
 
/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static uint16_t g_shaperAllocated[SJA1105_N_SWITCHES] = {0};        /* each bit specifies if a shaper ID is allocated or not */
static uint32_t g_idleSlope[SJA1105_N_SWITCHES][SJA1105_N_SHAPER];  /* (B/s) Shadow copy of the idleSlope configurations */
static uint8_t  g_port[SJA1105_N_SWITCHES][SJA1105_N_SHAPER];       /* port to which the shaper is configured */
static uint8_t  g_vlanPrio[SJA1105_N_SWITCHES][SJA1105_N_SHAPER];   /* vlanPrio of the shaper */

static const uint16_t gk_classMeasurementInterval[8] =
{
	0,
	0,
	0,
	0,
	SJA1105_CLASS_D_MEASUREMENT_INTERVAL,
	SJA1105_CLASS_C_MEASUREMENT_INTERVAL,
	SJA1105_CLASS_B_MEASUREMENT_INTERVAL,
	SJA1105_CLASS_A_MEASUREMENT_INTERVAL
};
 
/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t allocateCbs(uint8_t switchId);
static void deallocateCbs(uint8_t physicalShaperId, uint8_t switchId);
static uint8_t getShaperId(uint8_t port, uint8_t switchId, uint8_t vlanPrio);

static uint32_t calculateIdleSlope(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint16_t classMeasurementInterval);
static uint32_t calculateSendSlope(uint32_t idleSlope, uint8_t physicalPort, uint8_t switchId);
static void getPhysicalShaperId(uint8_t logicalShaperId, uint8_t *p_physicalShaperId, uint8_t *p_switchId);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Register resources for a stream with the credit based shaper algorithm
*
* \param[in]  maxFrameSize (B) Maximum frame size that the Talker will produce, excluding any overhead for media-specific framing
* \param[in]  maxIntervalFrames Maximum number of frames that the Talker may transmit in one class measurement interval
* \param[in]  port Port at which the CBS will be instantiated
* \param[in]  vlanPrio Priority to which the CBS will be applied
*
* \return uint8_t: returns the shaper ID which was used. 
*/
extern uint8_t SJA1105_registerStreamToCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret = 0;
	uint8_t logicalShaperId;
	uint8_t physicalShaperId;
	SJA1105_creditBasedShaperParameters_t cbsParameters;
	SJA1105_port_t physicalPort;

	if (SJA1105_getPhysicalPort(port, &physicalPort) == 0U)
	{  /* valid port */
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);
		if (gk_classMeasurementInterval[vlanPrio] > 0U)
		{
			cbsParameters.idleSlope = calculateIdleSlope(maxFrameSize, maxIntervalFrames, gk_classMeasurementInterval[vlanPrio]);
			if (logicalShaperId != SJA1105_INVALID_SHAPER_ID)
			{  /* a shaper already exists, update slope values */
				physicalShaperId = logicalShaperId % SJA1105_N_SHAPER;
				cbsParameters.idleSlope += g_idleSlope[physicalPort.switchId][physicalShaperId];
			}
			cbsParameters.sendSlope = calculateSendSlope(cbsParameters.idleSlope, physicalPort.physicalPort, physicalPort.switchId);
			cbsParameters.creditHi  = MAX_CREDIT;
			cbsParameters.creditLo  = MIN_CREDIT;

			ret = SJA1105_configCbs(cbsParameters, port, vlanPrio);
		}
	}
	
	return ret;
}

/**
* \brief Deregister resources for a stream with the credit based shaper algorithm
*
* \param[in]  maxFrameSize (B) Maximum frame size that the Talker will produce, excluding any overhead for media-specific framing
* \param[in]  maxIntervalFrames Maximum number of frames that the Talker may transmit in one class measurement interval
* \param[in]  port Port at which the CBS will be instantiated
* \param[in]  vlanPrio Priority to which the CBS will be applied
*
* \return uint8_t: returns 0 upon success, else failed
*/
extern uint8_t SJA1105_deregisterStreamFromCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret = 1;
	uint8_t logicalShaperId;
	uint8_t physicalShaperId;
	SJA1105_creditBasedShaperParameters_t cbsParameters;
	SJA1105_port_t physicalPort;

	if (SJA1105_getPhysicalPort(port, &physicalPort) == 0U)
	{
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);
		physicalShaperId = logicalShaperId % SJA1105_N_SHAPER;

		if (logicalShaperId != SJA1105_INVALID_SHAPER_ID)
		{  /* matching shaper found - proceed to update */
			cbsParameters.idleSlope = g_idleSlope[physicalPort.switchId][physicalShaperId] - calculateIdleSlope(maxFrameSize, maxIntervalFrames, gk_classMeasurementInterval[vlanPrio]);
			cbsParameters.sendSlope = calculateSendSlope(cbsParameters.idleSlope, physicalPort.physicalPort, physicalPort.switchId);
			cbsParameters.creditHi  = MAX_CREDIT;
			cbsParameters.creditLo  = MIN_CREDIT;
			ret = SJA1105_configCbs(cbsParameters, port, vlanPrio);	
		}
	}

	return ret;
}

/**
* \brief Configure a Credit based shaper
*
* \param[in]  cbsParameters Struct containing the configuration data for the CBS
* \param[in]  port Port at which the CBS will be instantiated
* \param[in]  vlanPrio Priority to which the CBS will be applied
*
* \return uint8_t: returns the shaper ID which was used. If invalid ID is returned, no shaper resources were available.
*/
extern uint8_t SJA1105_configCbs(SJA1105_creditBasedShaperParameters_t cbsParameters, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret;
	uint8_t logicalShaperId =  SJA1105_INVALID_SHAPER_ID;
	SJA1105_creditBasedShapingBlock4Argument_t creditBasedShapingBlock4;
	SJA1105_port_t physicalPort;

	if (SJA1105_getPhysicalPort(port, &physicalPort) == 0U)
	{
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);

		if (logicalShaperId == SJA1105_INVALID_SHAPER_ID)
		{  /* no shaper is allocated for the requested priority and port - Allocate */
			logicalShaperId = allocateCbs(physicalPort.switchId);
		}
		creditBasedShapingBlock4.shaperId = logicalShaperId % SJA1105_N_SHAPER;

		if (logicalShaperId  !=  SJA1105_INVALID_SHAPER_ID)
		{  /* a shaper block is allocated */
			creditBasedShapingBlock4.valid = 1;
			creditBasedShapingBlock4.cbsPort = port;
			creditBasedShapingBlock4.cbsPrio = vlanPrio;
			ret =  SJA1105_setCreditBasedShapingBlock0(cbsParameters.idleSlope, physicalPort.switchId);
			ret += SJA1105_setCreditBasedShapingBlock1(cbsParameters.sendSlope, physicalPort.switchId);
			ret += SJA1105_setCreditBasedShapingBlock2(cbsParameters.creditHi, physicalPort.switchId);
			ret += SJA1105_setCreditBasedShapingBlock3(cbsParameters.creditLo, physicalPort.switchId);
			ret += SJA1105_setCreditBasedShapingBlock4(&creditBasedShapingBlock4, physicalPort.switchId);
			if (ret == 0U)
			{  /* configuration successful - store shaper context information */
				g_idleSlope[physicalPort.switchId][creditBasedShapingBlock4.shaperId] = cbsParameters.idleSlope;
				g_port[physicalPort.switchId][creditBasedShapingBlock4.shaperId]      = port;
				g_vlanPrio[physicalPort.switchId][creditBasedShapingBlock4.shaperId]  = vlanPrio;
			}
		}
	}

	return logicalShaperId;
}

/**
* \brief Remove a Credit based shaper
*
* \param[in]  shaperId ID of the shaper which will be removed
*
* \return uint8_t Returns 0 upon success, else failed
*/
extern uint8_t SJA1105_removeCbs(uint8_t shaperId)
{
	uint8_t ret;
	uint8_t physicalShaperId;
	uint8_t switchId;
	SJA1105_creditBasedShapingBlock4Argument_t creditBasedShapingBlock4;

	getPhysicalShaperId(shaperId, &physicalShaperId, &switchId);

	creditBasedShapingBlock4.shaperId = physicalShaperId;
	creditBasedShapingBlock4.valid = 0;
	creditBasedShapingBlock4.cbsPort = 0;
	creditBasedShapingBlock4.cbsPrio = 0;

	ret = SJA1105_setCreditBasedShapingBlock4(&creditBasedShapingBlock4, switchId);
	if (ret == 0U)
	{  /* successfully deallocated */
		deallocateCbs(physicalShaperId, switchId);
	}
	return ret;
}

/**
* \brief Allocate a shaper
*
* \return uint8_t: returns the ID of the allocated shaper. If invalid ID is returned, no resources shaper resources were available.
*/
static uint8_t allocateCbs(uint8_t switchId)
{
	uint8_t i;
	uint8_t shaperId = SJA1105_INVALID_SHAPER_ID; /* init with invalid ID */
	for(i = 0; i < SJA1105_N_SHAPER; i++)
	{
		if (((g_shaperAllocated[switchId] >> i) & 1U) == 0U)
		{
			/* Unallocated shaper found */
			shaperId = i + (uint8_t) (switchId * SJA1105_N_SHAPER);
			g_shaperAllocated[switchId] |= (uint16_t) ((uint16_t) 1 << i);
			break;
		}
	}
	return shaperId;
}

/**
* \brief Deallocate a shaper
*
* \param[in]  shaperId ID of the shaper that should be deallocated
*/
static void deallocateCbs(uint8_t physicalShaperId, uint8_t switchId)
{
	g_shaperAllocated[switchId] &= (uint16_t) ~((uint16_t) (((uint16_t) 1) << physicalShaperId));
}

/**
* \brief Calculate the idleSlope
*
* \param[in]  maxFrameSize (B) Maximum frame size that the Talker will produce, excluding any overhead for media-specific framing
* \param[in]  maxIntervalFrames Maximum number of frames that the Talker may transmit in one class measurement interval
* \param[in]  classMeasurementInterval (us) Time of the interval restricting maxIntervalFrames in microseconds
*
* \return uint32_t Returns the idleSlope (B/s) calculated
*/
static uint32_t calculateIdleSlope(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint16_t classMeasurementInterval)
{
	uint32_t idleSlope;

	idleSlope = ((uint32_t) maxFrameSize + MEDIA_SPECIFIC_FRAMING_OVERHEAD) * maxIntervalFrames * MEGA / classMeasurementInterval;

	return idleSlope;
}

/**
* \brief Calculate the sendSlope
*
* \param[in]  maxFrameSize (B) Maximum frame size that the Talker will produce, excluding any overhead for media-specific framing
*
* \return uint32_t Returns the sendSlope (B/s) calculated
*/
static uint32_t calculateSendSlope(uint32_t idleSlope, uint8_t physicalPort, uint8_t switchId)
{
	uint32_t sendSlope = 0;
	uint16_t speed;  /* (Mbps) port speed */
	SJA1105_portStatusMiixArgument_t portStatus;

	if (SJA1105_getPortStatusMiix(&portStatus, physicalPort, switchId) == 0U)
	{
		switch (portStatus.speed)
		{
			case e_speed_10_MBPS:  speed = 10;   break;
			case e_speed_100_MBPS: speed = 100;  break;
			default:               speed = 1000; break;  /* invalid configuration */
		}
		sendSlope = (speed * (MEGA / BYTE)) - idleSlope;
	}

	return sendSlope;
}

/**
* \brief Get the ID of the shaper block assigned to a priority queue at a port
*
* \param[in]  port Port at which the shaper is searched for
* \param[in]  vlanPrio Priority for which the shaper is configured
*
* \return uint8_t Returns shaper ID. If no matching shaper exists, an invalid shaper ID is returned
*/
static uint8_t getShaperId(uint8_t port, uint8_t switchId, uint8_t vlanPrio)
{
	uint8_t i;
	uint8_t shaperId;
	shaperId = SJA1105_INVALID_SHAPER_ID;

	for (i=0; i < SJA1105_N_SHAPER; i++)
	{
		if ((((g_shaperAllocated[switchId] >> i) & 1U) == 1U)  /* Shaper active */
		 && (g_port[switchId][i] == port)            /* Shaper is configured for matching port */
		 && (g_vlanPrio[switchId][i] == vlanPrio))    /* Shaper is configured for matching priority */
		{  /* Found matching shaper */
			shaperId = i + (uint8_t) (switchId * SJA1105_N_SHAPER);
			break;
		}
	}
	return shaperId;
}

/**
* \brief Get the physical (switch-local) shaper ID based on the logical one
*
* \param[in]  logicalShaperId Logical Shaper ID (globally unique)
* \param[out] p_physicalShaperId Pointer to physical Shaper ID
* \param[out] p_switchId ID of the switch the shaper corresponds to
*
*/
static void getPhysicalShaperId(uint8_t logicalShaperId, uint8_t *p_physicalShaperId, uint8_t *p_switchId)
{
	*p_physicalShaperId = logicalShaperId % SJA1105_N_SHAPER;
	*p_switchId = (uint8_t) ((logicalShaperId - *p_physicalShaperId) / SJA1105_N_SHAPER);
}
