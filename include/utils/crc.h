#ifndef CRC_HDR
#define CRC_HDR

#include <stdio.h>
#include <stdint.h>

void init_CRC7_table();

uint8_t CRC7(const uint8_t message[], const uint32_t length);

void init_CRC_tables();

void init_CRC16_table(uint16_t* crc16_table, uint16_t crc16_poly);

uint16_t CRC16(const uint8_t message[], const uint32_t length, uint16_t* crc16_table, uint16_t init);

uint16_t CRC16_CCIT_ZERO(const uint8_t message[], const uint32_t length);

#endif