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
* \file NXP_SJA1105_portConfig.c
*
* \author NXP Semiconductors
*
* \date 2017-01-24
*
* \brief Configure speed and mode of a port
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_portConfig.h"
#include "NXP_SJA1105_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105_clockGenerationUnit.h"
#include "NXP_SJA1105_switchCore.h"
#include "NXP_SJA1105_config.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define DIVIDE_BY_10 9U

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static uint8_t g_pll1Enabled[SJA1105_N_SWITCHES] = {0}; /* Indicates the PLL1 enabled status for all the switches */

/******************************************************************************
* INTERNAL TYPE DEFINITIONS
*****************************************************************************/

typedef enum
{
	e_clkSrcCategory_TX   = 0,
	e_clkSrcCategory_RX   = 1,
	e_clkSrcCategory_IDIV = 2
} clkSrcCategory_t;

typedef struct
{
	uint8_t base;    /* base index of port 0 clk source */
	uint8_t offset;  /* per port offset for the clk source */
} clkSrcProperties_t;

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t configPort(uint8_t port, uint8_t switchId, SJA1105_speed_t speed, SJA1105_xmiiMode_t xmiiMode, uint8_t phyMode);
static uint8_t enablePLL1(uint8_t switchId);
static SJA1105_clksrc_t getClkSrcAtPort(clkSrcCategory_t clkSrcCategory, uint8_t port);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Automatically configure CGU and ACU setting of all ports.\n
* This function reads out the port configuration from the ACU.
* It corresponds to the configuration loaded into the switch core.
* Consequently, the right CGU and ACU settings are taken.
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
extern uint8_t SJA1105_autoConfigPorts(void)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t port;
	SJA1105_portStatusMiixArgument_t portStatus;

	for (switchId=0; switchId < SJA1105_N_SWITCHES; switchId++)
	{
		g_pll1Enabled[switchId] = 0;
		for (port=0; port < SJA1105_N_PORTS; port++)
		{
			ret += SJA1105_getPortStatusMiix(&portStatus, port, switchId);
			ret += configPort(port, switchId, portStatus.speed, portStatus.xmiiMode, portStatus.phyMode);
		}
	}

	return ret;
}

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
* \brief Automatically configure CGU and ACU setting of all ports.\n
* This function reads out the port configuration from the ACU.
* It corresponds to the configuration loaded into the switch core.
* Consequently, the right CGU and ACU settings are taken.
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
extern uint8_t SJA1105_autoConfigPorts_extended(uint8_t switchId)
{
	uint8_t ret = 0;
	uint8_t port;
	SJA1105_portStatusMiixArgument_t portStatus;

	g_pll1Enabled[switchId] = 0;
	for (port=0; port < SJA1105_N_PORTS; port++)
	{
		ret += SJA1105_getPortStatusMiix(&portStatus, port, switchId);
		ret += configPort(port, switchId, portStatus.speed, portStatus.xmiiMode, portStatus.phyMode);
	}


	return ret;
}

