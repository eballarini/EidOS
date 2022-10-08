#include "sdcard/sdcard.h"
#include <stdint.h>
#include <FreeRTOS.h>
#include <p32xxxx.h>                // Include PIC32 specifics header file
#include <sys/attribs.h>
#include <utils/crc.h>
#include <proc/p32mz2048efh064.h>
#include "sys/hardwaredefs.h"
#include <driver/spi.h>

SD_dev sd[SD_CARD_DEVICE_NUM];
 
void sd_assert()
{
    SD_SELECT=0;
}

void sd_unassert()
{
    SD_SELECT=1;
}

void sd_updelay(uint8_t spi_num)
{
   TickType_t xLastWakeTime;
   int i;
    
   xLastWakeTime = xTaskGetTickCount();
   vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
   
    //init delay
    for (i=0;i<100;i++)
    {
    /*sd_send_spi(0xFF);*/
    spi_send(0xFF, spi_num);
    }
    
}

//send command
int send_command( uint8_t cmd_index, uint8_t *argument, uint8_t spi_num)
{
 uint8_t cmd_code=0, crc=0, crc_arg[5];

 //filtering command bits
 cmd_code=cmd_index&0x3f;
 //setting transmitter bit
 cmd_code=cmd_code|0x40;
 
 
 sd_assert();
 
 crc_arg[0]=cmd_code;
 crc_arg[1]=argument[0];
 crc_arg[2]=argument[1];
 crc_arg[3]=argument[2];
 crc_arg[4]=argument[3];
 
 //calculate CRC
 crc=CRC7(crc_arg, 5);
 
 spi_send(0xFF, spi_num);
 	
 spi_send(cmd_code, spi_num);
 
 spi_send(argument[0], spi_num);
 spi_send(argument[1], spi_num);
 spi_send(argument[2], spi_num);
 spi_send(argument[3], spi_num);
 
    
 //send CRC + end bit
 
 spi_send(crc<<1|0x01, spi_num);
 
 spi_send(0xFF, spi_num);
 
 sd_unassert();
}

//returns number of bytes received
int get_response(RESPONSE_FIELDS *response, uint8_t response_type, uint8_t spi_num)
{
    uint8_t buffer;
    int counter=100, i;
    
    sd_assert();
    
    response->byte_num=0;
    
    do
    {
        /*sd_receive_spi(&buffer);*/
        spi_receive(&buffer, spi_num);
        counter--;
        if (counter<1)
          /*timeout*/
          return 1;
    }while(buffer==0xFF);
    
    
    response->response[0]=buffer;
    
    if (response_type==R1_TYPE)
        response->byte_num=1;
    else if (response_type==R2_TYPE)
            response->byte_num=2;
    else if (response_type==R7_TYPE || response_type==R3_TYPE)
            response->byte_num=5;
    
    /* error in first byte*/
    if (buffer!=0&&buffer!=1)
        return 2;

    for (i=1; i<response->byte_num; i++)
    {

    spi_receive(&buffer, spi_num);
    response->response[i]=buffer;
    }
    
    sd_unassert();
    return 0;
    
}

//some commands
int go_idle(RESPONSE_FIELDS *response, SD_dev * sd)
{
 uint8_t argument[4]={0,0,0,0};   
 int response_exit;
 
 send_command( GO_IDLE_STATE , argument, sd->spi_num);

 response_exit=get_response( response, R1_TYPE, sd->spi_num);
 
 if(response_exit==0)
     return CMD_RESPONSE;
 else if(response_exit==1)
 {
 
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==2)
  {
 
  return CMD_GENERIC_ERR;
 }
 
 return CMD_NO_RESPONSE;
}

int send_if_cond(RESPONSE_FIELDS *response, SD_dev * sd)
{
 uint8_t argument[4]={0,0,VHS_3V,CHECK_PATTERN};   
 int response_exit;
 
 send_command( SEND_IF_COND , argument, sd->spi_num);
 //todo GET response
 response_exit=get_response( response, R7_TYPE, sd->spi_num);
 
 if(response_exit==1)
 {
  
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==2)
  {
  
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
  
  return CMD_GENERIC_ERR;
 }
 
 return CMD_RESPONSE;
}

