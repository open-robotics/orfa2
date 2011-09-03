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

#include "ch.h"
#include "hal.h"
#include "ohal.h"
#include "chprintf.h"


/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIOB, GPIOB_LED);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOB, GPIOB_LED);
    chThdSleepMilliseconds(500);
  }

  return 0;
}

static RCServoConfig rcscfg = {
	{
	RCS_CHANNEL(GPIOE, GPIOE_S00),
	RCS_CHANNEL(GPIOE, GPIOE_S01),
	RCS_CHANNEL(GPIOE, GPIOE_S02),
	RCS_CHANNEL(GPIOE, GPIOE_S03),
	RCS_CHANNEL(GPIOE, GPIOE_S04),
	RCS_CHANNEL(GPIOE, GPIOE_S05),
	RCS_CHANNEL(GPIOE, GPIOE_S06),
	RCS_CHANNEL(GPIOD, GPIOD_S07),
	RCS_CHANNEL(GPIOD, GPIOD_S08),
	RCS_CHANNEL(GPIOD, GPIOD_S09),
	RCS_CHANNEL(GPIOD, GPIOD_S10),
	RCS_CHANNEL(GPIOD, GPIOD_S11),
	RCS_CHANNEL(GPIOC, GPIOC_S12),
	RCS_CHANNEL(GPIOC, GPIOC_S13),
	RCS_CHANNEL(GPIOC, GPIOC_S14),
	RCS_CHANNEL(GPIOC, GPIOC_S15),
	RCS_CHANNEL(GPIOA, GPIOA_S16),
	RCS_CHANNEL(GPIOC, GPIOC_S17),
	RCS_CHANNEL(GPIOC, GPIOC_S18),
	RCS_CHANNEL(GPIOC, GPIOC_S19)
	}
};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  ohalInit();
  chSysInit();

  sdStart(&SD1, NULL);

  rcsStart(&RCSD2, &rcscfg);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
	//servo_init();
	chprintf((struct BaseChannel*)&SD1, "i am a live!\r\n");
    chThdSleepMilliseconds(500);
  }

  return 0;
}
