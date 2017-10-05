#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <graphics.h>

#include <graphics.h>
#include <sprite.h>
#include "usb_serial.h"

typedef enum { false, true } bool;

char buffer[20];

char *TestChar = "Test\r\n";

bool collision(Sprite spriteOne, Sprite spriteTwo) {
	int spriteOne_x = spriteOne.x + 1;
	int spriteOne_y = spriteOne.y + 1;

	int spriteTwo_x = spriteTwo.x;
	int spriteTwo_y = spriteTwo.y;

	int spriteOne_w = spriteOne.width;
	int spriteOne_h = spriteOne.height;

	int spriteTwo_w = spriteTwo.width;
	int spriteTwo_h = spriteTwo.height;

	int collided = true;

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

void draw_double(uint8_t x, uint8_t y, double time, double minutes, colour_t colour) {
    snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, time);
    draw_string(x, y, buffer, colour);
}

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

void setup_usb_serial(void) {

	// Set up LCD and display message
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_string(10, 10, "Connect USB...", FG_COLOUR);
	show_screen();

	usb_init();
	// || !usb_serial_get_control()
	while ( !usb_configured()){
		// Block until USB is ready.
	}

	clear_screen();
	draw_string(10, 10, "USB connected", FG_COLOUR);
	show_screen();
}

#endif