int send_setblocklen(RESPONSE_FIELDS *response, uint16_t size, SD_dev * sd)
{
 uint8_t argument[4]={0,0,(uint8_t)size/256,(uint8_t)size%256};   
 int response_exit;
 
 send_command( SET_BLOCKLEN , argument, sd->spi_num);
 //todo GET response
 response_exit=get_response( response, R1_TYPE, sd->spi_num);
 
 if(response_exit==1)
 {
  
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==2)
  {
  
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
    
    if (response->response[0]&R1_PARAMETER_ERR_FLAG)
      return CMD_PARAMETER_ERR;
  
  return CMD_GENERIC_ERR;
 }
 
 return CMD_RESPONSE;
}

int read_ocr(RESPONSE_FIELDS *response, SD_dev * sd)
{
 uint8_t argument[4]={0,0,0,0};   
 int response_exit;
 
 send_command( READ_OCR , argument, sd->spi_num);

 response_exit=get_response( response, R3_TYPE, sd->spi_num);
 
 if(response_exit==1)
 {
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==2)
  {
  
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
  
  return CMD_GENERIC_ERR;
}
  return CMD_RESPONSE;
}

int send_app_cmd(RESPONSE_FIELDS *response, SD_dev * sd)
{
 uint8_t argument[4]={0,0,0,0};   
 int response_exit;
 
 send_command( APP_CMD , argument, sd->spi_num);

 response_exit=get_response( response, R1_TYPE, sd->spi_num);
 
 if(response_exit==SPI_NO_RESPONSE)
 {
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==SPI_ERROR_RESPONSE)
  {
  
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
  
  return CMD_GENERIC_ERR;
}
  return CMD_RESPONSE;
}

int send_op_cond_cmd(RESPONSE_FIELDS *response, SD_dev * sd)
{
 uint8_t argument[4]={64,0,0,0};   
 int response_exit;
 
 send_command( SEND_OP_COND , argument, sd->spi_num);

 response_exit=get_response( response, R1_TYPE, sd->spi_num);
 
 if(response_exit==SPI_NO_RESPONSE)
 {
  return CMD_NO_RESPONSE;
 }
 else if (response_exit==SPI_ERROR_RESPONSE)
  {
  
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
  
  return CMD_GENERIC_ERR;
  }
  return CMD_RESPONSE;  
}

int send_app_op_cond_cmd(RESPONSE_FIELDS *response, SD_dev * sd)
{
    
    int response_exit;
    
    response_exit=send_app_cmd(response, sd);
    
    if(response_exit==CMD_RESPONSE&&response->response[0]<2)
    {
    send_op_cond_cmd(response, sd);
    return CMD_RESPONSE;  
    }
    else
    return CMD_GENERIC_ERR;
}

int send_readblock_cmd(RESPONSE_FIELDS *response, uint32_t address, SD_dev * sd)
{
 uint8_t argument[4];
 int response_exit;
 
 argument[3]=address%256;
 argument[2]=(address>>8)%256;
 argument[1]=(address>>16)%256;
 argument[0]=(address>>24)%256;
 
 
 send_command( READ_SINGLE_BLOCK , argument, sd->spi_num);

 response_exit=get_response( response, R1_TYPE, sd->spi_num);
 
 if(response_exit==SPI_NO_RESPONSE)
  {
  return CMD_NO_RESPONSE;
  }
 else if (response_exit==SPI_ERROR_RESPONSE)
  {
  if (response->response[0]&R1_ILLEGAL_FLAG)
      return CMD_ILLEGAL_ERR;
  
  return CMD_GENERIC_ERR;
  }
  return CMD_RESPONSE;
}

int sd_read_sector(SD_dev * sd, uint8_t * data, uint32_t sector_address, uint32_t sector_size, uint32_t sector_num)
{
    uint32_t block_count, sector_count, sector_block_ratio;
    int result;
    
    sector_block_ratio=sector_size/sd->blocklen;
    
    for(sector_count=0;sector_count<sector_num; sector_count++)
    {   
    for(block_count=0;block_count<sector_block_ratio; block_count++)
    {
        result=sd_read_block(((sector_address+sector_count)*sector_block_ratio)+(block_count), sd, (data+(sector_count*sector_size)+(block_count*(sd->blocklen) )));
        
        if(result!=BLOCK_READ_OK)
            {
            if (result==BLOCK_READ_CARD_NOT_READY)
                return SD_SECTOR_READ_CARD_NOT_READY;
            else
                return SD_SECTOR_READ_FAIL;
            }
    }
    }
    
    return SD_SECTOR_READ_OK;
}

