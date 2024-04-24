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
