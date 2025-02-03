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

#ifndef SYS_TIME
#define SYS_TIME

#include <FreeRTOS.h>

typedef uint32_t time_t;

typedef struct
{
    uint32_t year;
    uint32_t mon;
    uint32_t mday;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t tm_wday;	
    uint32_t tm_yday;	
    uint32_t tm_isdst;
}tm;

void time_unix_to_tm(tm* tm_time, time_t unix_time);

time_t time(time_t offset);

void vTask_systime_update();

#endif