int read_block(uint8_t* block, SD_dev * sd/*, uint8_t spi_num*/)
{
    uint8_t buffer, crc[2];
    unsigned int i, j;
    uint16_t crc_res;
    
    sd_assert();
    
    for(j=0; j<100000; j++) 
    {
    spi_receive(&buffer, sd->spi_num);
    
    if (buffer==0xFE)
        {
        
        for(i=0; i<sd->blocklen; i++)
            {
            
            spi_receive(&buffer, sd->spi_num);
            
            block[i]=buffer;
            } 
        
        //CRC
        
        spi_receive(&buffer, sd->spi_num);
        crc[0]=buffer;
        
        spi_receive(&buffer, sd->spi_num);
        crc[1]=buffer;
        
        crc_res=CRC16_CCIT_ZERO(block, sd->blocklen);
        
        
        spi_send(0xFF, sd->spi_num);
        
        
        spi_send(0xFF, sd->spi_num);
        
        
        sd_unassert();
        
        
        if(crc_res!=(uint16_t)crc[1]+((uint16_t)(crc[0])<<8))
            return BLOCK_READ_CRC;
            
        
        return BLOCK_READ_OK;
        }
    else if ((!(buffer&0xF0))&&SD_TOKEN_OUT_OF_RANGE(buffer))
        return BLOCK_READ_OOR;
    else if ((!(buffer&0xF0))&&SD_TOKEN_CARD_ECC(buffer))  
        return BLOCK_READ_ECC;
    else if ((!(buffer&0xF0))&&SD_TOKEN_CC(buffer))
        return BLOCK_READ_CC;
    else if ((!(buffer&0xF0))&&SD_TOKEN_ERROR(buffer))
        return BLOCK_READ_ERR;
    }
   
   sd_unassert();
   return BLOCK_READ_FAIL;     
}

int sd_read_block(uint32_t address, SD_dev * sd, uint8_t* data)
{
    RESPONSE_FIELDS response;
    int exit;
    
    if(sd->state!=OPERATIONAL)
        return BLOCK_READ_CARD_NOT_READY;
    
    if(sd->ccs==0)
        address*=sd->blocklen;
    
    exit=send_readblock_cmd(&response, address, sd);
    
    if((exit==CMD_RESPONSE)&&(response.response[0]==0))
    {
        if(read_block(data, sd)==BLOCK_READ_OK)
            return BLOCK_READ_OK;
        else
            return BLOCK_READ_FAIL;
    }
    else
        return BLOCK_READ_FAIL;
    
}

void configure_sd_cards()
{  
    
    int i;
    
    sd[0].spi_num=SD0_SPI;
}

uint8_t sd_get_card_status(SD_dev * sd)
{
    if(sd->state==OPERATIONAL)
    {
        return SD_OPERATIONAL;
    }
    else 
    return SD_UNUSABLE;
}

void initialize_all_sd_cards()
{
    initialize_sd_card(&sd[0]);
}

int sd_set_block_size(SD_dev * sd)
{
    RESPONSE_FIELDS response;
    unsigned int exit, block_size;
    
    if(sd->ccs==0)
    {
        block_size=512;
                
        while(block_size>2)
        {
        exit=send_setblocklen(&response, block_size, sd);
        if(response.response[0]==0)
        {
        write_log ("\r\nsd: set block len to 0x\0");
        printchartohex((uint8_t)block_size);
        sd->blocklen=block_size;
        block_size=0;
        }
        
        block_size=block_size/2;
        }
               
    }
    else
        sd->blocklen=512;   
}

