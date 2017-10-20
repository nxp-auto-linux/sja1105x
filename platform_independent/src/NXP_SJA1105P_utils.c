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
* \file NXP_SJA1105P_utils.c
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Utility functions for switch operation
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_utils.h"
#include "NXP_SJA1105P_config.h"

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Reconstruct correct timestamp form lower bits of measurement and complete timestamp
*
* \param[in]    timeStampL lower bits of the timestamp recorded
* \param[inout] p_timeStamp Memory location of the full time stamp to be assembled
*
*/
void SJA1105P_reconstructTimeStamp(uint32_t timeStampL, uint64_t *p_timeStamp)
{
	uint32_t timeStampLRecent;  /* lower bits of the more recent measurement */
	uint64_t timeStampLMask;

	timeStampLMask = ((uint64_t) (((uint64_t) 1) << (SJA1105P_TIMESTAMP_LENGTH))) - 1U;
	timeStampLRecent = (uint32_t) (*p_timeStamp & timeStampLMask);

	if (timeStampLRecent < timeStampL)
	{
		/* at least overrun occurred since the original measurement */
		*p_timeStamp = (uint64_t) (*p_timeStamp - (uint64_t) (((uint64_t) 1) << SJA1105P_TIMESTAMP_LENGTH));  /* compensate for one overrun */ 
	}
	*p_timeStamp &= ~((uint64_t) timeStampLMask);  /* set lower bits to 0 */
	*p_timeStamp |= ((uint64_t) timeStampL) & timeStampLMask;
}
