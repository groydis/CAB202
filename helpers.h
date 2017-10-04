#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <graphics.h>

#include <graphics.h>
#include <sprite.h>
#include "usb_serial.h"

typedef enum { false, true } bool;

char buffer[20];

int collision(Sprite spriteOne, Sprite spriteTwo) {
	int spriteOne_x = spriteOne.x;
	int spriteOne_y = spriteOne.y;

	int spriteTwo_x = spriteTwo.x;
	int spriteTwo_y = spriteTwo.y;

	int spriteOne_w = spriteOne.width;
	int spriteOne_h = spriteOne.height;

	int spriteTwo_w = spriteTwo.width;
	int spriteTwo_h = spriteTwo.height;
	// 1 = TRUE
	// 0 = FALSE
	int collided = 1;

	if ( spriteTwo_x >= spriteOne_x + spriteOne_w ) {
		collided = 0; 
	}
	if ( spriteTwo_y >= spriteOne_y + spriteOne_h  ) {
		collided = 0;
	}
	if ( spriteOne_x >= spriteTwo_x + spriteTwo_w ) {
		collided = 0;
	}
	if ( spriteOne_y >= spriteTwo_y + spriteTwo_h ) {
		collided = 0;
	}

	return collided;
}

void draw_double(uint8_t x, uint8_t y, double time, double minutes, colour_t colour) {
    snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, time);
    draw_string(x, y, buffer, colour);
}

//--------------------------------------------------
// Helper Functions
//--------------------------------------------------

//int potPosition( void ) {
//	uint16_t adc = adc_read(1);
//	
//	float max_adc = 1023.0;
//	long max_lcd_adc = (adc*(long)(LCD_X - 12)) / max_adc;
//	
//	return max_lcd_adc + 2;
//}

void sendLine(char* string) {
    // Send all of the characters in the string
    unsigned char char_count = 0;
    while (*string != '\0') {
        usb_serial_putchar(*string);
        string++;
        char_count++;
    }

    // Go to a new line (force this to be the start of the line)
    usb_serial_putchar('\r');
    usb_serial_putchar('\n');
}

#endif