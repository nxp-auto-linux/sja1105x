/*
* AVB switch driver module for SJA1105
* Copyright (C) 2016 NXP Semiconductors
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
/**
*
* \file  hal_export.c
*
* \author Laurent Brando
*
* \date 2016-07-04
*
* \brief HAL ethernet interface export
*
*****************************************************************************/

#include <linux/module.h>

#include "NXP_SJA1105P_ethIf.h"
#include "NXP_SJA1105P_mgmtRoutes.h"
#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_ptp.h"
#include "NXP_SJA1105P_sgmii.h"

EXPORT_SYMBOL(SJA1105P_initAutoPortMapping);
EXPORT_SYMBOL(SJA1105P_initManualPortMapping);
EXPORT_SYMBOL(SJA1105P_ethIfTick);
EXPORT_SYMBOL(SJA1105P_forwardRecvFrames);
EXPORT_SYMBOL(SJA1105P_registerFrameSendCB);
EXPORT_SYMBOL(SJA1105P_registerFrameRecvCB);
EXPORT_SYMBOL(SJA1105P_initSwitchEthIf);
EXPORT_SYMBOL(SJA1105P_subscribeEthTypeForSwitchIf);
EXPORT_SYMBOL(SJA1105P_recvSwitchFrame);
EXPORT_SYMBOL(SJA1105P_recvSwitchFrameLoop);
EXPORT_SYMBOL(SJA1105P_sendSwitchFrame);
EXPORT_SYMBOL(SJA1105P_initEndPointEthIf);
EXPORT_SYMBOL(SJA1105P_recvEndPointFrame);
EXPORT_SYMBOL(SJA1105P_recvEndPointFrameLoop);
EXPORT_SYMBOL(SJA1105P_sendEndPointFrame);

EXPORT_SYMBOL(SJA1105P_setupMgmtRoute);
EXPORT_SYMBOL(SJA1105P_pollAndDispatchEgressTimeStampsTick);
EXPORT_SYMBOL(SJA1105P_registerEgressTimeStampHandler);
EXPORT_SYMBOL(SJA1105P_getEgressTimeStamp);

EXPORT_SYMBOL(SJA1105P_initPtp);
EXPORT_SYMBOL(SJA1105P_getPtpClk);
EXPORT_SYMBOL(SJA1105P_setPtpClk);
EXPORT_SYMBOL(SJA1105P_setPtpClkRatio);
EXPORT_SYMBOL(SJA1105P_getPtpClkRatio);
EXPORT_SYMBOL(SJA1105P_addOffsetToPtpClk);

#if 0
EXPORT_SYMBOL(SJA1105P_setDigitalControl2);
EXPORT_SYMBOL(SJA1105P_getAutonegControl);
EXPORT_SYMBOL(SJA1105P_setAutonegControl);
EXPORT_SYMBOL(SJA1105P_setDigitalControl1);
EXPORT_SYMBOL(SJA1105P_getBasicControl);
EXPORT_SYMBOL(SJA1105P_setBasicControl);
EXPORT_SYMBOL(SJA1105P_getDigitalControl1);
#endif

MODULE_LICENSE( "GPL" );
