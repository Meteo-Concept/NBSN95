/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Helper functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/******************************************************************************
 * @file    utilities.h
 * @author  MCD Application Team
 * @version V1.1.4
 * @date    08-January-2018
 * @brief   Header for driver utilities.c module
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "hw_conf.h"

/* BACKUP_PRIMASK MUST be implemented at the begining of the funtion
   that implement a critical section
   PRIMASK is saved on STACK and recovered at the end of the funtion
   That way RESTORE_PRIMASK ensures that no irq would be triggered in case of
   unbalanced enable/disable, reentrant code etc...*/
#define BACKUP_PRIMASK() uint32_t primask_bit = __get_PRIMASK()
#define DISABLE_IRQ() __disable_irq()
#define ENABLE_IRQ() __enable_irq()
#define RESTORE_PRIMASK() __set_PRIMASK(primask_bit)

/* prepocessor directive to align buffer*/
#define ALIGN(n) __attribute__((aligned(n)))

typedef uint32_t TimerTime_t;

/*!
 * \brief Returns the minimum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/*!
 * \brief Returns the maximum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param [IN] n power value
 * \retval result of raising 2 to the power n
 */
#define POW2(n) (1 << n)

/*!
 * \brief  Find First Set
 *         This function identifies the least significant index or position of
 * the bits set to one in the word
 *
 * \param [in]  value  Value to find least significant index
 * \retval bitIndex    Index of least significat bit at one
 */
__STATIC_INLINE uint8_t __ffs(uint32_t value) {
  return (uint32_t)(32 - __CLZ(value & (-value)));
}

/*!
 * \brief Initializes the pseudo random generator initial value
 *
 * \param [IN] seed Pseudo random generator initial value
 */
void srand1(uint32_t seed);

/*!
 * \brief Computes a random number between min and max
 *
 * \param [IN] min range minimum value
 * \param [IN] max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr(int32_t min, int32_t max);

/*!
 * \brief Copies size elements of src array to dst array
 *
 * \remark STM32 Standard memcpy function only works on pointers that are
 * aligned
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Copies size elements of src array to dst array reversing the byte
 * order
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpyr(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Set size elements of dst array with value
 *
 * \remark STM32 Standard memset function only works on pointers that are
 * aligned
 *
 * \param [OUT] dst   Destination array
 * \param [IN]  value Default value
 * \param [IN]  size  Number of bytes to be copied
 */
void memset1(uint8_t *dst, uint8_t value, uint16_t size);

/*!
 * \brief Converts a nibble to an hexadecimal character
 *
 * \param [IN] a   Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar(uint8_t a);

#endif // __UTILITIES_H__
