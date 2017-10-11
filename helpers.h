#ifndef HELPERS_H_
#define HELPERS_H_

#include "usb_serial.h"

typedef enum { false, true } bool;

char buffer[20];

char *TestChar = "Test\r\n";

bool collision(Sprite spriteOne, Sprite spriteTwo) {
	int spriteOne_x = spriteOne.x;
	int spriteOne_y = spriteOne.y;

	int spriteTwo_x = spriteTwo.x;
	int spriteTwo_y = spriteTwo.y;

	int spriteOne_w = spriteOne.width;
	int spriteOne_h = spriteOne.height;

	int spriteTwo_w = spriteTwo.width;
	int spriteTwo_h = spriteTwo.height;

	if (spriteTwo.is_visible && spriteOne.is_visible) {
		bool collided = true;

		if ( spriteTwo_x >= spriteOne_x + spriteOne_w ) {
			collided = false; 
		}
		if ( spriteTwo_y >= spriteOne_y + spriteOne_h  ) {
			collided = false;
		}
		if ( spriteOne_x >= spriteTwo_x + spriteTwo_w ) {
			collided = false;
		}
		if ( spriteOne_y >= spriteTwo_y + spriteTwo_h ) {
			collided = false;
		}
		return collided;
	}

	return false;
}


void draw_double(uint8_t x, uint8_t y, double time, double minutes, colour_t colour) {
    snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, time);
    draw_string(x, y, buffer, colour);
}

#endif