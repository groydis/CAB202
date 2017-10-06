#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <graphics.h>
#include <sprite.h>
#include "usb_serial.h"
#include "lcd_model.h"

typedef enum { false, true } bool;

char buffer[20];

char *TestChar = "Test\r\n";

bool debug = false;

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

bool monster_visble(Sprite monster) {
	if (monster.x <= 83 && monster.x >= 0) {
		if (monster.y <= 47 && monster.y >= 0) {
			return true;
		}
	}
	return false;
}

void draw_double(uint8_t x, uint8_t y, double time, double minutes, colour_t colour) {
    snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, time);
    draw_string(x, y, buffer, colour);
}

void drawCentred(unsigned char y, char* string) {
	unsigned char l = 0, i = 0;
	while (string[i] != '\0') { l++; i++; }
	char x = 42-(l*5/2);
	draw_string((x > 0) ? x : 0, y, string, FG_COLOUR);
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

void send_debug_data() {

}

void setup_usb_serial(void) {

	// Set up LCD and display message
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_string(10, 10, "Connect USB...", FG_COLOUR);
	show_screen();

	usb_init();
	if (debug == true) {
		while ( !usb_configured() || !usb_serial_get_control()){
			// Block until USB is ready.
		}
	} else {
		while ( !usb_configured()){
			// Block until USB is ready.
		}
	}

	clear_screen();
	draw_string(10, 10, "USB connected", FG_COLOUR);
	show_screen();
}

#endif