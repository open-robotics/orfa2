/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

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
#include "rhal.h"
#include "palmap.h"

/**
 * @brief PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config = {
 {0, 0, 0},
 {0, 0, 0}
};
#endif

#if RHAL_USE_RCSERVO || defined(__DOXYGEN__)
const RCServoConfig rcs_default_config;
#endif

const struct palmap_pad_config palmap_pads[] =
{
	/* PORT A */
	PM_PAD(IOPORT1, 0),
	PM_PAD(IOPORT1, 1),
	PM_PAD(IOPORT1, 2),
	PM_PAD(IOPORT1, 3),
	PM_PAD(IOPORT1, 4),
	PM_PAD(IOPORT1, 5),
	PM_PAD(IOPORT1, 6),
	PM_PAD(IOPORT1, 7),
	/* PORT D */
	PM_PAD(IOPORT2, 0),
	PM_PAD(IOPORT2, 1),
	PM_PAD(IOPORT2, 2),
	PM_PAD(IOPORT2, 3),
	PM_PAD(IOPORT2, 4),
	PM_PAD(IOPORT2, 5),
	PM_PAD(IOPORT2, 6),
	PM_PAD(IOPORT2, 7)
};

const struct palmap_port_config palmap_ports[] =
{
		{ 'A', 0 },
		{ 'D', 8 }
};

/*
 * Board-specific initialization code.
 */
void boardInit(void) {
}
