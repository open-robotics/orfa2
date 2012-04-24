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
#include "rosapp.h"

#include "shell.h"


const ShellCommand shell_cmds[] = {
  { "eterm", appEterm },
  { "ros", appRos },
  {}
};

ShellConfig shell_cfg = {
  .sc_channel = (BaseChannel*)&SD1,
  .sc_commands = shell_cmds
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
  rhalInit();
  chSysInit();

  sdStart(&SD1, NULL);
#if HAL_USE_ADC
  adcStart(&ADCD1, NULL);
#endif
  dcmStart(&DCMD1, NULL);

  servoInit();

  shellInit();
  shellCreate(&shell_cfg, 1024, 7); /* TODO restart if terminated */

  while (!chThdShouldTerminate()) {

    palClearPad(GPIOB, GPIOB_LED);
    chThdSleepMilliseconds(500);

    palSetPad(GPIOB, GPIOB_LED);
    chThdSleepMilliseconds(500);
  }

  return 0;
}
