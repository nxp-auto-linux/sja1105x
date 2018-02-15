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
* \file NXP_SJA1105P_portConfig.h
*
* \author NXP Semiconductors
*
* \date 2017-07-04
*
* \brief Configure speed and mode of a port
*
*****************************************************************************/

#ifndef NXP_SJA1105P_PORT_CONFIG_H
#define NXP_SJA1105P_PORT_CONFIG_H

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105P_auxiliaryConfigurationUnit.h"
#include "NXP_SJA1105P_sgmii.h"

/******************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************/

extern uint8_t SJA1105P_autoConfigPorts(void);
extern uint8_t SJA1105P_setupClockDelay(uint16_t delay, uint8_t port, uint8_t switchId, SJA1105P_direction_t direction);
extern uint8_t SJA1105P_resetClockDelay(uint8_t port, uint8_t switchId, SJA1105P_direction_t direction);
extern uint8_t initSgmii(uint8_t switchId, uint8_t autoNegotiation, SJA1105P_speed_t speed, uint8_t phyMode);


#endif /* NXP_SJA1105P_PORT_CONFIG_H */
