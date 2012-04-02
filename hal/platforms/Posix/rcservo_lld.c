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
 * @file    STM32/rcservo_lld.c
 * @brief   RCServo Driver subsystem low level driver source template.
 *
 * @addtogroup RCServo
 * @{
 */

#include "ch.h"
#include "rhal.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

RCServoDriver RCSD1;

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
 * @brief   Low level RCServo driver initialization.
 *
 * @notapi
 */
void rcs_lld_init(void) {

  rcsObjectInit(&RCSD1);
}

/**
 * @brief   Configures and activates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void rcs_lld_start(RCServoDriver *rcsp) {

  (void) rcsp;
  if (RCSD1.state == RCS_STOP) {
    /* Clock activation and timer reset.*/
  }
}

/**
 * @brief   Deactivates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void rcs_lld_stop(RCServoDriver *rcsp) {

  (void) rcsp;
  if (RCSD1.state == RCS_READY) {
    /* Clock deactivation.*/

  }
}

void rcs_lld_enable_channel(RCServoDriver *rcsp,
                            rcschannel_t channel,
                            rcswidth_t width) {

  (void) rcsp;
  RCSD1.widths[channel] = width;
}

void rcs_lld_disable_channel(RCServoDriver *rcsp, rcschannel_t channel) {

  (void) rcsp;
  RCSD1.widths[channel] = 0;
}

void rcs_lld_sync(RCServoDriver *rcsp) {
  int i;
  struct timeval tv;

  (void) rcsp;
  gettimeofday(&tv, NULL);
  printf("%d.%d rcsSync:", tv.tv_sec, tv.tv_usec);
  for (i = 0; i < RCS_CHANNELS; i++)
    if (RCSD1.widths[i] > 0)
      printf(" %d:%d", i, RCSD1.widths[i]);
  printf("\n");
}

#endif /* RHAL_USE_RCSERVO */

/** @} */
