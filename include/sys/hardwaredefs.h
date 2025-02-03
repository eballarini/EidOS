/*
EidOS - a small RTOS for PIC microntrollers
Copyright (C) 2022  Emanuele Ballarini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef HARDWAREDEFS
#define HARDWAREDEFS
#include <devlib/cdev.h>
#include <sdcard/sdcard.h>

#ifdef EMZ64

#define CONSOLE_UART UART4_DEVNUM
#define SYS_LED LATBbits.LATB8
#define USER_LED1 LATBbits.LATB9
#define USER_LED2 LATBbits.LATB10
#define OLED_NRESET LATFbits.LATF1
#define SD_SELECT LATGbits.LATG9
#define SD_CHECK_PRESENCE PORTFbits.RF0

//sd card specific confs
#define VHS_SUPPLIED VHS_3V

#define SD_CARD_DEVICE_NUM 1

#define SD0_NAME "sd0"
#define SD0_SPI 2

#endif

void prvSetupHardware();

#endif
