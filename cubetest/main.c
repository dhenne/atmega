/*
 * main.c
 *
 *  Created on: 27.07.2015
 *      Author: henne
 */

#ifndef MAIN_C
#define MAIN_C

// #define F_CPU 8000000UL
#define F_CPU 14745800UL
#define __AVR_ATmega16__

#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

#include "avr/io.h"
#include <util/delay.h>
#include "avr/interrupt.h"
#include <avr/signal.h>
//#include "effects.h"
uint16_t internalMap[3] = { 0};
uint8_t counter = 0;
uint8_t changed = 0;

void test() {
	PORTA = 0xff;
	PORTB = 0x0f;
}

void ioinit(void) {
	DDRB = 0xF0; // ersten 4 bits für layer enable
	DDRA = 0xFF; // vordere 8 leds
	DDRC = 0xFF; // hintere 8 leds

	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
}

void intInit(void) {
	// TIMER
	// FB interrupt
	// 14745600/128/30 = 3840 interrupts per second
	// 3840/4 = 960 frames per second
	/*TCCR1A |= 0;
	TCCR1B |= (1<< WGM12) | (1<<CS11) | (1<<CS10);
	TCNT1 = 0;
	OCR1A = 24999;
	TIMSK |= (1 << OCIE1A);
	*/
	OCR0 = 29; // interrupt at counter = 29
	TCCR0 |= (1<<CS20) | (1<<CS22); // prescaler = 128
	TCCR0 |= (1<<WGM21); // CTC mode. Reset counter when OCR2 is reached
	TCNT0 = 0x00; // initial counter value = 0
	TIMSK |= (1<< OCIE0);
	//TIMSK |= (1<<OCIE1A);   // Interrupt freischalten


	counter = 0;
	changed = 0;
}
SIGNAL(TIMER0_COMP_vect) {

	uint8_t tmp_c = 0x00;
	uint16_t layerb = internalMap[counter];
	PORTC = layerb;
	PORTA = (layerb>>8);

	/*tmp_c = (layerb>>8); // r will be reversed bits of v; first get LSB of v
	int s = sizeof(tmp_c) * 7; // extra shift needed at end

	for (layerb >>= 1; layerb; layerb >>= 1)
	{
		tmp_c <<= 1;
		tmp_c |= layerb & 1;
	  	s--;
	}
	tmp_c <<= s; // shift when v's highest bits are zero

	PORTA=tmp_c;*/
	if (counter == 0)
		cbi(PORTB, 3);
	else
		cbi(PORTB, counter - 1);
	sbi(PORTB, counter);
	if (counter == 3)
		counter = 0;
	else
		counter++;
}
void svoxel(uint8_t x, uint8_t y, uint8_t z) {// x-rechts y-hoch z -hinten
	switch (z) {
		case 3:
			internalMap[y] |= (1<<x);
			break;
		case 2:
			internalMap[y] |= (1<<x+4);
			break;
		case 1:
			internalMap[y] |= (1<<-x+15);
			break;
		case 0:
			internalMap[y] |= (1<<-x+11);
			break;
		default:
			break;
	}
}

void cvoxel(uint8_t x, uint8_t y, uint8_t z) {
	switch (z) {
		case 3:
			internalMap[y] &= ~(1<<x);
			break;
		case 2:
			internalMap[y] &= ~(1<<x+4);
			break;
		case 1:
			internalMap[y] &= ~(1<<-x+15);
			break;
		case 0:
			internalMap[y] &= ~(1<<-x+11);
			break;
		default:
			break;
	}
}

void clearcube() {
	for (uint8_t x = 0; x < 4 ; x++) {
		for (uint8_t y = 0; y < 4 ; y++) {
			for (uint8_t z = 0; z < 4 ; z++) {
				cvoxel(x,y,z);
			}
		}
	}
}

void firsteffect() {
	int x,y,z = 0;
	uint8_t direction = 1;
	while (1) {
		for (y = 0; y < 4 ; y++) {
			z=0;
			for (z=0; z<4; z++) {
				x=0;
				for (x = 0; x<4; x++) {
					if (direction==1) {
						svoxel(x,y,z);
					} else {
						cvoxel(x,y,z);
					}
					_delay_ms(45);

				}
			}
			//svoxel(x,y,z,0);
		}
		direction ^= 0b1;
	}
}


