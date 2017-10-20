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
* \file NXP_SJA1105P_ptp.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Functions for the Precision Time Protocol (PTP)
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_ptp.h"
#include "NXP_SJA1105P_config.h"

/******************************************************************************
* Defines
*****************************************************************************/

#define TIME_TO_FIRST_TOGGLE (uint64_t) (1000*1000*1000/8)  /* Start toggling after 1 s */
 
/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static uint32_t g_clkRatio;
static SJA1105P_ptpControl2Argument_t g_ptpControl2;

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Initialize PTP clock
*
* Configures the PTP clock of the switch for gPTP operation. 
* After initialization, the PTP pin of the switch will toggle.
*
* \return uint8_t: 0: init successful, else: init failed
*/
extern uint8_t SJA1105P_initPtp(void)
{
	uint8_t ret = 0;
	uint8_t switchId;

	g_ptpControl2.valid        = 1;
	g_ptpControl2.startptpcp   = 1;
	g_ptpControl2.stopptpcp    = 0;
	g_ptpControl2.syncCascaded = 0;
	g_ptpControl2.resptp       = 0;
	g_ptpControl2.corrclk4ts   = 1;
	g_ptpControl2.ptpclkadd    = 1;
	g_ptpControl2.ptpclksub    = 0;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{  /* init all PTP clocks with the right settings */
		ret += SJA1105P_setPtpControl2(&g_ptpControl2, switchId);
	}

	ret += SJA1105P_setPtpClkRatio(SJA1105P_INITIAL_CLK_RATIO);

	#if SJA1105P_N_SWITCHES > 1U
		ret += SJA1105P_syncCascadedClocks();
	#endif

	return ret;
}

/**
* \brief Read the current value of the switch PTP clock
*
* The clock is represented as a multiple of 8 ns.
*
* \param[out] p_clkVal Memory location where the clock value will be stored
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_getPtpClk(uint64_t *p_clkVal)
{
	return SJA1105P_getPtpControl3(p_clkVal, SJA1105P_MASTER_SWITCH);
}

/**
* \brief Write a value to the switch PTP clock
*
* The clock is represented as a multiple of 8 ns.
* In setups with more than one switch cascaded, this function should not be used.
* It will lead to inconsistencies due to clock offset.
* Instead, clocks should be modified through ::SJA1105P_addOffsetToPtpClk
*
* \param[in]  clkVal Value to which the PTP clock will be set
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_setPtpClk(uint64_t clkVal)
{
	uint8_t ret = 0;

	if ((g_ptpControl2.ptpclkadd == 1U) || (g_ptpControl2.ptpclksub == 1U))
	{  /* change mode of clock modification */
		g_ptpControl2.ptpclkadd  = 0;
		g_ptpControl2.ptpclksub  = 0;
		ret += SJA1105P_setPtpControl2(&g_ptpControl2, SJA1105P_g_ptpMasterSwitch);
	}
	/* Set clock value */
	ret += SJA1105P_setPtpControl3(clkVal, SJA1105P_g_ptpMasterSwitch);

	if ((SJA1105P_N_SWITCHES > 1U) && (ret == 0U))
	{  /* synchronize cascaded switches to the adjusted clock value */
		ret = SJA1105P_syncCascadedClocks();
	}

	return ret;
}

/**
* \brief Adjust the clock rate of the PTP clock
*
* \param[in]  clkRatio fixed-point clock rate value with a single-bit integer part and a 31-bit fractional part allowing for sub-ppb rate corrections
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_setPtpClkRatio(uint32_t clkRatio)
{
	uint8_t ret = 0;
	uint8_t switchId;
	g_clkRatio = clkRatio;

	 
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += SJA1105P_setPtpControl4(clkRatio, switchId);
	}
	return ret;
}

/**
* \brief Read the current value of the clock ratio
*
* \param[out] p_clkRatio Current value of the clock ratio
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_getPtpClkRatio(uint32_t *p_clkRatio)
{
	*p_clkRatio = g_clkRatio;
	return 0;
}

/**
* \brief Add an offset to the PTP clock
*
* The offset value is represented as a multiple of 8 ns.
*
* \param[in]  clkAddVal Value to be added to the current clock value
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_addOffsetToPtpClk(uint64_t clkAddVal)
{
	uint8_t ret = 0;
	uint8_t switchId;

	if (g_ptpControl2.ptpclkadd == 0U)
	{
		g_ptpControl2.ptpclkadd = 1;
		g_ptpControl2.ptpclksub = 0;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += SJA1105P_setPtpControl2(&g_ptpControl2, switchId);
		}
	}
	/* Add offset */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += SJA1105P_setPtpControl3(clkAddVal, switchId);
	}

	return ret;
}

/**
* \brief Subtract an offset from the PTP clock
*
* The offset value is represented as a multiple of 8 ns.
*
* \param[in]  clkSubVal Value to be subtracted from the current clock value
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_subtractOffsetFromPtpClk(uint64_t clkSubVal)
{
	uint8_t ret = 0;
	uint8_t switchId;

	if (g_ptpControl2.ptpclksub == 0U)
	{
		g_ptpControl2.ptpclkadd = 0;
		g_ptpControl2.ptpclksub = 1;
		for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
		{
			ret += SJA1105P_setPtpControl2(&g_ptpControl2, switchId);
		}
	}
	/* Add offset */
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		ret += SJA1105P_setPtpControl3(clkSubVal, switchId);
	}

	return ret;
}

/**
* \brief Synchronize the clocks of multiple SJA1105Ps
*
* The devices must be connected through the PTP pin,
* with one device configured as PTP master.
*
* \return uint8_t: 0: successful, else: failed
*/
extern uint8_t SJA1105P_syncCascadedClocks(void)
{
	uint8_t  ret = 0;
	uint8_t  switchId;
	uint64_t masterTimestamp;
	uint64_t slaveTimestamp;
	uint64_t offset;

	/* Step 1: issue synchronization impulse */
	g_ptpControl2.syncCascaded = 1;
	/* Clock add mode will be needed later to correct the offset */
	g_ptpControl2.ptpclkadd = 1;
	g_ptpControl2.ptpclksub = 0;
	ret += SJA1105P_setPtpControl2(&g_ptpControl2, SJA1105P_g_ptpMasterSwitch);
	g_ptpControl2.syncCascaded = 0;

	/* Step 2: Calculate offset and perform correction */
	ret += SJA1105P_getPtpControl6(&masterTimestamp, SJA1105P_g_ptpMasterSwitch);
	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		if (SJA1105P_g_avbParameters.ptpMaster[switchId] == 0U)
		{  /* this is a slave which has to be synchronized */
			ret += SJA1105P_getPtpControl6(&slaveTimestamp, switchId);
			offset = masterTimestamp - slaveTimestamp;
			ret += SJA1105P_setPtpControl2(&g_ptpControl2, switchId);  /* set into add mode */
			ret += SJA1105P_setPtpControl3(offset, switchId);  /* add offset */
		}
	}

	/* Step 3: Rest the synchronization impulse */
	ret += SJA1105P_setPtpControl2(&g_ptpControl2, SJA1105P_g_ptpMasterSwitch);

	return ret;
}