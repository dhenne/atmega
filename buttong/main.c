/*
 * main.c
 *
 *  Created on: 18.06.2015
 *      Author: henne
 */


#define F_CPU 14745800UL
#define __AVR_ATmega16__


#include <avr/io.h>
#include <util/delay.h>

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1


void main(void) {

	DDRB |= (1<<PC0);
	//DDRA &= ~(1<<PD0);
	PORTB |= 0x00; //(1<<PC0);

	while(1) //infinite loop
	{
		tbi(PORTB,PB0);
	}
}
