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

