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

#ifndef LAN8710A
#define LAN8710A

#define LAN8710A_BASE_CTRL_REG 0
#define LAN8710A_BASE_STATUS_REG 1
#define LAN8710A_PHY_ID1_REG 2
#define LAN8710A_PHY_ID2_REG 3
#define LAN8710A_AUTONEG_ADV_REG 4
#define LAN8710A_AUTONEG_LPA_REG 5
#define LAN8710A_AUTONEG_EXP_REG 6
#define LAN8710A_MODE_STATUS_REG 17
#define LAN8710A_SPECIAL_MODE_REG 18
#define LAN8710A_SYMB_ERR_REG 26
#define LAN8710A_CTRL_STATUS_IND_REG 27
#define LAN8710A_ISR_REG 29
#define LAN8710A_INT_MASK_REG 30
#define LAN8710A_PHY_SPECIAL_CTRL_STATUS_REG 31

#define LAN8710A_PHY_SPECIAL_ENABLE_AUTONEG 0x1000
#define LAN8710A_PHY_SPECIAL_RESTART_AUTONEG 0x0200
#define LAN8710A_BASE_CTRL_REG_RESTART 0x8000

#define ETH_PHY_RX_BUFF_NUM 5

typedef struct
{
    volatile union
        {
        struct
            {
            uint8_t: 7;
            uint8_t EOWN: 1;
            uint8_t NPV: 1;
            uint8_t: 7;
            uint16_t bCount: 11;
            uint8_t: 3;
            uint8_t EOP: 1;
            uint8_t SOP: 1;
            };
        //unsigned intw;
        }hdr; // descriptor header
    uint8_t* pEDBuff; // data buffer address
    uint64_t status; // tx packet status
    uint32_t next_ed; // next descriptor (hdr.NPV==1);
}__attribute__ ((__packed__)) EthDescr; // hardware Tx descriptor (linked).


void init_rx_buff();
void CheckRXframes();
void p32_mac_lan8710a_readreg(uint8_t reg_addr, uint16_t * data_ptr);
void p32_mac_lan8710a_writereg(uint8_t reg_addr, uint16_t data);
void init_eth();

#endif
