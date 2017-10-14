#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>
#include <graphics.h>
#include <sprite.h>

#include "usb_serial.h"
#include "lcd_model.h"
#include "bitmaps.h"

#define NUM_OF_MONSTERS 5
#define NUM_OF_TREASURE 5
#define NUM_OF_WALLS_ACROSS 3
#define NUM_OF_WALLS_DOWN 3

typedef enum 
{ 
	false, true
} bool;

typedef enum 
{
	left, right, bottom, top, none
} direction;

typedef struct 
{
    int x;
    int y;

} Location;

bool main_menu_enabled = false;
bool game_running = false;
bool loading = false;
bool game_over = false;

int player_lives = 3;
int current_floor = 0;
int player_score = 0;

bool hasKey = false;
bool hasShield = false;
bool hasBowArrow = false;
bool hasBomb = false;

bool can_move_up = true;
bool can_move_down = true;
bool can_move_left = true;
bool can_move_right = true;

char *flr_msg = "Floor: %02d";
char *lives_msg = "Lives: %02d";
char *score_msg = "Score: %02d";
char *debug_msg = "Time[%f] | Score: %d | Floor: :%d | Player X/Y : (%.2f/%.2f) | Lives: %d";
char *test_msg = "";

Sprite player;
Sprite monster[NUM_OF_MONSTERS];
Sprite key;
Sprite door;
Sprite tower;
Sprite shield;
Sprite bomb;
Sprite bowarrow;
Sprite treasure[NUM_OF_TREASURE];
Sprite border_top;
Sprite border_left;
Sprite border_right;
Sprite border_bottom;
Sprite wall_down[NUM_OF_WALLS_DOWN];
Sprite wall_across[NUM_OF_WALLS_ACROSS];

sprite_id all_sprite_store[] = {
	&monster[0],
	&monster[1],
	&monster[2],
	&monster[3],
	&monster[4],
	&key,
	&door,
	&tower,
	&shield,
	&bomb,
	&bowarrow,
	&treasure[0],
	&treasure[1],
	&treasure[2],
	&treasure[3],
	&treasure[4],
	&border_top,
	&border_left,
	&border_right,
	&border_bottom,
	&wall_down[0],
	&wall_down[1],
	&wall_down[2],
	&wall_across[0],
	&wall_across[1],
	&wall_across[2]
};


// ---------------------------------------------------------
//	GAME SCREEN FUNCTIONS
// ---------------------------------------------------------
bool paused ( void );
void show_pause_screen( void );
void show_game_over_screen( void );
void show_loading_screen( void );
void show_main_menu( void );

// ---------------------------------------------------------
//	LEVEL CREATION FUNCTIONS
// ---------------------------------------------------------
void load_level( void );
void generate_random_level( void );
Location place_item( Sprite sprite_to_place );
Location random_door_location( void );

// ---------------------------------------------------------
//	SPRITE FUNCTIONS
// ---------------------------------------------------------
void initialise_sprites( void );
void initialise_treasure( void );
void initialise_monsters( void );
void initialise_borders( void );
void initialise_walls ( void );
void draw_sprites( void );

// ---------------------------------------------------------
//	MOVEMENT FUCNTIONS
// ---------------------------------------------------------
void player_movement( void );
void monster_movement( void );
bool near_bottom( void );
bool near_top( void );
bool near_left( void );
bool near_right( void );
bool collision_from_top (Sprite sprite_collided);
bool collision_from_bottom (Sprite sprite_collided);
bool collision_from_left (Sprite sprite_collided);
bool collision_from_right (Sprite sprite_collided);

void handle_collision( void );
// ---------------------------------------------------------
//	HELPER FUNCTIONS
// ---------------------------------------------------------
void draw_centred( unsigned char y, char* string );
int random_number( int min, int max );
bool collision( Sprite* sprite_one, Sprite* sprite_two );
direction collision_direction ( Sprite* sprite_one, Sprite* sprite_two );
bool visability_check( Sprite* sprite_one );
// ---------------------------------------------------------
//	TIME FUCNTIONS
// ---------------------------------------------------------
float current_time( void );
void display_time( int x, int y );
float get_system_time( void );

