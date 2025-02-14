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

#include <devlib/cdev.h>
#include <sys/sysutils.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LOG_SIZE 100

DEV* log_dev;

unsigned int setlogdev(DEV* device)
{
    log_dev=device;
}

unsigned int write_log(char* message)
{
    writearray(log_dev, message, MAX_LOG_SIZE, '\0');
}

void printchartohex(uint8_t input)
{
    char literal[3];
    
    ctoliteral(input,literal);
    write_log (literal);
}

void log_string_int(char* message, uint32_t data)
{
   char literal[10]="\0";
   
   writearray(log_dev, message, MAX_LOG_SIZE, '\0'); 
   
   writearray(log_dev, literal, MAX_LOG_SIZE, '\0'); 
}
