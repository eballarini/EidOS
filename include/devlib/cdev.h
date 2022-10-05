/* 
 * File:   cdev.h
 * Author: Emanuele Ballarini
 *
 */

#ifndef CDEV_H
#define	CDEV_H
#include <stdlib.h>
#include <FreeRTOS.h>
#include <queue.h>
//#include <devlib/buffer.h>

//#define UART1_DEVNUM 0
#define UART4_DEVNUM 0
#define DEV_NUM 1
#define READ_BUFF 5
#define CDEV_BUFFER_LEN 10
//buffer delay in tick periods
#define BUFF_FULL_DELAY 1
#define BUFF_EMPTY_DELAY 1
#define RECEIVE_ISR_QUEUE 20

#define ERR_DEV_CLOSED -1

typedef enum {DEV_CLOSED,DEV_INIT,DEV_OPEN} DEV_STATUS;
typedef enum {SHARED, DEDICATED} DEV_TYPE;

/*device structure*/
typedef struct dev{
  //FIFO_buff* in;
  QueueHandle_t in;
  //FIFO_buff* out;
  QueueHandle_t out;
  /*dev_num=maj_dev_num<<16+min_dev_num*/
  unsigned int dev_num;
  /*driver: returns default configuration pointer*/
  unsigned int* (*config_words)();
  DEV_STATUS status;
  DEV_TYPE type;
  //unsigned int owner_proc;
  /*driver: open device with configuration pointer, device pointer*/
  void (*open)(unsigned int *, unsigned int);
  /*driver: read char from device*/
  unsigned int (*readchar)(char*, unsigned int);
  /*driver: write char to device*/
  void (*writechar)(char*, unsigned int);
  /*driver: close device with minor dev num*/
  void (*close)(unsigned int);
}DEV;

/*device opening structure*/
typedef struct dev_parameters{
   unsigned int* config;
   unsigned int dev_num;
}DEV_PARAM;

#ifndef CDEV_H_DEFS_ONLY

/*application interface to devices*/

unsigned int* cdev_get_conf(unsigned int dev_num);

int dev_receive_from_ISR();

int initialize_receive_queue();

void initialize_dev_list();

/*-1 process not owner of the device, 0 ok, 2 device not open, 3 unknown error*/
int writechar(DEV* device, char out);

/*-1 process not owner of the device, 0 ok, 1 buffer empty, 2 device not open, 3 unknown error*/
int readchar(DEV* device, char *in);

/*echo added*/
int readechochar(DEV* device, char *in);

/*returns number of written chars*/
int swritearray(DEV* device, char *out, unsigned int len);

/*returns number of written chars, ends on endian or len*/
int writearray(DEV* device, char *out, unsigned int len, char endian);

/*returns number of read chars, -1 means failed*/
int sreadarray(DEV* device, char *in, unsigned int len);

/*returns number of read chars, -1 means failed, ends on endian or len*/
int readarray(DEV* device, char *in, unsigned int len, char endian);

/*echo added*/
int readechoarray(DEV* device, char *in, unsigned int len, char endian);

/*returns device pointer, open device with major device number*/
DEV* opendev(unsigned int *config, unsigned int dev_num);

/*closes device with device pointer*/
int closedev(DEV* device);

/*kernel interface to devices*/

void flushdev(DEV* device);

void readdev(DEV* device);

void readdevFromISR(DEV* device);

void flushall();

void readall();

void openall();

void force_closeall();

void force_closedev(DEV* device);

void vTaskIOhandler();

#endif

#endif	/* CDEV_H */

