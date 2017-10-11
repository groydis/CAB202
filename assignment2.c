#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <lcd.h>
#include <macros.h>

#include <graphics.h>
#include <sprite.h>

#include "helpers.h"
#include "bitmaps.h"
#include "timer.h"
#include "lcd_model.h"

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

typedef struct 
{
    int x;
    int y;

} Location;

#define NUM_OF_MONSTERS 4
#define NUM_OF_WALLS_ACROSS 3
#define NUM_OF_WALLS_DOWN 3

Sprite player;
Sprite monster[NUM_OF_MONSTERS];
Sprite key;
Sprite door;
Sprite tower;

//Sprite shield;
//Sprite bomb;
//Sprite bowarrow;

Sprite border_top;
Sprite border_left;
Sprite border_right;
Sprite border_bottom;

Sprite wall_down[NUM_OF_WALLS_DOWN];

Sprite wall_across[NUM_OF_WALLS_ACROSS];

int rand_number( int min, int max)
{
	//srand(overflow_counter);
   	return rand() % (min - max + 1) + min;
}

void init_level_sprites () {
		sprite_init(&player, (LCD_X / 2) - playerWidthPixels / 2, (LCD_Y / 2), playerWidthPixels, playerHeightPixels, playerBitmaps);
	    
	    for (int i = 0; i < NUM_OF_MONSTERS; i++) {
	    	sprite_init(&monster[i], -10000, 0, monsterWidthPixels, monsterHeightPixels, monsterBitmaps);
	    }
		sprite_init(&key, 0, 0, keyWidthPixels, keyHeightPixels, keyBitmaps);
		sprite_init(&door, 0, 0, doorWidthPixels, doorHeightPixels, doorBitmaps);
		
		sprite_init(&tower, (LCD_X / 2) - towerWidthPixels / 2, -12, towerWidthPixels, towerHeightPixels, towerBitmaps);
		// BORDERS
		sprite_init(&border_top, -21, -12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
		sprite_init(&border_left, -21, -36, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
		sprite_init(&border_right, LCD_X + 21 - left_right_wallWidthPixels, -36, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
		sprite_init(&border_bottom, -21, LCD_Y + 12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);

		// WALLS FOR MAZE
		for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
			sprite_init(&wall_across[i], -10000, 0, wall_acrossWidthPixels, wall_acrossHeightPixels, wall_acrossBitmaps);

		}
		for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
			sprite_init(&wall_down[i], -10000, 0, wall_downWidthPixels, wall_downHeightPixels, wall_downBitmaps);
		}
}

Location random_door_location(int floor) {

	Location door_floor_0;
	Location door_floor_1;
	Location door_floor_2;
	Location door_floor_3;

	if (floor > 0) {
		door_floor_0.x = -21 + left_right_wallWidthPixels + 2;
		door_floor_0.y = -12 + top_bottom_wallHeightPixels +  2;

		door_floor_1.x = (LCD_X + 21) - door.width  - 4;
		door_floor_1.y = -12 + top_bottom_wallHeightPixels + 2;

		door_floor_2.x = -21 + left_right_wallWidthPixels +  2;
		door_floor_2.y = (LCD_Y + 12) - door.height - 2;

		door_floor_3.x = (LCD_X + 21) - door.width - 4;
		door_floor_3.y = (LCD_Y + 12) - door.height - 2;


		Location door_locations[4] = {door_floor_0, door_floor_1, door_floor_2, door_floor_3};

		return door_locations[rand_number(0,4)];

	}

	door_floor_0.x = (LCD_X / 2) - (doorWidthPixels / 2) + 3;
	door_floor_0.y = (towerHeightPixels - doorHeightPixels) - 12;
	return door_floor_0;
}

Location random_key_location(int floor) {

	Location key_floor_0;
	Location key_floor_1;
	Location key_floor_2;
	Location key_floor_3;

	if (floor > 0) {
		key_floor_0.x = LCD_X / 2;
		key_floor_0.y = -5;

		key_floor_1.x = LCD_X + 5;
		key_floor_1.y = LCD_Y / 2;

		key_floor_2.x = LCD_X / 2;
		key_floor_2.y = LCD_Y + 5;

		key_floor_3.x = -5;
		key_floor_3.y = LCD_Y / 2;

		Location key_locations[4] = {key_floor_0, key_floor_1, key_floor_2, key_floor_3};

		return key_locations[rand_number(0,4)];

	}

	key_floor_0.x = -10;
	key_floor_0.y = 0;
	return key_floor_0;
}

void load_level(int level) {
	if (level == 0) {
		Location door_loc = random_door_location(level);
		Location key_loc = random_key_location(level);

		player.x = (LCD_X / 2) - playerWidthPixels / 2;
		player.y = (LCD_Y / 2);

		key.x = key_loc.x;
		key.y = key_loc.y;
		door.x = door_loc.x;
		door.y = door_loc.y;

		monster[0].x = (LCD_X + 10) - monsterWidthPixels - 2;
		monster[0].y = 0;
				
		border_top.x = -21;
		border_top.y = -12;

		border_left.x = -21;
		border_left.y = -36;

		border_right.x = LCD_X + 21 - left_right_wallWidthPixels;
		border_right.y = - 36;

		border_bottom.x = -21;
		border_bottom.y = LCD_Y + 12;

	} else {
		Location door_loc = random_door_location(level);
		Location key_loc = random_key_location(level);

		player.x = (LCD_X / 2) - playerWidthPixels / 2;
		player.y = (LCD_Y / 2);

		key.x = key_loc.x;
		key.y = key_loc.y;
		door.x = door_loc.x;
		door.y = door_loc.y;

		border_top.x = -21;
		border_top.y = -12;

		border_left.x = -21;
		border_left.y = -36;

		border_right.x = LCD_X + 21 - left_right_wallWidthPixels;
		border_right.y = - 36;

		border_bottom.x = -21;
		border_bottom.y = LCD_Y + 12;

		int max_left = border_left.x + border_left.width + 2 + door.width + 2;
		int max_right = border_right.x - 2 - door.width - 2;
		int max_top = border_top.y + border_top.height + 2 + door.height + 2;
		int max_bottom = border_bottom.y - 2 - door.height - 2 - 2;

		int magical_random_map = rand_number(0, 10);
		if (magical_random_map == 0) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_top;

			wall_down[0].x = max_left;
			wall_down[0].y = wall_across[0].y;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_top;

			wall_down[1].x = max_right;
			wall_down[1].y = wall_across[1].y;

			wall_down[2].x = LCD_X / 2;
			wall_down[2].y = border_top.y + border_top.height;

			wall_across[2].x = wall_down[0].x;
			wall_across[2].y = wall_down[0].y + wall_downHeightPixels;
		} else if (magical_random_map == 1) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_bottom;

			wall_down[0].x = max_left;
			wall_down[0].y = wall_across[0].y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_top;

			wall_down[1].x = max_right;
			wall_down[1].y = wall_across[1].y;

			wall_down[2].x = LCD_X / 2;
			wall_down[2].y = border_bottom.y - wall_downHeightPixels;

			wall_across[2].x = wall_down[0].x;
			wall_across[2].y = wall_down[0].y;
		} else if (magical_random_map == 2) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_bottom;

			wall_down[0].x = max_left;
			wall_down[0].y = wall_across[0].y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_top;

			wall_down[1].x = wall_across[1].x;
			wall_down[1].y = wall_across[1].y;

			wall_down[2].x = LCD_X / 2;
			wall_down[2].y = border_bottom.y - wall_downHeightPixels;

			wall_across[2].x = wall_down[0].x;
			wall_across[2].y = wall_down[0].y;
		} else if (magical_random_map == 3) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_bottom;

			wall_down[0].x = max_left;
			wall_down[0].y = wall_across[0].y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_bottom;

			wall_down[1].x = wall_across[1].x;
			wall_down[1].y = wall_across[1].y - wall_downHeightPixels;

			wall_down[2].x = LCD_X / 2;
			wall_down[2].y = border_bottom.y - wall_downHeightPixels;

			wall_across[2].x = wall_down[0].x;
			wall_across[2].y = wall_down[0].y;
		} else if (magical_random_map == 4) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y =  max_bottom;

			wall_down[0].x = max_left;
			wall_down[0].y =  wall_across[0].y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y =  max_bottom;

			wall_down[1].x = wall_across[1].x;
			wall_down[1].y =  wall_across[1].y - wall_downHeightPixels;

			wall_down[2].x = LCD_X / 2;
			wall_down[2].y =  border_bottom.y - wall_downHeightPixels;

			wall_across[2].x = wall_down[1].x - wall_acrossWidthPixels;
			wall_across[2].y =  wall_down[1].y;
		} else if (magical_random_map == 5) {
			wall_down[0].x = max_left;
			wall_down[0].y = border_top.y + border_top.height;

			wall_across[0].x = wall_down[0].x;
			wall_across[0].y = wall_down[0].y + wall_downHeightPixels;

			wall_down[1].x = max_left;
			wall_down[1].y = border_bottom.y - wall_downHeightPixels;

			wall_across[1].x = wall_down[1].x;
			wall_across[1].y = wall_down[1].y;

			wall_down[2].x = max_right - wall_downWidthPixels;
			wall_down[2].y = border_top.y + border_top.height;

			wall_across[2].x = border_right.x - wall_acrossWidthPixels;
			wall_across[2].y = max_bottom;
		} else if (magical_random_map == 6) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y =  max_bottom;

			wall_down[0].x = max_left;
			wall_down[0].y =  wall_across[0].y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y =  max_top;

			wall_down[1].x = wall_across[0].x + wall_acrossWidthPixels - 3;
			wall_down[1].y =  wall_across[1].y;

			wall_down[2].x = LCD_X /2;
			wall_down[2].y =  border_top.y + border_top.height;

			wall_across[2].x = border_right.x - wall_acrossWidthPixels;
			wall_across[2].y =  max_bottom;
		} else if (magical_random_map == 7) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y =  max_bottom;

			wall_down[0].x = LCD_X / 2;
			wall_down[0].y =  border_top.y + border_top.height;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y =  max_bottom;

			wall_down[1].x = wall_across[1].x;
			wall_down[1].y =  wall_across[1].y - wall_downHeightPixels;

			wall_down[2].x = max_right;
			wall_down[2].y =  wall_across[1].y - wall_downHeightPixels;

			wall_across[2].x = border_left.x + border_left.width;
			wall_across[2].y =  max_top;
		} else if (magical_random_map == 8) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_bottom;

			wall_down[0].x = LCD_X / 2;
			wall_down[0].y = border_bottom.y - wall_downHeightPixels;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_top;

			wall_down[1].x = wall_across[1].x;
			wall_down[1].y = wall_across[1].y;

			wall_down[2].x = max_right;
			wall_down[2].y = wall_across[1].y;

			wall_across[2].x = border_left.x + border_left.width;
			wall_across[2].y = max_top;
		} else if (magical_random_map == 9) {
			wall_across[0].x = border_left.x + border_left.width;
			wall_across[0].y = max_top;

			wall_down[0].x = max_left;
			wall_down[0].y = wall_across[0].y;

			wall_across[1].x = border_right.x - wall_acrossWidthPixels;
			wall_across[1].y = max_top;

			wall_down[1].x = wall_across[0].x + wall_acrossWidthPixels - 3;
			wall_down[1].y = wall_across[1].y;

			wall_down[2].x = LCD_X /2;
			wall_down[2].y = border_top.y + border_top.height;

			wall_across[2].x = border_right.x - wall_acrossWidthPixels;
			wall_across[2].y = max_bottom;
		}

		monster[0].x = wall_across[0].x + (wall_acrossWidthPixels/ 2);
		monster[0].y = wall_across[0].y + 5;

		monster[1].x = wall_across[1].x + (wall_acrossWidthPixels/ 2);
		monster[1].y = wall_across[1].y - 8;

		monster[2].x = wall_across[2].x + (wall_acrossWidthPixels/ 2);
		monster[2].y = wall_across[2].y + 5;

		monster[3].x = rand_number(max_left, max_right);
		monster[3].y = border_bottom.y - 8;		
	}
}