// ---------------------------------------------------------
//	USB SERIAL FUCNTIONS
// ---------------------------------------------------------
void setup_usb_serial( void );
void usb_serial_send( char * message );

// ---------------------------------------------------------
//	SPRITE FUNCTIONS
// ---------------------------------------------------------
void initialise_sprites( void ) {
	sprite_init(&player, (LCD_X / 2) - playerWidthPixels / 2, (LCD_Y / 2), playerWidthPixels, playerHeightPixels, playerBitmaps);
	sprite_init(&key, 0, 0, keyWidthPixels, keyHeightPixels, keyBitmaps);
	sprite_init(&door, 0, 0, doorWidthPixels, doorHeightPixels, doorBitmaps);
	sprite_init(&tower, (LCD_X / 2) - towerWidthPixels / 2, -12, towerWidthPixels, towerHeightPixels, towerBitmaps);
	sprite_init(&shield, -10000, 0, shieldWidthPixels, shieldHeightPixels, shieldBitmaps);
	sprite_init(&bomb, -10000, 0, bombWidthPixels, bombHeightPixels, bombBitmaps);
	sprite_init(&bowarrow, -10000, 0, bowarrowWidthPixels, bowarrowHeightPixels, bowarrowBitmaps);
	initialise_treasure();
	initialise_monsters();
	initialise_borders();
	initialise_walls();
}

void initialise_treasure( void ) {
	for (int i = 0; i < NUM_OF_TREASURE; i++) {
		sprite_init(&treasure[i], -100000, 0, treasureWidthPixels, treasureHeightPixels, treasureBitmaps);
	}
}

void initialise_monsters( void ) {
	for (int i = 0; i < NUM_OF_MONSTERS; i++) {
    	sprite_init(&monster[i], -10000, 0, monsterWidthPixels, monsterHeightPixels, monsterBitmaps);
    }
}