int initialize_sd_card(SD_dev * sd)
{
    RESPONSE_FIELDS response;
    unsigned int i, exit, counter=0, operational_time_counter=0, block_size;
    TickType_t xLastWakeTime;  

    init_CRC_tables();
    sd_unassert();
    
    sd->state=INIT;
    
    spi_init(sd->spi_num);
    
    sd_updelay(sd->spi_num);
            
    
    while (sd->state!=OPERATIONAL&&sd->state!=UNUSABLE)
    {
        switch(sd->state)
        {
            case INIT:
                
                counter++;
                if(counter>100)
                {
                    sd->state=UNUSABLE;
                }
                
                exit=go_idle(&response, sd);
                
                if(exit==CMD_RESPONSE&&(response.response[0]&R1_IDLE_FLAG==R1_IDLE_FLAG))
                {
                    sd->state=IDLE;
                    counter=0;
                }
                
                xLastWakeTime = xTaskGetTickCount();
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
                
                break;
            case IDLE:
                
                exit=send_if_cond(&response, sd);
                
                if(exit==CMD_RESPONSE&&(response.response[4]==CHECK_PATTERN)&&(response.response[3]==VHS_SUPPLIED))
                {                  
                  sd->state=OCR;
                  sd->version=2;
                }
                else if(exit==CMD_NO_RESPONSE || exit==CMD_GENERIC_ERR)
                {
                  sd->state=UNUSABLE;
                }
                else if(exit==CMD_ILLEGAL_ERR)
                {
                  sd->state=OCR;
                  sd->version=1;
                }
                
                xLastWakeTime = xTaskGetTickCount();
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
                
                break;
            case OCR:

                exit=read_ocr(&response, sd);
                
                if(exit==CMD_RESPONSE)
                {
                  
                  sd->voltage[0]=response.response[2];
                  sd->voltage[1]=response.response[3];
                  
                  if (SD_VDD_3233(response.response[2])&&SD_VDD_3334(response.response[2]))
                      sd->state=OP_COND; 
                  else
                      sd->state=UNUSABLE;
                }
                else if(exit==CMD_NO_RESPONSE || exit==CMD_GENERIC_ERR)
                {
                  sd->state=INIT;
                }
                else if(exit==CMD_ILLEGAL_ERR)
                {
                  sd->state=UNUSABLE;
                }
                                
                xLastWakeTime = xTaskGetTickCount();
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
                
                break;
            case OP_COND:
                
                operational_time_counter++;

                if(operational_time_counter>1000)
                {
                    sd->state=UNUSABLE;
                }
                
                exit=send_app_op_cond_cmd(&response, sd);

                if (exit==CMD_RESPONSE && response.response[0]==0)
                 {
                 sd->state=CCS;
                 }
                else if (exit==CMD_ILLEGAL_ERR)
                 {
                 sd->state=UNUSABLE;    
                 }
                    
                xLastWakeTime = xTaskGetTickCount();
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
                
                break;
            case CCS:

                operational_time_counter++;

                if(operational_time_counter>1000)
                {
                    sd->state=UNUSABLE;
                }
                
                exit=read_ocr(&response, sd);
                
                if (exit==CMD_RESPONSE&&SD_BUSY(response.response[1]))
                 { 
                   sd->ccs=SD_CCS(response.response[1]);
                   sd_set_block_size(sd);
                   spi_set_clock(sd->spi_num);
                   sd->state=OPERATIONAL;
                  }
                                
                xLastWakeTime = xTaskGetTickCount();
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
                
                break;
            case OPERATIONAL:         
                break;
            case UNUSABLE:
                break;
            default:
                break;
        }
    }
    
    if(sd->state==OPERATIONAL)
    {
       write_log ("\r\nsd: CARD OPERATIONAL\0");
    }
    else if (sd->state==UNUSABLE)
    {
       write_log ("\r\nsd: CARD UNUSABLE");
       return 1;
    }
    
    if(sd->ccs==0)
        write_log ("\r\nsd: standard capacity card");
    else if (sd->ccs==1)
        write_log ("\r\nsd: high capacity card");
    
    if(sd->version==1)
        write_log ("\r\nsd: version 1 card");
    else if (sd->version==2)
        write_log ("\r\nsd: version 2 card");

    return 0;
}