void snake(){
	uint8_t x,y,z, v;
	for (int direction = 0 ; direction < 6 ; direction++) {
		switch (direction) {
		case 0:
			x = 0;
			y = 0;
			z = 0;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					x++;
				} else if(v<6){
					y++;
				} else if(v<9){
					x--;
				} else if(v<11){
					y--;
				} else if(v < 13){
					x++;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					x--;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		case 1:
			x = 3;
			y = 0;
			z = 0;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					z++;
				} else if(v<6){
					y++;
				} else if(v<9){
					z--;
				} else if(v<11){
					y--;
				} else if(v < 13){
					z++;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					z--;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		case 2:
			x = 3;
			y = 0;
			z = 3;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					x--;
				} else if(v<6){
					y++;
				} else if(v<9){
					x++;
				} else if(v<11){
					y--;
				} else if(v < 13){
					x--;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					x++;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		case 3:
			x = 0;
			y = 0;
			z = 3;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					z--;
				} else if(v<6){
					y++;
				} else if(v<9){
					z++;
				} else if(v<11){
					y--;
				} else if(v < 13){
					z--;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					z++;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		case 4:
			x = 0;
			y = 0;
			z = 0;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					x++;
				} else if(v<6){
					y++;
				} else if(v<9){
					x--;
				} else if(v<11){
					y--;
				} else if(v < 13){
					x++;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					x--;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		case 5:
			x = 0;
			y = 0;
			z = 0;
			for(v = 0; v < 16; v++){
				svoxel(x,y,z);
				if(v<3){
					x++;
				} else if(v<6){
					y++;
				} else if(v<9){
					x--;
				} else if(v<11){
					y--;
				} else if(v < 13){
					x++;
				} else if(v < 14){
					y++;
				} else if(v < 16){
					x--;
				}
				_delay_ms(50);
			}
			clearcube();
			break;
		}
	_delay_ms(50);
	}
	clearcube();
}

void ebenen2(uint8_t type){
		for (int direction = 0 ; direction < 6 ; direction++) {
			for (uint_fast8_t zval = 0 ; zval < 4 ; zval++) {
				clearcube();
				for (uint8_t yval = 0; yval < 4 ; yval++) {
					for (uint8_t xval = 0; xval < 4 ; xval++) {
						switch (direction) {
						case 0:
							svoxel(3-xval,3-yval,3-zval);
							if(type >= 2){
								svoxel(zval,yval,zval);
							}
							break;
						case 1:
							svoxel(3-zval,3-xval,3-yval);
							if(type >= 2){
								svoxel(xval,zval,zval);
							}
							break;
						case 2:
							svoxel(3-yval,3-zval,3-xval);
							if(type >= 2){
								svoxel(zval,yval,yval);
							}
							break;
						case 3:
							svoxel(xval,yval,zval);
							if(type >= 2){
								svoxel(zval,xval,yval);
							}
							break;
						case 4:
							svoxel(zval,xval,yval);
							if(type >= 2){
								svoxel(yval,yval,zval);
							}
							break;
						case 5:
							svoxel(yval,zval,xval);
							if(type >= 2){
								svoxel(yval,yval,zval);
							}
							break;
						default:
							break;
						}

					}
				}
				_delay_ms(500);
			}
		}
		clearcube();
}

void ebenen() {
	uint8_t oldx, oldy, oldz =0;
	for (int direction = 0 ; direction < 6 ; direction++) {
		for (uint_fast8_t zval = 0 ; zval < 4 ; zval++) {
			clearcube();
			for (uint8_t yval = 0; yval < 4 ; yval++) {
				for (uint8_t xval = 0; xval < 4 ; xval++) {
					switch (direction) {
					case 1:
						svoxel(3-xval,3-yval,3-zval);
						break;
					case 3:
						svoxel(3-zval,3-xval,3-yval);
						break;
					case 5:
						svoxel(3-yval,3-zval,3-xval);
						break;
					case 0:
						svoxel(xval,yval,zval);
						break;
					case 2:
						svoxel(zval,xval,yval);
						break;
					case 4:
						svoxel(yval,zval,xval);
						break;
					default:
						break;
					}

				}
			}
			_delay_ms(200);
		}
	}
	clearcube();
}


void main(void) {

	ioinit();
	intInit();
	// intIntit();
	sei();
	clearcube();

	//ebenen2();
	while(1) {
		//snake();
		ebenen();
	}
}

#endif
