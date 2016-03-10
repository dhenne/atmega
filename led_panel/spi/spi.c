/*
 * spi.c
 *
 *  Created on: 05.03.2016
 *      Author: henne
 */

#include "spi.h"
#include "../../uart-ttl/fifo.h"
#include <stdint.h>
#include "../../uart-ttl/uart.h"


#define PANEL1_HIGH() 	(PORTA |= (1<<PA0))
#define PANEL1_LOW() 	(PORTA &= ~(1<<PA0))
#define WR_CLOCK_HIGH() 	(PORTC |= (1<<PC2))
#define WR_CLOCK_LOW() 		(PORTC &= ~(1<<PC2))
#define DATA_HIGH() 		(PORTC |= (1<<PC0))
#define DATA_LOW() 			(PORTC &= ~(1<<PC0))


void spi_write_pixel(uint8_t _row, uint8_t _column, pixel* _pixel){

}

void test_light_entire_display() {
	//for (uint8_t i = 0 ; i < 0x3F ; i++) {
	for (uint8_t i = 0 ; i < 2 ; i++) {
		uart_blocking_send_uint8_t(i);
		uart_blocking_send_line("");
		PANEL1_LOW();
		send_ram(i, 0b00001111);
		PANEL1_HIGH();
	}
	uart_send_line("ready");
}

void send_ram(uint8_t _ram_address, uint8_t _value) {
	// send 101
	WR_CLOCK_LOW();
	DATA_HIGH();
	WR_CLOCK_HIGH();
	WR_CLOCK_LOW();
	DATA_LOW();
	WR_CLOCK_HIGH();
	WR_CLOCK_LOW();
	DATA_HIGH();
	WR_CLOCK_HIGH();

	// send ram address
	uint8_t mask = 0b01000000;
	for (int index = 0 ; index < 7 ; ++index) {
		WR_CLOCK_LOW();
		if ( (_ram_address & (mask>>index)) == 0) {
			DATA_LOW();
		} else {
			DATA_HIGH();
		}
		WR_CLOCK_HIGH();
	}

	mask = 0b00000001;
	for (int index = 0 ; index < 4 ; ++index) {
			WR_CLOCK_LOW();
			if ( (_value & (mask<<index)) == 0) {
				DATA_LOW();
			} else {
				DATA_HIGH();
			}
			WR_CLOCK_HIGH();
		}

}

void init_display() {
	WR_CLOCK_HIGH();
	uart_send_line("initializing Display");
	PANEL1_LOW();
	send_cmd(1); //SYS ON
	PANEL1_HIGH();
	uart_blocking_send_line("----------");
	PANEL1_LOW();
	send_cmd(3); // LED ON
	PANEL1_HIGH();
	PANEL1_LOW();
	send_cmd(0b0010100); // P-MOS open drain output and 8 COM option
	PANEL1_HIGH();
}

void send_cmd(uint8_t _cmdcode) {
	// send 100
		WR_CLOCK_LOW();
		DATA_HIGH();
		WR_CLOCK_HIGH();
		WR_CLOCK_LOW();
		DATA_LOW();
		WR_CLOCK_HIGH();
		WR_CLOCK_LOW();
		DATA_LOW();
		WR_CLOCK_HIGH();


		uint8_t mask = 0b10000000;
		for (int index = 0 ; index < 8 ; ++index) {
			WR_CLOCK_LOW();
			if ( (_cmdcode & (mask>>index)) == 0) {
				DATA_LOW();
			} else {
				DATA_HIGH();
			}
			WR_CLOCK_HIGH();
		}

}
