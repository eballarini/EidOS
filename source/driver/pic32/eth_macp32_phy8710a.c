
#include <p32xxxx.h>
#include <proc/p32mz2048efh064.h>
#include "sys/log.h"
#include "driver/eth_macp32_phy8710a.h"

#define PHY_ADDRESS 0x0000

uint8_t mac_address[6];

EthDescr rx_descr_list[ETH_PHY_RX_BUFF_NUM];
EthDescr tx_descr_list[ETH_PHY_RX_BUFF_NUM];

uint8_t rxbuff[ETH_PHY_RX_BUFF_NUM][1536];
uint8_t txbuff[ETH_PHY_RX_BUFF_NUM][1536];

extern __inline__ unsigned int __attribute__((always_inline)) virt_to_phys(const void* p) 
{ 
 return (int)p<0?((int)p&0x1fffffffL):(unsigned int)((unsigned char*)p+0x40000000L); 
}

void init_rx_buff()
{
    uint32_t i;
    
    for(i=0;i<ETH_PHY_RX_BUFF_NUM-1;i++)
    {
        rx_descr_list[i].hdr.bCount=0;
        rx_descr_list[i].hdr.EOP=0;
        rx_descr_list[i].hdr.SOP=0;
        
        rx_descr_list[i].status=0;
        rx_descr_list[i].pEDBuff=&rxbuff[i][0];
        
        rx_descr_list[i].hdr.NPV=0;
        rx_descr_list[i].hdr.EOWN=1;
    }
    
    rx_descr_list[ETH_PHY_RX_BUFF_NUM-1].next_ed=(uint32_t)&rx_descr_list[0];
        
    rx_descr_list[ETH_PHY_RX_BUFF_NUM-1].hdr.NPV=1;
    
    ETHRXSTbits.RXSTADDR=(uint32_t)&rx_descr_list[0];
    
    ETHCON1bits.RXEN=1;

}

void CheckRXframes()
{
    uint32_t i;
    
   write_log("\r\nethrx: received frame stats:\r\n");
   printchartohex(ETHSTATbits.BUFCNT);
   write_log("\r\n");
   
   printchartohex(ETHRXOVFLOWbits.RXOVFLWCNT>>8);
   printchartohex(ETHRXOVFLOWbits.RXOVFLWCNT);
   
   write_log("\r\n");
   write_log("\r\n");
   
    for(i=0;i<ETH_PHY_RX_BUFF_NUM-1;i++)
    {
        printchartohex(rx_descr_list[i].hdr.EOWN);
        write_log("\r\n");
    }
}

