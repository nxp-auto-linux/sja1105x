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
* \file NXP_SJA1105_switchCore.c
*
* \author NXP Semiconductors
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief Status information and control registers of the switch
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define WORD0 0U
#define WORD1 1U
#define WORD2 2U

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief This function is used to GET data of the general_status_device_0
*
* \param[out] p_id device identification code
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusDevice0(uint32_t *p_id, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GSD_0_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_id = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_device_1
*
* \param[out] p_generalStatusDevice1 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusDevice1Argument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusDevice1(SJA1105_generalStatusDevice1Argument_t *p_generalStatusDevice1, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GSD_1_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GSD_1_CONFIGS_SHIFT);  /* shift to right aligned position */
	p_generalStatusDevice1->configs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GSD_1_CRCCHKL_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GSD_1_CRCCHKL_SHIFT);  /* shift to right aligned position */
	p_generalStatusDevice1->crcchkl = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GSD_1_IDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GSD_1_IDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusDevice1->ids = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GSD_1_CRCCHKG_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GSD_1_CRCCHKG_SHIFT);  /* shift to right aligned position */
	p_generalStatusDevice1->crcchkg = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GSD_1_NSLOT_MASK);   /* mask desired bits */
	p_generalStatusDevice1->nslot = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_l2
*
* \param[out] p_generalStatusL2 Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusL2Argument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusL2(SJA1105_generalStatusL2Argument_t *p_generalStatusL2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_GS_L2_WORD_COUNT] = {0};
	uint64_t macaddTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_GS_L2_WORD_COUNT, (uint32_t) SJA1105_GS_L2_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (SJA1105_GS_L2_PORTENF_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->portenf = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_L2_FWDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_L2_FWDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->fwds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_L2_MACFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_L2_MACFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->macfds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_L2_ENFFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_L2_ENFFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->enffds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_L2_L2BUSYFDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_L2_L2BUSYFDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusL2->l2busyfds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_L2_L2BUSYS_MASK);   /* mask desired bits */
	p_generalStatusL2->l2busys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (SJA1105_GS_L2_MACADD_SHIFT_0);  /* shift to right aligned position */
	macaddTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_GS_L2_MACADD_SHIFT_OVLP);
	p_generalStatusL2->macadd = (uint64_t) macaddTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_hashconfs
*
* \param[out] p_generalStatusHashconfs Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusHashconfsArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusHashconfs(SJA1105_generalStatusHashconfsArgument_t *p_generalStatusHashconfs, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_GSH_WORD_COUNT] = {0};
	uint64_t macaddhcTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_GSH_WORD_COUNT, (uint32_t) SJA1105_GSH_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GSH_VLANIDHC_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GSH_VLANIDHC_SHIFT);  /* shift to right aligned position */
	p_generalStatusHashconfs->vlanidhc = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GSH_HASHCONFS_MASK);   /* mask desired bits */
	p_generalStatusHashconfs->hashconfs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (SJA1105_GSH_MACADDHC_SHIFT_0);  /* shift to right aligned position */
	macaddhcTmp  = (uint64_t) ((uint16_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	macaddhcTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_GSH_MACADDHC_SHIFT_OVLP);
	p_generalStatusHashconfs->macaddhc = (uint64_t) macaddhcTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_vlan
*
* \param[out] p_generalStatusVlan Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusVlanArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusVlan(SJA1105_generalStatusVlanArgument_t *p_generalStatusVlan, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GS_VLAN_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GS_VLAN_WPVLANID_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->wpvlanid = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GS_VLAN_PORT_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->port = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_VLAN_VLANBUSYS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_VLAN_VLANBUSYS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->vlanbusys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_VLAN_WRONGPORTS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_VLAN_WRONGPORTS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->wrongports = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_VLAN_VNOTFOUNDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_VLAN_VNOTFOUNDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusVlan->vnotfounds = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_memory
*
* \param[out] p_generalStatusMemory Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusMemoryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusMemory(SJA1105_generalStatusMemoryArgument_t *p_generalStatusMemory, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GSM_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GSM_EMPTYS_SHIFT);  /* shift to right aligned position */
	p_generalStatusMemory->emptys = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GSM_BUFFERS_MASK);   /* mask desired bits */
	p_generalStatusMemory->buffers = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_drop
*
* \param[out] p_generalStatusDrop Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusDropArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusDrop(SJA1105_generalStatusDropArgument_t *p_generalStatusDrop, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GS_DROP_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GS_DROP_PORT_SHIFT);  /* shift to right aligned position */
	p_generalStatusDrop->port = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_DROP_FWDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GS_DROP_FWDS_SHIFT);  /* shift to right aligned position */
	p_generalStatusDrop->fwds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_DROP_PARTS_MASK);   /* mask desired bits */
	p_generalStatusDrop->parts = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_ram_error
*
* \param[out] p_ramparerr Any set bit indicates a parity error in a memory block.
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusRamError(uint32_t *p_ramparerr, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_GS_RAM_E_WORD_COUNT] = {0};
	uint32_t ramparerrTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_GS_RAM_E_WORD_COUNT, (uint32_t) SJA1105_GS_RAM_E_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_RAM_E_RAMPARERR_MASK_0);   /* mask desired bits */
	ramparerrTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (SJA1105_GS_RAM_E_RAMPARERR_MASK_1);   /* mask desired bits */
	ramparerrTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) SJA1105_GS_RAM_E_RAMPARERR_SHIFT_OVLP);
	*p_ramparerr = (uint32_t) ramparerrTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the general_status_ram_init
