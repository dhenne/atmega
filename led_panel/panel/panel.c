/*
 * panel.c
 *
 *  Created on: 05.03.2016
 *      Author: henne
 */

#include "panel.h"
#include "avr/iom32.h"


void display_image(pixel* _image[PANEL_SIZE_Y][PANEL_SIZE_X]){

	pixel* panel_one[PANEL_SIZE_Y/4][PANEL_SIZE_X];
	unsigned int current_y = 0;

	for(int k = 0; k < PANEL_SIZE_Y/4; k++){
		for(int i = 0; i < PANEL_SIZE_X; i++){
			panel_one[k][i] = _image[current_y][i];
		}
		current_y++;
	}

	pixel* panel_two[PANEL_SIZE_Y/4][PANEL_SIZE_X];

	for(int k = 0; k < PANEL_SIZE_Y/4; k++){
		for(int i = 0; i < PANEL_SIZE_X; i++){
			panel_two[k][i] = _image[current_y][i];
		}
		current_y++;
	}

	pixel* panel_three[PANEL_SIZE_Y/4][PANEL_SIZE_X];

	for(int k = 0; k < PANEL_SIZE_Y/4; k++){
		for(int i = 0; i < PANEL_SIZE_X; i++){
			panel_three[k][i] = _image[current_y][i];
		}
		current_y++;
	}

	pixel* panel_four[PANEL_SIZE_Y/4][PANEL_SIZE_X];

	for(int k = 0; k < PANEL_SIZE_Y/4; k++){
		for(int i = 0; i < PANEL_SIZE_X; i++){
			panel_four[k][i] = _image[current_y][i];
		}
		current_y++;
	}

	display_image_for_panel(0x01, panel_one);
	display_image_for_panel(0x02, panel_two);
	display_image_for_panel(0x03, panel_three);
	display_image_for_panel(0x04, panel_four);
};

void display_image_for_panel(uint8_t _panel_address, pixel* _image[PANEL_SIZE_Y/4][PANEL_SIZE_X]){
	// choose panel
	switch(_panel_address){
		case 0x01:
			cbi(PORTA, PA1);
			break;
		case 0x02:
			cbi(PORTA, PA2);
			break;
		case 0x03:
			cbi(PORTA, PA3);
			break;
		case 0x04:
			cbi(PORTA, PA4);
			break;
	}

	// send pixel information
	uint8_t row = 0x00;
	uint8_t column = 0x00;

	for(int y = 0; y < PANEL_SIZE_Y/4; y++){
		for(int x = 0; x < PANEL_SIZE_X; x++){
			pixel* cur_pixel = _image[y][x];

			//calculating memory address
			row = row + 0x02;

			spi_write_pixel(row, column, cur_pixel);
		}
		column = column + 1;
	}
};
