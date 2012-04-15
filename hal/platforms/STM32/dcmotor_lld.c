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

#if RHAL_USE_DCMOTOR || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* 2 kHz */
#define DCM_FREQ    2000000
#define DCM_PERIOD  1000

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

DCMotorDriver DCMD1;

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

static PWMConfig pwmcfg = {
  DCM_FREQ,
  DCM_PERIOD,
  NULL,
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  /* HW */
  0,
  0
};

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
  palClearPad(GPIOC, GPIOC_M0DIRP);
  palSetPad(GPIOC, GPIOC_M0DIRN);
  palClearPad(GPIOB, GPIOB_M1DIRP);
  palSetPad(GPIOB, GPIOB_M1DIRN);
  palClearPad(GPIOE, GPIOE_MDDIR1);
  palClearPad(GPIOE, GPIOE_MDDIR2);
  pwmStart(&PWMD1, &pwmcfg);
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
  pwmStop(&PWMD1);
}

void dcm_lld_enable_channel(DCMotorDriver *dcmp,
                            dcmchannel_t channel,
                            dcmwidth_t width) {

  (void) dcmp;
  if ((DCMD1.widths[channel] > 0) != (width > 0)) {
    pwmDisableChannelI(&PWMD1, channel);

    if (channel == 0) {
      palClearPad(GPIOC, GPIOC_M0DIRP);
      palClearPad(GPIOC, GPIOC_M0DIRN);
      PWMD1.tim->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
    }
    else if (channel == 1) {
      palClearPad(GPIOB, GPIOB_M1DIRP);
      palClearPad(GPIOB, GPIOB_M1DIRN);
      PWMD1.tim->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);
    }
  }

  if (channel == 0) {
      if (width > 0) {
        palSetPad(GPIOC, GPIOC_M0DIRP);
        PWMD1.tim->CCER |= TIM_CCER_CC1NE;
      }
      else {
        palSetPad(GPIOC, GPIOC_M0DIRN);
        PWMD1.tim->CCER |= TIM_CCER_CC1E;
      }
  }
  else if (channel == 1) {
      if (width > 0) {
        palSetPad(GPIOB, GPIOB_M1DIRP);
        PWMD1.tim->CCER |= TIM_CCER_CC2NE;
      }
      else {
        palSetPad(GPIOB, GPIOB_M1DIRN);
        PWMD1.tim->CCER |= TIM_CCER_CC2E;
      }
  }
  else if (channel == 2) {
    palWritePad(GPIOE, GPIOE_MDDIR1, width < 0);
  }
  else if (channel == 3) {
    palWritePad(GPIOE, GPIOE_MDDIR2, width < 0);
  }

  DCMD1.widths[channel] = width;
  if (width < 0)
    width = -width;

  pwmEnableChannelI(&PWMD1, channel,
      PWM_FRACTION_TO_WIDTH(&PWMD1, 1000, width));
}

void dcm_lld_disable_channel(DCMotorDriver *dcmp, dcmchannel_t channel) {

  (void) dcmp;
  pwmDisableChannelI(&PWMD1, channel);

  if (channel == 0) {
    palClearPad(GPIOC, GPIOC_M0DIRP);
    palClearPad(GPIOC, GPIOC_M0DIRN);
    PWMD1.tim->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE);
  }
  else if (channel == 1) {
    palClearPad(GPIOB, GPIOB_M1DIRP);
    palClearPad(GPIOB, GPIOB_M1DIRN);
    PWMD1.tim->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE);
  }

  DCMD1.widths[channel] = 0;
}

#endif /* RHAL_USE_DCMOTOR */

/** @} */