*
* \param[in]  rparinit Parity flag initialization pattern
* \return uint8_t
*/
uint8_t SJA1105_setGeneralStatusRamInit(uint32_t rparinit, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_GS_RAM_INIT_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((rparinit <= 0x3FFFFFFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) SJA1105_GS_RAM_INIT_RPARINIT_MASK_0) & rparinit;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) rparinit >> (uint8_t) SJA1105_GS_RAM_INIT_RPARINIT_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_GS_RAM_INIT_RPARINIT_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_GS_RAM_INIT_WORD_COUNT, (uint32_t) SJA1105_GS_RAM_INIT_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_ram_init
*
* \param[out] p_rparinit Parity flag initialization pattern
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusRamInit(uint32_t *p_rparinit, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_GS_RAM_INIT_WORD_COUNT] = {0};
	uint32_t rparinitTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_GS_RAM_INIT_WORD_COUNT, (uint32_t) SJA1105_GS_RAM_INIT_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_GS_RAM_INIT_RPARINIT_MASK_0);   /* mask desired bits */
	rparinitTmp  = (uint32_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (SJA1105_GS_RAM_INIT_RPARINIT_MASK_1);   /* mask desired bits */
	rparinitTmp |= (uint32_t) ((uint32_t) cResult << (uint8_t) SJA1105_GS_RAM_INIT_RPARINIT_SHIFT_OVLP);
	*p_rparinit = (uint32_t) rparinitTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ethernet_port_status_control_reset
*
* \param[in]  clearport This field is used to reset the MAC-level diagnostics counters and flags for each port.
* \return uint8_t
*/
uint8_t SJA1105_setEthernetPortStatusControlReset(uint8_t clearport, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((clearport <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) SJA1105_E_PORT_SCR_CLEARPORT_MASK) & clearport;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_E_PORT_SCR_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ethernet_port_status_control_transmission
*
* \param[in]  inhibittx This vector represents the set of ports on which the transmission is inhibited. A port will inhibit transmission if and only if the respective bit is set to 1.
* \return uint8_t
*/
uint8_t SJA1105_setEthernetPortStatusControlTransmission(uint8_t inhibittx, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((inhibittx <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) SJA1105_E_PORT_SCT_INHIBITTX_MASK) & inhibittx;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_E_PORT_SCT_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_0
*
* \param[in]  ptppinst Synchronized PTP clock value at which the switch shall start toggling the external PTP_CLK pin
* \return uint8_t
*/
uint8_t SJA1105_setPtpControl0(uint64_t ptppinst, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_PTP_C_0_WORD_COUNT];
	uint32_t tmp;

	cData = (uint32_t) ptppinst;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (ptppinst >> (uint8_t) SJA1105_PTP_C_0_PTPPINST_SHIFT_OVLP);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_PTP_C_0_WORD_COUNT, (uint32_t) SJA1105_PTP_C_0_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_1
*
* \param[in]  ptppindur This field specifies the interval between two edges of the external clock on pin PTP_CLK
* \return uint8_t
*/
uint8_t SJA1105_setPtpControl1(uint32_t ptppindur, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) ptppindur;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_PTP_C_1_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_2
*
* \param[in]  ptpControl2 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_ptpControl2Argument_t
* \return uint8_t
*/
uint8_t SJA1105_setPtpControl2(const SJA1105_ptpControl2Argument_t *pk_ptpControl2, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_ptpControl2->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->startptpcp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->stopptpcp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->resptp <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->corrclk4ts <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_ptpControl2->ptpclkadd <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_ptpControl2->valid) << ((uint8_t) SJA1105_PTP_C_2_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_ptpControl2->startptpcp) << ((uint8_t) SJA1105_PTP_C_2_STARTPTPCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PTP_C_2_STARTPTPCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->stopptpcp) << ((uint8_t) SJA1105_PTP_C_2_STOPPTPCP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PTP_C_2_STOPPTPCP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->resptp) << ((uint8_t) SJA1105_PTP_C_2_RESPTP_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PTP_C_2_RESPTP_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_ptpControl2->corrclk4ts) << ((uint8_t) SJA1105_PTP_C_2_CORRCLK4TS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_PTP_C_2_CORRCLK4TS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_PTP_C_2_PTPCLKADD_MASK) & pk_ptpControl2->ptpclkadd;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_PTP_C_2_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_3
*
* \param[in]  ptpclkval PTP clock
* \return uint8_t
*/
uint8_t SJA1105_setPtpControl3(uint64_t ptpclkval, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_PTP_C_3_WORD_COUNT];
	uint32_t tmp;

	cData = (uint32_t) ptpclkval;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) (ptpclkval >> (uint8_t) SJA1105_PTP_C_3_PTPCLKVAL_SHIFT_OVLP);  /* shift to right align subfield with register */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_PTP_C_3_WORD_COUNT, (uint32_t) SJA1105_PTP_C_3_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_3
*
* \param[out] p_ptpclkval PTP clock
* \return uint8_t
*/
uint8_t SJA1105_getPtpControl3(uint64_t *p_ptpclkval, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_PTP_C_3_WORD_COUNT] = {0};
	uint64_t ptpclkvalTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_PTP_C_3_WORD_COUNT, (uint32_t) SJA1105_PTP_C_3_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	ptpclkvalTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	ptpclkvalTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_PTP_C_3_PTPCLKVAL_SHIFT_OVLP);
	*p_ptpclkval = (uint64_t) ptpclkvalTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the ptp_control_4
*
* \param[in]  ptpclkrate This field determines the speed of PTPCLK. It implements a fixed-point clock rate value with a single-bit integer part and a 31-bit fractional part. PTPCLKVAL will tick at the rate of PTPTSCLK multiplied by this field.
* \return uint8_t
*/
uint8_t SJA1105_setPtpControl4(uint32_t ptpclkrate, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) ptpclkrate;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_PTP_C_4_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the ptp_control_5
*
* \param[out] p_ptptsclk PTP timestamp clock that is used to timestamp MAC management frames on ingress and egress
* \return uint8_t
*/
uint8_t SJA1105_getPtpControl5(uint64_t *p_ptptsclk, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_PTP_C_5_WORD_COUNT] = {0};
	uint64_t ptptsclkTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_PTP_C_5_WORD_COUNT, (uint32_t) SJA1105_PTP_C_5_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	ptptsclkTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	ptptsclkTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_PTP_C_5_PTPTSCLK_SHIFT_OVLP);
	*p_ptptsclk = (uint64_t) ptptsclkTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_address_lookup_table_reconfiguration_entry
