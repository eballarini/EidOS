#include <stdio.h>
#include <stdint.h>

uint8_t table[256];

uint16_t crc16_ccitt_table[256];



const uint8_t crc7_poly = 0b10001001;

const uint16_t crc16_ccitt_poly = 0x1021;

void init_CRC7_table()
{
    uint8_t index, crc, j;
    uint32_t counter;
    
    printf("\niniting table\n");
    
    for (counter=0;counter<256; counter++)
    {
     crc = (uint8_t)counter;
     for (j = 0; j < 8; j++) {     
      crc = (crc & 0x80u) ? ((crc << 1) ^ (crc7_poly << 1)) : (crc << 1);
      }
    table[counter]=crc;
    }
    printf("\ntable compiled\n");
    
}

uint8_t CRC7(const uint8_t message[], const uint32_t length) 
{

  uint8_t crc = 0;
  uint32_t i=0;
  
  for (i = 0; i < length; i++) {
     crc ^= message[i];
     
     crc=table[crc];
  }

  return crc >> 1;
}


void init_CRC16_table(uint16_t* crc16_table, uint16_t crc16_poly)
{
    uint16_t crc, j;
    uint32_t counter;
    
    printf("\niniting table\n");

    
    for (counter=0;counter<256; counter++)
    {
     crc=counter;

     for (j = 0; j < 16; j++) {
 
      crc = (crc & 0x8000u) ? ((crc << 1) ^ (crc16_poly)) : (crc<<1);
      
      }
     
     crc16_table[counter]=crc;
     printf("value: %x, CRC: %x\n", counter, crc16_table[counter]);
    }
    printf("\ntable compiled\n");
    
}

uint16_t CRC16(const uint8_t message[], const uint32_t length, uint16_t* crc16_table, uint16_t init) 
{

  uint16_t crc;
  uint32_t i=0 ;

  crc=init;
  
  for (i = 0; i < length; i++) 
  {
     crc = (crc<<8 ) ^ crc16_table[(crc>>8)^message[i]];
  }

  return crc;
}

void init_CRC_tables()
{
    init_CRC7_table();
    init_CRC16_table(crc16_ccitt_table, crc16_ccitt_poly);
}


uint16_t CRC16_CCIT_ZERO(const uint8_t message[], const uint32_t length)
{
 return CRC16(message, length, crc16_ccitt_table, 0);
}