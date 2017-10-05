#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <lcd.h>
#include <macros.h>

#include <graphics.h>
#include <sprite.h>

#include "helpers.h"
#include "timer.h"
#include "lcd_model.h"
#include "usb_serial.h"
#include "game_screens.h"
#include "controller.h"
#include "levels.h"
#include "movement.h"

bool paused = false;
bool main_menu_enabled = false;
bool game_running = false;

int player_lives = 3;
int current_floor = 0;
int player_score = 0;

bool hasKey;

void show_game_screen( void ) {

}

void show_pause_screen( void ) {
	clear_screen();

	draw_pause_screen(current_floor, player_lives, player_score);

	show_screen(); 
}

void show_main_menu( void ) {
	char *countdown = "Ready?";
	int main_menu_counter = 11;
	int main_menu_countdown = 3;

	if (sw2_pressed() || sw3_pressed()) {

		while (main_menu_counter > 0) {

			main_menu_counter--;
			sprintf(countdown, "%d", main_menu_countdown);
			clear_screen();

			if (main_menu_counter % 3 == 0) {
				sprintf(countdown, "%d", main_menu_countdown);
				main_menu_countdown--;
				draw_main_menu(countdown, LCD_X / 2);
			}

			_delay_ms(100);
			draw_main_menu(countdown, LCD_X / 2);
			show_screen();
		}

		main_menu_enabled = false;
		game_running = true;
		load_level(current_floor);
	}
	if (main_menu_enabled) {
		draw_main_menu(countdown, 28);
		show_screen();
	}
}

void setup( void ) {
	//sendLine("Running Setup");
	// SET CLOCK SPEED
	set_clock_speed(CPU_8MHz);

	init_controllers();

	// BACKLIGHT
    CLEAR_BIT(DDRC, 7);
    SET_BIT(PORTC, 7);

    // INIT THE LCD
    lcd_init(LCD_DEFAULT_CONTRAST);

   	// Timer 0 in normal mode, with pre-scaler 1024 ==> ~30Hz overflow.
	// Timer overflow on.
	TCCR0A = 0;
	TCCR0B = 5; 
	TIMSK0 = 1; 

	// Enable interrupts globally.
	sei();
	
	// Initialise the USB serial
    usb_init();

	clear_screen();

	show_screen();

	main_menu_enabled = true;
	//sendLine("Setup Complete");
}

void process( void ) {
	draw_level(current_floor);

	if (BIT_IS_SET(PINB, 0)) {
		paused = true;
	}
	
	paused = false;
	if (collision(player, key)) {
		hasKey = true;
	}
	movement(current_floor, hasKey);

	if (paused == false) {
		clear_screen();
		draw_level(current_floor);
		show_screen();
	}

	
}

int main ( void ) {
	setup_usb_serial();

	setup();

	while (main_menu_enabled) {
		show_main_menu();
	}

	while (game_running) {
		timer();
		if (paused) {
			show_pause_screen();
		} else {
			process();
			_delay_ms(10);
		}
	}

	return 0;
}