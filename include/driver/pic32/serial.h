/* 
 * File:   serial.h
 * Author: Emanuele Ballarini
 *
 */

#ifndef SERIAL_H
#define	SERIAL_H

//#include <devlib/buffer.h>

void ser_init(unsigned int* config, unsigned int dev_pointer);

/*return 0 if there's no char, 1 if a char has been read*/
unsigned int ser_readchar(char* in, unsigned int minor);

void ser_writechar(char* out, unsigned int minor);

void ser_close(unsigned int minor);

/*returns length of config in bytes*/
unsigned int* ser_config_words ();

#endif	/* SERIAL_H */

