/*
 * main.c
 *
 *  Created on: 17.07.2015
 *      Author: henne
 */


#include "common.h"
#include "uart.h"
#include "fifo.h"

extern fifo fifo_tx_uart;
extern fifo fifo_rx_uart;

void test() {
    uint8_t character = 'A';
    while (!(UCSRA & (1<<UDRE)));

    char* send = "sent";

    while (1) {

    	//uart_send_byte(character);
        if ( !fifo_empty(&fifo_rx_uart)) {
           	uart_receive_byte(&character);
        	tbi(PORTB,PB0);
        	uart_send_byte(character);
        	uart_send_line("");
        	uart_send_line("line sent");
        }
    }
}


//int main (void)
//{
//
//	DDRB |= (1<<PC0);
//
//	uart_init();
//	test();
//
//    return 0;
//}
