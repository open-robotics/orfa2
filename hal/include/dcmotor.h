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
 * @file    dcmotor.h
 * @brief   DC Motor Driver macros and structures.
 *
 * @addtogroup DCMotor
 * @{
 */

#ifndef _DCMOTOR_H_
#define _DCMOTOR_H_

#if RHAL_USE_DCMOTOR || defined(__DOXYGEN__)

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
  DCM_UNINIT = 0,                   /**< Not initialized.                   */
  DCM_STOP = 1,                     /**< Stopped.                           */
  DCM_READY = 2,                    /**< Ready.                             */
} dcmstate_t;

#include "dcmotor_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void dcmInit(void);
  void dcmObjectInit(DCMotorDriver *dcmp);
  void dcmStart(DCMotorDriver *dcmp, const DCMotorConfig *config);
  void dcmStop(DCMotorDriver *dcmp);
  void dcmEnableChannel(DCMotorDriver *dcmp,
                        dcmchannel_t channel,
                        dcmwidth_t width);
  void dcmDisableChannel(DCMotorDriver *dcmp, dcmchannel_t channel);
#ifdef __cplusplus
}
#endif

#endif /* RHAL_USE_DCMOTOR */

#endif /* _DCMOTOR_H_ */

/** @} */
