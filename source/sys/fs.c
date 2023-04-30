#include "source/ff.h"
#include <sys/hardwaredefs.h>
#include "sdcard/sdcard.h"
#include "task.h"
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
    
    res=f_mount(&root_fs,"",1);
    
    if(res!=FR_OK)
      {
        write_log ("\r\nfs: mount error\0");
      }
    else
       {
       write_log ("\r\nfs: file system mounted\0"); 
       }
    
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
