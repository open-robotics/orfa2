/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

	ORFA2 - Copyright (C) 2012 Vladimir Ermakov.

    This file is part of ORFA2.
    Based on board.c for OLIMEX STM32-P103 board written
    by Giovanni Di Sirio.

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
#include "hal.h"
#include "rhal.h"

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config =
{
  {VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
  {VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
  {VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
  {VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH},
  {VAL_GPIOEODR, VAL_GPIOECRL, VAL_GPIOECRH},
#if STM32_HAS_GPIOF
  {0xffffffff, 0x88888888, 0x88888888},
#if STM32_HAS_GPIOG
  {0xffffffff, 0x88888888, 0x88888888},
#endif
#endif
};
#endif

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)
const RCServoConfig rcs_default_config =
{
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
#endif

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {

  stm32_clock_init();
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {
	/* TODO remap */
}
