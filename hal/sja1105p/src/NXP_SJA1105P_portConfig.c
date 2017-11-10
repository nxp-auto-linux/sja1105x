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
* \file NXP_SJA1105P_portConfig.c
*
* \author NXP Semiconductors
*
* \date 2017-03-02
*
* \brief Configure speed and mode of a port
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_portConfig.h"
#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_clockGenerationUnit.h"
#include "NXP_SJA1105P_sgmii.h"
#include "NXP_SJA1105P_switchCore.h"
#include "NXP_SJA1105P_config.h"
#include <linux/printk.h>
/******************************************************************************
* DEFINES
*****************************************************************************/

#define SGMII_PORT 4U

#define CLOCK_DELAY_81_0_DEG 810U  /**< 81.0 degrees */
#define CLOCK_DELAY_92_7_DEG 927U  /**< 92.7 degrees */
#define MINIMUM_CLK_DELAY 738U  /**< 73.8 degrees */
#define STEPSIZE_CLK_DELAY  9U  /**<  0.9 degrees */

/******************************************************************************
* INTERNAL VARIABLES
*****************************************************************************/

static uint8_t g_pll1Enabled[SJA1105P_N_SWITCHES] = {0};  /* Indicates the PLL1 enabled status for all the switches */

/******************************************************************************
* INTERNAL TYPE DEFINITIONS
*****************************************************************************/

typedef enum
{
	SJA1105P_e_clkSrcCategory_TX   = 0,
	SJA1105P_e_clkSrcCategory_RX   = 1,
	SJA1105P_e_clkSrcCategory_IDIV = 2
} clkSrcCategory_t;

typedef struct
{
	uint8_t base;    /* base index of port 0 clk source */
	uint8_t offset;  /* per port offset for the clk source */
} clkSrcProperties_t;

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint8_t configPort(uint8_t port, uint8_t switchId, SJA1105P_speed_t speed, SJA1105P_xmiiMode_t xmiiMode, uint8_t phyMode);
static uint8_t makeTxPinsHighSpeed(uint8_t port, uint8_t switchId);
static uint8_t enablePLL1(uint8_t switchId);
static SJA1105P_clksrc_t getClkSrcAtPort(clkSrcCategory_t clkSrcCategory, uint8_t port);

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
extern uint8_t SJA1105P_autoConfigPorts(void)
{
	uint8_t ret = 0;
	uint8_t switchId;
	uint8_t port;
	SJA1105P_portStatusMiixArgument_t portStatus;

	for (switchId = 0; switchId < SJA1105P_N_SWITCHES; switchId++)
	{
		g_pll1Enabled[switchId] = 0;
		for (port = 0; port < SJA1105P_N_PORTS; port++)
		{
			ret += SJA1105P_getPortStatusMiix(&portStatus, port, switchId);
			ret += configPort(port, switchId, portStatus.speed, portStatus.xmiiMode, portStatus.phyMode);
		}
	}

	return ret;
}

extern uint8_t SJA1105P_autoConfigPorts_extended(uint8_t switchId)
{
	uint8_t ret = 0;
	uint8_t port;
	SJA1105P_portStatusMiixArgument_t portStatus;

	g_pll1Enabled[switchId] = 0;
	for (port = 0; port < SJA1105P_N_PORTS; port++)
	{
		ret += SJA1105P_getPortStatusMiix(&portStatus, port, switchId);
		ret += configPort(port, switchId, portStatus.speed, portStatus.xmiiMode, portStatus.phyMode);
	}

	return ret;
}


