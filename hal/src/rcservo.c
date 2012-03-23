/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

	ORFA2 - Copyrigth (C) 2012 Vladimir Ermakov.

    This file is part of ORFA2.

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
 * @file    rcservo.c
 * @brief   RC Servo Driver code.
 *
 * @addtogroup RCservo
 * @{
 */

#include "ch.h"
#include "rhal.h"
#include <string.h>

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   RC Servo Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void rcsInit(void) {

  rcs_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p RCServoDriver structure.
 *
 * @param[out] rcsp     pointer to the @p RCServoDriver object
 *
 * @init
 */
void rcsObjectInit(RCServoDriver *rcsp) {

  memset(rcsp, 0, sizeof(*rcsp));
  rcsp->state  = RCS_STOP;
  rcsp->config = NULL;
}

/**
 * @brief   Configures and activates the RC Servo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 * @param[in] config    pointer to the @p RCServoConfig object
 *
 * @api
 */
void rcsStart(RCServoDriver *rcsp, const RCServoConfig *config) {

  chDbgCheck((rcsp != NULL) && (config != NULL), "rcsStart");

  chSysLock();
  chDbgAssert((rcsp->state == RCS_STOP) || (rcsp->state == RCS_READY),
              "rcsStart(), #1", "invalid state");
  rcsp->config = config;
  rcs_lld_start(rcsp);
  rcsp->state = RCS_READY;
  chSysUnlock();
}

/**
 * @brief   Deactivates the RC Servo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @api
 */
void rcsStop(RCServoDriver *rcsp) {

  chDbgCheck(rcsp != NULL, "rcsStop");

  chSysLock();
  chDbgAssert((rcsp->state == RCS_STOP) || (rcsp->state == RCS_READY),
              "rcsStop(), #1", "invalid state");
  rcs_lld_stop(rcsp);
  rcsp->state = RCS_STOP;
  chSysUnlock();
}

/**
 * @brief   Enables a RCServo channel.
 * @pre     The RCServo unit must have been activated using @p rcsStart().
 *
 * @param[in] rcsp      pointer to a @p RCServoDriver object
 * @param[in] channel   servo channel identifier (0..RCS_CHANNELS-1)
 * @param[in] with      servo pulse width in usecs
 *
 * @api
 */
void rcsEnableChannel(RCServoDriver *rcsp,
                      rcschannel_t channel,
                      rcswidth_t with) {

  chDbgCheck((rcsp != NULL) && (channel < RCS_CHANNELS),
             "rcsEnableChannel");

  if (with > RCS_PW_MAX)
    with = RCS_PW_MAX;
  else if (with < RCS_PW_MIN)
    with = RCS_PW_MIN;

  //chSysLock();
  chDbgAssert(rcsp->state == RCS_READY,
              "rcsEnableChannel(), #1", "not ready");
  rcs_lld_enable_channel(rcsp, channel, with);
  //chSysUnlock();
}

/**
 * @brief   Disables a RCServo channel.
 * @pre     The RCServo unit must have been activated using @p rcsStart().
 *
 * @param[in] rcsp      pointer to a @p RCServoDriver object
 * @param[in] channel   servo channel identifier (0..RCS_CHANNELS-1)
 *
 * @api
 */
void rcsDisableChannel(RCServoDriver *rcsp, rcschannel_t channel) {

  chDbgCheck((rcsp != NULL) && (channel < RCS_CHANNELS),
             "rcsDisableChannel");

  chSysLock();
  chDbgAssert(rcsp->state == RCS_READY,
              "rcsDisableChannel(), #1", "not ready");
  rcs_lld_disable_channel(rcsp, channel);
  chSysUnlock();
}

/**
 * @brief   Sync all RCServo channels.
 * @pre     The RCServo unit must have been activated using @p rcsStart().
 *
 * @param[in] rcsp      pointer to a @p RCServoDriver object
 *
 * @api
 */
void rcsSync(RCServoDriver *rcsp) {

  chDbgCheck(rcsp != NULL, "rcsSync");

  //chSysLock();
  chDbgAssert(rcsp->state == RCS_READY,
              "rcsSync(), #1", "not ready");
  rcs_lld_sync(rcsp);
  //chSysUnlock();
}

#endif /* RHAL_USE_RCSERVO */

/** @} */
