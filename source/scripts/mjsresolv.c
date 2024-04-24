
#include <FreeRTOS.h>
#include <sys/eid_io.h>
#include <source/ff.h>
#include <sys/hardwaredefs.h>
#include <task.h>

void foo(int x) {
  printf("Hello %d!\n", x);
}

void mjs_delay(int a)
{ 
  //const TickType_t xDelay;
    
  //sleep(a/1000);
  TickType_t xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  
  vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(a));
}

void mjs_pwm_set_dc (int a , int b){printf("pwm_set_dc\n"); }
void mjs_pwm_set_period (int a, int b){printf("pwm_set_period\n"); }
void mjs_pwm_on (int a){printf("pwm_on\n"); }
void mjs_pwm_off (int a){printf("pwm_off\n"); }

void mjs_gpio_set(int a){printf("gpio_set\n"); }
void mjs_gpio_unset(int a){printf("gpio_unset\n"); }
void mjs_gpio_flip(int a){printf("gpio_flip\n"); }


int mjs_temp_less_than(int a ) {return 0;}
int mjs_temp_greater_than(int a ) {return 1;}

int mjs_humidity_less_than(int a ) {return 1;}
int mjs_humidity_greater_than(int a ) {return 1;}

int mjs_light_less_than(int a ) {return 1;}
int mjs_light_greater_than(int a ) {return 1;}

int mjs_pressure_less_than(int a ) {return 1;}
int mjs_pressure_greater_than(int a ) {return 1;}

void mjs_wait(unsigned int wait_time)
{
  TickType_t xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( wait_time ) );
}

void mjs_flip_gpio(int num)
{
    switch(num)
    {
        case 1:
            USER_LED1=~USER_LED1;
            break;
        case 2:
            USER_LED2=~USER_LED2;
            break;
        default:
            break;
    }
}

void mjs_res_prints(char* string)
{
    printf("%s", string);
}

char* filetext(char* file_name)
{ 
 
   FIL file;
   FRESULT fres;
   
   unsigned int read_bytes;
  
   char * buffer;
  
   fres = f_open(&file, file_name, FA_READ);

   if (fres == FR_OK)
    {
        buffer=pvPortMalloc(f_size(&file)+1);
        
        if(buffer==0)
        {
            printf("\r\nmalloc failed\r\n");
            return NULL;
        }
        
        f_read(&file, buffer, f_size(&file), &read_bytes);
        
        buffer[f_size(&file)]='\0';
      
        
        f_close(&file);
        
        return buffer;
    }
    else
    {
      printf("\r\nfile not found \r\n");
      buffer=pvPortMalloc(1);
      buffer[0]='\0';
      return buffer;
    }
  
}