/**
* \brief Setup a clock delay at a given port. 
*
* \param[in]  delay (0.1 degrees) Delay value in multiple of 0.1 degrees.
* Programmable values are between 73.8 and 101.7 degrees with 0.9 degrees
* step size. Recommended is 81 degrees (delay=810, 1.8 ns for RGMII)
* \param[in]  port Port to be configured
* \param[in]  switchId switch at which the delay should be configured
* \param[in]  direction Selection of delay in Tx or Rx direction
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
extern uint8_t SJA1105P_setupClockDelay(uint16_t delay, uint8_t port, uint8_t switchId, SJA1105P_direction_t direction)
{
	uint8_t ret;
	SJA1105P_cfgPadMiixIdArgument_t cfgPadMiixId;
	uint8_t cDelay;  /* clock delay in the representation used by the hardware */

	/* translate delay to hardware representation */
	delay  = (uint16_t) (delay - MINIMUM_CLK_DELAY);    /* remove offset */
	cDelay = (uint8_t)  (delay / (STEPSIZE_CLK_DELAY)); /* normalize */
	ret = SJA1105P_getCfgPadMiixId(&cfgPadMiixId, port, switchId);
	if (direction == SJA1105P_e_direction_TX)
	{
		cfgPadMiixId.txcPd = 0;         /* Power Up */
		cfgPadMiixId.txcBypass = 0;     /* No Bypassing */
		cfgPadMiixId.txcDelay = cDelay; /* Delay */
	}
	else
	{
		cfgPadMiixId.rxcPd = 0;         /* Power Up */
		cfgPadMiixId.rxcBypass = 0;     /* No Bypassing */
		cfgPadMiixId.rxcDelay = cDelay; /* Delay */
	}
	if (ret == 0U)
	{  /* Only set delay if register read was successful */
		ret = SJA1105P_setCfgPadMiixId(&cfgPadMiixId, port, switchId);
	}
	return ret;
}

/**
* \brief Initialized the SGMII core
*
* \param[in]  switchId SwitchId of the switch instance to be configured
* \param[in]  autoNegotiation Selects whether auto-negotiation will be used
* \param[in]  speed If no auto-negotiation is selected, defines the speed
* \param[in]  speed If auto-negotiation is selected, defines MAC mode/PHY mode
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
extern uint8_t initSgmii(uint8_t switchId, uint8_t autoNegotiation, SJA1105P_speed_t speed, uint8_t phyMode)
{
	uint8_t ret;

	SJA1105P_basicControlArgument_t    basicControl;
	SJA1105P_digitalControl2Argument_t digitalControl2;
	SJA1105P_autonegControlArgument_t  autonegControl;
	SJA1105P_digitalControl1Argument_t digitalControl1;

	ret  = SJA1105P_getBasicControl(&basicControl, switchId);
	if (autoNegotiation == 0U)
	{
		basicControl.speedSelect = speed;
	}
	basicControl.autonegEnable = 0;  /* Disable auto-negotiation */
	ret += SJA1105P_setBasicControl(&basicControl, switchId);

	/* Invert Tx polarity */
	digitalControl2.txPolInv = 1;
	digitalControl2.rxPolInv = 0;
	ret += SJA1105P_setDigitalControl2(&digitalControl2, switchId);

	if (autoNegotiation == 1U)
	{
		ret += SJA1105P_getAutonegControl(&autonegControl, switchId);
		autonegControl.phyMode = phyMode;
		autonegControl.autonegMode = SJA1105P_e_autonegMode_CL37_SGMII;
		ret += SJA1105P_setAutonegControl(&autonegControl, switchId);
		if (phyMode == 0U)
		{  /* MAC mode */
			ret += SJA1105P_getDigitalControl1(&digitalControl1, switchId);
			digitalControl1.macAutoSw = 1;  /* Automatically switch speed after auto-negotiation is complete */
			ret += SJA1105P_setDigitalControl1(&digitalControl1, switchId);
		}

		basicControl.autonegEnable = 1;  /* Enable auto-negotiation */
		ret += SJA1105P_setBasicControl(&basicControl, switchId);
	}

	return ret;
}

