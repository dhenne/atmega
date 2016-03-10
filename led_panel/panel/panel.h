/*
 * panel.h
 *
 *  Created on: 05.03.2016
 *      Author: henne
 */

#ifndef PANEL_PANEL_H_
#define PANEL_PANEL_H_

#include <stdbool.h>
#include "../pixel_information/pixel.h"
#include "../common.h"


void display_image(pixel* _image[PANEL_SIZE_Y][PANEL_SIZE_X]);
void display_image_for_panel(uint8_t _panel_nr, pixel* _image[PANEL_SIZE_Y/4][PANEL_SIZE_X]);

#endif /* PANEL_PANEL_H_ */
