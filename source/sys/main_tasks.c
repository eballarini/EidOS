#include <stddef.h>                     
#include <stdlib.h>                     
#include <devlib/cdev.h>
#include <mshell/mshell.h>
#include "sys/sysutils.h"
#include "task.h"
#include "sys/fs.h"
#include "sys/systime.h"
#include <sys/hardwaredefs.h>


int init_main_tasks()
{
	BaseType_t task_create_result;
		
    /* Perform any hardware setup necessary. */
    prvSetupHardware();
    
    /*start led blinking to check for system activity*/
    task_create_result=xTaskCreate(vTask_sys_led_blink, "os active led blinker", 100, NULL, 1, NULL);
    
    /*start process handling input and output to/from devices*/
    task_create_result=xTaskCreate(vTaskIOhandler, "iohandler", 1000, NULL, 1, NULL);
    
    /*start serial console to handle device*/
    task_create_result=xTaskCreate(vTaskCon, "console", 2000, NULL, 1, NULL);
    
    /*start drive handlers*/
    task_create_result=xTaskCreate(vTaskDriveHandler, "drivehandler", 500, NULL, 1, NULL);
    
    task_create_result=xTaskCreate(vTask_systime_update, "systime updater", 500, NULL, 1, NULL);
    
 
#ifdef START_SCHEDULE
    /* Start the task scheduler. */
    vTaskStartScheduler();
#endif
}