/**
* \brief Configure speed and xMII mode of a given port.
*
* For MII and RMII, PHY and MAC mode can be selected.
*
* \param[in]  port Port to be configured
* \param[in]  switchId SwitchId of the port to be configured
* \param[in]  speed Port speed
* \param[in]  xmiiMode Port xMII mode
* \param[in]  phyMode If set, Port is configured in PHY mode. Else, MAC mode is used
*
* \return uint8_t Returns 0 upon successful configuration, else failed.
*/
static uint8_t configPort(uint8_t port, uint8_t switchId,  SJA1105P_speed_t speed, SJA1105P_xmiiMode_t xmiiMode, uint8_t phyMode)
{
	uint8_t ret;

	SJA1105P_miixClockControlRegisterArgument_t miixClockControlRegister;
	SJA1105P_idivCControlRegisterArgument_t idivCControlRegister;
	SJA1105P_prodIdArgument_t prodId;

	miixClockControlRegister.autoblock = 1;
	miixClockControlRegister.pd        = 0;

	idivCControlRegister.autoblock     = 1;
	idivCControlRegister.pd            = 0;
	idivCControlRegister.idiv          = SJA1105P_e_idiv_BY_10;

	//printk("[configPort]: port %d switch id %d speed %d xmii_mode %d phy_mode %d\n", port,switchId,speed,xmiiMode,phyMode);
	switch (xmiiMode)
	{
		case SJA1105P_e_xmiiMode_MII:
		{
			miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_RX, port);
			ret = SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_MII_RX_CLK, switchId);
			if (phyMode == 0U)
			{  /* MII MAC Mode */
				idivCControlRegister.pd = 1;  /* deactivate IDIV to save power */
				ret += SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
				miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_TX, port);
				ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_MII_TX_CLK, switchId);
			}
			else
			{  /* MII PHY Mode */
				if (speed == SJA1105P_e_speed_10_MBPS)
				{
					ret = SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
				}
				if (ret == 0U)
				{  /* everything went fine so far, go on */
					miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_IDIV, port);
					ret  = SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_MII_TX_CLK, switchId);
					ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_EXT_TX_CLK, switchId);
					ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_EXT_RX_CLK, switchId);
				}
			}
			break;
		}
		case SJA1105P_e_xmiiMode_RMII:
		{
			idivCControlRegister.pd = 1;  /* deactivate IDIV to save power */
			ret = SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_TX, port);
			ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_RMII_REF_CLK, switchId);
			if (phyMode == 0U)
			{  /* RMII MAC Mode */
				ret += enablePLL1(switchId);
				miixClockControlRegister.clksrc = SJA1105P_e_clksrc_PLL1;
				ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_EXT_TX_CLK, switchId);
			}
			break;
		}
		case SJA1105P_e_xmiiMode_RGMII:
		{
			ret  = makeTxPinsHighSpeed(port, switchId);
			if (port != SJA1105P_g_generalParameters.cascPort[switchId])
			{
			    unsigned int okay = 1;
			    if ((switchId == 0) && (port ==  1 || port == 3))
				okay=0;
			    if( okay )
			    {

			    /* only add the delay on the host port side, not on the opposite (cascaded port) to avoid having the delay twice */
				ret += SJA1105P_setupClockDelay(CLOCK_DELAY_92_7_DEG, port, switchId, SJA1105P_e_direction_TX);
				/* 855 1 ns in wrong direction; 765
				 * CLOCK_DELAY_92_7_DEG */
				ret +=
				SJA1105P_setupClockDelay(CLOCK_DELAY_92_7_DEG, port, switchId, SJA1105P_e_direction_RX);

			    }
			}
			miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_IDIV, port);
			if (speed != SJA1105P_e_speed_100_MBPS)
			{
				if (speed == SJA1105P_e_speed_1_GBPS)
				{
					idivCControlRegister.pd = 1;  /* switch off to save power */
					miixClockControlRegister.clksrc = SJA1105P_e_clksrc_PLL0;
				}
				ret += SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			}
			ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_RGMII_TX_CLK, switchId);
			break;
		}
		default:  /* SGMII or unused */
		{
			/* check for SGMII capability */
			ret = SJA1105P_getProdId(&prodId, switchId);
			if ((prodId.partNr == SJA1105P_e_partNr_SJA1105P) || (prodId.partNr == SJA1105P_e_partNr_SJA1105Q) || (port != SGMII_PORT))
			{  /* Not SGMII capable - this is an unused port*/
				idivCControlRegister.pd = 1;  /* deactivate IDIV */
				ret += SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
			}

			if (ret == 0U)
			{  /* port is SGMII capable and everything went fine so far */
				ret = makeTxPinsHighSpeed(port, switchId);
				miixClockControlRegister.clksrc = getClkSrcAtPort(SJA1105P_e_clkSrcCategory_IDIV, port);
				if (speed != SJA1105P_e_speed_100_MBPS)
				{
					if (speed == SJA1105P_e_speed_1_GBPS)
					{
						idivCControlRegister.pd = 1;  /* switch off to save power */
						miixClockControlRegister.clksrc = SJA1105P_e_clksrc_PLL0;
					}
					ret += SJA1105P_setIdivCControlRegister(&idivCControlRegister, port, switchId);
				}
				ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_MII_TX_CLK, switchId);	
				ret += SJA1105P_setMiixClockControlRegister(&miixClockControlRegister, port, SJA1105P_e_miixInternalClk_MII_RX_CLK, switchId);	
				ret += initSgmii(switchId, 0, speed, phyMode);  /* Init SGMII without auto-negotiation */
			}
			break;
		}
	}
	return ret;
}

