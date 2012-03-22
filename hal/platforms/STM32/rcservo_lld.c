/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    templates/rcs_lld.c
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

#if STM32_RCS_USE_TIM2
RCServoDriver RCSD2;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if STM32_RCS_USE_TIM2 || defined(__DOXYGEN__)
/**
 * @brief   Common TIM2...TIM5 IRQ handler.
 * @note    It is assumed that the various sources are only activated if the
 *          associated callback pointer is not equal to @p NULL in order to not
 *          perform an extra check in a potentially critical interrupt handler.
 *
 * @param[in] rcsp      pointer to a @p RCServoDriver object
 */
static void serve_interrupt(RCServoDriver *rcsp) {
  uint16_t sr;
  int i;

  sr  = rcsp->tim->SR;
  sr &= rcsp->tim->DIER;
  rcsp->tim->SR = ~(TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF |
                    TIM_SR_CC4IF | TIM_SR_UIF);

  if ((sr & TIM_SR_CC1IF) != 0) {
    for (i = 0; i < RCS_GPIOS; i++)
      rcsp->gpio[i]->BRR = rcsp->scurr->clrmask[i];

    if (rcsp->scurr < rcsp->slast) {
      rcsp->scurr++;
      rcsp->tim->CCR1 = rcsp->scurr->clrcmp;
    }
  }

  if ((sr & TIM_SR_UIF) != 0) {
    for (i = 0; i < RCS_GPIOS; i++)
      rcsp->gpio[i]->BSRR = rcsp->enabled[i];

    if (rcsp->next_sfirst != NULL && rcsp->next_slast != NULL) {
      rcsp->sfirst = rcsp->next_sfirst;
      rcsp->slast = rcsp->next_slast;
      rcsp->next_sfirst = NULL;
      rcsp->next_slast = NULL;
    }
    rcsp->scurr = rcsp->sfirst;
    rcsp->tim->CCR1 = rcsp->scurr->clrcmp;
  }
}
#endif /* STM32_RCS_USE_TIM2 */

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if STM32_RCS_USE_TIM2
/**
 * @brief   TIM2 interrupt handler.
 *
 * @isr
 */
CH_IRQ_HANDLER(TIM2_IRQHandler) {

  CH_IRQ_PROLOGUE();

  serve_interrupt(&RCSD2);

  CH_IRQ_EPILOGUE();
}
#endif /* STM32_RCS_USE_TIM2 */

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level RCServo driver initialization.
 *
 * @notapi
 */
void rcs_lld_init(void) {

#if STM32_RCS_USE_TIM2
  rcsObjectInit(&RCSD2);
  RCSD2.tim = TIM2;
  RCSD2.gpio[0] = GPIOA;
  RCSD2.gpio[1] = GPIOB;
  RCSD2.gpio[2] = GPIOC;
  RCSD2.gpio[3] = GPIOD;
  RCSD2.gpio[4] = GPIOE;
  RCSD2.sfirst = RCSD2.steps[0];
  RCSD2.scurr = RCSD2.sfirst;
  RCSD2.slast = RCSD2.sfirst;
#endif
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

  if (rcsp->state == RCS_STOP) {
    /* Clock activation and timer reset.*/
#if STM32_RCS_USE_TIM2
    if (&RCSD2 == rcsp) {
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      RCC->APB1RSTR = RCC_APB1RSTR_TIM2RST;
      RCC->APB1RSTR = 0;
      nvicEnableVector(TIM2_IRQn,
                       CORTEX_PRIORITY_MASK(STM32_RCS_TIM2_IRQ_PRIORITY));
      clock = STM32_TIMCLK1;
    }
#endif

    rcsp->tim->CCMR1 = 0;
    rcsp->tim->CCMR2 = 0;
  }
  else {
    /* Driver re-configuration scenario, it must be stopped first.*/
    rcsp->tim->CR1  = 0;                    /* Timer disabled.              */
    rcsp->tim->DIER = 0;                    /* All IRQs disabled.           */
    rcsp->tim->SR   = 0;                    /* Clear eventual pending IRQs. */
    rcsp->tim->CCR1 = 0;                    /* Comparator 1 disabled.       */
    rcsp->tim->CCR2 = 0;                    /* Comparator 2 disabled.       */
    rcsp->tim->CCR3 = 0;                    /* Comparator 3 disabled.       */
    rcsp->tim->CCR4 = 0;                    /* Comparator 4 disabled.       */
    rcsp->tim->CNT  = 0;                    /* Counter reset to zero.       */
  }

  /* Timer configuration.*/
  psc = (clock / RCS_FREQ) - 1;
  chDbgAssert((psc <= 0xFFFF) &&
              ((psc + 1) * RCS_FREQ) == clock,
              "rcs_lld_start(), #1", "invalid frequency");
  rcsp->tim->PSC  = (uint16_t)psc;
  rcsp->tim->ARR  = (uint16_t)(RCS_PERIOD - 1);

  rcsp->tim->CCER = 0;
  rcsp->tim->EGR  = TIM_EGR_UG;             /* Update event.                */
  rcsp->tim->DIER = TIM_DIER_UIE | TIM_DIER_CC1IE;
  rcsp->tim->SR   = 0;                      /* Clear pending IRQs.          */
  /* Timer configured and started.*/
  rcsp->tim->CR1  = TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_CEN;
}

/**
 * @brief   Deactivates the RCServo peripheral.
 *
 * @param[in] rcsp      pointer to the @p RCServoDriver object
 *
 * @notapi
 */
void rcs_lld_stop(RCServoDriver *rcsp) {

  if (rcsp->state == RCS_READY) {
    /* Clock deactivation.*/

  }
}

void rcs_lld_enable_channel(RCServoDriver *rcsp,
                            rcschannel_t channel,
                            rcswidth_t width) {

  rcsp->widths[channel] = width;
}

void rcs_lld_disable_channel(RCServoDriver *rcsp, rcschannel_t channel) {

  rcsp->widths[channel] = 0;
}

void rcs_lld_sync(RCServoDriver *rcsp) {
  RCServoStep *steps;
  uint16_t i, len;

  if (rcsp->next_sfirst != NULL)
    return; // TODO error

  steps = (rcsp->sfirst == rcsp->steps[0]) ? rcsp->steps[1] : rcsp->steps[0];

  for (i = 0, len = 0; i < RCS_CHANNELS; i++) {
    RCServoStep *p;
    uint16_t l, u, idx;
    uint16_t gpio = rcsp->config->channels[i].gpio;
    uint16_t mask = rcsp->config->channels[i].mask;
    rcscnt_t pw = rcsp->widths[i];

    /* skip if disabled */
    if (pw == 0) {
      rcsp->enabled[gpio] &= ~mask;
      continue;
    }

    /* enable */
    rcsp->enabled[gpio] |= mask;

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
      /* step founded */
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
  rcsp->next_sfirst = steps;
  rcsp->next_slast = steps + len - 1;
}

#endif /* RHAL_USE_RCSERVO */

/** @} */