void draw_level(int level) {
	if (level == 0) {

	    sprite_draw(&tower);
	    sprite_draw(&door);
	    sprite_draw(&border_top);
	    sprite_draw(&border_left);
	    sprite_draw(&border_right);
	    sprite_draw(&border_bottom);

	   	sprite_draw(&player);
	    sprite_draw(&monster[0]);
	    sprite_draw(&key);

	} else {

		sprite_draw(&border_top);
	    sprite_draw(&border_left);
	    sprite_draw(&border_right);
	    sprite_draw(&border_bottom);

	   	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
	    	sprite_draw(&wall_down[i]);
		}

	   	for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
	    	sprite_draw(&wall_across[i]);
		}

	    sprite_draw(&door);
	    sprite_draw(&key);
	    sprite_draw(&player);

	    for (int i = 0; i < NUM_OF_MONSTERS; i++) {
	    	sprite_draw(&monster[i]);
		}

	}
}

bool nearBottom (void) {
	if (border_bottom.y == LCD_Y - 2 && BIT_IS_SET(PINB, 7)) {
		return true;
	}
	return false;
}

bool nearTop( void ) {
	if (border_top.y + border_top.height == 2 && BIT_IS_SET(PIND, 1)) {
		return true;
	}
	return false;
}

bool nearLeft ( void ) {
	if (border_left.x + border_left.width == 2 && BIT_IS_SET(PINB, 1)) {
		return true;
	}
	return false;
}

