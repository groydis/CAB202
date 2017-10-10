#include <stdio.h>
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
#include "movement.h"
#include "levels.h"

bool main_menu_enabled = false;
bool game_running = false;
bool loading = false;

int player_lives = 3;
int current_floor = 0;
int player_score = 0;

bool hasKey = true;

char *flr_msg = "Floor: %02d";
char *lives_msg = "Lives: %02d";
char *score_msg = "Score: %02d";

Sprite the_monsters[4];
Sprite the_walls[6];

void turnOnLed0( void ) {
    //  (d) Set pin 2 of the Port B output register. No other pins should be 
    //  affected. 
    SET_BIT(PORTB, 2);
}

void turnOffLed0( void ) {
    //  (e) Clear pin 2 of the Port B output register. No other pins should be
    //  affected.
    CLEAR_BIT(PORTB, 2);
}

bool paused () {
	if (BIT_IS_SET(PINB, 0)) {
		return true;
	}
	return false;
}

void show_pause_screen() {
	clear_screen();

	sprintf(flr_msg, "Floor: %d", current_floor);
	sprintf(lives_msg, "Lives: %d", player_lives);
	sprintf(score_msg, "Score: %d", player_score);


    drawCentred(2, "P A U S E D");
	drawCentred(LCD_Y / 5 + 2, flr_msg);
	display_time(15,LCD_Y / 5 * 2 + 2);

	drawCentred(LCD_Y / 5 * 3 + 2, score_msg);
	drawCentred(LCD_Y / 5 * 4 + 2, lives_msg);

	show_screen(); 
}

void show_loading_screen() {
	clear_screen();
	current_floor++;

	sprintf(score_msg, "Score: %d", player_score);
	sprintf(flr_msg, "Floor: %d", current_floor);

	drawCentred(5, "L O A D I N G");
	drawCentred(15, score_msg);
	drawCentred(25, flr_msg);

	show_screen();
	load_level(current_floor);

	_delay_ms(1000);

	the_walls[0] = wall_across_1;
	the_walls[1] = wall_across_2;
	the_walls[2] = wall_across_3;
	the_walls[3] = wall_down_1;
	the_walls[4] = wall_down_2;
	the_walls[5] = wall_down_3;

	the_monsters[0] = monster;
	the_monsters[1] = monster1;
	the_monsters[2] = monster2;
	the_monsters[3] = monster3;

	loading = false;
	hasKey = false;
}

void show_main_menu() {
	char *countdown = "Ready?";
	int main_menu_counter = 11;
	int main_menu_countdown = 3;

	if (BIT_IS_SET(PINF, 5) || BIT_IS_SET(PINF, 6)) {

		while (main_menu_counter > 0) {

			main_menu_counter--;
			sprintf(countdown, "%d", main_menu_countdown);
			clear_screen();

			if (main_menu_counter % 3 == 0) {
				sprintf(countdown, "%d", main_menu_countdown);
				main_menu_countdown--;

				drawCentred(3, "ANSI-TOWER");
				drawCentred(LCD_Y / 4 + 2, "Greyden Scott");
				drawCentred(LCD_Y / 4 * 2 + 2, "N9935924");
				draw_string(LCD_X / 2, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);
			}

			_delay_ms(100);

			drawCentred(3, "ANSI-TOWER");
		    drawCentred(LCD_Y / 4 + 2, "Greyden Scott");
		    drawCentred(LCD_Y / 4 * 2 + 2, "N9935924");
		    draw_string(LCD_X / 2, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);

			show_screen();
		}

		main_menu_enabled = false;
		game_running = true;
		load_level(current_floor);
	}
	if (main_menu_enabled) {
		drawCentred(3, "ANSI-TOWER");
		drawCentred(LCD_Y / 4 + 2, "Greyden Scott");
		drawCentred(LCD_Y / 4 * 2 + 2, "N9935924");
		draw_string(28, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);
		show_screen();
	}
}

void setup() {
	// SET CLOCK SPEED
	set_clock_speed(CPU_8MHz);

	// SW2 SW3
    CLEAR_BIT(DDRF, 5);
    CLEAR_BIT(DDRF, 6);

    // LEFT RIGHT UP DOWN CENTER JOYSTICK
    CLEAR_BIT(DDRB, 1);
    CLEAR_BIT(DDRB, 7);
    CLEAR_BIT(DDRB, 0);
    CLEAR_BIT(DDRD, 0);
    CLEAR_BIT(DDRD, 1);

    // LED 
    SET_BIT(DDRB, 2);

	// BACKLIGHT
    CLEAR_BIT(DDRC, 7);
    SET_BIT(PORTC, 7);

    for (int i = 0; i < 8; i++) {
		CLEAR_BIT(PORTB, i);
	}

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

	clear_screen();

	show_screen();

	main_menu_enabled = true;
}

void monster_movement (Sprite sprite) {

}


