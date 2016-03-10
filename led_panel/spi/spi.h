
#ifndef SPI_H
#define SPI_H

#include "../common.h"
#include "../pixel_information/pixel.h"




//void spi_init(void);
//void uart_send_byte(uint8_t _character);
// void uart_receive_byte(uint8_t* _character);
void spi_write_pixel(uint8_t _row, uint8_t _column, pixel* _pixel);

void init_display();
void test_light_entire_display();
void send_cmd(uint8_t _cmdcode);
void send_ram(uint8_t _ram_address, uint8_t _value);
#endif
