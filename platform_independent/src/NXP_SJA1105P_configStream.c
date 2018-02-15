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

#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_spi.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define CONFIG_BASE_ADDR (0x20000U)  /**< Base address of the configuration area */
#define N_CONFIGS 3U  /**< Number of configurations that can be loaded */
#define N_BURSTS_CONFIG0 5U  /**< Number of bursts in configuration 0 */
#define N_BURSTS_CONFIG1 4U  /**< Number of bursts in configuration 1 */
#define N_BURSTS_CONFIG2 4U  /**< Number of bursts in configuration 2 */

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_loadConfig(uint8_t configIndex, uint8_t switchId)
{
	uint8_t ret = 0;
	uint8_t block;

	/* Automatically generated from cfg_applicationBoard.hex */
	uint32_t *p_configBurstList0[N_BURSTS_CONFIG0];
	const uint8_t  k_burstLength0[N_BURSTS_CONFIG0] = {64, 64, 64, 64, 30};
	static uint32_t configBurst0_0[64] = {0xAE00030EU, 0x05000000U, 0x00000005U, 0x9A1CA02BU, 0x00420000U, 0x04250000U, 0xFFF88AC0U, 0xFFFFFFFFU, 0x0000FFFFU, 0x4C8C9BEAU, 0x06000000U, 0x0000005AU, 0x4ED2C50FU, 0x02FB0000U, 0x00EE73E8U, 0x02FB1000U, 0x04EE73E8U, 0x02FB2000U, 0x08EE73E8U, 0x02FB3000U, 0x0CEE73E8U, 0x02FB4000U, 0x10EE73E8U, 0x02FB5000U, 0x14EE73E8U, 0x02FB6000U, 0x18EE73E8U, 0x02FB7000U, 0x1CEE73E8U, 0x02FB0000U, 0x20EE7064U, 0x02FB1000U, 0x24EE700AU, 0x02FB2000U, 0x28EE700AU, 0x02FB3000U, 0x2CEE700AU, 0x02FB4000U, 0x30EE700AU, 0x02FB5000U, 0x34EE700AU, 0x02FB6000U, 0x38EE700AU, 0x02FB7000U, 0x3CEE700AU, 0x02FB0000U, 0x40EE7064U, 0x02FB1000U, 0x44EE7000U, 0x02FB2000U, 0x48EE7000U, 0x02FB3000U, 0x4CEE7000U, 0x02FB4000U, 0x50EE7000U, 0x02FB5000U, 0x54EE7000U, 0x02FB6000U, 0x58EE7000U, 0x02FB7000U, 0x5CEE7000U, 0x02FB0000U, 0x60EE7064U, 0x02FB1000U};
	static uint32_t configBurst0_1[64] = {0x64EE7064U, 0x02FB2000U, 0x68EE7064U, 0x02FB3000U, 0x6CEE7064U, 0x02FB4000U, 0x70EE7064U, 0x02FB5000U, 0x74EE7064U, 0x02FB6000U, 0x78EE7064U, 0x02FB7000U, 0x7CEE7064U, 0x02FB0000U, 0x80EE7064U, 0x02FB1000U, 0x84EE7064U, 0x02FB2000U, 0x88EE7064U, 0x02FB3000U, 0x8CEE7064U, 0x02FB4000U, 0x90EE7064U, 0x02FB5000U, 0x94EE7064U, 0x02FB6000U, 0x98EE7064U, 0x02FB7000U, 0x9CEE7064U, 0x02FB0000U, 0xA0EE73E8U, 0x02FB0000U, 0xA4EE7064U, 0x02FB0000U, 0xA8EE7064U, 0x02FB0000U, 0xACEE7064U, 0x02FB0000U, 0xB0EE7064U, 0x0787A30DU, 0x07000000U, 0x00000022U, 0xDD3964CCU, 0xF8000000U, 0x003FF07FU, 0x00000000U, 0x003FFF80U, 0x08000000U, 0x003FFF80U, 0x10000000U, 0x003FFF80U, 0x18000000U, 0x003FFF80U, 0x20000000U, 0x003FFF80U, 0x28000000U, 0x003FFF80U, 0x30000000U, 0x003FFF80U, 0x38000000U, 0x003FFF80U, 0x40000000U, 0x003FFF80U, 0x48000000U};
	static uint32_t configBurst0_2[64] = {0x003FFF80U, 0x50000000U, 0x003FFF80U, 0x58000000U, 0x003FFF80U, 0x60000000U, 0x003FFF80U, 0x68000000U, 0x003FFF80U, 0x70000000U, 0x003FFF80U, 0x78000000U, 0x003FFF80U, 0x095A03A7U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7BDF58DU, 0x10000000U, 0xEF7BF58DU, 0x10000000U, 0xDEF7F58DU, 0x10000000U, 0xBDEFF58DU, 0x10000000U, 0x7FDFF58DU, 0x00000000U, 0x00000000U, 0x92000000U, 0x00000024U, 0x24000000U, 0x00000049U, 0xB6000000U, 0x0000006DU, 0x48000000U, 0x00000092U, 0xDA000000U, 0x000000B6U, 0x6C000000U, 0x000000DBU, 0xFE000000U, 0x000000FFU, 0x0978A15DU, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U};
	static uint32_t configBurst0_3[64] = {0xFE3FFC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0xDF73FD2AU, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x02000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x34E31D78U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x321A4000U, 0x0C8320C8U, 0x0320C832U, 0x6FEF4A56U, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x40980DF7U, 0xE6C9318CU, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U, 0x42040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFFDD37FFU, 0xFFFF0000U, 0x0000FFFFU, 0x0EFFFFFFU, 0x80C20000U, 0x00000301U, 0x450180C2U, 0xD7C68C8DU, 0x13000000U, 0x00000014U};
	static uint32_t configBurst0_4[30] = {0x9D19F2F3U, 0x90000000U, 0x700003D0U, 0xFC1DC994U, 0xFDBFFFFFU, 0x1DBFFFFFU, 0xA0000000U, 0x60002625U, 0xFC1DA73FU, 0xFDBFFFFFU, 0x19BFFFFFU, 0x40000000U, 0xC0004C4BU, 0xFC1D8119U, 0xFDBFFFFFU, 0x15BFFFFFU, 0x80000000U, 0x80009896U, 0xFC1D34CEU, 0xFDBFFFFFU, 0x11BFFFFFU, 0x9DE75D2DU, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x24040000U, 0x792BFEC9U, 0x00000000U, 0x00000000U, 0x1864766BU};

	/* Automatically generated from cfg_cascadedBoard_switch0.hex */
	uint32_t *p_configBurstList1[N_BURSTS_CONFIG1];
	const uint8_t  k_burstLength1[N_BURSTS_CONFIG1] = {64, 64, 64, 19};
	static uint32_t configBurst1_0[64] = {0xAE00030EU, 0x06000000U, 0x00000050U, 0x216F256BU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU};
	static uint32_t configBurst1_1[64] = {0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xE99963C1U, 0x07000000U, 0x00000002U, 0x7D0BCBF2U, 0xF8000000U, 0x003FF07FU, 0xC3202DD3U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7FDF58DU, 0x10000000U, 0xEF79F58DU, 0x10000000U, 0xDEF5F58DU, 0x10000000U, 0xBDEDF58DU, 0x10000000U, 0x7BDDF58DU, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x7945BC6FU, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U};
	static uint32_t configBurst1_2[64] = {0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0102U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0102U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0102U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x3DD1904CU, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x02000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x34E31D78U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x0071C000U, 0x00000000U, 0x00000000U, 0xC3F704B9U, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x40980DF7U, 0xE6C9318CU, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U};
	static uint32_t configBurst1_3[19] = {0x42040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFFD837FFU, 0xFFFF0000U, 0x0000FFFFU, 0x0EFFFFFFU, 0x80C20000U, 0x00000301U, 0x450180C2U, 0x1A4FBCC4U, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x64980000U, 0xD2037A4FU, 0x00000000U, 0x00000000U, 0x43E06734U};

	/* Automatically generated from cfg_cascadedBoard_switch1.hex */
	uint32_t *p_configBurstList2[N_BURSTS_CONFIG2];
	const uint8_t  k_burstLength2[N_BURSTS_CONFIG2] = {64, 64, 64, 19};
	static uint32_t configBurst2_0[64] = {0xAE00030EU, 0x06000000U, 0x00000050U, 0x216F256BU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x03FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x07FFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0BFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU};
	static uint32_t configBurst2_1[64] = {0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x0FFFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xFEFB0000U, 0x13FFFFFFU, 0xE99963C1U, 0x07000000U, 0x00000002U, 0x7D0BCBF2U, 0xF8000000U, 0x003FF07FU, 0xC3202DD3U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7BDF58DU, 0x10000000U, 0xEF7BF58DU, 0x10000000U, 0xDEF7F58DU, 0x10000000U, 0xBDEFF58DU, 0x10000000U, 0x7BDFF58DU, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0xC004A606U, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U};
	static uint32_t configBurst2_2[64] = {0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0104U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U, 0xFE3FFC03U, 0x00000001U, 0x07FC0102U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x95725AC0U, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x02000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x34E31D78U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x0071C000U, 0x00000000U, 0x00000000U, 0xC3F704B9U, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x00980DF7U, 0x9015701CU, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U};
	static uint32_t configBurst2_3[19] = {0x42040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFFDD37FFU, 0xFFFF0000U, 0x0000FFFFU, 0x0EFFFFFFU, 0x80C20000U, 0x00000301U, 0x4D0180C2U, 0xD91D04BFU, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x64920000U, 0x28EC92C5U, 0x00000000U, 0x00000000U, 0x43E06734U};

	uint32_t **pp_configBurstList[N_CONFIGS];
	const uint8_t *kp_burstLength[N_CONFIGS];
	const uint8_t k_nBursts[N_CONFIGS] = {N_BURSTS_CONFIG0, N_BURSTS_CONFIG1, N_BURSTS_CONFIG2};

	p_configBurstList0[0] = configBurst0_0;
	p_configBurstList0[1] = configBurst0_1;
	p_configBurstList0[2] = configBurst0_2;
	p_configBurstList0[3] = configBurst0_3;
	p_configBurstList0[4] = configBurst0_4;

	p_configBurstList1[0] = configBurst1_0;
	p_configBurstList1[1] = configBurst1_1;
	p_configBurstList1[2] = configBurst1_2;
	p_configBurstList1[3] = configBurst1_3;

	p_configBurstList2[0] = configBurst2_0;
	p_configBurstList2[1] = configBurst2_1;
	p_configBurstList2[2] = configBurst2_2;
	p_configBurstList2[3] = configBurst2_3;

	pp_configBurstList[0] = p_configBurstList0;
	pp_configBurstList[1] = p_configBurstList1;
	pp_configBurstList[2] = p_configBurstList2;

	kp_burstLength[0] = k_burstLength0;
	kp_burstLength[1] = k_burstLength1;
	kp_burstLength[2] = k_burstLength2;

	for (block = 0; block < k_nBursts[configIndex]; block++)
	{
		if (SJA1105P_gpf_spiWrite32(switchId, kp_burstLength[configIndex][block], CONFIG_BASE_ADDR + block, pp_configBurstList[configIndex][block]) != 0U)
		{
			ret = 1;
			break;  /* configuration was unsuccessful */
		}
	}

	return ret;
}