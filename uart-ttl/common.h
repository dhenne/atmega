
#ifndef COMMON
#define COMMON

#define F_CPU 14745800UL                    // set the CPU clock
#define __AVR_ATmega32__

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "avr/interrupt.h"
#include <avr/signal.h>


#define BAUD 9600                           // define baud
#define BAUDRATE ((F_CPU)/(BAUD*8UL)-1)    // set baudrate value for UBRR
//#define BAUDRATE 0x005F

#endif // COMMON