bool nearRight ( void ) {
	if (border_right.x == LCD_X - 2 && BIT_IS_SET(PIND, 0)) {
		return true;
	}
	return false;
}

void movement( int level, bool hasKey ) {
	if (BIT_IS_SET(PIND, 1) && collision(player, border_top) == false) {

		if (nearTop() || nearBottom() || player.y != LCD_Y / 2) {
			
			player.y--;
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y++;
			}
			door.y++;

			border_top.y++;
			border_left.y++;
			border_right.y++;
			border_bottom.y++;

			if (level == 0) {
		    	monster[0].y++;
		    	tower.y++;
			} else {
				for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
					wall_down[i].y++;
				}
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					wall_across[i].y++;
				}
				for (int i = 0; i < NUM_OF_MONSTERS; i++) {
					monster[i].y++;
				}
			}
		}
	}
	else if (BIT_IS_SET(PINB, 7) && collision(player, border_bottom) == false) {

		if (nearBottom() || nearTop() || player.y != LCD_Y / 2) {
			
			player.y++;
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} 

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y--;
			}
	    	door.y--;

			border_top.y--;
			border_left.y--;
			border_right.y--;
			border_bottom.y--;

			if (level == 0) {
		    	monster[0].y--;
		    	tower.y--;
			} else {
				for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
					wall_down[i].y--;
				}
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					wall_across[i].y--;
				}
				for (int i = 0; i < NUM_OF_MONSTERS; i++) {
					monster[i].y--;
				}
			}
		}
	}
	else if (BIT_IS_SET(PINB, 1) && collision(player, border_left) == false) {

		if (nearLeft() || nearRight() || player.x != (LCD_X / 2) - playerWidthPixels / 2) {
			
			player.x--;
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x++;
			}
			door.x++;

			border_top.x++;
			border_left.x++;
			border_right.x++;
			border_bottom.x++;

			if (level == 0) {
		    	monster[0].x++;
		    	tower.x++;
			} else {
				for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
					wall_down[i].x++;
				}
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					wall_across[i].x++;
				}
				for (int i = 0; i < NUM_OF_MONSTERS; i++) {
					monster[i].x++;
				}
			}
		}
	}

	else if (BIT_IS_SET(PIND, 0) && collision(player, border_right) == false) {

		if (nearRight() || nearRight() || player.x != (LCD_X / 2) - playerWidthPixels / 2 ) {
			
			player.x++;
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x--;
			}
	    	door.x--;

			border_top.x--;
			border_left.x--;
			border_right.x--;
			border_bottom.x--;

			if (level == 0) {
		    	monster[0].x--;
		    	tower.x--;
			} else {
				for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
					wall_down[i].x--;
				}
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					wall_across[i].x--;
				}
				for (int i = 0; i < NUM_OF_MONSTERS; i++) {
					monster[i].x--;
				}
			}
		}
	}
}

