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
 * @file    dcmotor.c
 * @brief   DC Motor Driver code.
 *
 * @addtogroup DCMotor
 * @{
 */

#include "ch.h"
#include "rhal.h"

#if RHAL_USE_DCMOTOR || defined(__DOXYGEN__)

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
 * @brief   DC Motor Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void dcmInit(void) {

  dcm_lld_init();
}

/**
 * @brief   Initializes the standard part of a @p DCMotorDriver structure.
 *
 * @param[out] dcmp     pointer to the @p DCMotorDriver object
 *
 * @init
 */
void dcmObjectInit(DCMotorDriver *dcmp) {

  dcmp->state  = DCM_STOP;
  dcmp->config = NULL;
}

/**
 * @brief   Configures and activates the DC Motor peripheral.
 *
 * @param[in] dcmp      pointer to the @p DCMotorDriver object
 * @param[in] config    pointer to the @p DCMotorConfig object
 *
 * @api
 */
void dcmStart(DCMotorDriver *dcmp, const DCMotorConfig *config) {

  chDbgCheck((dcmp != NULL) && (config != NULL), "dcmStart");

  chSysLock();
  chDbgAssert((dcmp->state == DCM_STOP) || (dcmp->state == DCM_READY),
              "dcmStart(), #1", "invalid state");
  dcmp->config = config;
  dcm_lld_start(dcmp);
  dcmp->state = DCM_READY;
  chSysUnlock();
}

/**
 * @brief   Deactivates the DC Motor peripheral.
 *
 * @param[in] dcmp      pointer to the @p DCMotorDriver object
 *
 * @api
 */
void dcmStop(DCMotorDriver *dcmp) {

  chDbgCheck(dcmp != NULL, "rcsStop");

  chSysLock();
  chDbgAssert((dcmp->state == DCM_STOP) || (dcmp->state == DCM_READY),
              "dcmStop(), #1", "invalid state");
  dcm_lld_stop(dcmp);
  dcmp->state = DCM_STOP;
  chSysUnlock();
}

/**
 * @brief   Enables a DC Motor channel.
 * @pre     The DCMotor unit must have been activated using @p dcmStart().
 *
 * @param[in] dcmp      pointer to a @p DCMotorDriver object
 * @param[in] channel   channel identifier (0..DCM_CHANNELS-1)
 * @param[in] with      pulse width [-1000 .. +1000]
 *
 * @api
 */
void dcmEnableChannel(DCMotorDriver *dcmp,
                      dcmchannel_t channel,
                      dcmwidth_t with) {

  chDbgCheck((dcmp != NULL) && (channel < DCM_CHANNELS),
             "dcmEnableChannel");

  if (with > DCM_MAX)
    with = DCM_MAX;
  else if (with < DCM_MIN)
    with = DCM_MIN;

  //chSysLock();
  chDbgAssert(dcmp->state == DCM_READY,
              "dcmEnableChannel(), #1", "not ready");
  dcm_lld_enable_channel(dcmp, channel, with);
  //chSysUnlock();
}

/**
 * @brief   Disables a DC Motor channel.
 * @pre     The DCMotor unit must have been activated using @p dcmStart().
 *
 * @param[in] dcmp      pointer to a @p DCMotorDriver object
 * @param[in] channel   servo channel identifier (0..DCM_CHANNELS-1)
 *
 * @api
 */
void dcmDisableChannel(DCMotorDriver *dcmp, dcmchannel_t channel) {

  chDbgCheck((dcmp != NULL) && (channel < DCM_CHANNELS),
             "dcmDisableChannel");

  chSysLock();
  chDbgAssert(dcmp->state == DCM_READY,
              "dcmDisableChannel(), #1", "not ready");
  dcm_lld_disable_channel(dcmp, channel);
  chSysUnlock();
}

#endif /* RHAL_USE_DCMOTOR */

/** @} */
