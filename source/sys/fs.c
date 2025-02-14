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

#include <source/ff.h>
#include <sys/hardwaredefs.h>
#include <sdcard/sdcard.h>
#include <task.h>
#include <sys/log.h>
#include <source/ff.h>
#include <sys/systime.h>
#include <stdio.h>

extern FATFS root_fs;

void vTaskDriveHandler(void* pvParameters)
{
    FRESULT res;  
    TickType_t xLastWakeTime;
    
    configure_sd_cards();
    
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 2000 ) );
    
    if(SD_CHECK_PRESENCE==1)
    {
    res=f_mount(&root_fs,"",1);
    
    if(res!=FR_OK)
      {
        write_log ("\r\nfs: mount error\0");
      }
    else
       {
       write_log ("\r\nfs: file system mounted\0"); 
       }
    }
    else
       write_log ("\r\nfs: sd card absent\0"); 
    
    vTaskDelete(NULL);
}

DWORD get_fattime (void)
{
    time_t t;
    tm stm;


    t = time(0);
   
    time_unix_to_tm(&stm, t);

    return (DWORD)(stm.year - 80) << 25 |
           (DWORD)(stm.mon + 1) << 21 |
           (DWORD)stm.mday << 16 |
           (DWORD)stm.hour << 11 |
           (DWORD)stm.minute << 5 |
           (DWORD)stm.second >> 1;
}