void move_sprites( int current_floor, bool hasKey ) {
	if (current_floor == 0) {
    	movement(current_floor, hasKey);
	} else {
		movement(current_floor, hasKey);
	}
	draw_level(current_floor);
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

	_delay_ms(2000);

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
	init_level_sprites();
	main_menu_enabled = true;
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
		if (monster[0].is_visible) {
			if (monster[0].x < player.x){
		        monster[0].x += 0.1;
		    }
		    if (monster[0].x > player.x){
		        monster[0].x -= 0.1;
		    }
		    if (monster[0].y < player.y){
		        monster[0].y += 0.1;
		    }
		    if (monster[0].y > player.y){
		        monster[0].y -= 0.1;
		    }
		}
		if (collision(monster[0], tower)) {
			if (monster[0].x <= tower.x + tower.width) {
				monster[0].x += 0.1;
			} else if (monster[0].x + monster[0].width >= tower.x) {
				monster[0].x -= 0.1;
			}
			if (monster[0].y <= tower.y + tower.height) {
				monster[0].y += 0.1;
			} else if (monster[0].y + monster[0].height >= tower.y) {
				monster[0].y -= 0.1;
			}
		}
	} else {
		for (int j = 0; j < NUM_OF_MONSTERS; j++) {
			if (monster[j].is_visible) {
				if (monster[j].x < player.x){
			        monster[j].x += 0.1;
			    }
			    if (monster[j].x > player.x){
			        monster[j].x -= 0.1;
			    }
			    if (monster[j].y < player.y){
			        monster[j].y += 0.1;
			    }
			    if (monster[j].y > player.y){
			        monster[j].y -= 0.1;
			    }
			}
			for (int i = 0; i < 3; i++) {				
				if (collision(monster[j], wall_across[i])) {
					if (monster[j].x <= wall_across[i].x + wall_across[i].width) {
						monster[j].x += 0.1;
					} else if (monster[j].x + monster[j].width >= wall_across[i].x) {
						monster[j].x -= 0.1;
					}
					if (monster[j].y <= wall_across[i].y + wall_across[i].height) {
						monster[j].y += 0.1;
					} else if (monster[j].y + monster[j].height >= wall_across[i].y) {
						monster[j].y -= 0.1;
					}
				}
				if (collision(monster[j], wall_down[i])) {
					if (monster[j].x <= wall_down[i].x + wall_down[i].width) {
						monster[j].x += 0.1;
					} else if (monster[j].x + monster[j].width >= wall_down[i].x) {
						monster[j].x -= 0.1;
					}
					if (monster[j].y <= wall_down[i].y + wall_down[i].height) {
						monster[j].y += 0.1;
					} else if (monster[j].y + monster[j].height >= wall_down[i].y) {
						monster[j].y -= 0.1;
					}
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