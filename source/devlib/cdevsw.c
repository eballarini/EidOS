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
#include <stdlib.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <driver/pic32/serial.h>
#include <queue.h>

#include <sys/hardwaredefs.h>


/*declaration for closedev usage*/
void flushdev(DEV* device);

/*device list structure*/
DEV cdev[DEV_NUM]={
    {
        0, 0, 0,  ser_config_words, DEV_CLOSED, DEDICATED,  ser_init, ser_readchar,ser_writechar,ser_close
    },
};

/* bit bucket device*/
DEV bit_bucket;


/*function to get default config words*/
unsigned int* cdev_get_conf(unsigned int dev_num)
{
    return cdev[dev_num].config_words();
}

/*function to write a char to a device*/
int writechar(DEV* device, char out)
{ 
	TickType_t xLastWakeTime;
    
	if(device->status!=DEV_OPEN)
	/*2 means device is not open*/
	return ERR_DEV_CLOSED;
	
    while(xQueueSendToBack(device->out, &out, 0)!=pdPASS)
        {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, BUFF_FULL_DELAY );
        }
    
    return 0;

}

/*function to read a char from a device*/
int readchar(DEV* device, char *in)
{  
    TickType_t xLastWakeTime;
    
   	if(device->status!=DEV_OPEN)
	/*2 means device is not open*/
	return ERR_DEV_CLOSED;

     while(xQueueReceive(device->in, in, 0)!=pdPASS)
        {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, BUFF_EMPTY_DELAY );
        }
    
    return 0;
}

/*function to read a char from a device and echo it back*/
int readechochar(DEV* device, char *in)
{  
    TickType_t xLastWakeTime;
    
   	if(device->status!=DEV_OPEN)
	/*ERR_DEV_CLOSED means device is not open*/
	return ERR_DEV_CLOSED;

     while(xQueueReceive(device->in, in, 0)!=pdPASS)
        {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, BUFF_EMPTY_DELAY );
        }
    writechar(device, *in);
    
    return 0;   
}


/*function writing an array with 'len' size, returns number of written chars*/
int swritearray(DEV* device, char *out, unsigned int len)
{
int ret, i;
TickType_t xLastWakeTime;

for(i=0; i<len; i++)
{
    ret=writechar(device, out[i]);
    if (ret==ERR_DEV_CLOSED)
    {
        return ERR_DEV_CLOSED;
    }
}

return i;
}

/*function writing an array with 'len' size, ended by endian char, returns number of written chars*/
int writearray(DEV* device, char *out, unsigned int len, char endian)
{
int ret, i;
TickType_t xLastWakeTime;

for(i=0; i<len; i++)
{
    ret=writechar(device, out[i]);
    if (ret==ERR_DEV_CLOSED)
    {
        return ERR_DEV_CLOSED;
    }
    
    if (out[i]==endian)
        return i;
}
return i;
}

/*function reading an array with 'len' size, returns number of read chars, -1 means failed*/
int sreadarray(DEV* device, char *in, unsigned int len)
{
char buffer;
int ret, i=0;
TickType_t xLastWakeTime;

for(i=0; i<len; i++)
{
    ret=readchar(device,&buffer);
    if (ret==ERR_DEV_CLOSED)
    {
        return ERR_DEV_CLOSED;
    }
    else 
        in[i]=buffer;
}

return i;

}

/*function reading an array with 'len' size, ended by endian char, returns number of read chars, -1 means failed*/
int readarray(DEV* device, char *in, unsigned int len, char endian)
{
char buffer;
int ret, i=0;
TickType_t xLastWakeTime;

for(i=0; i<len; i++)
{
    ret=readchar(device,&buffer);
    if (ret==ERR_DEV_CLOSED)
    {
        return ERR_DEV_CLOSED;
    }
    else 
        in[i]=buffer;
    
    if (in[i]==endian)
        return i;
}

return i;

}

/*function reading and echoing an array with 'len' size, ended by endian char, returns number of read chars*/
int readechoarray(DEV* device, char *in, unsigned int len, char endian)
{
    
char buffer;
int ret, i=0;
TickType_t xLastWakeTime;

for(i=0; i<len; i++)
{
    ret=readechochar(device,&buffer);
    if (ret==ERR_DEV_CLOSED)
    {
        return ERR_DEV_CLOSED;
    }
    else 
        in[i]=buffer;
    
    if (in[i]==endian)
    {
        return i;
    }
}

return i;

}

