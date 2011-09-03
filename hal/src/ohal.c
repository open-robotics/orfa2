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
 * @file    ohal.c
 * @brief   ORFA HAL subsystem code.
 *
 * @addtogroup HAL
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "ohal.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   HAL initialization.
 * @details This function invokes the low level initialization code then
 *          initializes all the drivers enabled in the HAL. Finally the
 *          board-specific initialization is performed by invoking
 *          @p boardInit() (usually defined in @p board.c).
 *
 * @init
 */
void ohalInit(void) {

#if HAL_USE_RCSERVO || defined(__DOXYGEN__)
  rcsInit();
#endif
}

/** @} */