*
* \param[in]  l2AddressLookupTableReconfigurationEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setL2AddressLookupTableReconfigurationEntry(const SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *pk_l2AddressLookupTableReconfigurationEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_L2_ALTRE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2AddressLookupTableReconfigurationEntry->index <= 0x3FFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationEntry->enfport <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationEntry->destports <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationEntry->macaddr <= 0xFFFFFFFFFFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->index) << ((uint8_t) SJA1105_L2_ALTRE_INDEX_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_ALTRE_INDEX_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->enfport) << ((uint8_t) SJA1105_L2_ALTRE_ENFPORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_ALTRE_ENFPORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->destports) << ((uint8_t) SJA1105_L2_ALTRE_DESTPORTS_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->destports >> (uint8_t) SJA1105_L2_ALTRE_DESTPORTS_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_L2_ALTRE_DESTPORTS_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->macaddr) << ((uint8_t) SJA1105_L2_ALTRE_MACADDR_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	tmp = (uint32_t) (pk_l2AddressLookupTableReconfigurationEntry->macaddr >> (uint8_t) SJA1105_L2_ALTRE_MACADDR_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_L2_ALTRE_MACADDR_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationEntry->vlanid) << ((uint8_t) SJA1105_L2_ALTRE_VLANID_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD2] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_L2_ALTRE_WORD_COUNT, (uint32_t) SJA1105_L2_ALTRE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_address_lookup_table_reconfiguration_entry
*
* \param[out] p_l2AddressLookupTableReconfigurationEntry Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getL2AddressLookupTableReconfigurationEntry(SJA1105_l2AddressLookupTableReconfigurationEntryArgument_t *p_l2AddressLookupTableReconfigurationEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_L2_ALTRE_WORD_COUNT] = {0};
	uint8_t destportsTmp;
	uint64_t macaddrTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_L2_ALTRE_WORD_COUNT, (uint32_t) SJA1105_L2_ALTRE_ADDR, registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_L2_ALTRE_INDEX_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRE_INDEX_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationEntry->index = (uint16_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult  &= (uint32_t) (SJA1105_L2_ALTRE_ENFPORT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRE_ENFPORT_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationEntry->enfport = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD0];
	cResult >>= (uint32_t) (SJA1105_L2_ALTRE_DESTPORTS_SHIFT_0);  /* shift to right aligned position */
	destportsTmp  = (uint8_t) ((uint8_t) cResult);

	cResult   = (uint32_t) registerValue[WORD1];
	cResult  &= (uint32_t) (SJA1105_L2_ALTRE_DESTPORTS_MASK_1);   /* mask desired bits */
	destportsTmp |= (uint8_t) ((uint8_t) cResult << (uint8_t) SJA1105_L2_ALTRE_DESTPORTS_SHIFT_OVLP);
	p_l2AddressLookupTableReconfigurationEntry->destports = (uint8_t) destportsTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD1];
	cResult >>= (uint32_t) (SJA1105_L2_ALTRE_MACADDR_SHIFT_0);  /* shift to right aligned position */
	macaddrTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD2];
	cResult  &= (uint32_t) (SJA1105_L2_ALTRE_MACADDR_MASK_1);   /* mask desired bits */
	macaddrTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_L2_ALTRE_MACADDR_SHIFT_OVLP);
	p_l2AddressLookupTableReconfigurationEntry->macaddr = (uint64_t) macaddrTmp;  /* deliver result */

	cResult   = (uint32_t) registerValue[WORD2];
	cResult >>= (uint32_t) (SJA1105_L2_ALTRE_VLANID_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationEntry->vlanid = (uint16_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_address_lookup_table_reconfiguration_control
*
* \param[in]  l2AddressLookupTableReconfigurationControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setL2AddressLookupTableReconfigurationControl(const SJA1105_l2AddressLookupTableReconfigurationControlSetArgument_t *pk_l2AddressLookupTableReconfigurationControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2AddressLookupTableReconfigurationControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationControl->rdwrset <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationControl->valident <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2AddressLookupTableReconfigurationControl->mgmtroute <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationControl->valid) << ((uint8_t) SJA1105_L2_ALTRC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationControl->rdwrset) << ((uint8_t) SJA1105_L2_ALTRC_RDWRSET_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_ALTRC_RDWRSET_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationControl->valident) << ((uint8_t) SJA1105_L2_ALTRC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_ALTRC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2AddressLookupTableReconfigurationControl->mgmtroute) << ((uint8_t) SJA1105_L2_ALTRC_MGMTROUTE_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_ALTRC_MGMTROUTE_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_L2_ALTRC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_address_lookup_table_reconfiguration_control
*
* \param[out] p_l2AddressLookupTableReconfigurationControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getL2AddressLookupTableReconfigurationControl(SJA1105_l2AddressLookupTableReconfigurationControlGetArgument_t *p_l2AddressLookupTableReconfigurationControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_L2_ALTRC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_VALID_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->valid = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_ALTRC_RDWRSET_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_RDWRSET_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->rdwrset = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_ALTRC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_ERRORS_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->errors = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_ALTRC_LOCKEDS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_LOCKEDS_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->lockeds = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_ALTRC_VALIDENT_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_VALIDENT_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->valident = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_ALTRC_MGMTROUTE_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_ALTRC_MGMTROUTE_SHIFT);  /* shift to right aligned position */
	p_l2AddressLookupTableReconfigurationControl->mgmtroute = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_forwarding_table_reconfiguration_entry
*
* \param[in]  l2ForwardingTableReconfigurationEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_l2ForwardingTableReconfigurationEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setL2ForwardingTableReconfigurationEntry(const SJA1105_l2ForwardingTableReconfigurationEntryArgument_t *pk_l2ForwardingTableReconfigurationEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_L2_FTRE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap0 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap1 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap2 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap3 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap4 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap5 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap6 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->vlanPmap7 <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->flDomain <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->reachPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationEntry->bcDomain <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap0) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_0_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_0_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap1) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_1_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_1_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap2) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_2_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap2 >> (uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_2_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_2_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap3) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_3_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_3_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap4) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_4_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_4_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap5) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_5_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_5_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap6) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_6_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_6_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->vlanPmap7) << ((uint8_t) SJA1105_L2_FTRE_VLAN_PMAP_7_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_VLAN_PMAP_7_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->flDomain) << ((uint8_t) SJA1105_L2_FTRE_FL_DOMAIN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_FL_DOMAIN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->reachPort) << ((uint8_t) SJA1105_L2_FTRE_REACH_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_L2_FTRE_REACH_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationEntry->bcDomain) << ((uint8_t) SJA1105_L2_FTRE_BC_DOMAIN_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_L2_FTRE_WORD_COUNT, (uint32_t) SJA1105_L2_FTRE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_forwarding_table_reconfiguration_control
*
* \param[in]  l2ForwardingTableReconfigurationControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_l2ForwardingTableReconfigurationControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setL2ForwardingTableReconfigurationControl(const SJA1105_l2ForwardingTableReconfigurationControlArgument_t *pk_l2ForwardingTableReconfigurationControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2ForwardingTableReconfigurationControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_l2ForwardingTableReconfigurationControl->index <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2ForwardingTableReconfigurationControl->valid) << ((uint8_t) SJA1105_L2_FTRC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) SJA1105_L2_FTRC_INDEX_MASK) & pk_l2ForwardingTableReconfigurationControl->index;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_L2_FTRC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_forwarding_table_reconfiguration_control
*
* \param[out] p_errors This value indicates whether a dynamic reconfiguration attempt was successful.
* \return uint8_t
*/
uint8_t SJA1105_getL2ForwardingTableReconfigurationControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_L2_FTRC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_FTRC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_L2_FTRC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the vlan_lookup_table_reconfiguration_entry
*
* \param[in]  vlanLookupTableReconfigurationEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_vlanLookupTableReconfigurationEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setVlanLookupTableReconfigurationEntry(const SJA1105_vlanLookupTableReconfigurationEntryArgument_t *pk_vlanLookupTableReconfigurationEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_VLAN_LTRE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_vlanLookupTableReconfigurationEntry->vlanid <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationEntry->tagPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationEntry->vlanBc <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationEntry->vmembPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationEntry->vegrMirr <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationEntry->vingMirr <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vlanid) << ((uint8_t) SJA1105_VLAN_LTRE_VLANID_SHIFT_0);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vlanid >> (uint8_t) SJA1105_VLAN_LTRE_VLANID_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_VLAN_LTRE_VLANID_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->tagPort) << ((uint8_t) SJA1105_VLAN_LTRE_TAG_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_VLAN_LTRE_TAG_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vlanBc) << ((uint8_t) SJA1105_VLAN_LTRE_VLAN_BC_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_VLAN_LTRE_VLAN_BC_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vmembPort) << ((uint8_t) SJA1105_VLAN_LTRE_VMEMB_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_VLAN_LTRE_VMEMB_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vegrMirr) << ((uint8_t) SJA1105_VLAN_LTRE_VEGR_MIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_VLAN_LTRE_VEGR_MIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationEntry->vingMirr) << ((uint8_t) SJA1105_VLAN_LTRE_VING_MIRR_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_VLAN_LTRE_WORD_COUNT, (uint32_t) SJA1105_VLAN_LTRE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the vlan_lookup_table_reconfiguration_control
*
* \param[in]  vlanLookupTableReconfigurationControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_vlanLookupTableReconfigurationControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setVlanLookupTableReconfigurationControl(const SJA1105_vlanLookupTableReconfigurationControlArgument_t *pk_vlanLookupTableReconfigurationControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_vlanLookupTableReconfigurationControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_vlanLookupTableReconfigurationControl->valident <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationControl->valid) << ((uint8_t) SJA1105_VLAN_LTRC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_vlanLookupTableReconfigurationControl->valident) << ((uint8_t) SJA1105_VLAN_LTRC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_VLAN_LTRC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_VLAN_LTRC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_block_0
*
* \param[in]  idleSlope Value at which the credit counter gets increased at a rate of bytes per second times link speed
* \return uint8_t
*/
uint8_t SJA1105_setCreditBasedShapingBlock0(uint32_t idleSlope, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) idleSlope;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CBSB_0_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_block_1
*
* \param[in]  sendSlope Value at which the credit counter gets decreased at a rate of bytes per second times link speed
* \return uint8_t
*/
uint8_t SJA1105_setCreditBasedShapingBlock1(uint32_t sendSlope, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) sendSlope;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CBSB_1_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_block_2
*
* \param[in]  creditHi Value at which the credit counter positively saturates upon transmission of a frame
* \return uint8_t
*/
uint8_t SJA1105_setCreditBasedShapingBlock2(uint32_t creditHi, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) creditHi;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CBSB_2_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_block_3
*
* \param[in]  creditLo Value at which the credit counter negatively saturates upon transmission of a frame
* \return uint8_t
*/
uint8_t SJA1105_setCreditBasedShapingBlock3(uint32_t creditLo, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	cData = (uint32_t) creditLo;  /* write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CBSB_3_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the credit_based_shaping_block_4
*
* \param[in]  creditBasedShapingBlock4 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_creditBasedShapingBlock4Argument_t
* \return uint8_t
*/
uint8_t SJA1105_setCreditBasedShapingBlock4(const SJA1105_creditBasedShapingBlock4Argument_t *pk_creditBasedShapingBlock4, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_creditBasedShapingBlock4->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingBlock4->shaperId <= 0xFU) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingBlock4->cbsPort <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_creditBasedShapingBlock4->cbsPrio <= 0x7U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_creditBasedShapingBlock4->valid) << ((uint8_t) SJA1105_CBSB_4_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_creditBasedShapingBlock4->shaperId) << ((uint8_t) SJA1105_CBSB_4_SHAPER_ID_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CBSB_4_SHAPER_ID_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_creditBasedShapingBlock4->cbsPort) << ((uint8_t) SJA1105_CBSB_4_CBS_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_CBSB_4_CBS_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_CBSB_4_CBS_PRIO_MASK) & pk_creditBasedShapingBlock4->cbsPrio;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_CBSB_4_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the retagging_table_reconfiguration_entry
*
* \param[in]  retaggingTableReconfigurationEntry Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_retaggingTableReconfigurationEntryArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setRetaggingTableReconfigurationEntry(const SJA1105_retaggingTableReconfigurationEntryArgument_t *pk_retaggingTableReconfigurationEntry, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t registerValue[SJA1105_RTRE_WORD_COUNT];
	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_retaggingTableReconfigurationEntry->destports <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->useDestports <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->doNotLearn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->vlanEgr <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->vlanIng <= 0xFFFU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->ingPort <= 0x1FU) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationEntry->egrPort <= 0x1FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->destports) << ((uint8_t) SJA1105_RTRE_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRE_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->useDestports) << ((uint8_t) SJA1105_RTRE_USE_DESTPORTS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRE_USE_DESTPORTS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->doNotLearn) << ((uint8_t) SJA1105_RTRE_DO_NOT_LEARN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRE_DO_NOT_LEARN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->vlanEgr) << ((uint8_t) SJA1105_RTRE_VLAN_EGR_SHIFT_0);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD0] = cData;
	tmp = (uint32_t) ((uint32_t) pk_retaggingTableReconfigurationEntry->vlanEgr >> (uint8_t) SJA1105_RTRE_VLAN_EGR_SHIFT_OVLP);  /* shift to right align subfield with register */
	tmp = ((uint32_t) SJA1105_RTRE_VLAN_EGR_MASK_1) & tmp;  /* mask field bits and add to data */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->vlanIng) << ((uint8_t) SJA1105_RTRE_VLAN_ING_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRE_VLAN_ING_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->ingPort) << ((uint8_t) SJA1105_RTRE_ING_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRE_ING_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationEntry->egrPort) << ((uint8_t) SJA1105_RTRE_EGR_PORT_SHIFT);  /* shift value to correct bit position */
	cData |= tmp;  /* add field to write data */
	registerValue[WORD1] = cData;
	ret = SJA1105_gpf_spiWrite32(deviceSelect, SJA1105_RTRE_WORD_COUNT, (uint32_t) SJA1105_RTRE_ADDR, registerValue);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the retagging_table_reconfiguration_control
