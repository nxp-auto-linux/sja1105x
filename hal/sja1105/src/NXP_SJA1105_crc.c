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
* \file NXP_SJA1105_crc.c
*
* \version 0.2.3
*
* \date 2017-01-24
*
* \brief CRC calculation for the Address Resolution Lookup
*
* Generated with pycrc v0.9, simplified, and modified for MISRA-C compliance
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
*****************************************************************************/

#include "typedefs.h"

#include "NXP_SJA1105_crc.h"
#include "NXP_SJA1105_config.h"

/******************************************************************************
* DEFINES
*****************************************************************************/

#define BIT8 0x80U
#define FULL_NOTATION_MASK 0x1FFU

/******************************************************************************
* INTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

static uint16_t transformKoopmanToFull(void);

/******************************************************************************
* FUNCTIONS
*****************************************************************************/

/**
 * Calculate the CRC to be used as hash index for the ARL table
 *
 * \param data       Pointer to a buffer of \a data_len bytes.
 * \param dataLength Number of bytes in the \a data buffer.
 * \return           The calculated crc value.
 */
extern uint8_t SJA1105_calculateHash(const uint8_t *pk_data, uint8_t dataLength)
{
	uint8_t i;
	uint8_t byte;
	uint8_t bit;
	uint8_t c;
	uint8_t crc = 0;
	
	uint16_t polyFull = transformKoopmanToFull();

	for (byte = 0; byte < dataLength; byte++)
	{
		c = pk_data[byte];
		for (i = BIT8; i > 0U; i >>= 1U)
		{
			bit = (crc & BIT8);
            if (bit > 0U)
            {
                bit = 1;
            }
			if ((c & i) != 0U)
			{
				bit = bit ^ 1U;
			}
			crc <<= 1;
			if (bit != 0U)
			{
				crc = (uint8_t) (crc ^  polyFull);
			}
		}
	}
	return crc;
}

/**
 * Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 */
extern void SJA1105_updateHashPoly(uint16_t new_poly_value)
{
	SJA1105_g_poly = new_poly_value;
}

static uint16_t transformKoopmanToFull(void)
{
	uint16_t fullNotation = SJA1105_g_poly;
	
	fullNotation <<= 1;
	fullNotation |= 1U;
	fullNotation &= FULL_NOTATION_MASK;
	
	return fullNotation;
}
