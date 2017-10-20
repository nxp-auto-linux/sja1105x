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
* \file NXP_SJA1105P_cbs.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Configuration of the Credit Based Shaper (CBS) blocks
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_cbs.h"
#include "NXP_SJA1105P_config.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define MAX_CREDIT 0x3FFFFFFFU  /* maximum 32-bit value represented in 2s complement */
#define MIN_CREDIT 0x3FFFFFFFU  /* minimum 32-bit value represented in 2s complement */

#define MEGA 1000000U
#define BYTE 8U

#define N_TRAFFIC_CLASSES 8U
 
/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static const uint16_t gk_classMeasurementInterval[N_TRAFFIC_CLASSES] =
{
	0,
	0,
	0,
	0,
	SJA1105P_CLASS_D_MEASUREMENT_INTERVAL,
	SJA1105P_CLASS_C_MEASUREMENT_INTERVAL,
	SJA1105P_CLASS_B_MEASUREMENT_INTERVAL,
	SJA1105P_CLASS_A_MEASUREMENT_INTERVAL
};
 
/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t  getShaperId(uint8_t port, uint8_t switchId, uint8_t vlanPrio);
static uint8_t  getCbsEntry(SJA1105P_creditBasedShapingEntryArgument_t *p_entry, uint8_t physicalShaperId, uint8_t switchId);

static uint32_t calculateIdleSlope(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint16_t classMeasurementInterval);
static uint32_t calculateSendSlope(uint32_t idleSlope, uint8_t physicalPort, uint8_t switchId);
static void     getPhysicalShaperId(uint8_t logicalShaperId, uint8_t *p_physicalShaperId, uint8_t *p_switchId);

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
extern uint8_t SJA1105P_registerStreamToCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret = 1;
	uint8_t logicalShaperId;
	uint8_t physicalShaperId;
	SJA1105P_creditBasedShaperParameters_t cbsParameters;
	SJA1105P_port_t physicalPort;
	SJA1105P_creditBasedShapingEntryArgument_t entry;

	if (SJA1105P_getPhysicalPort(port, &physicalPort) == 0U)
	{  /* valid port */
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);
		if (gk_classMeasurementInterval[vlanPrio] > 0U)
		{   /* This is a traffic class with valid AVB settings */
			cbsParameters.idleSlope = calculateIdleSlope(maxFrameSize, maxIntervalFrames, gk_classMeasurementInterval[vlanPrio]);
			if (logicalShaperId != SJA1105P_INVALID_SHAPER_ID)
			{  /* a shaper already exists, update slope values */
				physicalShaperId = logicalShaperId % SJA1105P_N_SHAPER; 
				ret = getCbsEntry(&entry, physicalShaperId, physicalPort.switchId);
				cbsParameters.idleSlope += entry.idleSlope;  /* add new idle slope to existing value */
			}
			cbsParameters.sendSlope = calculateSendSlope(cbsParameters.idleSlope, physicalPort.physicalPort, physicalPort.switchId);
			cbsParameters.creditHi  = MAX_CREDIT;
			cbsParameters.creditLo  = MIN_CREDIT;

			ret += SJA1105P_configCbs(&cbsParameters, port, vlanPrio);
		}
	}
	
	return ret;
}