*
* \param[in]  retaggingTableReconfigurationControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_retaggingTableReconfigurationControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setRetaggingTableReconfigurationControl(const SJA1105_retaggingTableReconfigurationControlArgument_t *pk_retaggingTableReconfigurationControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_retaggingTableReconfigurationControl->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationControl->valident <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_retaggingTableReconfigurationControl->index <= 0x3FU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_retaggingTableReconfigurationControl->valid) << ((uint8_t) SJA1105_RTRC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_retaggingTableReconfigurationControl->valident) << ((uint8_t) SJA1105_RTRC_VALIDENT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_RTRC_VALIDENT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_RTRC_INDEX_MASK) & pk_retaggingTableReconfigurationControl->index;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_RTRC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the retagging_table_reconfiguration_control
*
* \param[out] p_errors If found set on read, the most recent access resulted in an error because it was issued prior to completing the configuration load procedure.
* \return uint8_t
*/
uint8_t SJA1105_getRetaggingTableReconfigurationControl(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_RTRC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_RTRC_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_RTRC_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the general_parameters_block_reconfiguration
*
* \param[in]  generalParametersBlockReconfiguration Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_generalParametersBlockReconfigurationArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setGeneralParametersBlockReconfiguration(const SJA1105_generalParametersBlockReconfigurationArgument_t *pk_generalParametersBlockReconfiguration, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_generalParametersBlockReconfiguration->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_generalParametersBlockReconfiguration->mirrorp <= 0x7U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_generalParametersBlockReconfiguration->valid) << ((uint8_t) SJA1105_GPBR_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) SJA1105_GPBR_MIRRORP_MASK) & pk_generalParametersBlockReconfiguration->mirrorp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_GPBR_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the general_parameters_block_reconfiguration
*
* \param[out] p_errors If found set on read, the most recent access resulted in an error because dynamic reconfiguration was not enabled.
* \return uint8_t
*/
uint8_t SJA1105_getGeneralParametersBlockReconfiguration(uint8_t *p_errors, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_GPBR_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GPBR_ERRORS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_GPBR_ERRORS_SHIFT);  /* shift to right aligned position */
	*p_errors = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to SET data of the mac_configuration_table_reconfiguration_0
