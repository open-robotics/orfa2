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
 * @file    STM32/rcservo_lld.h
 * @brief   RCServo Driver subsystem low level driver header template.
 *
 * @addtogroup RCServo
 * @{
 */

#ifndef _RCSERVO_LLD_H_
#define _RCSERVO_LLD_H_

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief Number of RC servo channels per driver.
 */
#define RCS_CHANNELS							20

/**
 * @brief Number of GPIO ports used.
 */
#define RCS_GPIOS                 5

#define RCS_GPIOA                 0
#define RCS_GPIOB                 1
#define RCS_GPIOC                 2
#define RCS_GPIOD                 3
#define RCS_GPIOE                 4

#define RCS_PW_MIN                500
#define RCS_PW_MAX                2500

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   RCSD2 interrupt priority level setting.
 */
#if !defined(STM32_RCS_TIM2_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define STM32_RCS_TIM2_IRQ_PRIORITY         7
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !STM32_HAS_TIM2
#error "TIM2 not present in the selected device"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

typedef uint8_t             rcschannel_t;
typedef uint16_t            rcscnt_t;
typedef uint16_t            rcswidth_t;

/**
 * @brief   RC Servo channel configuration structure.
 */
typedef struct {
  uint16_t              gpio;
  uint16_t              mask;
} RCServoChannelConfig;

/**
 * @brief   RC Servo one step
 */
typedef struct {
  uint16_t                  clrmask[RCS_GPIOS];
  rcscnt_t                  clrcmp;
} RCServoStep;

/**
 * @brief   Type of a structure representing an RCServo driver.
 */
typedef struct RCServoDriver RCServoDriver;

/**
 * @brief   Driver configuration structure.
 * @note    It could be empty on some architectures.
 */
typedef struct {
  RCServoChannelConfig      channels[RCS_CHANNELS];
} RCServoConfig;

/**
 * @brief   Structure representing an RCServo driver.
 */
struct RCServoDriver {
  /**
   * @brief Driver state.
   */
  rcsstate_t                state;
  /**
   * @brief Current configuration data.
   */
  const RCServoConfig       *config;
  /* End of the mandatory fields.*/
  ioportmask_t              enabled[RCS_GPIOS];
  rcswidth_t                widths[RCS_CHANNELS];

  RCServoStep               steps[2][RCS_CHANNELS];
  RCServoStep               *sfirst, *scurr, *slast;
  RCServoStep               *next_sfirst, *next_slast;
  };

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

#define RCS_CHANNEL(gpio, pin) { RCS_##gpio, 1<<pin }

#define rcs_lld_get_width(rcsp, channel) \
	((rcsp)->widths[(channel)])

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern RCServoDriver RCSD1;
extern const RCServoConfig rcs_default_config;

#ifdef __cplusplus
extern "C" {
#endif
  void rcs_lld_init(void);
  void rcs_lld_start(RCServoDriver *rcsp);
  void rcs_lld_stop(RCServoDriver *rcsp);
  void rcs_lld_enable_channel(RCServoDriver *rcsp,
                              rcschannel_t channel,
                              rcswidth_t width);
  void rcs_lld_disable_channel(RCServoDriver *rcsp, rcschannel_t channel);
  void rcs_lld_sync(RCServoDriver *pcsp);
#ifdef __cplusplus
}
#endif

#endif /* RHAL_USE_RCSERVO */

#endif /* _RCSERVO_LLD_H_ */

/** @} */
