#ifndef SD_CARD_HEAD
#define SD_CARD_HEAD

#define GO_IDLE_STATE 	           0 	
#define SEND_OP_COND               1    
#define SWITCH_FUNC 	           6    
#define SEND_IF_COND 	           8    
#define SEND_CSD 	               9 	
#define SEND_CID 	               10 	
#define STOP_TRANSMISSION 	       12 	
#define SEND_STATUS 	           13 	
#define SET_BLOCKLEN 	           16 	
#define READ_SINGLE_BLOCK 	       17 	
#define READ_MULTIPLE_BLOCK 	   18 	
#define WRITE_SINGLE_BLOCK 	       24 	
#define WRITE_MULTIPLE_BLOCK 	   25 	
#define PROGRAM_CSD 	           27 	
#define SET_WRITE_PROT 	           28 	
#define CLR_WRITE_PROT 	           29 	
#define SEND_WRITE_PROT 	       30 	
#define ERASE_WR_BLK_START_ADDR    32 	
#define ERASE_WR_BLK_END_ADDR 	   33 	
#define ERASE 	                   38 	
#define LOCK_UNLOCK 	           42 	
#define APP_CMD 	               55 	
#define GEN_CMD 	               56 	
#define READ_OCR 	               58 	
#define CRC_ON_OFF 	               59 	

#define R1_IDLE_FLAG			0x01
#define R1_ERASE_RESET_FLAG		0x02
#define R1_ILLEGAL_FLAG			0x04
#define R1_CRC_ERR_FLAG			0x08
#define R1_ERASE_SEQUENCE_FLAG	0x10
#define R1_ADDRESS_ERR_FLAG		0x20
#define R1_PARAMETER_ERR_FLAG	0x40
#define R1_MSB_FLAG				0x80		//always 0

#define R2_LOCKED_FLAG				0
#define R2_WRITE_PROTECT_FLAG		1
#define R2_ERROR_FLAG				2
#define R2_CONTROLLER_ERR_FLAG		3
#define R2_FAILED_ECC_FLAG			4
#define R2_WP_VIOLATION_FLAG		5
#define R2_ERASE_PARAM_ERR_FLAG		6
#define R2_OUT_OF_RANGE_FLAG		7		//always 0

#define R1_TYPE		1
#define R1B_TYPE	2
#define R2_TYPE		3
#define R3_TYPE		4
#define R7_TYPE		7

#define VHS_3V 1
#define VHS_LOW 2

#define CHECK_PATTERN 0xAA
#define CMD_RESPONSE 0
#define CMD_NO_RESPONSE 1
#define CMD_GENERIC_ERR 2
#define CMD_ILLEGAL_ERR 3
#define CMD_PARAMETER_ERR 4

#define SPI_ERROR_RESPONSE  2
#define SPI_NO_RESPONSE     1
#define SPI_GOT_RESPONSE    0

#define BLOCK_READ_OK   0
#define BLOCK_READ_FAIL   1
#define BLOCK_READ_CARD_NOT_READY 2
#define BLOCK_READ_OOR 3
#define BLOCK_READ_ECC 4
#define BLOCK_READ_CC 5
#define BLOCK_READ_ERR 6
#define BLOCK_READ_CRC 7

#define SD_SECTOR_READ_OK 0
#define SD_SECTOR_READ_FAIL 1
#define SD_SECTOR_READ_CARD_NOT_READY 2

#define SD_SECTOR_WRITE_OK 0
#define SD_SECTOR_WRITE_FAIL 1
#define SD_SECTOR_WRITE_CARD_NOT_READY 2

#define SD_OPERATIONAL 0
#define SD_UNUSABLE 1

#define SD_WRITE_TIMEOUT_CYCLES 1000
#define SD_WRITE_BUSY_TIMEOUT_CYCLES 1000
#define WRITE_ACCEPT_TOKEN 0b00000101
#define WRITE_CRC_ERR_TOKEN 0b00001011
#define WRITE_ERR_TOKEN 0b00001101

#define BLOCK_WRITE_OK 0
#define BLOCK_WRITE_FAIL 1
#define BLOCK_WRITE_CRC 2
#define BLOCK_WRITE_ERR 3
#define BLOCK_WRITE_CARD_NOT_READY 4

#define SD_CSD_READ_OK 0
#define SD_CSD_READ_FAIL 1

