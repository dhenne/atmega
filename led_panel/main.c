/*
 * main.c
 *
 *  Created on: 04.03.2016
 *      Author: henne
 */
#include "common.h"
#include "panel/panel.h"
#include "pixel_information/pixel.h"
#include "../uart-ttl/uart.h"
#include "../uart-ttl/fifo.h"
#include <time.h>
#include <stdlib.h>
#include "spi/spi.h"

void test_richard() {
	pixel* image[PANEL_SIZE_Y][PANEL_SIZE_X];

		for(int y = 0; y < PANEL_SIZE_Y; y++){
			for(int x = 0; x < PANEL_SIZE_Y; x++){
				pixel cur_pixel;
				cur_pixel.dim = rand() % (255 + 1 - 0) + 0;
				cur_pixel.red = 0;
				cur_pixel.yellow = 0;
				cur_pixel.blue = 120 + (rand() % (125 + 1 - 0) + 0);
				image[y][x] = &cur_pixel;
			}
		}

		display_image(image);
}

int main (void)
{
	srand(time(NULL));
	uart_init();
	//stdout = &uart_stdout;
	init_display();
	test_light_entire_display();
	while (1);
    return 0;
}
