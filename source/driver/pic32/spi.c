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


#include <p32xxxx.h>  

#include <FreeRTOS.h>

#include <task.h>

int spi_init(uint8_t spi_num)
{
	uint8_t rData;
    
    switch(spi_num)
    {
     case 2 :
	//IEC4CLR=0x0001C000;
    // disable all interrupts
    IEC4bits.SPI2RXIE=0;
    IEC4bits.SPI2TXIE=0;
    IEC4bits.SPI2EIE=0;
	
	SPI2CON = 0; // Stops and resets the SPI2. 
    SPI2CON2 = 0; //reset CON2 register
	rData=SPI2BUF;// clears the receive buffer
	
	//Interrupt settings
    
    // clear any existing event
    IFS4bits.SPI2RXIF=0;
    IFS4bits.SPI2TXIF=0;
	IFS4bits.SPI2EIF=0;
    
	// clear the priority and subpriority
    
    IPC35bits.SPI2RXIP=0;
    IPC35bits.SPI2RXIP=0;
    
    IPC35bits.SPI2EIP=0;
    IPC35bits.SPI2EIP=0;
	
    IPC36bits.SPI2TXIP=0;
    IPC36bits.SPI2TXIP=0;
    
	// set the priority to 4 subpri to 0
    
    IPC35bits.SPI2RXIP=4;
    IPC35bits.SPI2RXIP=0;
    
    IPC35bits.SPI2EIP=4;
    IPC35bits.SPI2EIP=0;
	
    IPC36bits.SPI2TXIP=4;
    IPC36bits.SPI2TXIP=0;
	
	// Enable RX, TX interrupts 
	
    IEC4bits.SPI2RXIE=1;
    IEC4bits.SPI2TXIE=1;
    //IEC4bits.SPI2EIE=1;
	
	SPI2BRG=399;// use FPB/800 clock frequency ((FpB/2FSCK) - 1)
	SPI2STATCLR=0x40;// clear the Overflow
    // Master Mode Slave select enable, SPI ON, 8 bits transfer, SMP=1, ClockEdge fall =1, Master mode
    // SPI ON, 8 bits transfer, SMP=0, ClockEdge fall =1, Master mode
	SPI2CON=0x00008320;
    
    break;
    default:
    break;
    }
}

void spi_set_clock(uint8_t spi_num)
{
    switch(spi_num)
    {
    case 2 :
    SPI2CONbits.ON=0;	
    SPI2BRG=9;// use FPB/20 clock frequency FSCK=FpB/(2*(SPIBRG+1))
    SPI2CONbits.ON=1;
    break;
    default:
    break;
    }
}

uint8_t spi_send(uint8_t data, uint8_t spi_num)
{
    TickType_t xLastWakeTime;
    
    switch(spi_num)
    {
    case 2 :
    
    while (SPI2STATbits.SPITBE!=1)
    {
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
    }

    SPI2BUF=data;
    
    while (SPI2STATbits.SPIRBF!=1)
    {
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
    }
    
    return SPI2BUF;
    
    default:
        return 0;
    }
}

int spi_receive(uint8_t* data, uint8_t spi_num)
{  
    uint8_t buffer;

    buffer=spi_send(0xFF, spi_num);
    
    *data=buffer;
    
    return 0;
    
}