void initialise_borders ( void ) {
		sprite_init(&border_top, -21, -12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
		sprite_init(&border_left, -21, -36, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
		sprite_init(&border_right, LCD_X + 21 - left_right_wallWidthPixels, -36, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
		sprite_init(&border_bottom, -21, LCD_Y + 12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
}

void initialise_walls ( void ) {
	for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
		sprite_init(&wall_across[i], -10000, 0, wall_acrossWidthPixels, wall_acrossHeightPixels, wall_acrossBitmaps);

	}
	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		sprite_init(&wall_down[i], -10000, 0, wall_downWidthPixels, wall_downHeightPixels, wall_downBitmaps);
	}
}

void draw_sprites( void ) {

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
	for (int i = 0; i < NUM_OF_MONSTERS; i++) {
    	sprite_draw(&monster[i]);
	}
	for (int i = 0; i < NUM_OF_TREASURE; i++) {
    	sprite_draw(&treasure[i]);
	}

	sprite_draw(&shield);
	sprite_draw(&bowarrow);
	sprite_draw(&bomb);

	sprite_draw(&tower);

	sprite_draw(&door);
	sprite_draw(&key);
	sprite_draw(&player);
}

// ---------------------------------------------------------
//	MOVEMENT FUNCTIONS
// ---------------------------------------------------------

void player_movement( void ) {
	// UP 
	if (BIT_IS_SET(PIND, 1) && collision(&player, &border_top) == false && collision_from_bottom(player) == false) 
	{
		if (near_top() || near_bottom() || player.y != LCD_Y / 2) 
		{
			
			player.y--;

			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			}
		} 
		else 
		{
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y++;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			} else {
				shield.y++;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} else {
				bowarrow.y++;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			} else {
				bomb.y++;
			}
			for (int i = 0; i < NUM_OF_TREASURE; i++) {
				treasure[i].y++;
			}
			for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
				wall_down[i].y++;
			}
			for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
				wall_across[i].y++;
			}
			for (int i = 0; i < NUM_OF_MONSTERS; i++) {
				monster[i].y++;
			}
			tower.y++;
			door.y++;
			border_top.y++;
			border_left.y++;
			border_right.y++;
			border_bottom.y++;
		}
	}
	// DOWN	
	else if (BIT_IS_SET(PINB, 7) && collision(&player, &border_bottom) == false && collision_from_top(player) == false) {

		if (near_bottom() || near_top() || player.y != LCD_Y / 2) {
			
			player.y++;

			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} 
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y--;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			} else {
				shield.y--;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} else {
				bowarrow.y--;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			} else {
				bomb.y--;
			}
			for (int i = 0; i < NUM_OF_TREASURE; i++) {
				treasure[i].y--;
			}
			for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
				wall_down[i].y--;
			}
			for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
				wall_across[i].y--;
			}
			for (int i = 0; i < NUM_OF_MONSTERS; i++) {
				monster[i].y--;
			}
			tower.y--;
			door.y--;
			border_top.y--;
			border_left.y--;
			border_right.y--;
			border_bottom.y--;
		}
	}
	// LEFT
	else if (BIT_IS_SET(PINB, 1) && collision(&player, &border_left) == false && collision_from_right(player) == false) {

		if (near_left() || near_right() || player.x != (LCD_X / 2) - playerWidthPixels / 2) {
			
			player.x--;

			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} 
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x++;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			} else {
				shield.x++;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} else {
				bowarrow.x++;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			} else {
				bomb.x++;
			}
			for (int i = 0; i < NUM_OF_TREASURE; i++) {
				treasure[i].x++;
			}
			for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
				wall_down[i].x++;
			}
			for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
				wall_across[i].x++;
			}
			for (int i = 0; i < NUM_OF_MONSTERS; i++) {
				monster[i].x++;
			}
			tower.x++;
			door.x++;
			border_top.x++;
			border_left.x++;
			border_right.x++;
			border_bottom.x++;
		}
	}
	// RIGHT
	else if (BIT_IS_SET(PIND, 0) && collision(&player, &border_right) == false && collision_from_left(player) == false) {

		if (near_left() || near_right() || player.x != (LCD_X / 2) - playerWidthPixels / 2 ) {
			
			player.x++;

			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} 
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x--;
			}
			if (hasShield) {
				shield.y = player.y + player.height - 4;
				shield.x = player.x + player.width + 2;
			} else {
				shield.x--;
			}
			if (hasBowArrow) {
				bowarrow.y = player.y + player.height - 4;
				bowarrow.x = player.x + player.width + 2;
			} else {
				bowarrow.x--;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height - 4;
				bomb.x = player.x + player.width + 2;
			} else {
				bomb.x--;
			}
			for (int i = 0; i < NUM_OF_TREASURE; i++) {
				treasure[i].x--;
			}
			for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
				wall_down[i].x--;
			}
			for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
				wall_across[i].x--;
			}
			for (int i = 0; i < NUM_OF_MONSTERS; i++) {
				monster[i].x--;
			}
			tower.x--;
			door.x--;
			border_top.x--;
			border_left.x--;
			border_right.x--;
			border_bottom.x--;
		}
	}
}
void monster_movement( void ) {
	for (int j = 0; j < NUM_OF_MONSTERS; j++) {
		if (visability_check(&monster[j])) {
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
	}
}

bool near_bottom ( void ) {
	if (border_bottom.y == LCD_Y - 2 && BIT_IS_SET(PINB, 7)) {
		return true;
	}
	return false;
}

bool near_top( void ) {
	if (border_top.y + border_top.height == 2 && BIT_IS_SET(PIND, 1)) {
		return true;
	}
	return false;
}

bool near_left ( void ) {
	if (border_left.x + border_left.width == 2 && BIT_IS_SET(PINB, 1)) {
		return true;
	}
	return false;
}

bool near_right ( void ) {
	if (border_right.x == LCD_X - 2 && BIT_IS_SET(PIND, 0)) {
		return true;
	}
	return false;
}

bool collision_from_top (Sprite sprite_collided) {
	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
			if (collision(&sprite_collided, &wall_down[i]) || collision(&sprite_collided, &wall_across[j])) {
				if(collision_direction(&sprite_collided, &wall_down[i]) == top || collision_direction(&sprite_collided, &wall_across[j]) == top) {
					return true;
				} else {
					return false;
				}
			}
		}
	}
	return false;
}
bool collision_from_bottom (Sprite sprite_collided) {
	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
			if (collision(&sprite_collided, &wall_down[i]) || collision(&sprite_collided, &wall_across[j])) {
				if(collision_direction(&sprite_collided, &wall_down[i]) == bottom || collision_direction(&sprite_collided, &wall_across[j]) == bottom) {
					return true;
				} else {
					return false;
				}
			}
		}
	}
	return false;
}
bool collision_from_left (Sprite sprite_collided) {
	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
			if (collision(&sprite_collided, &wall_down[i]) || collision(&sprite_collided, &wall_across[j])) {
				if(collision_direction(&sprite_collided, &wall_down[i]) == left || collision_direction(&sprite_collided, &wall_across[j]) == left) {
					return true;
				} else {
					return false;
				}
			}
		}
	}
	return false;
}
bool collision_from_right (Sprite sprite_collided) {
	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
			if (collision(&sprite_collided, &wall_down[i]) || collision(&sprite_collided, &wall_across[j])) {
				if(collision_direction(&sprite_collided, &wall_down[i]) == right || collision_direction(&sprite_collided, &wall_across[j]) == right) {
					return true;
				} else {
					return false;
				}
			}
		}
	}
	return false;	
}
void handle_collision( void ) {
	for (int x = 0; x < NUM_OF_MONSTERS; x++) {
		if (visability_check(&monster[x])) {
			for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
				for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
					if (collision(&monster[x], &wall_down[i]) || collision(&monster[x], &wall_across[j])) {
						if (collision_from_top(monster[x])) {
							monster[x].y--;
						}
						if (collision_from_bottom(monster[x])) {
							monster[x].y++;
						}
						if (collision_from_left(monster[x])) {
							monster[x].x--;
						}
						if (collision_from_right(monster[x])) {
							monster[x].x++;
						}
					}
				}
			}
		
			if (collision(&monster[x], &tower)) {
				if (collision_direction(&monster[x], &tower) == top) {
					monster[x].y--;
				}
				if (collision_direction(&monster[x], &tower) == bottom) {
					monster[x].y++;
				}
				if (collision_direction(&monster[x], &tower) == left) {
					monster[x].x--;
				}
				if (collision_direction(&monster[x], &tower) == right) {
					monster[x].x++;
				}
			}
			if (collision(&player, &monster[x])) {
				// TODO: Recude Lives and Die;
				usb_serial_send("Player Collided With Monster.");
			}
		}
	}
	for (int i = 0; i < NUM_OF_TREASURE; i++) {
		if (visability_check(&treasure[i])) {
			if (collision(&player, &treasure[i])) {
				player_score += 10;
				treasure[i].x = -1000;
				usb_serial_send("Player Collected Treasure.");
			}
		}
	}
	if (visability_check(&key)) {
		if (collision(&player, &key)) {
			hasKey = true;
			//usb_serial_send("Player Collected The Key.");
		}
	}
	if (visability_check(&door)) {
		if (collision(&player, &door) && hasKey) {
			loading = true;
			player_score +=100;
			usb_serial_send("Player Used The Key On The Door To Proceed To The Next Floor.");
		}
	}
	if (visability_check(&tower)) {
		if (collision(&player, &tower)) {
			if (collision_direction(&player, &tower) == bottom) {
				player.y++;
			}
			if (collision_direction(&player, &tower) == left) {
				player.x--;
			}
			if (collision_direction(&player, &tower) == right) {
				player.x++;
			}
		}
	}
	if (visability_check(&shield) && !hasShield) {
		if (collision(&player, &shield)) {
			hasShield = true;
			hasBomb = false;
			hasBowArrow = false;
			usb_serial_send("Player Has Picked Up A Shield.");
		}
	}
	if (visability_check(&bowarrow) && !hasBowArrow) {
		if (collision(&player, &bowarrow)) {
			hasShield = false;
			hasBomb = false;
			hasBowArrow = true;
			usb_serial_send("Player Has Picked Up A Bow & Arrow.");
		}
	}
	if (visability_check(&bomb)) {
		if (collision(&player, &bomb) && !hasBomb) {
			hasShield = false;
			hasBomb = true;
			hasBowArrow = false;
			usb_serial_send("Player Has Picked Up A Bomb.");
		}
	}
}

