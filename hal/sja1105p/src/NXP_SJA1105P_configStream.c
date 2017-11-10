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

	/* Automatically generated from cfg_applicationBoard_SJA1105QS.hex */
	uint32_t *p_configBurstList0[N_BURSTS_CONFIG0];
	const uint8_t  k_burstLength0[N_BURSTS_CONFIG0] = {64, 64, 64, 64, 30};
	static uint32_t configBurst0_0[64] = {0xAE00030EU, 0x05000000U, 0x00000005U, 0x9A1CA02BU, 0x00420000U, 0x04250000U, 0xFFF88AC0U, 0xFFFFFFFFU, 0x0000FFFFU, 0x4C8C9BEAU, 0x06000000U, 0x0000005AU, 0x4ED2C50FU, 0x02FB0000U, 0x00EE73E8U, 0x02FB1000U, 0x04EE73E8U, 0x02FB2000U, 0x08EE73E8U, 0x02FB3000U, 0x0CEE73E8U, 0x02FB4000U, 0x10EE73E8U, 0x02FB5000U, 0x14EE73E8U, 0x02FB6000U, 0x18EE73E8U, 0x02FB7000U, 0x1CEE73E8U, 0x02FB0000U, 0x20EE73E8U, 0x02FB1000U, 0x24EE7064U, 0x02FB2000U, 0x28EE7064U, 0x02FB3000U, 0x2CEE7064U, 0x02FB4000U, 0x30EE7064U, 0x02FB5000U, 0x34EE7064U, 0x02FB6000U, 0x38EE7064U, 0x02FB7000U, 0x3CEE7064U, 0x02FB0000U, 0x40EE73E8U, 0x02FB1000U, 0x44EE7000U, 0x02FB2000U, 0x48EE7000U, 0x02FB3000U, 0x4CEE7000U, 0x02FB4000U, 0x50EE7000U, 0x02FB5000U, 0x54EE7000U, 0x02FB6000U, 0x58EE7000U, 0x02FB7000U, 0x5CEE7000U, 0x02FB0000U, 0x60EE73E8U, 0x02FB1000U};
	static uint32_t configBurst0_1[64] = {0x64EE73E8U, 0x02FB2000U, 0x68EE73E8U, 0x02FB3000U, 0x6CEE73E8U, 0x02FB4000U, 0x70EE73E8U, 0x02FB5000U, 0x74EE73E8U, 0x02FB6000U, 0x78EE73E8U, 0x02FB7000U, 0x7CEE73E8U, 0x02FB0000U, 0x80EE73E8U, 0x02FB1000U, 0x84EE73E8U, 0x02FB2000U, 0x88EE73E8U, 0x02FB3000U, 0x8CEE73E8U, 0x02FB4000U, 0x90EE73E8U, 0x02FB5000U, 0x94EE73E8U, 0x02FB6000U, 0x98EE73E8U, 0x02FB7000U, 0x9CEE73E8U, 0x02FB0000U, 0xA0EE73E8U, 0x02FB0000U, 0xA4EE73E8U, 0x02FB0000U, 0xA8EE73E8U, 0x02FB0000U, 0xACEE73E8U, 0x02FB0000U, 0xB0EE73E8U, 0x268D5F54U, 0x07000000U, 0x00000022U, 0xDD3964CCU, 0x58000000U, 0x003FF011U, 0x00000000U, 0x003FFF80U, 0x08000000U, 0x003FFF80U, 0x10000000U, 0x003FFF80U, 0x18000000U, 0x003FFF80U, 0x20000000U, 0x003FFF80U, 0x28000000U, 0x003FFF80U, 0x30000000U, 0x003FFF80U, 0x38000000U, 0x003FFF80U, 0x40000000U, 0x003FFF80U, 0x48000000U};
	static uint32_t configBurst0_2[64] = {0x003FFF80U, 0x50000000U, 0x003FFF80U, 0x58000000U, 0x003FFF80U, 0x60000000U, 0x003FFF80U, 0x68000000U, 0x003FFF80U, 0x70000000U, 0x003FFF80U, 0x78000000U, 0x003FFF80U, 0x9E284B54U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7BDF58DU, 0x10000000U, 0xEF7BF58DU, 0x10000000U, 0xDEF7F58DU, 0x10000000U, 0xBDEFF58DU, 0x10000000U, 0x7FDFF58DU, 0x00000000U, 0x00000000U, 0x92000000U, 0x00000024U, 0x24000000U, 0x00000049U, 0xB6000000U, 0x0000006DU, 0x48000000U, 0x00000092U, 0xDA000000U, 0x000000B6U, 0x6C000000U, 0x000000DBU, 0xFE000000U, 0x000000FFU, 0x0978A15DU, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U};
	static uint32_t configBurst0_3[64] = {0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x5FFE1105U, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x02000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x34E31D78U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x321A4000U, 0x0C8320C8U, 0x0320C832U, 0x6FEF4A56U, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U, 0x42040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFFDD37FFU, 0xFFFF0000U, 0x0000FFFFU, 0x0EFFFFFFU, 0x80C20000U, 0x00000301U, 0x450180C2U, 0xD7C68C8DU, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x40980DF7U, 0xE6C9318CU, 0x13000000U, 0x00000014U};
	static uint32_t configBurst0_4[30] = {0x9D19F2F3U, 0x90000000U, 0x700003D0U, 0xFC1DC994U, 0xFDFFFFFFU, 0x1DFFFFFFU, 0xA0000000U, 0x60002625U, 0xFC1DA73FU, 0xFDFFFFFFU, 0x19FFFFFFU, 0x40000000U, 0xC0004C4BU, 0xFC1D8119U, 0xFDFFFFFFU, 0x15FFFFFFU, 0x80000000U, 0x80009896U, 0xFC1D34CEU, 0xFDFFFFFFU, 0x11FFFFFFU, 0x41FBF56DU, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x40120000U, 0x2F6CEE5FU, 0x00000000U, 0x00000000U, 0xFA94C50BU};

	/* Automatically generated from cfg_cascadedBoard_switch0.hex */
	uint32_t *p_configBurstList1[N_BURSTS_CONFIG1];
	const uint8_t  k_burstLength1[N_BURSTS_CONFIG1] = {64, 64, 64, 51};
	static uint32_t configBurst1_0[64] = {0xAE00030EU, 0x06000000U, 0x00000050U, 0x216F256BU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU};
	static uint32_t configBurst1_1[64] = {0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFA2E19F8U, 0x07000000U, 0x00000022U, 0xDD3964CCU, 0x58000000U, 0x003FF011U, 0x00000000U, 0x003FFF80U, 0x08000000U, 0x003FFF80U, 0x10000000U, 0x003FFF80U, 0x18000000U, 0x003FFF80U, 0x20000000U, 0x003FFF80U, 0x28000000U, 0x003FFF80U, 0x30000000U, 0x003FFF80U, 0x38000000U, 0x003FFF80U, 0x40000000U, 0x003FFF80U, 0x48000000U, 0x003FFF80U, 0x50000000U, 0x003FFF80U, 0x58000000U, 0x003FFF80U, 0x60000000U, 0x003FFF80U, 0x68000000U, 0x003FFF80U, 0x70000000U, 0x003FFF80U, 0x78000000U, 0x003FFF80U, 0x9E284B54U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7FDF58DU};
	static uint32_t configBurst1_2[64] = {0x10000000U, 0xEF79F58DU, 0x10000000U, 0xDEF5F58DU, 0x10000000U, 0xBDEDF58DU, 0x10000000U, 0x7BDDF58DU, 0x00000000U, 0x00000000U, 0x92000000U, 0x00000024U, 0x24000000U, 0x00000049U, 0xB6000000U, 0x0000006DU, 0x48000000U, 0x00000092U, 0xDA000000U, 0x000000B6U, 0x6C000000U, 0x000000DBU, 0xFE000000U, 0x000000FFU, 0xDE03FA6FU, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U};
	static uint32_t configBurst1_3[51] = {0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0xAF331A68U, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x00000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x54BFA833U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x0071C000U, 0x00000000U, 0x00000000U, 0xC3F704B9U, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U, 0x46040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFF1837FFU, 0xFFFF0000U, 0xFFFFFFFFU, 0x0EFFFFFFU, 0x80C20000U, 0xFFFFFF01U, 0x45FFFFFFU, 0x31685A7FU, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x40980DF7U, 0xE6C9318CU, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x69180000U, 0x97319EB9U, 0x00000000U, 0x00000000U, 0x236CAA2FU};

	/* Automatically generated from cfg_cascadedBoard_switch1.hex */
	uint32_t *p_configBurstList2[N_BURSTS_CONFIG2];
	const uint8_t  k_burstLength2[N_BURSTS_CONFIG2] = {64, 64, 64, 51};
	static uint32_t configBurst2_0[64] = {0xAE00030EU, 0x06000000U, 0x00000050U, 0x216F256BU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x03FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x07FFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0BFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU};
	static uint32_t configBurst2_1[64] = {0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x0FFFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFEF70000U, 0x13FFFFFFU, 0xFA2E19F8U, 0x07000000U, 0x00000022U, 0xDD3964CCU, 0x58000000U, 0x003FF011U, 0x00000000U, 0x003FFF80U, 0x08000000U, 0x003FFF80U, 0x10000000U, 0x003FFF80U, 0x18000000U, 0x003FFF80U, 0x20000000U, 0x003FFF80U, 0x28000000U, 0x003FFF80U, 0x30000000U, 0x003FFF80U, 0x38000000U, 0x003FFF80U, 0x40000000U, 0x003FFF80U, 0x48000000U, 0x003FFF80U, 0x50000000U, 0x003FFF80U, 0x58000000U, 0x003FFF80U, 0x60000000U, 0x003FFF80U, 0x68000000U, 0x003FFF80U, 0x70000000U, 0x003FFF80U, 0x78000000U, 0x003FFF80U, 0x9E284B54U, 0x08000000U, 0x0000001AU, 0x6AF62353U, 0x10000000U, 0xF7BDF58DU};
	static uint32_t configBurst2_2[64] = {0x10000000U, 0xEF7BF58DU, 0x10000000U, 0xDEF7F58DU, 0x10000000U, 0xBDEFF58DU, 0x10000000U, 0x7BDFF58DU, 0x00000000U, 0x00000000U, 0x92000000U, 0x00000024U, 0x24000000U, 0x00000049U, 0xB6000000U, 0x0000006DU, 0x48000000U, 0x00000092U, 0xDA000000U, 0x000000B6U, 0x6C000000U, 0x000000DBU, 0xFE000000U, 0x000000FFU, 0x6742E006U, 0x09000000U, 0x00000028U, 0x0DB43AC9U, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0104U, 0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x80000000U, 0xFE08AC03U, 0x00000001U, 0x08FC0102U};
	static uint32_t configBurst2_3[51] = {0xBF404FE4U, 0xE017FB02U, 0x06FF40CFU, 0xFFF03BFCU, 0x0790D0E4U, 0x0D000000U, 0x00000004U, 0x12D08C8FU, 0x00000000U, 0x00000000U, 0x04008010U, 0x00010020U, 0x54BFA833U, 0x0E000000U, 0x00000003U, 0xC8A7CEE6U, 0x0071C000U, 0x00000000U, 0x00000000U, 0xC3F704B9U, 0x11000000U, 0x0000000BU, 0xEFA3E65AU, 0x01C00000U, 0x46040000U, 0xFFFFFC45U, 0xFFFFFFFFU, 0xFF1D37FFU, 0xFFFF0000U, 0xFFFFFFFFU, 0x0EFFFFFFU, 0x80C20000U, 0xFFFFFF01U, 0x4DFFFFFFU, 0xF23AE204U, 0x10000000U, 0x00000004U, 0x8AA0DFBCU, 0x80000000U, 0x0DF03FFBU, 0xB2B78098U, 0x00980DF7U, 0x9015701CU, 0x4E000000U, 0x00000001U, 0x3A5D5E24U, 0x64920000U, 0x28EC92C5U, 0x00000000U, 0x00000000U, 0x236CAA2FU};

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