/**
* \brief De-register resources for a stream with the credit based shaper algorithm
*
* \param[in]  maxFrameSize (B) Maximum frame size that the Talker will produce, excluding any overhead for media-specific framing
* \param[in]  maxIntervalFrames Maximum number of frames that the Talker may transmit in one class measurement interval
* \param[in]  port Port at which the CBS will be instantiated
* \param[in]  vlanPrio Priority to which the CBS will be applied
*
* \return uint8_t: returns 0 upon success, else failed
*/
extern uint8_t SJA1105P_deregisterStreamFromCbs(uint16_t maxFrameSize, uint16_t maxIntervalFrames, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret = 1;
	uint8_t logicalShaperId;
	uint8_t physicalShaperId;
	SJA1105P_creditBasedShaperParameters_t cbsParameters;
	SJA1105P_port_t physicalPort;
	SJA1105P_creditBasedShapingEntryArgument_t entry;

	if (SJA1105P_getPhysicalPort(port, &physicalPort) == 0U)
	{
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);
		physicalShaperId = logicalShaperId % SJA1105P_N_SHAPER;

		if (logicalShaperId != SJA1105P_INVALID_SHAPER_ID)
		{  /* matching shaper found - proceed to update */
			ret  = getCbsEntry(&entry, physicalShaperId, physicalPort.switchId);
			cbsParameters.idleSlope = entry.idleSlope - calculateIdleSlope(maxFrameSize, maxIntervalFrames, gk_classMeasurementInterval[vlanPrio]);
			cbsParameters.sendSlope = calculateSendSlope(cbsParameters.idleSlope, physicalPort.physicalPort, physicalPort.switchId);
			cbsParameters.creditHi  = MAX_CREDIT;
			cbsParameters.creditLo  = MIN_CREDIT;
			ret += SJA1105P_configCbs(&cbsParameters, port, vlanPrio);
		}
	}

	return ret;
}

/**
* \brief Configure a Credit based shaper
*
* \param[in]  p_cbsParameters Struct containing the configuration data for the CBS
* \param[in]  port Port at which the CBS will be instantiated
* \param[in]  vlanPrio Priority to which the CBS will be applied
*
* \return uint8_t: returns the shaper ID which was used. If invalid ID is returned, no shaper resources were available or on error.
*/
extern uint8_t SJA1105P_configCbs(const SJA1105P_creditBasedShaperParameters_t *kp_cbsParameters, uint8_t port, uint8_t vlanPrio)
{
	uint8_t ret = 0;
	uint8_t errors;
	uint8_t logicalShaperId =  SJA1105P_INVALID_SHAPER_ID;
	SJA1105P_creditBasedShapingControlArgument_t control;
	SJA1105P_creditBasedShapingEntryArgument_t entry;
	SJA1105P_port_t physicalPort;

	if (SJA1105P_getPhysicalPort(port, &physicalPort) == 0U)
	{
		logicalShaperId = getShaperId(physicalPort.physicalPort, physicalPort.switchId, vlanPrio);

		if (logicalShaperId != SJA1105P_INVALID_SHAPER_ID)
		{  /* a shaper block is allocated */
			control.shaperId = logicalShaperId % SJA1105P_N_SHAPER;
			control.valid    = 1;
			control.rdwrset  = 1;

			entry.cbsPort   = port;
			entry.cbsPrio   = vlanPrio;
			entry.idleSlope = kp_cbsParameters->idleSlope;
			entry.sendSlope = kp_cbsParameters->sendSlope;
			entry.creditHi  = kp_cbsParameters->creditHi;
			entry.creditLo  = kp_cbsParameters->creditLo;
			ret += SJA1105P_setCreditBasedShapingEntry(&entry, physicalPort.switchId);
			ret += SJA1105P_setCreditBasedShapingControl(&control, physicalPort.switchId);
			ret += SJA1105P_getCreditBasedShapingControl(&errors, physicalPort.switchId);
			ret = (errors != 0U) ? (ret + 1U) : (ret);
		}
	}
	logicalShaperId = (ret == 0U) ? (logicalShaperId) : (SJA1105P_INVALID_SHAPER_ID);

	return logicalShaperId;
}