void sd_test_read()
{
    uint8_t * data;
    int i,j;
    
    RESPONSE_FIELDS response;
    unsigned int exit, block_size;
    
    char literal[2];
    
    data=pvPortMalloc(512);
    
    
    write_log ("\r\nreading first 4 blocks\r\n\0"); 
    for(j=0;j<4;j++)
    {
    if(sd_read_block(j,&sd[0],data)==BLOCK_READ_OK)
    {
    for(i=0;i<sd[0].blocklen;i++)
    { 
        printchartohex(data[i]);
    }
    }
    else if (BLOCK_READ_FAIL)
       write_log ("\r\nblock read failed\r\n\0"); 
    else if (BLOCK_READ_OOR)
       write_log ("\r\nblock read out of range\r\n\0");
    }
    
    write_log ("\r\nreading 4 blocks from 262144*4\r\n\0"); 
    for(j=262144*4;j<262144*4+4;j++)
    {
    if(sd_read_block(j,&sd[0],data)==BLOCK_READ_OK)
    {
    for(i=0;i<sd[0].blocklen;i++)
    { 
        printchartohex(data[i]);
    }
    }
    else if (BLOCK_READ_FAIL)
       write_log ("\r\nblock read failed\r\n\0"); 
    else if (BLOCK_READ_OOR)
       write_log ("\r\nblock read out of range\r\n\0");
    }
    
    write_log ("\r\nreading sector 0 \r\n\0");
    
    if (sd_read_sector(&sd[0], data, 0, 512, 1)==SD_SECTOR_READ_OK)
    {
      
      for(i=0;i<512;i++)
      {
                  printchartohex(data[i]);
      }
    }
    
    write_log ("\r\nreading sector 262144 \r\n\0");
    
    if (sd_read_sector(&sd[0], data, 262144, 512, 1)==SD_SECTOR_READ_OK)
    {
      
      for(i=0;i<512;i++)
      {
                  printchartohex(data[i]);
      }
    }    
    
}

//write functions
int send_writeblock_cmd(RESPONSE_FIELDS *response, uint32_t address, SD_dev * sd)
{
 uint8_t argument[4];
 int response_exit;
 
 argument[3]=address%256;
 argument[2]=(address>>8)%256;
 argument[1]=(address>>16)%256;
 argument[0]=(address>>24)%256;
 
 
 send_command( WRITE_SINGLE_BLOCK , argument, sd->spi_num);

 response_exit=get_response( response, R1_TYPE, sd->spi_num);

 
 if(response_exit==SPI_NO_RESPONSE)
  {
  return CMD_NO_RESPONSE;
  
  }
 else if (response_exit==SPI_ERROR_RESPONSE)
  {
     
     if (response->response[0]&R1_ILLEGAL_FLAG)
     {
      return CMD_ILLEGAL_ERR;
      
     }
  
  return CMD_GENERIC_ERR;
  }
  
  return CMD_RESPONSE;
}

int write_block(uint8_t* block, SD_dev * sd/*, uint8_t spi_num*/)
{
    uint8_t buffer, crc[2];
    unsigned int i, j, k;
    uint16_t crc_res;
    
    sd_assert();
	
	/*send start token*/
	spi_send(0xFE,sd->spi_num);
	
	for(i=0; i<sd->blocklen; i++)
    {
    spi_send(block[i], sd->spi_num);
    } 
	
	for(j=0; j<SD_WRITE_TIMEOUT_CYCLES; j++)
	{
	    spi_receive(&buffer, sd->spi_num);
        
        buffer=buffer&0b00011111;
        
		if (buffer==WRITE_ACCEPT_TOKEN)
			{    
				for(k=0; k<SD_WRITE_BUSY_TIMEOUT_CYCLES; k++)
				{
					spi_receive(&buffer, sd->spi_num);
                    
					if (buffer!=0x00)
						{
							sd_unassert();
						    return BLOCK_WRITE_OK;
						}
				}
				sd_unassert();
				return BLOCK_WRITE_FAIL;
			}		  
		else if (buffer==WRITE_CRC_ERR_TOKEN)
			{
			sd_unassert();
			return BLOCK_WRITE_CRC; 
			}
	    else if (buffer==WRITE_ERR_TOKEN)
			{
			sd_unassert();
			return BLOCK_WRITE_ERR;
			}			
	}
	
	sd_unassert();
    return BLOCK_WRITE_FAIL;  
	
	
}

