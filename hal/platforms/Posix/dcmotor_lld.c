/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

	ORFA2 - Copyright (C) 2011,2012 Vladimir Ermakov.

    This file is part of ORFA2.
	Based on STM32/pwm_lld.c from ChibiOS/RT written by Giovanni Di Sirio.

    ORFA2 is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ORFA2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    STM32/dcmotor_lld.c
 * @brief   DC Motor Driver subsystem low level driver source template.
 *
 * @addtogroup DCMotor
 * @{
 */

#include "ch.h"
#include "rhal.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#if RHAL_USE_DCMOTOR || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

DCMotorDriver DCMD1;

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level DCMotor driver initialization.
 *
 * @notapi
 */
void dcm_lld_init(void) {

  dcmObjectInit(&DCMD1);
}

/**
 * @brief   Configures and activates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void dcm_lld_start(DCMotorDriver *dcmp) {

  (void) dcmp;
}

/**
 * @brief   Deactivates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void dcm_lld_stop(DCMotorDriver *dcmp) {

  (void) dcmp;
}

void dcm_lld_enable_channel(DCMotorDriver *dcmp,
                            dcmchannel_t channel,
                            dcmwidth_t width) {
  struct timeval tv;

  (void) dcmp;
  gettimeofday(&tv, NULL);
  printf("%d.%d dcmEnableChannel: ch%d %d -> %d\n", tv.tv_sec, tv.tv_usec,
		  channel, DCMD1.widths[channel], width);
  DCMD1.widths[channel] = width;
}

void dcm_lld_disable_channel(DCMotorDriver *dcmp, dcmchannel_t channel) {

  (void) dcmp;
  DCMD1.widths[channel] = 0;
}

#endif /* RHAL_USE_DCMOTOR */

/** @} */