// ---------------------------------------------------------
//	AWW YEAH
// ---------------------------------------------------------

void setup( void ) {
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

	// Set Timer 0 to overflow approx 122 times per second.
	TCCR0B |= 4;
	TIMSK0 = 1;

    TCCR1A = 0;
    TCCR1B = 4;
    TIMSK1 = 1;

	// Enable interrupts.
	sei();

	setup_usb_serial();

	initialise_sprites();

	main_menu_enabled = true;
}

void process(void) {
	clear_screen();
	monster_movement();
	player_movement();
	handle_collision();
	draw_sprites();
	show_screen();
}

int main(void) {

	setup();

	while (main_menu_enabled) {
		show_main_menu();
	}

	while (game_running) {
		if (paused()) {
			show_pause_screen();
		} else if (game_over) {
			show_game_over_screen();
		}else if (loading) {
			show_loading_screen();
		} else {
			process();
			//_delay_ms(100);
		}
	}

	return 0;
}

// ---------------------------------------------------------
//	GAME SCREENS
// ---------------------------------------------------------
bool paused ( void ) {
	if (BIT_IS_SET(PINB, 0)) {
		return true;
	}
	return false;
}

void show_pause_screen( void ) {
	clear_screen();

	sprintf(flr_msg, "Floor: %0d", current_floor);
	sprintf(lives_msg, "Lives: %0d", player_lives);
	sprintf(score_msg, "Score: %0d", player_score);

    draw_centred(2, "P A U S E D");
	draw_centred(LCD_Y / 5 + 2, flr_msg);
	display_time(15,LCD_Y / 5 * 2 + 2);

	draw_centred(LCD_Y / 5 * 3 + 2, score_msg);
	draw_centred(LCD_Y / 5 * 4 + 2, lives_msg);

	show_screen(); 
}

