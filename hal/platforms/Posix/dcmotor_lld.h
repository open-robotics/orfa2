/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

	ORFA2 - Copyright (C) 2012 Vladimir Ermakov.

    This file is part of ORFA2.
	Based on STM32/pwm_lld.h from ChibiOS/RT written by Giovanni Di Sirio.

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
 * @file    STM32/dcmotor_lld.h
 * @brief   DC Motor Driver subsystem low level driver header template.
 *
 * @addtogroup DCMotor
 * @{
 */

#ifndef _DCMOTOR_LLD_H_
#define _DCMOTOR_LLD_H_

#if RHAL_USE_DCMOTOR || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief Number of RC servo channels per driver.
 */
#define DCM_CHANNELS							4

#define DCM_MIN                   -1000
#define DCM_MAX                   +1000

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

typedef uint8_t             dcmchannel_t;
typedef int16_t             dcmwidth_t;

/**
 * @brief   Type of a structure representing an DCMotor driver.
 */
typedef struct DCMotorDriver DCMotorDriver;

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
} DCMotorConfig;

/**
 * @brief   Structure representing an DCMotor driver.
 */
struct DCMotorDriver {
  /**
   * @brief Driver state.
   */
  dcmstate_t                state;
  /**
   * @brief Current configuration data.
   */
  const DCMotorConfig       *config;
  /* End of the mandatory fields.*/
  dcmwidth_t                widths[DCM_CHANNELS];
  };

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern DCMotorDriver DCMD1;

#ifdef __cplusplus
extern "C" {
#endif
  void dcm_lld_init(void);
  void dcm_lld_start(DCMotorDriver *dcmp);
  void dcm_lld_stop(DCMotorDriver *dcmp);
  void dcm_lld_enable_channel(DCMotorDriver *dcmp,
                              dcmchannel_t channel,
                              dcmwidth_t width);
  void dcm_lld_disable_channel(DCMotorDriver *dcmp, dcmchannel_t channel);
#ifdef __cplusplus
}
#endif

#endif /* RHAL_USE_DCMOTOR */

#endif /* _DCMOTOR_LLD_H_ */

/** @} */
