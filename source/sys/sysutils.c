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

#include <sys/hardwaredefs.h>
#include <task.h>
#include <p32xxxx.h>

char inttohexmap[]="0123456789ABCDEF"; /*{'0', '1', '2', '3', '4', '5', '6' , '7' ,'8' , '9', 'A', 'B', 'C', 'D', 'E', 'F'};*/

void vTask_sys_led_blink()
{
const TickType_t toggle_delay = 500 / portTICK_PERIOD_MS;    

for (;;)
{
SYS_LED=~SYS_LED;
vTaskDelay(toggle_delay);
}

}


void vTask_Idle()
{
    while(1)
    {}
}

void ctoliteral(uint8_t value, char* literal)
{
    literal[1]=inttohexmap[value%16];
    literal[0]=inttohexmap[value/16];
    literal[2]='\0';
}