void show_game_over_screen ( void ) {
	clear_screen();

	draw_centred(5, "GAME OVER");

	sprintf(score_msg, "Final Score: %d", player_score);
	sprintf(flr_msg, "Final Floor: %d", current_floor);

	draw_centred(15, score_msg);
	draw_centred(25, flr_msg);

	show_screen();

	if (BIT_IS_SET(PINF, 5) || BIT_IS_SET(PINF, 6)) {
		// TODO: RESTART GAME
	}
}

void show_loading_screen( void ) {
	clear_screen();

	current_floor++;
	

	sprintf(score_msg, "Score: %d", player_score);
	sprintf(flr_msg, "Floor: %d", current_floor);

	draw_centred(5, "L O A D I N G");
	draw_centred(15, score_msg);
	draw_centred(25, flr_msg);

	show_screen();
	load_level();
	hasKey = false;
	hasBomb = false;
	hasShield = false;
	hasBowArrow = false;

	_delay_ms(2000);

	loading = false;
}

void show_main_menu( void ) {
	clear_screen();

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

				draw_centred(3, "ANSI-TOWER");
				draw_centred(LCD_Y / 4 + 2, "Greyden Scott");
				draw_centred(LCD_Y / 4 * 2 + 2, "N9935924");
				draw_string(LCD_X / 2, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);
			}

			_delay_ms(100);

			draw_centred(3, "ANSI-TOWER");
		    draw_centred(LCD_Y / 4 + 2, "Greyden Scott");
		    draw_centred(LCD_Y / 4 * 2 + 2, "N9935924");
		    draw_string(LCD_X / 2, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);

			show_screen();
		}

		main_menu_enabled = false;
		game_running = true;
		load_level();
	}
	if (main_menu_enabled) {
		draw_centred(3, "ANSI-TOWER");
		draw_centred(LCD_Y / 4 + 2, "Greyden Scott");
		draw_centred(LCD_Y / 4 * 2 + 2, "N9935924");
		draw_string(28, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);
		show_screen();
	}
}

