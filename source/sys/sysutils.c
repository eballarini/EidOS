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