/**
* \brief Enable PLL1
* PLL1 is needed for RMII PHY mode operation
*
* \return uint8_t Returns 0 upon success, else failed.
*/
static uint8_t enablePLL1(uint8_t switchId)
{
	uint8_t ret = 0;

	SJA1105P_pllControlArgument_t pll1Control;
	if (g_pll1Enabled[switchId] == 0U)
	{  /* PLL1 has to be enabled first */
		pll1Control.pd         = 1;  /* disabled */
		pll1Control.bypass     = 0;  /* Not bypassed */
		pll1Control.fbsel      = 1;  /* Feedback selected */
		pll1Control.direct     = 0;  /* Direct enabled */
		pll1Control.psel       = 1;  /* P divider for 50 MHz generation */
		pll1Control.msel       = 1;  /* M divider for 50 MHz generation */
		pll1Control.phaseoutEn = 0;  /* Phase output disabled */
		pll1Control.autoblock  = 1;  /* Autoblock disabled */
		ret = SJA1105P_setPllControl(&pll1Control, 1, switchId);  /* Setup PLL1 */
		pll1Control.pd         = 0;  /* enabled */
		ret += SJA1105P_setPllControl(&pll1Control, 1, switchId);  /* Enable PLL1 */
		g_pll1Enabled[switchId] = 1;
	}
	return ret;
}

static SJA1105P_clksrc_t getClkSrcAtPort(clkSrcCategory_t clkSrcCategory, uint8_t port)
{
	SJA1105P_clksrc_t clkSrc;
	static clkSrcProperties_t clkSrcProperties[] = {
		{
			/*.base =*/   (uint8_t) SJA1105P_e_clksrc_MII0_TX_CLK,
			/*.offset =*/ ((uint8_t) SJA1105P_e_clksrc_MII1_TX_CLK) - ((uint8_t) SJA1105P_e_clksrc_MII0_TX_CLK)
		},
		{
			/*.base   =*/ (uint8_t) (uint8_t) SJA1105P_e_clksrc_MII0_RX_CLK,
			/*.offset =*/ ((uint8_t) SJA1105P_e_clksrc_MII1_RX_CLK) - ((uint8_t) SJA1105P_e_clksrc_MII0_RX_CLK)
		},
		{
			/*.base   =*/ (uint8_t) SJA1105P_e_clksrc_IDIV0,
			/*.offset =*/ ((uint8_t) SJA1105P_e_clksrc_IDIV1) - ((uint8_t) SJA1105P_e_clksrc_IDIV0)
		}
	};
	clkSrc = SJA1105P_convertToClksrc((uint32_t) (clkSrcProperties[clkSrcCategory].base + ((uint32_t) port * clkSrcProperties[clkSrcCategory].offset)));
	return clkSrc;
}

/**
* \brief Set slew rate of Tx pins to high speed
*/
static uint8_t makeTxPinsHighSpeed(uint8_t port, uint8_t switchId)
{
	SJA1105P_cfgPadMiixArgument_t cfgPadMiix;
	uint8_t ret;

	ret = SJA1105P_getCfgPadMiix(&cfgPadMiix, port, SJA1105P_e_direction_TX, switchId);   /* Read   cfgPadMiix */
	/* Modify cfgPadMiix */
	cfgPadMiix.clkOs  = SJA1105P_e_padOutputStageSpeed_HIGH_SPEED;
	cfgPadMiix.ctrlOs = SJA1105P_e_padOutputStageSpeed_HIGH_SPEED;
	cfgPadMiix.d32Os  = SJA1105P_e_padOutputStageSpeed_HIGH_SPEED;
	cfgPadMiix.d10Os  = SJA1105P_e_padOutputStageSpeed_HIGH_SPEED;
	ret += SJA1105P_setCfgPadMiix(&cfgPadMiix, port, SJA1105P_e_direction_TX, switchId);  /* Write  cfgPadMiix */

	return ret;
}