*
* \param[in]  macConfigurationTableReconfiguration0 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_macConfigurationTableReconfiguration0Argument_t
* \return uint8_t
*/
uint8_t SJA1105_setMacConfigurationTableReconfiguration0(const SJA1105_macConfigurationTableReconfiguration0Argument_t *pk_macConfigurationTableReconfiguration0, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration0->tpdelin) << ((uint8_t) SJA1105_MAC_CTR_0_TPDELIN_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	cData |= pk_macConfigurationTableReconfiguration0->tpdelout;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_MAC_CTR_0_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the mac_configuration_table_reconfiguration_1
*
* \param[in]  macConfigurationTableReconfiguration1 Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_macConfigurationTableReconfiguration1Argument_t
* \return uint8_t
*/
uint8_t SJA1105_setMacConfigurationTableReconfiguration1(const SJA1105_macConfigurationTableReconfiguration1Argument_t *pk_macConfigurationTableReconfiguration1, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_macConfigurationTableReconfiguration1->valid <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->speed <= 0x3U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->port <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->drpdtag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->drpuntag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->retag <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->dynLearn <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->egress <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->ingress <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->ingmirr <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->egrmirr <= 0x1U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->vlanprio <= 0x7U) ? 0U : 1U);
		valueCheckFailed += ((pk_macConfigurationTableReconfiguration1->vlanid <= 0xFFFU) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->valid) << ((uint8_t) SJA1105_MAC_CTR_1_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->speed) << ((uint8_t) SJA1105_MAC_CTR_1_SPEED_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_SPEED_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->port) << ((uint8_t) SJA1105_MAC_CTR_1_PORT_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_PORT_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->drpdtag) << ((uint8_t) SJA1105_MAC_CTR_1_DRPDTAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_DRPDTAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->drpuntag) << ((uint8_t) SJA1105_MAC_CTR_1_DRPUNTAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_DRPUNTAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->retag) << ((uint8_t) SJA1105_MAC_CTR_1_RETAG_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_RETAG_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->dynLearn) << ((uint8_t) SJA1105_MAC_CTR_1_DYN_LEARN_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_DYN_LEARN_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->egress) << ((uint8_t) SJA1105_MAC_CTR_1_EGRESS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_EGRESS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->ingress) << ((uint8_t) SJA1105_MAC_CTR_1_INGRESS_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_INGRESS_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->ingmirr) << ((uint8_t) SJA1105_MAC_CTR_1_INGMIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_INGMIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->egrmirr) << ((uint8_t) SJA1105_MAC_CTR_1_EGRMIRR_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_EGRMIRR_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) pk_macConfigurationTableReconfiguration1->vlanprio) << ((uint8_t) SJA1105_MAC_CTR_1_VLANPRIO_SHIFT);  /* shift value to correct bit position */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_VLANPRIO_MASK) & tmp;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	tmp = ((uint32_t) SJA1105_MAC_CTR_1_VLANID_MASK) & pk_macConfigurationTableReconfiguration1->vlanid;  /* mask field bits and add to data */
	cData |= tmp;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_MAC_CTR_1_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to SET data of the l2_lookup_parameters_control
