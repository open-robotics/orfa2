/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

    ORFA2 - Copyright (C) 2012 Vladimir Ermakov.

    This file is part of ORFA2.
	Based on board.h for OLIMEX STM32-P103 board written
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

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the Open Robotics OR-STM32F1-DS board.
 */

/*
 * Board identifier.
 */
#define BOARD_OR_STM32F1_DS
#define BOARD_OR_STM32F1_DS_REV1
#define BOARD_NAME              "Open Robotics OR-STM32F1-DS"

/*
 * Board frequencies.
 */
//#define STM32_LSECLK            32768 /* No rtc */
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F10X_HD

/*
 * IO pins assignments.
 *
 * NOTE: sch -> code: AGn -> An, Gn -> Dn
 */
#define GPIOA_A3        0
#define GPIOA_A2        1
#define GPIOA_A1        2
#define GPIOA_A0        3
#define GPIOA_DAC1      4
#define GPIOA_DAC2      5
#define GPIOA_E0F0      6
#define GPIOA_E0F1      7
#define GPIOA_S16       8
#define GPIOA_RBTX      9
#define GPIOA_RBRX      10
#define GPIOA_USBDM     11
#define GPIOA_USBDP     12
/* GPIOA 13, 14 - SWD */
#define GPIOA_RBGP0     15

#define GPIOB_M1DIRP    0
#define GPIOB_M1DIRN    1
#define GPIOB_LED       2   /* BOOT1 */
#define GPIOB_RBSCK     3
#define GPIOB_RBMISO    4
#define GPIOB_RBMOSI    5
#define GPIOB_RBSCL     6
#define GPIOB_RBSDA     7
#define GPIOB_CAN_RX    8
#define GPIOB_CAN_TX    9
#define GPIOB_I2C_SCL   10
#define GPIOB_I2C_SDA   11
#define GPIOB_D0        12
#define GPIOB_D1        13
#define GPIOB_D2        14
#define GPIOB_D3        15

#define GPIOC_A7        0
#define GPIOC_A6        1
#define GPIOC_A5        2
#define GPIOC_A4        3
#define GPIOC_M0DIRP    4
#define GPIOC_M0DIRN    5
#define GPIOC_S12       6
#define GPIOC_S13       7
#define GPIOC_S14       8
#define GPIOC_S15       9
#define GPIOC_S17       10
#define GPIOC_S18       11
#define GPIOC_S19       12
/* GPIOC 13...15 - NC */

#define GPIOD_RBGP1     0
#define GPIOD_RBGP2     1
#define GPIOD_RBGP0     2   /* leave floating, line controled by PA15 */
/* GPIOD 3, 4 - NC */
#define GPIOD_S09       5
#define GPIOD_S08       6
#define GPIOD_S07       7
#define GPIOD_D4        8
#define GPIOD_D5        9
#define GPIOD_D6        10
#define GPIOD_D7        11
#define GPIOD_E1F0      12
#define GPIOD_E1F1      13
#define GPIOD_S10       14
#define GPIOD_S11       15

#define GPIOE_S06       0
#define GPIOE_S05       1
#define GPIOE_S04       2
#define GPIOE_S03       3
#define GPIOE_S02       4
#define GPIOE_S01       5
#define GPIOE_S00       6
#define GPIOE_MDDIR1    7
#define GPIOE_M0PWMN    8
#define GPIOE_M0PWMP    9
#define GPIOE_M1PWMN    10
#define GPIOE_M1PWMP    11
#define GPIOE_MDDIR2    12
#define GPIOE_MDPWM1    13
#define GPIOE_MDPWM2    14
/* GPIOE 15 - NC */

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA8  - OD        (S16)
 * PA9  - Alt PP    (USART1 TX)
 * PA10 - In        (USART1 RX)
 * PA11 - In        (USB D-)
 * PA12 - In        (USB D+)
 */
#define VAL_GPIOACRL            0x88888888      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x888444B7      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB0  - PP        (M1DIRP)
 * PB1  - PP        (M1DIRN)
 * PB2  - PP        (LED)
 * PB3  - Alt PP    (RB SPI3 SCK)
 * PB4  - In        (RB SPI3 MISO)
 * PB5  - Alt PP    (RB SPI3 MOSI)
 * PB6  - Alt OD    (RB I2C1 SCL)
 * PB7  - Alt OD    (RB I2C1 SDA)
 * PB8  - In        (CAN RX)
 * PB9  - Alt PP    (CAN TX)
 * PB10 - Alt OD    (I2C2 SCL)
 * PB11 - Alt OD    (I2C2 SDA)
 */
#define VAL_GPIOBCRL            0xFFB4B333      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x8888FFB4      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFF8

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC4  - PP        (M0DIRP)
 * PC5  - PP        (M0DIRN)
 * PC6  - OD        (S12)
 * PC7  - OD        (S13)
 * PC8  - OD        (S14)
 * PC9  - OD        (S15)
 * PC10 - OD        (S17)
 * PC11 - OD        (S18)
 * PC12 - OD        (S19)
 */
#define VAL_GPIOCCRL            0x77338888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88877777      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFCF

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD2  - In        (RBGP0)
 * PD5  - OD        (S09)
 * PD6  - OD        (S08)
 * PD7  - OD        (S07)
 * PD14 - OD        (S10)
 * PD15 - OD        (S11)
 */
#define VAL_GPIODCRL            0x77788488      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x77888888      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 * PE0  - OD        (S06)
 * PE1  - OD        (S05)
 * PE2  - OD        (S04)
 * PE3  - OD        (S03)
 * PE4  - OD        (S02)
 * PE5  - OD        (S01)
 * PE6  - OD        (S00)
 * PE7  - PP        (MDDIR1)
 * PE8  - Alt PP    (M0PWMN)
 * PE9  - Alt PP    (M0PWMP)
 * PE10 - Alt PP    (M1PWMN)
 * PE11 - Alt PP    (M1PWMP)
 * PE12 - PP        (MDDIR2)
 * PE13 - Alt PP    (MDPWM1)
 * PE14 - Alt PP    (MDPWM2)
 */
#define VAL_GPIOECRL            0x37777777      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x8BB3BBBB      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFF807F

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) (0) /*palClearPad(GPIOC, GPIOC_USB_DISC)*/

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) (0) /*palSetPad(GPIOC, GPIOC_USB_DISC)*/

/*
 * PAL Map constants
 */
#define PALMAP_PADS_SIZE	16
#define PALMAP_PORTS_SIZE	2

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