void load_level( void ) {
		player.x = (LCD_X / 2) - playerWidthPixels / 2;
		player.y = (LCD_Y / 2);

		border_top.x = -21;
		border_top.y = -12;

		border_left.x = -21;
		border_left.y = -36;

		border_right.x = LCD_X + 21 - left_right_wallWidthPixels;
		border_right.y = - 36;

		border_bottom.x = -21;
		border_bottom.y = LCD_Y + 12;
	if (current_floor == 0) {
		key.x = -10;
		key.y = 0;
		door.x = (LCD_X / 2) - (doorWidthPixels / 2) + 3;
		door.y = (towerHeightPixels - doorHeightPixels) - 12;

		monster[0].x = (LCD_X + 10) - monsterWidthPixels - 2;
		monster[0].y = 0;
		
		treasure[0].x = random_number(border_left.x + border_left.width + treasure[0].width, border_right.x - treasure[0].width);
		treasure[0].y = border_bottom.y - treasure[0].height - treasure[0].height;
	} else {
		// REMOVE TOWER
		tower.x = -10000;
		// RESET ITEMS
		shield.x = -1000;
		bowarrow.x = -1000;
		bomb.x = -1000;
		key.x = -1000;
		// RESET ALL TREASURE AND MONSTERS
		for (int i = 0; i < NUM_OF_MONSTERS; i++) {
			monster[i].x = -1000;
		}
		for (int i = 0; i < NUM_OF_TREASURE; i++) {
			treasure[i].x = -1000;
		}

		generate_random_level();
	}
}

void generate_random_level( void ) {
	int max_left = border_left.x + border_left.width + 2 + door.width + 2;
	int max_right = border_right.x - 2 - door.width - 2;
	int max_top = border_top.y + border_top.height + 2 + door.height + 2;
	int max_bottom = border_bottom.y - 2 - door.height - 2 - 2;

	Location door_loc = random_door_location();
	door.x = door_loc.x;
	door.y = door_loc.y;

	wall_across[0].x = border_left.x + border_left.width;
	wall_down[0].x = random_number(max_left, wall_across[0].x + wall_across[0].width - wall_down[0].width);
	wall_across[1].x = border_right.x - wall_across[1].width;
	wall_down[1].x = max_right - wall_down[1].width;

	wall_across[2].x = max_left;
	wall_down[2].x = LCD_X / 2;

	int rng = random_number(0,4);
	if (rng >= 2) {
		wall_across[0].y = max_top;
		wall_down[0].y = wall_across[0].y + wall_across[0].height;

		wall_across[2].y = wall_down[0].y + wall_down[0].height;
	} else  {
		wall_across[0].y = max_bottom;
		wall_down[0].y = wall_across[0].y - wall_down[0].height;

		wall_across[2].y = wall_down[0].y - wall_across[2].height;
	}
	rng = random_number(0,4);
	if (rng >= 2) {
		wall_across[1].y = max_bottom;
		wall_down[1].y = wall_across[1].y - wall_down[1].height;
	} else  {
		wall_across[1].y = max_top;
		wall_down[1].y = wall_across[1].y + wall_across[1].height;
	}
	rng = random_number(0,4);
	if (rng >= 2) {
		wall_across[2].x = max_left;
		if (wall_across[0].y == max_top) {
			wall_across[2].y = wall_down[0].y + wall_down[0].height;

			wall_down[2].y = border_top.y + border_top.height; 
		} else {
			wall_across[2].y = wall_down[0].y;

			wall_down[2].y = border_bottom.y - wall_down[2].height;
		}
	} else  {
		wall_across[2].x = max_right - wall_across[2].width;
		if (wall_across[1].y == max_top) {
			wall_across[2].y = wall_down[1].y + wall_down[1].height;

			wall_down[2].y = border_top.y + border_top.height; 
		} else {
			wall_across[2].y = wall_down[1].y;

			wall_down[2].y = border_bottom.y - wall_down[2].height;
		}
	}
	
	int amt_treasure = random_number(0, NUM_OF_TREASURE + 1);
	int amt_monster = random_number(0, NUM_OF_MONSTERS + 1);

	Location key_loc = place_item(key);
	key.x = key_loc.x;
	key.y = key_loc.y;

	for (int i = 0; i < amt_treasure; i++) {
		Location treasure_loc = place_item(treasure[i]);
		treasure[i].x = treasure_loc.x;
		treasure[i].y = treasure_loc.y;
	}
	for (int i = 0; i < amt_monster; i++) {
		Location monster_loc = place_item(monster[i]);
		monster[i].x = monster_loc.x;
		monster[i].y = monster_loc.y;
	}
	if (random_number(0,30) <= 10) {
		Location shield_loc = place_item(shield);
		shield.x = shield_loc.x;
		shield.y = shield_loc.y;
	} 
	if (random_number(0,30) <= 10) {
		Location bowarrow_loc = place_item(bowarrow);
		bowarrow.x = bowarrow_loc.x;
		bowarrow.y = bowarrow_loc.y;
	} 
	if (random_number(0,30) <= 10) {
		Location bomb_loc = place_item(bomb);
		bomb.x = bomb_loc.x;
		bomb.y = bomb_loc.y;
	} 

}

