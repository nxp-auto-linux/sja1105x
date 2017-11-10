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
* \file NXP_SJA1105P_spi.c
*
* \author NXP Semiconductors
*
* \version 0.1.0
*
* \date 2017-03-02
*
* \brief Definition of SPI read and write functions
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* EXPORTED VARIABLES
*****************************************************************************/

SJA1105P_spiRead32_cb_t SJA1105P_gpf_spiRead32 = NULL;
SJA1105P_spiWrite32_cb_t SJA1105P_gpf_spiWrite32 = NULL;

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

extern void SJA1105P_registerSpiRead32CB(SJA1105P_spiRead32_cb_t pf_spiRead32)
{
	SJA1105P_gpf_spiRead32 = pf_spiRead32;
}
extern void SJA1105P_registerSpiWrite32CB(SJA1105P_spiWrite32_cb_t pf_spiWrite32)
{
	SJA1105P_gpf_spiWrite32 = pf_spiWrite32;
}
