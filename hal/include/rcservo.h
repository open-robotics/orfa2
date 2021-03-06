/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

	ORFA2 - Copyright (C) 2012 Vladimir Ermkov.

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
 * @file    rcservo.h
 * @brief   RC Servo Driver macros and structures.
 *
 * @addtogroup RCServo
 * @{
 */

#ifndef _RCSERVO_H_
#define _RCSERVO_H_

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  RCS_UNINIT = 0,                   /**< Not initialized.                   */
  RCS_STOP = 1,                     /**< Stopped.                           */
  RCS_READY = 2,                    /**< Ready.                             */
} rcsstate_t;

#include "rcservo_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

#define rcsGetWidth(rcsp, channel) rcs_lld_get_width(rcsp, channel)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void rcsInit(void);
  void rcsObjectInit(RCServoDriver *rcsp);
  void rcsStart(RCServoDriver *rcsp, const RCServoConfig *config);
  void rcsStop(RCServoDriver *rcsp);
  void rcsEnableChannel(RCServoDriver *rcsp,
                        rcschannel_t channel,
                        rcswidth_t width);
  void rcsDisableChannel(RCServoDriver *rcsp, rcschannel_t channel);
  void rcsSync(RCServoDriver *rcsp);
#ifdef __cplusplus
}
#endif

#endif /* RHAL_USE_RCSERVO */

#endif /* _RCSERVO_H_ */

/** @} */
