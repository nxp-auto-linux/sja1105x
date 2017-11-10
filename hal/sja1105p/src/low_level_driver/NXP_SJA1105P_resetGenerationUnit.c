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
* \file NXP_SJA1105P_resetGenerationUnit.c
*
* \author NXP Semiconductors
*
* \version 0.1.0
*
* \date 2017-03-02
*
* \brief Provides reset sources across the device and can be used to trigger a software cold or hard reset via the SPI bus
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_resetGenerationUnit.h"
#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

/* register addresses */

/* register category reset_ctrl */
#define R_CTRL_ADDR (0x100440)  /**< Address of the resetCtrl register */

/* variable masks */

/* register category reset_ctrl */
#define R_CTRL_RST_CTRL_MASK (0x000001FFU)  /**< Bit mask for the RST_CTRL field */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to SET data of the reset_ctrl
*
* \param[in]  rstCtrl Reset trigger: {04: trigger a cold reset, 08: trigger a warm reset}
* \return uint8_t
*/
uint8_t SJA1105P_setResetCtrl(uint16_t rstCtrl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((rstCtrl <= 0x1FFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) R_CTRL_RST_CTRL_MASK) & rstCtrl;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105P_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) R_CTRL_ADDR, &cData);  /* write data via SPI */

	return ret;
}