void process() {
	clear_screen();

    if (collision(player, key)) {
    	hasKey = true;
    }
    if (collision(player, door) && hasKey) {
		loading = true;
	}
	if (current_floor == 0) {
		if (monster.is_visible) {
			if (monster.x < player.x){
		        monster.x += 0.1;
		    }
		    if (monster.x > player.x){
		        monster.x -= 0.1;
		    }
		    if (monster.y < player.y){
		        monster.y += 0.1;
		    }
		    if (monster.y > player.y){
		        monster.y -= 0.1;
		    }
		}
		if (collision(monster, tower)) {
			if (monster.x <= tower.x + tower.width) {
				monster.x += 0.1;
			} else if (monster.x + monster.width >= tower.x) {
				monster.x -= 0.1;
			}
			if (monster.y <= tower.y + tower.height) {
				monster.y += 0.1;
			} else if (monster.y + monster.height >= tower.y) {
				monster.y -= 0.1;
			}
		}
	} else {
		for (int i = 0; i < 6; i++) {
			if (monster.is_visible) {
				if (monster.x < player.x){
			        monster.x += 0.1;
			    }
			    if (monster.x > player.x){
			        monster.x -= 0.1;
			    }
			    if (monster.y < player.y){
			        monster.y += 0.1;
			    }
			    if (monster.y > player.y){
			        monster.y -= 0.1;
			    }
			}				
			if (collision(monster, the_walls[i])) {
				if (monster.x <= the_walls[i].x + the_walls[i].width) {
					monster.x += 0.1;
				} else if (monster.x + monster.width >= the_walls[i].x) {
					monster.x -= 0.1;
				}
				if (monster.y <= the_walls[i].y + the_walls[i].height) {
					monster.y += 0.1;
				} else if (monster.y + monster.height >= the_walls[i].y) {
					monster.y -= 0.1;
				}
			}
		}
		for (int i = 0; i < 6; i++) {
			if (monster1.is_visible) {
				if (monster1.x < player.x){
			        monster1.x += 0.1;
			    }
			    if (monster1.x > player.x){
			        monster1.x -= 0.1;
			    }
			    if (monster1.y < player.y){
			        monster1.y += 0.1;
			    }
			    if (monster1.y > player.y){
			        monster1.y -= 0.1;
			    }
			}				
			if (collision(monster1, the_walls[i])) {
				if (monster1.x <= the_walls[i].x + the_walls[i].width) {
					monster1.x += 0.1;
				} else if (monster1.x + monster1.width >= the_walls[i].x) {
					monster1.x -= 0.1;
				}
				if (monster1.y <= the_walls[i].y + the_walls[i].height) {
					monster1.y += 0.1;
				} else if (monster1.y + monster1.height >= the_walls[i].y) {
					monster1.y -= 0.1;
				}
			}
		}
		for (int i = 0; i < 6; i++) {
			if (monster2.is_visible) {
				if (monster2.x < player.x){
			        monster2.x += 0.1;
			    }
			    if (monster2.x > player.x){
			        monster2.x -= 0.1;
			    }
			    if (monster2.y < player.y){
			        monster2.y += 0.1;
			    }
			    if (monster2.y > player.y){
			        monster2.y -= 0.1;
			    }
			}				
			if (collision(monster2, the_walls[i])) {
				if (monster2.x <= the_walls[i].x + the_walls[i].width) {
					monster2.x += 0.1;
				} else if (monster2.x + monster2.width >= the_walls[i].x) {
					monster2.x -= 0.1;
				}
				if (monster2.y <= the_walls[i].y + the_walls[i].height) {
					monster2.y += 0.1;
				} else if (monster2.y + monster2.height >= the_walls[i].y) {
					monster2.y -= 0.1;
				}
			}
		}
		for (int i = 0; i < 6; i++) {
			if (monster3.is_visible) {
				if (monster3.x < player.x){
			        monster3.x += 0.1;
			    }
			    if (monster3.x > player.x){
			        monster3.x -= 0.1;
			    }
			    if (monster3.y < player.y){
			        monster3.y += 0.1;
			    }
			    if (monster3.y > player.y){
			        monster3.y -= 0.1;
			    }
			}				
			if (collision(monster3, the_walls[i])) {
				if (monster3.x <= the_walls[i].x + the_walls[i].width) {
					monster3.x += 0.1;
				} else if (monster3.x + monster3.width >= the_walls[i].x) {
					monster3.x -= 0.1;
				}
				if (monster3.y <= the_walls[i].y + the_walls[i].height) {
					monster3.y += 0.1;
				} else if (monster3.y + monster3.height >= the_walls[i].y) {
					monster3.y -= 0.1;
				}
			}
		}
	}
	move_sprites(current_floor, hasKey);
	show_screen();
}

int main () {
	setup_usb_serial();

	setup();

	while (main_menu_enabled) {
		show_main_menu();
	}

	while (game_running) {
		timer();
		if (paused()) {
			show_pause_screen();
		} else if (loading == true) {
			show_loading_screen();
		} else {
			process();
			_delay_ms(100);
		}
	}

	return 0;
}