#define SD_CID_READ_OK 0
#define SD_CID_READ_FAIL 1

#define SD_BUSY_FALSE 0
#define SD_BUSY_TRUE 1

#define ACC_3_3_V_FLAG 

#define SD_VDD_2728(X)         X & 0b10000000
#define SD_VDD_2829(X)         X & 0b00000001
#define SD_VDD_2930(X)         X & 0b00000010
#define SD_VDD_3031(X)         X & 0b00000100
#define SD_VDD_3132(X)         X & 0b00001000
#define SD_VDD_3233(X)         X & 0b00010000
#define SD_VDD_3334(X)         X & 0b00100000
#define SD_VDD_3435(X)         X & 0b01000000
#define SD_VDD_3536(X)         X & 0b10000000

#define SD_CCS(X)   X & 0b01000000
#define SD_BUSY(X)   X & 0b10000000

#define SD_TOKEN_OUT_OF_RANGE(X)    X & 0b00001000
#define SD_TOKEN_CARD_ECC(X)        X & 0b00000100
#define SD_TOKEN_CC(X)        X & 0b00000010
#define SD_TOKEN_ERROR(X)        X & 0b00000001

#define SD_GET_DATA_FAIL 1
#define SD_GET_DATA_OK  0

//#define SD_SPI(X) SD ## X ## _SPI

#include <stdint.h>

typedef enum {
    INIT, IDLE, UNUSABLE, OCR, CCS, OP_COND, OPERATIONAL
}SD_STATE;

typedef struct {
    SD_STATE state;
    char* name;
    uint8_t name_length;
    uint8_t voltage[2];
    uint8_t ccs;
    uint8_t version;
    uint32_t blocklen;
    uint8_t spi_num;
    uint8_t busy;
}SD_dev;

 typedef struct command_fields {
     uint8_t    end_bit:1;            // always 1
     uint8_t    crc:7;                // CRC
     uint32_t   argument;            // 32 bit argument
     uint8_t    index:6;            // command index
     uint8_t   transmitter_bit: 1;    // always 1
     uint8_t    start_bit: 1;        // always 0
 }CMD_FIELDS;  
 
 typedef struct response_fields{
	 uint8_t	first_byte;
	 uint8_t	second_byte;
     uint8_t    response[6];
     uint8_t    byte_num;
 }RESPONSE_FIELDS;
 
 
 //send command
 int send_command( uint8_t cmd_index, uint8_t *argument, uint8_t spi_num);
 
 //returns number of bytes received
 int get_response(RESPONSE_FIELDS *response, uint8_t response_type, uint8_t spi_num);
 
 //some commands
 int go_idle(RESPONSE_FIELDS *response, SD_dev * sd);
 
 int send_if_cond(RESPONSE_FIELDS *response, SD_dev * sd);
 
 int read_ocr(RESPONSE_FIELDS *response, SD_dev * sd);
 
 int send_app_cmd(RESPONSE_FIELDS *response, SD_dev * sd);
 
 int initialize_sd_card(SD_dev * sd);
 
 void sd_unassert();
 
 void sd_assert();
 
 void initialize_all_sd_cards();
 
 void sd_test_read();
 
 int sd_read_sector(SD_dev * sd, uint8_t * data, uint32_t sector_address, uint32_t sector_size, uint32_t sector_num);
 
 int sd_write_sector(SD_dev * sd, uint8_t * data, uint32_t sector_address, uint32_t sector_size, uint32_t sector_num);
 
 int sd_get_data(SD_dev * sd, uint8_t * data, uint32_t len );
 
 int sd_read_cid(SD_dev * sd, uint8_t * data);
 
 int sd_read_csd(SD_dev * sd, uint8_t * data);
 
 int sd_read_register(SD_dev * sd, uint8_t * data, uint8_t cmd_index, uint32_t len);
 
 int sd_read_busy_check(SD_dev * sd);
 
 int sd_read_block(uint32_t address, SD_dev * sd, uint8_t* data);
 
 int sd_write_block(uint32_t address, SD_dev * sd, uint8_t* data);
 
 void configure_sd_cards();
 
 int sd_print_csd(SD_dev * sd);
  
 int sd_print_cid(SD_dev * sd);
 
 uint8_t sd_get_card_status(SD_dev * sd);
 
#endif