/*kernel functions operating on real device*/

/*function to open a device*/
DEV* opendev(unsigned int *config, unsigned int dev_num)
{
    unsigned int i, *def_config, config_len;
    
    if (cdev[dev_num].status==DEV_CLOSED)
        {   
			cdev[dev_num].status=DEV_INIT;

			if(config!=0)
			cdev[dev_num].open(config, dev_num);
			else
			{
			def_config=cdev[dev_num].config_words();
			cdev[dev_num].open(def_config, dev_num);
			//free(def_config);
			}

			cdev[dev_num].status=DEV_OPEN;
			cdev[dev_num].in = xQueueCreate(CDEV_BUFFER_LEN, sizeof(char));
            cdev[dev_num].out = xQueueCreate(CDEV_BUFFER_LEN,sizeof(char));
			return &cdev[dev_num];
        }
    else if (cdev[dev_num].status==DEV_OPEN&&cdev[dev_num].type==SHARED)
        return &cdev[dev_num];
    else if (cdev[dev_num].status==DEV_OPEN&&cdev[dev_num].type==DEDICATED)
        return &bit_bucket;
  
   return 0;
}

/*function to close a device*/
int closedev(DEV* device)
{
	
	if(device->status!=DEV_OPEN)
	/*2 means device is not open*/
	return 2;
	
    /*flush output buffer*/
    flushdev(device);
    device->close(device->dev_num);
    device->status=DEV_CLOSED;
    /*0 means is ok*/
    return 0;
}


/*function to flush a device*/
void flushdev(DEV* device)
{
    char buffer;
    BaseType_t output; 

    output=xQueueReceive(device->out, &buffer, 0);
    
    while(output==pdPASS)
    {
    device->writechar(&buffer, device->dev_num);
    output=xQueueReceive(device->out, &buffer, 0);
    }  
}

/*function to read a device*/
void readdev(DEV* device)
{
    char buffer;
	BaseType_t input; 
    
	/*0 means there's no char to be read, 1 it's OK*/
    while(device->readchar(&buffer, device->dev_num)!=0&&uxQueueMessagesWaiting(device->in)<CDEV_BUFFER_LEN-1)
    {          
    input=xQueueSendToBack(device->in, &buffer, 10);
    }
}

/*function to read a device fom Interrupt Service Routine*/
void readdevFromISR(DEV* device)
{
    char buffer;
	BaseType_t input, xHigherPriorityTaskWoken;
    
    xHigherPriorityTaskWoken=pdFALSE;
    
	/*0 means there's no char to be read, 1 it's OK*/
    while(device->readchar(&buffer, device->dev_num)!=0&&uxQueueMessagesWaitingFromISR(device->in)<CDEV_BUFFER_LEN-1)
    {          
    input=xQueueSendToBackFromISR(device->in, &buffer, &xHigherPriorityTaskWoken);
    } 
    
    if( xHigherPriorityTaskWoken )
    {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

/*functions working on all devices*/
void flushall()
{
    unsigned int i;
    char buffer;
    
    for(i=0; i<DEV_NUM; i++)
    {
		if (cdev[i].status==DEV_OPEN)
        flushdev(&cdev[i]);
    } 
}

void readall()
{
    unsigned int i;
    
    for(i=0; i<DEV_NUM; i++)
    { 	
			/*0 means ther's no char to be read, 1 it's OK*/
			if (cdev[i].status==DEV_OPEN)
			readdev(&cdev[i]);
    }  
}

void openall()
{ 
    unsigned int i, *config;
    
    for(i=0; i<DEV_NUM; i++)
    {	
		cdev[i].config_words(config);
		opendev(config, i);
		/*deallocating config array*/
		free(config);
    }

}

/*functions to force operations on devices*/
void force_closeall()
{
   unsigned int i;
   
    for(i=0; i<DEV_NUM; i++)
    {
		force_closedev(&cdev[i]);
    }
}



void force_closedev(DEV* device)
{
	if (device->status==DEV_OPEN)
	{
	device->close(device->dev_num);
	device->status=DEV_CLOSED;
	}
}

/*task to handle periodic tasks on devices like flushing them*/

void vTaskIOhandler()
{
 TickType_t xLastWakeTime;

 xLastWakeTime = xTaskGetTickCount();

 while(1)
 {
    flushall();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
 }
}
