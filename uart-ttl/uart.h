
#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_send_byte(uint8_t _character);
void uart_receive_byte(uint8_t* _character);
void uart_send_string(char* _string);

void uart_blocking_send_line(char* _string);
void uart_blocking_send_byte(uint8_t _character);
void uart_send_uint8_t(int _int);

#endif
