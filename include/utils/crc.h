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
