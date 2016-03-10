#include "uart.h"
#include "fifo.h"
#include "common.h"

uint8_t tx_buffer[BUFFER_SIZE_DEF];
uint8_t rx_buffer[BUFFER_SIZE_DEF];
fifo fifo_tx_uart = {tx_buffer, sizeof(tx_buffer), 0, 0};
fifo fifo_rx_uart = {rx_buffer, sizeof(rx_buffer), 0, 0};


// function to initialize UART
void uart_init(void)
{
	UBRRH = 0x00;
	UBRRL = 0x00;
	UBRRH = (BAUDRATE >> 8);
	UBRRL = BAUDRATE; //set baud rate

	UCSRA |= (1<<RXC) | (1<<TXC) | (1<<U2X); // Enable Receive and transmit complete interrupts


    UCSRB |= (1<<RXCIE) | (1<<TXCIE); //enable receiver and transmitter
    UCSRB |= (1<<UDRIE);
    //UCSRC |= (1<<URSEL); // direct writes from UBRRH to UCSRC
    //UCSRC = 0x00;
    //UCSRC &= ~(1<<UMSEL); // Async uart

    UCSRC |= (1<<URSEL) | (1<<UCSZ0)|(1<<UCSZ1);// 8bit data format

    UCSRB |= (1<<TXEN)|(1<<RXEN);

    sei();
}

// function to receive data
unsigned char uart_recieve(void)
{
    while((UCSRA & (1<<RXC)) == 0);

    return UDR;                             // return 8-bit data
}

ISR(USART_RXC_vect) { // Receive complete
	fifo_add(&fifo_rx_uart, UDR);
}

ISR(USART_TXC_vect) { // Send complete
	//tbi(PORTB,PB0);
	if (fifo_empty(&fifo_tx_uart)) {
		if ( UCSRB & (1<<UDRIE)) { //clear data register interrupt enable bit
			UCSRB &= ~(1<<UDRIE);
		}
	} else {
		uint8_t tx_data;
	    fifo_rem(&fifo_tx_uart, &tx_data);
	    UDR = tx_data;
	}
}

ISR(USART_UDRE_vect, ISR_ALIASOF(USART_TXC_vect));


void uart_blocking_send_byte(uint8_t _character) {
	while (fifo_full(&fifo_tx_uart)) {
	}
	fifo_add(&fifo_tx_uart, _character);
	UCSRB |= (1<<UDRIE);
}

void uart_blocking_send_line(char* _string) {
	while ( *_string != '\0') {
			uart_blocking_send_byte(*_string);
			_string++;
	}
	uart_blocking_send_byte(0xD); // carriage return
	uart_blocking_send_byte(0xC); // line feed
}

void uart_blocking_send_string(char* _string) {
	while ( *_string != '\0') {
		uart_blocking_send_byte(*_string);
		_string++;
	}
}


void uart_receive_byte(uint8_t* _character) {
    fifo_rem(&fifo_rx_uart, _character);
}

void uart_send_byte(uint8_t _character) {
	fifo_add(&fifo_tx_uart, _character);
    UCSRB |= (1<<UDRIE);
}


void uart_send_string(char* _string) {
	while ( *_string != '\0') {
		uart_send_byte(*_string);
		_string++;
	}
}

void uart_send_line(char* _string) {
	uart_send_string(_string);
	uart_send_byte(0xD); // carriage return
	uart_send_byte(0xC); // line feed
}

void uart_blocking_send_uint8_t(int _int) {
	char buf[sizeof(int)*3+2];
	snprintf(buf, sizeof buf, "%d", _int);
	uart_blocking_send_string(buf);
}
