
#ifndef SPI_H
#define	SPI_H

int spi_init(uint8_t spi_num);

void spi_set_clock(uint8_t spi_num);

uint8_t spi_send(uint8_t data, uint8_t spi_num);

int spi_receive(uint8_t* data, uint8_t spi_num);

#endif