Location place_item( Sprite sprite_to_place ) {
	int max_left = -21 + 6;
	int max_right = LCD_X + 21 - 6;
	int max_top = -12 + 6;
	int max_bottom = LCD_Y - 12 - 6;
	
	bool placing_object = false;

	Location new_item_location;

	do {
		placing_object = false;

		sprite_to_place.x = random_number(max_left, max_right);
		sprite_to_place.y = random_number(max_top, max_bottom);

		usb_serial_send(test_msg);
		for (int t = 0; t < NUM_OF_TREASURE; t++) {
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					for (int x = 0; x < NUM_OF_WALLS_DOWN; x++) {
						if (collision(&sprite_to_place, &wall_across[i]) ||
							collision(&sprite_to_place, &wall_down[i]) ||
							collision(&sprite_to_place, &player) ||
							collision(&sprite_to_place, &door) ||
							collision(&sprite_to_place, &key) ||
							collision(&sprite_to_place, &treasure[t]) ||
							collision(&sprite_to_place, &monster[m]) ||
							collision(&sprite_to_place, &shield) ||
							collision(&sprite_to_place, &bowarrow) ||
							collision(&sprite_to_place, &bomb)) {

							placing_object = true;
						}
					}
				}
			}
		}
		new_item_location.x = sprite_to_place.x;
		new_item_location.y = sprite_to_place.y;

	} while( placing_object );
	return new_item_location;
} 

Location random_door_location( void ) {

	Location door_floor_0;
	Location door_floor_1;
	Location door_floor_2;
	Location door_floor_3;

	if (current_floor > 0) {
		door_floor_0.x = -21 + left_right_wallWidthPixels + 2;
		door_floor_0.y = -12 + top_bottom_wallHeightPixels +  2;

		door_floor_1.x = (LCD_X + 21) - door.width  - 4;
		door_floor_1.y = -12 + top_bottom_wallHeightPixels + 2;

		door_floor_2.x = -21 + left_right_wallWidthPixels +  2;
		door_floor_2.y = (LCD_Y + 12) - door.height - 2;

		door_floor_3.x = (LCD_X + 21) - door.width - 4;
		door_floor_3.y = (LCD_Y + 12) - door.height - 2;


		Location door_locations[4] = {door_floor_0, door_floor_1, door_floor_2, door_floor_3};

		return door_locations[random_number(0,5)];

	}

	door_floor_0.x = (LCD_X / 2) - (doorWidthPixels / 2) + 3;
	door_floor_0.y = (towerHeightPixels - doorHeightPixels) - 12;
	return door_floor_0;
}

// ---------------------------------------------------------
//	HELPER FUNCTIONS
// ---------------------------------------------------------

void draw_centred( unsigned char y, char* string ) {
	unsigned char l = 0, i = 0;
	while (string[i] != '\0') { l++; i++; }
	char x = 42-(l*5/2);
	draw_string((x > 0) ? x : 0, y, string, FG_COLOUR);
}

int random_number( int min, int max) {
	//srand(overflow_counter);
   	return rand() % (min - max + 1) + min;
}

bool collision(Sprite* sprite_one, Sprite* sprite_two) {
	bool collided = true;
	
	int sprite_one_top = round(sprite_one->y);
	int sprite_one_left = round(sprite_one->x);
	int sprite_one_bottom = round(sprite_one->y) + sprite_one->height - 1;
	int sprite_one_right = round(sprite_one->x) + sprite_one->width - 1;
	
	int sprite_two_top = round(sprite_two->y) - 1;
	int sprite_two_left = round(sprite_two->x) - 1;
	int sprite_two_bottom = round(sprite_two->y) + sprite_two->height;
	int sprite_two_right = round(sprite_two->x) + sprite_two->width;
	
	
	if(sprite_one_top > sprite_two_bottom) collided = false;
	else if (sprite_one_bottom < sprite_two_top) collided = false;
	else if (sprite_one_left > sprite_two_right) collided = false;
	else if (sprite_one_right < sprite_two_left) collided = false;
	
	return collided;
}

