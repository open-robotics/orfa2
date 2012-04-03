/*
    ORFA2 - Copyright (C) 2012 Vladimir Ermakov.

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

#include "ch.h"
#include "rhal.h"
#include "chprintf.h"
#include "servo.h"
#include "eterm.h"

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
  rhalInit();
  chSysInit();

  sdStart(&SD1, NULL);

  servoInit();

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while (!chThdShouldTerminate()) {
	appEterm((struct BaseChannel*)&SD1, 0, NULL);
    chThdSleepMilliseconds(200);
  }

  return 0;
}