void init_eth()
{
    uint16_t data;
    uint32_t buffer, pin_conf, mii_mode;
    
    //ETH controller init
    
    //--test 
    //IEC1&=0xefffffff;
    
    //disable ethernet controller interrupts
    IEC4bits.ETHIE=0;
    
    //--test
    //ETHCON1&=0xffff7cff;
    
    
    ETHCON1bits.ON=0;
    ETHCON1bits.RXEN=0;
    ETHCON1bits.TXRTS=0;
    
    //buffer=ETHSTAT;
    
    buffer=ETHSTATbits.BUSY;
    
    //clear interrupt flag
    IFS4bits.ETHIF=0;
    
    ETHIEN=0x00000000;
    
    //--test
    ETHIRQ=0x00000000;
    
    //clear TX and RX start address
    ETHTXST=0;
    ETHRXST=0;
    
    //mac init
    /*EMAC1CFG1bits.SOFTRESET=1;*/
    EMAC1CFG1bits.RESETRMCS=1;
    EMAC1CFG1bits.RESETRFUN=1;
    EMAC1CFG1bits.RESETTMCS=1;
    EMAC1CFG1bits.RESETTFUN=1;
    
    pin_conf=DEVCFG3bits.FETHIO;
    
    mii_mode=DEVCFG3bits.FMIIEN;
    
    if(mii_mode==0)
    {
        write_log ("\r\n using RMII \r\n");
        EMAC1SUPPbits.RESETRMII=1;
        //1 for 100mbps
        EMAC1SUPPbits.SPEEDRMII=1;
    }
    
    EMAC1MCFGbits.RESETMGMT=1;
    EMAC1MCFGbits.RESETMGMT=0;
    
    // program the MIIM clock, divide by 40
    EMAC1MCFGbits.CLKSEL=0x8;
    
    //--test
    //IFS1&=0xefffffff;
    
    //--test
    //EMAC1MCFG=0x00008000; // issue reset
    
    //--test
    //EMAC1MCFG=0; // clear reset
    
    //--test
    //EMAC1MCFG=(0x8)<<2;// program the MIIM clock, divide by 40
    
    //--test
    //ETHCON1|=0x00008000;
    //ETHCON1bits.ON=1;
    
    //phy init
    
    //reset phy
    p32_mac_lan8710a_readreg(LAN8710A_BASE_CTRL_REG,&data);
    p32_mac_lan8710a_writereg(LAN8710A_BASE_CTRL_REG,data|LAN8710A_BASE_CTRL_REG_RESTART);
    
    //set RMII
    
    p32_mac_lan8710a_writereg(LAN8710A_SPECIAL_MODE_REG,0x40E0);
    
    //set negotiation capabilities
    
    p32_mac_lan8710a_writereg(LAN8710A_AUTONEG_ADV_REG,0x01E1);
    
    //start negotiation
    
    p32_mac_lan8710a_readreg(LAN8710A_BASE_CTRL_REG,&data);
    p32_mac_lan8710a_writereg(LAN8710A_BASE_CTRL_REG,data|LAN8710A_PHY_SPECIAL_ENABLE_AUTONEG|LAN8710A_PHY_SPECIAL_RESTART_AUTONEG);
    
    //verify status
    
    //write_log ("\r\n autoneg lpa register: \r\n\0");
    //print_ethphy_reg(LAN8710A_AUTONEG_LPA_REG);
    
    //write_log ("\r\n eth controller open \r\n\0");
    
    //disable 4B5B encoding
    p32_mac_lan8710a_writereg(LAN8710A_PHY_SPECIAL_CTRL_STATUS_REG,0x0000);
    
    //write_log ("\r\n phy special ctrl status register: \r\n\0");
    //print_ethphy_reg(LAN8710A_PHY_SPECIAL_CTRL_STATUS_REG);

    //mac configuration
    EMAC1CFG1bits.RXENABLE=1;
    EMAC1CFG1bits.TXPAUSE=1;
    EMAC1CFG1bits.RXPAUSE=1;
    
    EMAC1CFG2bits.AUTOPAD=1;
    EMAC1CFG2bits.VLANPAD=0;
    EMAC1CFG2bits.PADENABLE=1;
    EMAC1CFG2bits.CRCENABLE=1;
    EMAC1CFG2bits.FULLDPLX=1;
    EMAC1CFG2bits.HUGEFRM=0;
    
    EMAC1IPGTbits.B2BIPKTGP=0x15;
    EMAC1IPGRbits.NB2BIPKTGP1=0xC;
    EMAC1IPGRbits.NB2BIPKTGP2=0x12;
    
    //collision settings default
    //EMAC1CLRTbits.CWINDOW=0x37;
    //EMAC1CLRTbits.RETX=0xf;
    
    //default mtu 1518 bytes
    EMAC1MAXFbits.MACMAXF=0x05EE;
    
    mac_address[0]=EMAC1SA0bits.STNADDR6;
    mac_address[1]=EMAC1SA0bits.STNADDR5;
    mac_address[2]=EMAC1SA1bits.STNADDR4;
    mac_address[3]=EMAC1SA1bits.STNADDR3;
    mac_address[4]=EMAC1SA2bits.STNADDR2;
    mac_address[5]=EMAC1SA2bits.STNADDR1;
    
    //flow control settings
    
  /*  ETHCON1bits.PTV=0x00ff;
    ETHRXWMbits.RXFWM=0xff;
    ETHRXWMbits.RXEWM=0x80;
    ETHCON1bits.AUTOFC=1;*/
    
    //filters
    
    //CRC OK
    ETHRXFCbits.CRCOKEN=1;
    //Hash table
    ETHRXFCbits.HTEN=0;
    //Multicast
    ETHRXFCbits.MCEN=1;
    //Magic packet
    ETHRXFCbits.MPEN=0;
    //Not me unicast
    ETHRXFCbits.NOTMEEN=0;
    //Unicast
    ETHRXFCbits.UCEN=1;
    //Broadcast
    ETHRXFCbits.BCEN=1;
    //Runt 
    ETHRXFCbits.RUNTEN=0;
    
    //buffer size 1536 bytes
    ETHCON2bits.RXBUF_SZ=0x60;
    
    init_rx_buff();
    
    //ETHIENbits.w=0xFFFFFFFF;
    
    //IEC4bits.ETHIE=1;
    
    ETHCON1bits.RXEN=1;
    ETHCON1bits.ON=1;
    
    write_log ("\r\n mac address: \0");
    printchartohex((uint8_t)mac_address[5]);
    write_log (":\0");
    printchartohex((uint8_t)mac_address[4]);
    write_log (":\0");
    printchartohex((uint8_t)mac_address[3]);
    write_log (":\0");
    printchartohex((uint8_t)mac_address[2]);
    write_log (":\0");
    printchartohex((uint8_t)mac_address[1]);
    write_log (":\0");
    printchartohex((uint8_t)mac_address[0]);
    write_log ("\r\n\0");
    
}

void p32_mac_lan8710a_readreg(uint8_t reg_addr, uint16_t * data_ptr)
{
    
    while(EMAC1MIND&0x1);
    
    EMAC1MADR=reg_addr/*|((PHY_ADDRESS)<<8)*/;
    
    EMAC1MCMD=1;
    
	//EMAC1MADR=reg_addr;
	
	//remember to disable write or scan
	//EMAC1MCMD|=1;
	
    _nop();
    _nop();
    _nop();
    
    while(EMAC1MIND&0x1==0)
    {
         //write_log ("\r\n waiting for MIND to be asserted\r\n\0");
    }
    
	while(EMAC1MIND&0x1)
    {
         //write_log ("\r\n waiting for data\r\n\0");
    }
	
	EMAC1MCMD=0;
	
    //printchartohex((uint8_t)(EMAC1MRDD>>8));
    //printchartohex((uint8_t)EMAC1MRDD);
    
	*data_ptr=EMAC1MRDD;
}

void p32_mac_lan8710a_writereg(uint8_t reg_addr, uint16_t data)
{
    while(EMAC1MIND&0x1);
    
    EMAC1MADR=reg_addr|((PHY_ADDRESS)<<8);
    
    EMAC1MWTD=data;
    
    _nop();
    _nop();
    _nop();
    
    while(EMAC1MIND&0x1==0)
    {
         //write_log ("\r\n waiting for MIND to be asserted\r\n\0");
    }
    
	while(EMAC1MIND&0x1);
    {
         //write_log ("\r\n waiting for data to be written\r\n\0");
    }
}