direction collision_direction (Sprite* sprite_one, Sprite* sprite_two) {
	direction collided = none;
	
	int sprite_one_top = round(sprite_one->y);
	int sprite_one_left = round(sprite_one->x);
	int sprite_one_bottom = round(sprite_one->y) + sprite_one->height - 1;
	int sprite_one_right = round(sprite_one->x) + sprite_one->width - 1;
	
	int sprite_two_top = round(sprite_two->y);
	int sprite_two_left = round(sprite_two->x);
	int sprite_two_bottom = round(sprite_two->y) + sprite_two->height - 1;
	int sprite_two_right = round(sprite_two->x) + sprite_two->width - 1;
	
	
	if(sprite_one_top > sprite_two_bottom) collided = bottom;
	else if (sprite_one_bottom < sprite_two_top) collided = top;
	else if (sprite_one_left > sprite_two_right) collided = right;
	else if (sprite_one_right < sprite_two_left) collided = left;
	
	return collided;
}

bool visability_check(Sprite* sprite_one) {
	bool visable = false;
	int sprite_one_top = round(sprite_one->y);
	int sprite_one_left = round(sprite_one->x);
	int sprite_one_bottom = round(sprite_one->y) + sprite_one->height - 1;
	int sprite_one_right = round(sprite_one->x) + sprite_one->width - 1;

	if (sprite_one_top > LCD_Y && sprite_one_bottom < 0) visable = true;
	if (sprite_one_left < LCD_X && sprite_one_right > 0) visable = true;

	return visable;
}

// ---------------------------------------------------------
//	Timer overflow stuff.
// ---------------------------------------------------------

#define FREQ 8000000.0
#define PRESCALE 256.0
#define TIMER_SCALE 256.0

char buffer[20];
float minutes = 0;

float interval = 0;
uint8_t overflow_counter = 0;
uint8_t ovf_count = 0;

ISR(TIMER0_OVF_vect) {
	interval += TIMER_SCALE * PRESCALE / FREQ;
	if ( interval >= 0.5 ) {
		if (game_running) {
			//sprintf(debug_msg, "SYS TIME[%f] | Score: %0d | Floor: %0d | Player X/Y: (%.2f/%.2f) | Lives: %0d", get_system_time(), player_score, current_floor, player.x, player.y, player_lives);
			//usb_serial_send(debug_msg); //TODO: NEED TO PUT PROPER INFO IN
		}
		interval = 0;
	}
}

ISR(TIMER1_OVF_vect) {
	ovf_count++;
    overflow_counter++;
    if (round(overflow_counter) == 1800){
        overflow_counter = 0;
        minutes++;
    }
}

float get_system_time( void ) {
    return (ovf_count * 65536.0 + TCNT1) * 256.0  / FREQ;
}

float timer ( void ) {
	float time = ( overflow_counter * 65536.0 + TCNT1 ) * 256.0  / FREQ;
	return time;
}

void display_time(int x, int y) {
	snprintf(buffer, sizeof(buffer), "Time: %02.0f:%02.0f", minutes, timer());
    draw_string(x, y, buffer, FG_COLOUR);
}

// ---------------------------------------------------------
//	USB serial stuff.
// ---------------------------------------------------------

void usb_serial_send(char * string) {
    unsigned char char_count = 0;
    while (*string != '\0') {
        usb_serial_putchar(*string);
        string++;
        char_count++;
    }

    usb_serial_putchar('\r');
    usb_serial_putchar('\n');
}

void setup_usb_serial(void) {
	// Set up LCD and display message
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_string(10, 10, "Connect USB...", FG_COLOUR);
	show_screen();

	usb_init();

	while ( !usb_configured() || !usb_serial_get_control()) {
		// Block until USB is ready.
	}

	clear_screen();
	draw_string(10, 10, "USB connected", FG_COLOUR);
	show_screen();

	usb_serial_send("Welcome to ANSI-TOWER.");
	usb_serial_send("For CAB202 By Greyden Scott.");
	usb_serial_send("N9935924");
	_delay_ms(2000);
	usb_serial_send("Game is ready to play.");
}