/**
* \brief Configure speed and xMII mode of a given port.\n
* For MII and RMII, PHY and MAC mode can be selected.
*
* \param[in]  port Physical port to be configured
* \param[in]  switchId SwitchId of the port to be configured
* \param[in]  speed Port speed
* \param[in]  xmiiMode Port xMII mode
* \param[in]  phyMode If set, Port is configured in PHY mode. Else, MAC mode is used
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
static uint8_t configPort(uint8_t port, uint8_t switchId, SJA1105_speed_t speed, SJA1105_xmiiMode_t xmiiMode, uint8_t phyMode)
{
	uint8_t ret;
	SJA1105_miixClockControlRegisterArgument_t miixClockControlRegister;
	SJA1105_idivCControlRegisterArgument_t idivCControlRegister;
	SJA1105_cfgPadMiixArgument_t cfgPadMiix;

	miixClockControlRegister.autoblock = 1;
	miixClockControlRegister.pd        = 0;

	idivCControlRegister.autoblock     = 1;
	idivCControlRegister.pd            = 0;
	idivCControlRegister.idiv          = DIVIDE_BY_10;

	switch (xmiiMode)
	{
		case e_xmiiMode_MII:
		{
			miixClockControlRegister.clksrc = getClkSrcAtPort(e_clkSrcCategory_RX, port);
			ret = SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_MII_RX_CLK, switchId);
			if (phyMode == 0U)
			{  /* MII MAC Mode */
				idivCControlRegister.pd = 1;  /* deactivate IDIV to save power */
				ret += SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
				miixClockControlRegister.clksrc = getClkSrcAtPort(e_clkSrcCategory_TX, port);
				ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_MII_TX_CLK, switchId);
			}
			else
			{  /* MII PHY Mode */
				if (speed == e_speed_10_MBPS)
				{
					ret = SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
				}
				if (ret == 0U)
				{  /* everything went fine so far, go on */
					ret = SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
					miixClockControlRegister.clksrc = getClkSrcAtPort(e_clkSrcCategory_IDIV, port);
					ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_MII_TX_CLK, switchId);
					ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_EXT_TX_CLK, switchId);
					ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_EXT_RX_CLK, switchId);
				}
			}
			break;
		}
		case e_xmiiMode_RMII:
		{
			idivCControlRegister.pd = 1;  /* deactivate IDIV to save power */
			ret = SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			miixClockControlRegister.clksrc = getClkSrcAtPort(e_clkSrcCategory_TX, port);
			ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_RMII_REF_CLK, switchId);
			if (phyMode == 0U)
			{  /* RMII MAC Mode */
				ret += enablePLL1(switchId);
				miixClockControlRegister.clksrc = e_clksrc_PLL1;
				ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_EXT_TX_CLK, switchId);
			}
			break;
		}
		case e_xmiiMode_RGMII:  /* RGMII */
		{
			 /* Set slew rate of TX pins to Hi Speed */
			ret = SJA1105_getCfgPadMiix(&cfgPadMiix, port, e_direction_TX, switchId);  /* Read   cfgPadMiix */
			/* Modify cfgPadMiix */
			cfgPadMiix.clkOs = e_padOutputStageSpeed_HIGH_SPEED;
			cfgPadMiix.ctrlOs = e_padOutputStageSpeed_HIGH_SPEED;
			cfgPadMiix.d32Os = e_padOutputStageSpeed_HIGH_SPEED;
			cfgPadMiix.d10Os = e_padOutputStageSpeed_HIGH_SPEED;

			ret += SJA1105_setCfgPadMiix(&cfgPadMiix, port, e_direction_TX, switchId);  /* Write  cfgPadMiix */
			
			miixClockControlRegister.clksrc = getClkSrcAtPort(e_clkSrcCategory_IDIV, port);
			if (speed != e_speed_100_MBPS)
			{
				if (speed == e_speed_1_GBPS)
				{
					idivCControlRegister.pd = 1;  /* switch off to save power */
					miixClockControlRegister.clksrc = e_clksrc_PLL0;
				}
				ret += SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			}
			ret += SJA1105_setMiixClockControlRegister(&miixClockControlRegister, port, e_miixInternalClk_RGMII_TX_CLK, switchId);
			break;
		}
		default:  /* unused port */
		{
			idivCControlRegister.pd = 1;  /* deactivate IDIV to save power */
			ret += SJA1105_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			break;
		}
	}
	return ret;
}

/**
* \brief Enable PLL1\n
* PLL1 is needed for RMII PHY mode operation
*
* \return uint8_t Returns 0 upon success, else failed.
*/
static uint8_t enablePLL1(uint8_t switchId)
{
	uint8_t ret = 0;
	
	SJA1105_pll1CControlRegisterArgument_t pll1CControlRegister;
	if (g_pll1Enabled[switchId] == 0U)
	{  /* PLL1 has to be enabled first */
		pll1CControlRegister.pd        = 1;  /* disabled */
		pll1CControlRegister.bypass    = 0;  /* Not bypassed */
		pll1CControlRegister.fbsel     = 1;  /* Feedback selected */
		pll1CControlRegister.direct    = 0;  /* Direct enabled */
		pll1CControlRegister.psel      = 1;  /* P divider for 50 MHz generation */
		pll1CControlRegister.msel      = 1;  /* M divider for 50 MHz generation */
		pll1CControlRegister.autoblock = 1;  /* Autoblock disabled */
		ret = SJA1105_setPll1CControlRegister(&pll1CControlRegister, switchId);  /* Setup PLL1 */
		pll1CControlRegister.pd        = 0;  /* enabled */
		ret += SJA1105_setPll1CControlRegister(&pll1CControlRegister, switchId);  /* Enable PLL1 */
		g_pll1Enabled[switchId] = 1;
	}
	return ret;
}

static SJA1105_clksrc_t getClkSrcAtPort(clkSrcCategory_t clkSrcCategory, uint8_t port)
{
	SJA1105_clksrc_t clkSrc;
	static clkSrcProperties_t clkSrcProperties[] = {
		{
			/*.base =*/   (uint8_t) e_clksrc_MII0_TX_CLK,
			/*.offset =*/ ((uint8_t) e_clksrc_MII1_TX_CLK) - ((uint8_t) e_clksrc_MII0_TX_CLK)
		},
		{
			/*.base   =*/ (uint8_t) (uint8_t) e_clksrc_MII0_RX_CLK,
			/*.offset =*/ ((uint8_t) e_clksrc_MII1_RX_CLK) - ((uint8_t) e_clksrc_MII0_RX_CLK)
		},
		{
			/*.base   =*/ (uint8_t) e_clksrc_IDIV0,
			/*.offset =*/ ((uint8_t) e_clksrc_IDIV1) - ((uint8_t) e_clksrc_IDIV0)
		}
	};
	clkSrc = SJA1105_convertToClksrc((uint32_t) (clkSrcProperties[clkSrcCategory].base + ((uint32_t) port * clkSrcProperties[clkSrcCategory].offset)));
	return clkSrc;
}