*
* \param[in]  l2LookupParametersControl Struct containing all writable fields of the register.
* Details regarding the fields struct are found in the definition of ::SJA1105_l2LookupParametersControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105_setL2LookupParametersControl(const SJA1105_l2LookupParametersControlArgument_t *pk_l2LookupParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cData;

	uint32_t tmp;

	#ifdef VALUE_CHECKING
		uint8_t valueCheckFailed;
		
		/* Perform value checks on input variables */
		valueCheckFailed  = ((pk_l2LookupParametersControl->valid <= 0x1U) ? 0U : 1U);
		
		if (valueCheckFailed != 0U)
		{  /* value check failed */
			return valueCheckFailed;
		}
	#endif

	tmp = ((uint32_t) pk_l2LookupParametersControl->valid) << ((uint8_t) SJA1105_L2_LPC_VALID_SHIFT);  /* shift value to correct bit position */
	cData = (uint32_t) tmp;  /* write data */
	cData |= pk_l2LookupParametersControl->poly;  /* add field to write data */
	ret = SJA1105_gpf_spiWrite32(deviceSelect, 1U, (uint32_t) SJA1105_L2_LPC_ADDR, &cData);  /* write data via SPI */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_lookup_parameters_control
*
* \param[out] p_l2LookupParametersControl Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_l2LookupParametersControlArgument_t
* \return uint8_t
*/
uint8_t SJA1105_getL2LookupParametersControl(SJA1105_l2LookupParametersControlArgument_t *p_l2LookupParametersControl, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) SJA1105_L2_LPC_ADDR, &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_L2_LPC_VALID_SHIFT);  /* shift to right aligned position */
	p_l2LookupParametersControl->valid = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	p_l2LookupParametersControl->poly = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the general_status_ptp_egress