/**
* \brief Remove a Credit based shaper
*
* \param[in]  shaperId ID of the shaper which will be removed
*
* \return uint8_t Returns 0 upon success, else failed
*/
extern uint8_t SJA1105P_removeCbs(uint8_t shaperId)
{
	uint8_t ret;
	uint8_t errors;
	uint8_t physicalShaperId;
	uint8_t switchId;
	SJA1105P_creditBasedShapingControlArgument_t control;
	SJA1105P_creditBasedShapingEntryArgument_t entry = {0};

	getPhysicalShaperId(shaperId, &physicalShaperId, &switchId);

	control.shaperId = physicalShaperId;
	control.valid    = 1;
	control.rdwrset  = 1;

	/* these settings guarantee that no shaping is performed */
	entry.idleSlope = 0;
	entry.sendSlope = 0;

	ret  = SJA1105P_setCreditBasedShapingEntry(&entry, switchId);
	ret += SJA1105P_setCreditBasedShapingControl(&control, switchId);
	ret += SJA1105P_getCreditBasedShapingControl(&errors, switchId);
	ret = (errors != 0U) ? (ret + 1U) : (ret);

	return ret;
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

	idleSlope = ((uint32_t) maxFrameSize) * maxIntervalFrames * MEGA / classMeasurementInterval;

	return idleSlope;
}

/**
* \brief Calculate the sendSlope
*
* \param[in]  idleSlope (B/s) IdleSlope at the port
* \param[in]  physicalPort Physical port at which the shaper is
* \param[in]  switchId Switch instance of the port
*
* \return uint32_t Returns the sendSlope (B/s) calculated
*/
static uint32_t calculateSendSlope(uint32_t idleSlope, uint8_t physicalPort, uint8_t switchId)
{
	uint32_t sendSlope = 0;
	uint16_t speed;  /* (Mbps) port speed */
	SJA1105P_portStatusMiixArgument_t portStatus;

	if (SJA1105P_getPortStatusMiix(&portStatus, physicalPort, switchId) == 0U)
	{
		switch (portStatus.speed)
		{
			case SJA1105P_e_speed_10_MBPS:  speed = 10;   break;
			case SJA1105P_e_speed_100_MBPS: speed = 100;  break;
			default:                        speed = 1000; break;  /* invalid configuration */
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
	uint8_t ret = 0;
	uint8_t i;
	uint8_t shaperId = SJA1105P_INVALID_SHAPER_ID;
	SJA1105P_creditBasedShapingEntryArgument_t entry;

	for (i = 0; i < SJA1105P_N_SHAPER; i++)
	{
		ret += getCbsEntry(&entry, i, switchId);
		if ((entry.cbsPort == port) && (entry.cbsPrio == vlanPrio))
		{
			/* a shaper instance already exists for this port & prio */
			shaperId = i + (uint8_t) (switchId * SJA1105P_N_SHAPER);
			break;
		}
		else
		{ 
			if ((shaperId == SJA1105P_INVALID_SHAPER_ID) && (entry.sendSlope == 0U))
			{  /* shaper is not allocated to anything */
				/* if no shaper instance already exists, this free on will be taken */
				shaperId = i + (uint8_t) (switchId * SJA1105P_N_SHAPER);
			}
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
	*p_physicalShaperId = logicalShaperId % SJA1105P_N_SHAPER;
	*p_switchId = (uint8_t) ((logicalShaperId - *p_physicalShaperId) / SJA1105P_N_SHAPER);
}

/**
* \brief Get the CBS entry within a single switch instance
*
* \param[out] p_entry Pointer to the CBS entry
* \param[in]  physicalShaperId Physical Shaper ID
* \param[in]  switchId ID of the switch the shaper corresponds to
*
*/
static uint8_t getCbsEntry(SJA1105P_creditBasedShapingEntryArgument_t *p_entry, uint8_t physicalShaperId, uint8_t switchId)
{
	uint8_t ret;

	SJA1105P_creditBasedShapingControlArgument_t control = {1, 0, 0};  /* read access */

	control.shaperId = physicalShaperId;
	ret  = SJA1105P_setCreditBasedShapingControl(&control, switchId);
	ret += SJA1105P_getCreditBasedShapingEntry(p_entry, switchId);

	return ret;
}