int sd_write_block(uint32_t address, SD_dev * sd, uint8_t* data)
{
    RESPONSE_FIELDS response;
    int exit;
    
    if(sd->state!=OPERATIONAL)
        return BLOCK_WRITE_CARD_NOT_READY;
    
    if(sd->ccs==0)
        address*=sd->blocklen;
    
    exit=send_writeblock_cmd(&response, address, sd);
    
    if((exit==CMD_RESPONSE)&&(response.response[0]==0))
    {
        if((write_block(data, sd))==BLOCK_WRITE_OK)
            {
            return BLOCK_WRITE_OK;
            
            }
        else
            return BLOCK_WRITE_FAIL;
    }
    else
        return BLOCK_WRITE_FAIL;
    
}

int sd_write_sector(SD_dev * sd, uint8_t * data, uint32_t sector_address, uint32_t sector_size, uint32_t sector_num)
{
    uint32_t block_count, sector_count, sector_block_ratio;
    int result;
    
    sector_block_ratio=sector_size/sd->blocklen;
    
    for(sector_count=0;sector_count<sector_num; sector_count++)
    {   
    for(block_count=0;block_count<sector_block_ratio; block_count++)
    {
        result=sd_write_block(((sector_address+sector_count)*sector_block_ratio)+(block_count), sd, (data+(sector_count*sector_size)+(block_count*(sd->blocklen) )));
        
        if(result!=BLOCK_WRITE_OK)
            {
            if (result==BLOCK_WRITE_CARD_NOT_READY)
                return SD_SECTOR_WRITE_CARD_NOT_READY;
            else
                return SD_SECTOR_WRITE_FAIL;
            }
    }
    }
    
    return SD_SECTOR_WRITE_OK;
}
 
 int sd_read_cid(SD_dev * sd, uint8_t * data)
 {
     return sd_read_register(sd, data, SEND_CID, 16);
 }
 
 int sd_read_csd(SD_dev * sd, uint8_t * data)
 {
     return sd_read_register(sd, data, SEND_CSD, 16);
 }
 
 int sd_print_csd(SD_dev * sd)
 {
     uint8_t data[16];
     int i,j,exit;
     
     //reset
     for(j=0;j<16;j++)
     {
         data[i]=0;
     }
     
     
     exit=sd_read_csd(sd, data);
     
     if(exit==SD_CSD_READ_OK)
     {
        for(i=0;i<16;i++)
        {
            printchartohex(data[i]);
        }
     }

     return 0;
 }
 
  int sd_print_cid(SD_dev * sd)
 {
     uint8_t data[16];
     int i,j,exit;
     
     //reset
     for(j=0;j<16;j++)
     {
         data[i]=0;
     }
     
     exit=sd_read_cid(sd, data);
     
     if(exit==SD_CID_READ_OK)
     {
        for(i=0;i<16;i++)
        {
            printchartohex(data[i]);
        }
     }

     return 0;
 }
  
  int sd_get_data(SD_dev * sd, uint8_t * data, uint32_t len )
  {
    uint32_t i;
    uint8_t buff=0xFF;
      
    sd_assert();
    
    i=100;
    
    while(buff!=0xFE&&i>0)
    {
       i--;
       spi_receive(&buff, sd->spi_num);
    };
    
    if(buff!=0xFE)
    {
        sd_unassert();
        return SD_GET_DATA_FAIL;
    }
 
    for(i=0;i<len;i++)
    {
    spi_receive(&buff, sd->spi_num);
    data[i]=buff;
    printchartohex(data[i]);
    }
  
    //clean up dirty chars
    spi_receive(&buff, sd->spi_num);
    spi_receive(&buff, sd->spi_num);
    
    sd_unassert();
    
    return SD_GET_DATA_OK;
  }
  
 int sd_read_register(SD_dev * sd, uint8_t * data, uint8_t cmd_index, uint32_t len)
 {
 RESPONSE_FIELDS response;
 uint8_t argument[4]={0,0,0,0}, buffer;   
 int response_exit, i;
 
 send_command( cmd_index , argument, sd->spi_num);
 
 response_exit=get_response( &response, R1_TYPE, sd->spi_num);
 
  if (response_exit != 0) 
    {
		return  SD_CID_READ_FAIL;
    }
  else
  {
    sd_get_data(sd, data, len);
  }

 return SD_CID_READ_OK;
 
 }
 
 int sd_read_busy_check(SD_dev * sd)
 {
	return sd->busy;
 }