*
* \param[out] p_generalStatusPtpEgress Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_generalStatusPtpEgressArgument_t
* \param[in]  port Ethernet port number {0:4}
* \param[in]  timestamp Egress timestamp number number {0:1}
* \return uint8_t
*/
uint8_t SJA1105_getGeneralStatusPtpEgress(SJA1105_generalStatusPtpEgressArgument_t *p_generalStatusPtpEgress, uint8_t port, uint8_t timestamp, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_GS_PTP_E_ADDR + ((uint32_t) SJA1105_GS_PTP_E_PORT_INC * port) + ((uint32_t) timestamp)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_GS_PTP_E_PTPEGR_TS_SHIFT);  /* shift to right aligned position */
	p_generalStatusPtpEgress->ptpegrTs = (uint32_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_GS_PTP_E_UPDATE_MASK);   /* mask desired bits */
	p_generalStatusPtpEgress->update = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_memory_partition_status
*
* \param[out] p_l2MemoryPartitionStatus Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_l2MemoryPartitionStatusArgument_t
* \param[in]  partition Memory partition number {0:7}
* \return uint8_t
*/
uint8_t SJA1105_getL2MemoryPartitionStatus(SJA1105_l2MemoryPartitionStatusArgument_t *p_l2MemoryPartitionStatus, uint8_t partition, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_L2_MPS_ADDR + ((uint32_t) partition)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_L2_MPS_L2PARTS_SHIFT);  /* shift to right aligned position */
	p_l2MemoryPartitionStatus->l2parts = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_L2_MPS_N_L2PSPC_MASK);   /* mask desired bits */
	p_l2MemoryPartitionStatus->nL2pspc = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_level_diagnostic_counters
