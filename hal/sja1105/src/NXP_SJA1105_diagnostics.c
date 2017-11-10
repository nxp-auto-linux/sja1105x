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
* \file NXP_SJA1105_diagnostics.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Obtain diagnostic information on the switch
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_diagnostics.h"
#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_config.h"
#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define N_TRESHOLDS ((uint8_t) e_tempThreshold_POSITIVE135P5 - (uint8_t) e_tempThreshold_NOT_VALID)

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Retrieve a 64-bit Ethernet statistic counter value
*
* \param[in]  counterName Name of the statistic counter requested
* \param[in]  port Number of the port where the statistics are requested
* \param[out] p_counterValue Pointer to the value of the counter
*
* \return uint8_t Returns 0 upon success, else failed
*/
extern uint8_t SJA1105_get64bitEtherStatCounter(SJA1105_etherStat64_t counterName, uint64_t *p_counterValue, uint8_t port)
{
	uint8_t ret = 1;
	SJA1105_port_t physicalPort;

	if (SJA1105_getPhysicalPort(port, &physicalPort) == 0U)
	{
		switch (counterName)
		{
			case e_etherStat64_N_TXBYTE:
				ret = SJA1105_getHighLevelDiagnosticCounters10(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat64_N_TXFRM:
				ret = SJA1105_getHighLevelDiagnosticCounters11(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat64_N_RXBYTE:
				ret = SJA1105_getHighLevelDiagnosticCounters12(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat64_N_RXFRM:
				ret = SJA1105_getHighLevelDiagnosticCounters13(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			default:
				break;
		}
	}

	return ret;
}

/**
* \brief Retrieve a 32-bit Ethernet statistic counter value
*
* \param[in]  counterName Name of the statistic counter requested
* \param[in]  port Number of the port where the statistics are requested
* \param[out] p_counterValue Pointer to the value of the counter
*
* \return uint8_t Returns 0 upon success, else failed
*/
extern uint8_t SJA1105_get32bitEtherStatCounter(SJA1105_etherStat32_t counterName, uint32_t *p_counterValue, uint8_t port)
{
	uint8_t ret = 1;
	SJA1105_port_t physicalPort;

	if (SJA1105_getPhysicalPort(port, &physicalPort) == 0U)
	{
		switch (counterName)
		{
			case e_etherStat32_N_NOT_REACH:
				ret = SJA1105_getHighLevelDiagnosticCounters20(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_EGR_DISABLED:
				ret = SJA1105_getHighLevelDiagnosticCounters21(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_PART_DROP:
				ret = SJA1105_getHighLevelDiagnosticCounters22(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_QFULL:
				ret = SJA1105_getHighLevelDiagnosticCounters23(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_POLERR:
				ret = SJA1105_getHighLevelDiagnosticCounters14(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_CRCERR:
				ret = SJA1105_getHighLevelDiagnosticCounters15(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_SIZEERR:
				ret = SJA1105_getHighLevelDiagnosticCounters16(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_VLANERR:
				ret = SJA1105_getHighLevelDiagnosticCounters17(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			case e_etherStat32_N_N664ERR:
				ret = SJA1105_getHighLevelDiagnosticCounters18(p_counterValue, physicalPort.physicalPort, physicalPort.switchId);
				break;
			default:
				break;
		}
	}

	return ret;
}

/**
* \brief Retrieve the temperature sensor reading from the switches
*
* This function will use the built in temperature sensor to determine
* the rough temperature. Not that this is only approximate up to 
* around 5 degrees due to the sensor quantization.
*
* \param[out] a_temperature Array containing the readings
*
* \return uint8_t Returns 0 upon success, else failed
*/
extern uint8_t SJA1105_getTemperature(SJA1105_tempThreshold_t a_temperature[SJA1105_N_SWITCHES])
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t i;
	SJA1105_tsConfigArgument_t tempSensor;
	uint8_t exceeded;
	tempSensor.pd = 0;  /* sensor active */
	for (switchId = 0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		tempSensor.threshold = e_tempThreshold_NOT_VALID;  /* minimum temperature */
		for (i = 0; i < N_TRESHOLDS; i++)
		{
			tempSensor.threshold = SJA1105_convertToTempThreshold((uint32_t) (tempSensor.threshold) + 1U);
			ret += SJA1105_setTsConfig(&tempSensor, switchId);
			ret += SJA1105_getTsStatus(&exceeded, switchId);
			if (exceeded == 0U)
			{
				a_temperature[switchId] = tempSensor.threshold;
				break;
			}
		}
	}

	return ret;
}
