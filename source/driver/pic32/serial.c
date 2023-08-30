#include <stdlib.h>
#include <devlib/cdev.h>
#include <sys/hardwaredefs.h>
#include <p32xxxx.h>                // Include PIC32 specifics header file
#include <queue.h>
#include <xc.h>
#include <sys/attribs.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <devlib/cdev.h>

#define CONFIG_WORDS 3
#define CONFIG_WORD_SIZE 4
#define BAUDRATE 9600
#define SERIAL_CLK configPERIPHERAL_CLOCK_HZ

extern SemaphoreHandle_t dev_rx_wake;
extern QueueHandle_t dev_to_be_read;
extern TaskHandle_t char_receive_int_handler;

extern DEV cdev[];

//Set Baud rate as specified by config
//Enable Uart for 8-bit data, no parity, 1 STOP bit
//Enable Transmit and Receive
unsigned int ser_def_config[]={(SERIAL_CLK/(16*BAUDRATE))-1,0x8000,0x1400};


void ser_init(unsigned int* config, unsigned int dev_pointer)
{
    
    //QueueHandle_t in, out;

    switch(dev_pointer)
    {
#if defined UART1_DEVNUM
    case UART1_DEVNUM:
    /*specific registers configuration*/
    U1BRGCLR=0xFFFFFFFF;
    U1STA=0;
    U1MODE=0xFFFFFFFF;
    U1BRG=config[0];
    U1MODE=config[1];
    U1STASET=config[2];
    
    // Configure Interrupts
    // In datasheet: INTERRUPT IRQ, VECTOR AND BIT LOCATION
    IEC3bits.U1RXIE=0;
    // disable UART1 RX int U1RXIE, IEC3<17>
    IFS3bits.U1RXIF=0;
    // clear UART1 RX int U1RXIF, IFS3<17>
    IPC28bits.U1RXIP=2;
    // set UART1 int priority = 2, IPC28<12:10>
    IPC28bits.U1RXIS=0;
    // set UART1 int subpriority = 0, IPC28<9:8>
    IEC3bits.U1RXIE=1;
    // enable UART1 RX int U1RXIE, IEC3<17>
    break;
#endif
#if defined UART4_DEVNUM
    case UART4_DEVNUM:
    /*specific registers configuration*/
    U4BRGCLR=0xFFFFFFFF;
    U4STA=0;
    U4MODE=0xFFFFFFFF;
    U4BRG=config[0];
    U4MODE=config[1];
    U4STASET=config[2];
    
    // Configure Interrupts
    // In datasheet: INTERRUPT IRQ, VECTOR AND BIT LOCATION
    IEC5bits.U4RXIE=0;
    // disable UART1 RX int U1RXIE, IEC3<17>
    IFS5bits.U4RXIF=0;
    // clear UART1 RX int U1RXIF, IFS3<17>
    IPC42bits.U4RXIP=2;
    // set UART1 int priority = 2, IPC28<12:10>
    IPC42bits.U4RXIS=0;
    // set UART1 int subpriority = 0, IPC28<9:8>
    IEC5bits.U4RXIE=1;
    // enable UART1 RX int U1RXIE, IEC3<17>
    
    break;
#endif
    default:
    break;
    }
}

unsigned int ser_readchar(char* in, unsigned int dev_pointer)
{
    //test
    char buffer;

    
    switch(dev_pointer)
    {
#if defined UART1_DEVNUM
    case UART1_DEVNUM:
        if (U1STAbits.URXDA==1)
        {
        *in=U1RXREG;
        return 1;
        }
        return 0;
    break;
#endif
#if defined UART4_DEVNUM
    case UART4_DEVNUM:
        if (U4STAbits.URXDA==1)
        {
        *in=U4RXREG;
        return 1;
        }
        return 0;
    break;
#endif
    default:
    break;
    }
}

void ser_writechar(char* out, unsigned int dev_pointer)
{
    
    TickType_t xLastWakeTime;
    
    switch(dev_pointer)
    {
#if defined UART1_DEVNUM
    case UART1_DEVNUM:
        /*if TX is not ready wait*/
        while (U1STAbits.UTXBF!=0)
        {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
        }
    
        U1TXREG=*out;
    break;
#endif
#if defined UART4_DEVNUM
    case UART4_DEVNUM:
        /*if TX is not ready wait*/
        while (U4STAbits.UTXBF!=0)
        {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
        }
    
        U4TXREG=*out;
    break;
#endif
    default:
    break;
    }
}

void ser_close(unsigned int dev_pointer)
{
   /*specific functions*/

   switch(dev_pointer)
   {
#if defined UART1_DEVNUM
    case UART1_DEVNUM:
        /*clear enabled UART*/
        U1MODECLR=0x8000;
        /*clear enabled TX and RX*/
        U1STACLR=0x1400;
    break;
#endif
#if defined UART4_DEVNUM
    case UART4_DEVNUM:
        /*clear enabled UART*/
        U4MODECLR=0x8000;
        /*clear enabled TX and RX*/
        U4STACLR=0x1400;
    break;
#endif
    default:
    break;
    }
   
}

unsigned int* ser_config_words ()
{
return ser_def_config;
}

#if defined UART4_DEVNUM
void __ISR_AT_VECTOR(_UART4_RX_VECTOR, IPL2SOFT) u1_int_ser_redchar()
{   
    
    readdevFromISR(&cdev[UART4_DEVNUM]);
    IFS5bits.U4RXIF=0;

}

void __ISR_AT_VECTOR(_UART4_TX_VECTOR, IPL2SRS) u1_int_ser_wrotechar()
{   
    //do something
    IFS5bits.U4TXIF=0;
}
#endif


#if defined UART1_DEVNUM
void __ISR_AT_VECTOR(_UART1_RX_VECTOR, IPL2SRS) u1_int_ser_redchar()
{   
    readdev(&cdev[UART1_DEVNUM]);
    IFS3bits.U1RXIF=0;
}

void __ISR_AT_VECTOR(_UART1_TX_VECTOR, IPL2SRS) u1_int_ser_wrotechar()
{   
    //do something
    IFS3bits.U1TXIF=0;
}
#endif