*
* \param[out] p_macLevelDiagnosticCounters Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_macLevelDiagnosticCountersArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getMacLevelDiagnosticCounters(SJA1105_macLevelDiagnosticCountersArgument_t *p_macLevelDiagnosticCounters, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_MAC_LDC_ADDR + ((uint32_t) SJA1105_MAC_LDC_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_MAC_LDC_N_RUNT_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticCounters->nRunt = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_MAC_LDC_N_SOFERR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticCounters->nSoferr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult >>= (uint32_t) (SJA1105_MAC_LDC_N_ALIGNERR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticCounters->nAlignerr = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	p_macLevelDiagnosticCounters->nMiierr = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the mac_level_diagnostic_flags
*
* \param[out] p_macLevelDiagnosticFlags Location of the struct containing all readable fields of the register
* Details regarding the fields struct are found in the definition of ::SJA1105_macLevelDiagnosticFlagsArgument_t
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getMacLevelDiagnosticFlags(SJA1105_macLevelDiagnosticFlagsArgument_t *p_macLevelDiagnosticFlags, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_MAC_LDF_ADDR + ((uint32_t) SJA1105_MAC_LDF_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_SPCPRIOR_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_SPCPRIOR_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->spcprior = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_PORTDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_PORTDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->portdrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_LENDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_LENDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->lendrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_BWDROPS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_BWDROPS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->bwdrops = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_DRN664ERRS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_DRN664ERRS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->drn664errs = (uint8_t) cResult;  /* deliver result */

	cResult   = (uint32_t) registerValue;
	cResult  &= (uint32_t) (SJA1105_MAC_LDF_SPCERRS_MASK);   /* mask desired bits */
	cResult >>= (uint32_t) (SJA1105_MAC_LDF_SPCERRS_SHIFT);  /* shift to right aligned position */
	p_macLevelDiagnosticFlags->spcerrs = (uint8_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_0
*
* \param[out] p_nTxbyte Number of bytes transmitted to the respective port since power-on or reset. The counter will wrap
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters10(uint64_t *p_nTxbyte, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_HIGH_LDC_1_0_WORD_COUNT] = {0};
	uint64_t nTxbyteTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_HIGH_LDC_1_0_WORD_COUNT, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_0_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nTxbyteTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nTxbyteTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_HIGH_LDC_1_0_N_TXBYTE_SHIFT_OVLP);
	*p_nTxbyte = (uint64_t) nTxbyteTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_1
*
* \param[out] p_nTxfrm Number of frames transmitted to the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters11(uint64_t *p_nTxfrm, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_HIGH_LDC_1_1_WORD_COUNT] = {0};
	uint64_t nTxfrmTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_HIGH_LDC_1_1_WORD_COUNT, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_1_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nTxfrmTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nTxfrmTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_HIGH_LDC_1_1_N_TXFRM_SHIFT_OVLP);
	*p_nTxfrm = (uint64_t) nTxfrmTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_2
*
* \param[out] p_nRxbyte Number of bytes received on the respective port in MAC-level correct frames. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters12(uint64_t *p_nRxbyte, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_HIGH_LDC_1_2_WORD_COUNT] = {0};
	uint64_t nRxbyteTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_HIGH_LDC_1_2_WORD_COUNT, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_2_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nRxbyteTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nRxbyteTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_HIGH_LDC_1_2_N_RXBYTE_SHIFT_OVLP);
	*p_nRxbyte = (uint64_t) nRxbyteTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_3
*
* \param[out] p_nRxfrm Number of MAC-level correct frames received on the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters13(uint64_t *p_nRxfrm, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue[SJA1105_HIGH_LDC_1_3_WORD_COUNT] = {0};
	uint64_t nRxfrmTmp;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, SJA1105_HIGH_LDC_1_3_WORD_COUNT, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_3_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue[WORD0];
	nRxfrmTmp  = (uint64_t) cResult;

	cResult   = (uint32_t) registerValue[WORD1];
	nRxfrmTmp |= (uint64_t) ((uint64_t) cResult << (uint8_t) SJA1105_HIGH_LDC_1_3_N_RXFRM_SHIFT_OVLP);
	*p_nRxfrm = (uint64_t) nRxfrmTmp;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_4
*
* \param[out] p_nPolerr Number of frames that were dropped based on the policing operation. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters14(uint32_t *p_nPolerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_4_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nPolerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_5
*
* \param[out] p_nCrcerr Number of frames that had a receive-side CRC error on this port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters15(uint32_t *p_nCrcerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_5_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nCrcerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_6
*
* \param[out] p_nSizeerr Number of frames with an invalid length on this port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters16(uint32_t *p_nSizeerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_6_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nSizeerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_7
*
* \param[out] p_nVlanerr Number of frames that were dropped because the VLAN ID was either not found in the VLAN Lookup table or the respective port is not listed in the VMEMB_PORT vector of the configured VLANID. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters17(uint32_t *p_nVlanerr, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_7_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nVlanerr = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_1_8
*
* \param[out] p_nN664err Number of frames dropped because they had an Ether-Type field other than 0x800 while the DRPNONA664 flag is set for the respective port. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters18(uint32_t *p_nN664err, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_1_8_ADDR + ((uint32_t) SJA1105_HIGH_LDC_1_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nN664err = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_2_0
*
* \param[out] p_notReach Number of frames that produced a match in the L2 Lookup Table but were not forwarded to the port this counter is assigned to because the port is not reachable for the respective ingress port as per REACH_PORTS of the L2 Forwarding Table. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters20(uint32_t *p_notReach, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_2_0_ADDR + ((uint32_t) SJA1105_HIGH_LDC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_notReach = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_2_1
*
* \param[out] p_egrDisabled This field counts the number of frames that were not routed to the port this counter is assigned to because the port was down as per the flag at position 1 of PORTST. The counter will wrap
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters21(uint32_t *p_egrDisabled, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_2_1_ADDR + ((uint32_t) SJA1105_HIGH_LDC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_egrDisabled = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_2_2
*
* \param[out] p_partDrop Number of frames that were dropped on ingress because the respective memory partition of the port as defined per PARTITION of the L2 Policing Table received at this port did not have any space left. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters22(uint32_t *p_partDrop, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_2_2_ADDR + ((uint32_t) SJA1105_HIGH_LDC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_partDrop = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the high_level_diagnostic_counters_2_3
*
* \param[out] p_qfull Number of frames that were dropped on egress because the respective priority queue of the destination port as defined per VLAN_PMAP of the L2 Forwarding Table did not have any space left. The counter will wrap.
* \param[in]  port Ethernet port number {0:4}
* \return uint8_t
*/
uint8_t SJA1105_getHighLevelDiagnosticCounters23(uint32_t *p_qfull, uint8_t port, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_HIGH_LDC_2_3_ADDR + ((uint32_t) SJA1105_HIGH_LDC_2_PORT_INC * port)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_qfull = (uint32_t) cResult;  /* deliver result */

	return ret;
}

/**
* \brief This function is used to GET data of the l2_memory_partition_error_counters
*
* \param[out] p_nL2pspcdrn Number of frames dropped due to lack of L2 memory partition space. The counter will wrap.
* \param[in]  partition Memory partition number {0:7}
* \return uint8_t
*/
uint8_t SJA1105_getL2MemoryPartitionErrorCounters(uint32_t *p_nL2pspcdrn, uint8_t partition, uint8_t deviceSelect)
{
	uint8_t ret;
	uint32_t cResult;

	uint32_t registerValue = 0;

	/* split received register values into bit groups */
	ret = SJA1105_gpf_spiRead32(deviceSelect, 1U, (uint32_t) ((uint32_t) SJA1105_L2_MPEC_ADDR + ((uint32_t) partition)), &registerValue);  /* read data via SPI from register at address -0x1*/
	cResult   = (uint32_t) registerValue;
	*p_nL2pspcdrn = (uint32_t) cResult;  /* deliver result */

	return ret;
}
