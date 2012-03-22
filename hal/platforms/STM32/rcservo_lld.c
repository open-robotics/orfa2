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

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define RCS_FREQ    1000000
#define RCS_PERIOD  20000

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

RCServoDriver RCSD1;

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

static GPIO_TypeDef* const gpios[RCS_GPIOS] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/**
 * @brief   TIM2 interrupt handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(TIM2_IRQHandler) {
  uint16_t sr;
  int i;

  CH_IRQ_PROLOGUE();

  sr  = TIM2->SR;
  sr &= TIM2->DIER;
  TIM2->SR = ~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF |
		  TIM_SR_CC4IF | TIM_SR_UIF);

  if ((sr & TIM_SR_CC1IF) != 0) {
    for (i = 0; i < RCS_GPIOS; i++)
      gpios[i]->BRR = RCSD1.scurr->clrmask[i];

    if (RCSD1.scurr < RCSD1.slast) {
      RCSD1.scurr++;
      TIM2->CCR1 = RCSD1.scurr->clrcmp;
    }
  }

  if ((sr & TIM_SR_UIF) != 0) {
    for (i = 0; i < RCS_GPIOS; i++)
      gpios[i]->BSRR = RCSD1.enabled[i];

    if (RCSD1.next_sfirst != NULL && RCSD1.next_slast != NULL) {
      RCSD1.sfirst = RCSD1.next_sfirst;
      RCSD1.slast = RCSD1.next_slast;
      RCSD1.next_sfirst = NULL;
      RCSD1.next_slast = NULL;
    }
    RCSD1.scurr = RCSD1.sfirst;
    TIM2->CCR1 = RCSD1.scurr->clrcmp;
  }

  CH_IRQ_EPILOGUE();
}

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
  RCSD1.sfirst = RCSD1.steps[0];
  RCSD1.scurr = RCSD1.sfirst;
  RCSD1.slast = RCSD1.sfirst;
}

/**
 * @brief   Configures and activates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void rcs_lld_start(RCServoDriver *rcsp) {
  uint32_t clock, psc;

  (void) rcsp;
  if (RCSD1.state == RCS_STOP) {
    /* Clock activation and timer reset.*/
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1RSTR = RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR = 0;
    nvicEnableVector(TIM2_IRQn,
        CORTEX_PRIORITY_MASK(STM32_RCS_TIM2_IRQ_PRIORITY));
    clock = STM32_TIMCLK1;

    TIM2->CCMR1 = 0;
    TIM2->CCMR2 = 0;
  }
  else {
    /* Driver re-configuration scenario, it must be stopped first.*/
    TIM2->CR1  = 0;                    /* Timer disabled.              */
    TIM2->DIER = 0;                    /* All IRQs disabled.           */
    TIM2->SR   = 0;                    /* Clear eventual pending IRQs. */
    TIM2->CCR1 = 0;                    /* Comparator 1 disabled.       */
    TIM2->CCR2 = 0;                    /* Comparator 2 disabled.       */
    TIM2->CCR3 = 0;                    /* Comparator 3 disabled.       */
    TIM2->CCR4 = 0;                    /* Comparator 4 disabled.       */
    TIM2->CNT  = 0;                    /* Counter reset to zero.       */
  }

  /* Timer configuration.*/
  psc = (clock / RCS_FREQ) - 1;
  chDbgAssert((psc <= 0xFFFF) &&
              ((psc + 1) * RCS_FREQ) == clock,
              "rcs_lld_start(), #1", "invalid frequency");
  TIM2->PSC  = (uint16_t)psc;
  TIM2->ARR  = (uint16_t)(RCS_PERIOD - 1);

  TIM2->CCER = 0;
  TIM2->EGR  = TIM_EGR_UG;             /* Update event.                */
  TIM2->DIER = TIM_DIER_UIE | TIM_DIER_CC1IE;
  TIM2->SR   = 0;                      /* Clear pending IRQs.          */
  /* Timer configured and started.*/
  TIM2->CR1  = TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_CEN;
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
  RCServoStep *steps;
  uint16_t i, len;

  (void) rcsp;
  if (RCSD1.next_sfirst != NULL)
    return; // TODO error

  steps = (RCSD1.sfirst == RCSD1.steps[0]) ? RCSD1.steps[1] : RCSD1.steps[0];

  for (i = 0, len = 0; i < RCS_CHANNELS; i++) {
    RCServoStep *p;
    uint16_t l, u, idx;
    uint16_t gpio = RCSD1.config->channels[i].gpio;
    uint16_t mask = RCSD1.config->channels[i].mask;
    rcscnt_t pw = RCSD1.widths[i];

    /* skip if disabled */
    if (pw == 0) {
      RCSD1.enabled[gpio] &= ~mask;
      continue;
    }

    /* enable */
    RCSD1.enabled[gpio] |= mask;

    /* bsearch width blackjack and hookers */
    l = 0;
    u = len;
    while (l < u) {
      idx = (l + u) / 2;
      p = steps + idx;
      if (pw < p->clrcmp)
        u = idx;
      else if (pw > p->clrcmp)
        l = idx + 1;
      else
        break; /* match */
    }

    if (l < u)
      /* step exists */
      p->clrmask[gpio] |= mask;
    else {
      /* insert new step */
      p = steps + l;
      memmove(p + 1, p, sizeof(*p) * (len - l));
      memset(p->clrmask, 0, sizeof(p->clrmask));
      p->clrcmp = pw;
      p->clrmask[gpio] = mask;
      len++;
    }
  }

  /* set next buffer for interrupt */
  RCSD1.next_sfirst = steps;
  RCSD1.next_slast = steps + len - 1;
}

#endif /* RHAL_USE_RCSERVO */

/** @} */
