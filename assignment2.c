#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h> 
#include <math.h>
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
#define MAX_ARROWS 5

#define FREQ 8000000.0
#define PRESCALE 256.0
#define TIMER_SCALE 256.0

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

} location;

char buffer[20];
float minutes = 0;
float time_to_boom;

double prev_system_time = 0.0;
double overflow_counter = 0;
double ovf_count = 0;

bool player_respawning = false;

bool main_menu_enabled = false;
bool game_running = false;
bool loading = false;
bool game_over = false;

int player_lives = 3;
int current_floor = 0;
int player_score = 0;

bool hasKey = false;
bool hasShield = false;
bool hasBomb = false;

bool bomb_detonated = false;

bool can_move_up = true;
bool can_move_down = true;
bool can_move_left = true;
bool can_move_right = true;

char *flr_msg = "Floor: %02d";
char *lives_msg = "Lives: %02d";
char *score_msg = "Score: %02d";
char *debug_msg = "[%f] | Score: %d | Floor: :%d | Player X/Y : (%.2f/%.2f) | Lives: %d";

Sprite player;
Sprite monster[NUM_OF_MONSTERS];
Sprite key;
Sprite door;
Sprite tower;
Sprite shield;
Sprite bomb;
Sprite treasure[NUM_OF_TREASURE];
Sprite border_top;
Sprite border_left;
Sprite border_right;
Sprite border_bottom;
Sprite wall_down[NUM_OF_WALLS_DOWN];
Sprite wall_across[NUM_OF_WALLS_ACROSS];

// ---------------------------------------------------------
//	SPRITE FUNCTIONS
// ---------------------------------------------------------
void initialise_sprites( void );
void draw_sprites( void );
void reset_sprites( void );
// ---------------------------------------------------------
//	LEVEL CREATION FUNCTIONS
// ---------------------------------------------------------
void load_level( void );
void generate_random_level( void );
location place_item( Sprite sprite_to_place );
location random_door_location( void );
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
void respawn_player( void );
// ---------------------------------------------------------
//	GAME SCREEN FUNCTIONS
// ---------------------------------------------------------
bool paused ( void );
void show_pause_screen( void );
void show_game_over_screen( void );
void show_loading_screen( void );
void show_main_menu( void );
// ---------------------------------------------------------
//	COMBAT FUCNTIONS
// --------------------------------------------------------
void aim( void );
void throw_bomb(int x, int y);
void detonate_bomb( void );
// ---------------------------------------------------------
//	HELPER FUNCTIONS
// ---------------------------------------------------------
void draw_centred( unsigned char y, char* string );
int random_number( int min, int max );
bool collision( Sprite* sprite_one, Sprite* sprite_two );
direction collision_direction ( Sprite* sprite_one, Sprite* sprite_two );
bool visability_check( Sprite* sprite_one );
uint16_t adc_read(int channel);
void wait_for_input( void );
// ---------------------------------------------------------
//	TIME FUCNTIONS
// ---------------------------------------------------------
void debug_time_checker( void );
float current_time( void );
void display_time( int x, int y );
float get_system_time( void );
float timer ( void );
// ---------------------------------------------------------
//	USB SERIAL FUCNTIONS
// ---------------------------------------------------------
void setup_usb_serial( void );
void usb_serial_send( char * message );

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

    // LED 
    SET_BIT(DDRB, 2);
	SET_BIT(DDRB, 3);
	CLEAR_BIT(PORTB, 2);
	CLEAR_BIT(PORTB, 3);

    // ADC
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

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
	game_running = true;
	main_menu_enabled = true;
}

void process(void) {
	clear_screen();
	aim();
	monster_movement();
	player_movement();
	handle_collision();
	draw_sprites();
	detonate_bomb();
	show_screen();
}

int main(void) {

	setup();

	while (game_running) {
		if (paused()) {
			show_pause_screen();
		} else if (game_over) {
			show_game_over_screen();
		}else if (loading) {
			show_loading_screen();
		}else if (main_menu_enabled){
			show_main_menu();
		} else {
			process();
			//_delay_ms(100);
		}
	}

	return 0;
}
// ---------------------------------------------------------
//	SPRITE FUNCTIONS
// ---------------------------------------------------------
void initialise_sprites( void ) {
	sprite_init(&player, -1000, 0, playerWidthPixels, playerHeightPixels, playerBitmaps);
	sprite_init(&key, -1000, 0, keyWidthPixels, keyHeightPixels, keyBitmaps);
	sprite_init(&door, -1000, 0, doorWidthPixels, doorHeightPixels, doorBitmaps);
	sprite_init(&tower, -1000, 0, towerWidthPixels, towerHeightPixels, towerBitmaps);
	sprite_init(&shield, -1000, 0, shieldWidthPixels, shieldHeightPixels, shieldBitmaps);
	sprite_init(&bomb, -1000, 0, bombWidthPixels, bombHeightPixels, bombBitmaps);
	for (int i = 0; i < NUM_OF_TREASURE; i++) {
		sprite_init(&treasure[i], -100000, 0, treasureWidthPixels, treasureHeightPixels, treasureBitmaps);
	}
	for (int i = 0; i < NUM_OF_MONSTERS; i++) {
    	sprite_init(&monster[i], -10000, 0, monsterWidthPixels, monsterHeightPixels, monsterBitmaps);
    }
	sprite_init(&border_top, -21, -12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
	sprite_init(&border_left, -21, -14, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
	sprite_init(&border_right, LCD_X + 21 - left_right_wallWidthPixels, -14, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
	sprite_init(&border_bottom, -21, LCD_Y + 12, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
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

	for (int d = 0; d < NUM_OF_WALLS_DOWN; d++) {
	   	sprite_draw(&wall_down[d]);
	}
	for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
		sprite_draw(&wall_across[a]);
	}
	for (int m = 0; m < NUM_OF_MONSTERS; m++) {
    	sprite_draw(&monster[m]);
	}
	for (int t = 0; t < NUM_OF_TREASURE; t++) {
    	sprite_draw(&treasure[t]);
	}

	sprite_draw(&shield);
	sprite_draw(&bomb);

	sprite_draw(&tower);

	sprite_draw(&door);
	sprite_draw(&key);
	sprite_draw(&player);
}

void reset_sprites( void ) {

	int reset_x = 500;
	player.x = reset_x;
	player.y = 0;
	key.x = reset_x;
	key.y = 0;
	door.x = reset_x;
	door.y = 0;
	tower.x = reset_x;
	tower.y = 0;
	shield.x = reset_x;
	shield.y = 0;
	bomb.x = reset_x;
	bomb.y = 0;

	for (int t = 0; t < NUM_OF_TREASURE; t++) {
		treasure[t].x = reset_x;
		treasure[t].y = 0;
	}
	for (int m = 0; m < NUM_OF_MONSTERS; m++) {
		monster[m].x = reset_x;
		monster[m].y = 0;
	}
	for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
		wall_across[a].x = reset_x;
		wall_across[a].y = 0;
	}
	for (int d = 0; d <NUM_OF_WALLS_DOWN; d++) {
		wall_down[d].x = reset_x;
		wall_down[d].y = 0;
	}

	border_top.x = -21;
	border_top.y = -12;
	border_left.x = -21;
	border_left.y = 36;
	border_right.x = LCD_X + 21 - left_right_wallWidthPixels;
	border_right.y = -36;
	border_bottom.x = -21;
	border_bottom.y = LCD_Y + 12;
}
// ---------------------------------------------------------
//	LEVEL GENERATION FUCNTIONS
// ---------------------------------------------------------

void load_level( void ) {

	reset_sprites();

	player.x = (LCD_X / 2) - playerWidthPixels / 2;
	player.y = (LCD_Y / 2);
	
	hasKey = false;
	hasBomb = false;
	hasShield = false;

	border_top.x = -21;
	border_top.y = -12;

	border_left.x = -21;
	border_left.y = -14;

	border_right.x = LCD_X + 21 - left_right_wallWidthPixels;
	border_right.y = - 14;

	border_bottom.x = -21;
	border_bottom.y = LCD_Y + 12;

	if (current_floor == 0) {
		tower.x = (LCD_X / 2) - towerWidthPixels / 2l;
		tower.y = -12;
		
		key.x = -10;
		key.y = 0;

		door.x = (LCD_X / 2) - (doorWidthPixels / 2);
		door.y = (towerHeightPixels - doorHeightPixels) - 12;

		monster[0].x = (LCD_X + 10) - monsterWidthPixels - 2;
		monster[0].y = 0;
		
		treasure[0].x = border_left.x + border_left.width + treasure[0].width;
		treasure[0].y = border_bottom.y - treasure[0].height - treasure[0].height;

	} else {
		generate_random_level();
		respawn_player();
	}
}

void generate_random_level( void ) {
	int max_left = border_left.x + border_left.width + 2 + door.width + 2;
	int max_right = border_right.x - 2 - door.width - 2;
	int max_top = border_top.y + border_top.height + 2 + door.height + 2;
	int max_bottom = border_bottom.y - 2 - door.height - 2 - 2;

	location door_loc = random_door_location();
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
	int amt_monster = random_number(1, NUM_OF_MONSTERS + 1);

	location key_loc = place_item(key);
	key.x = key_loc.x;
	key.y = key_loc.y;

	for (int i = 0; i < amt_treasure; i++) {
		location treasure_loc = place_item(treasure[i]);
		treasure[i].x = treasure_loc.x;
		treasure[i].y = treasure_loc.y;
	}
	for (int i = 0; i < amt_monster; i++) {
		location monster_loc = place_item(monster[i]);
		monster[i].x = monster_loc.x;
		monster[i].y = monster_loc.y;
	}
	if (random_number(0,30) <= 10) {
		location shield_loc = place_item(shield);
		shield.x = shield_loc.x;
		shield.y = shield_loc.y;
	} 
	if (random_number(0,30) <= 10) {
		location bomb_loc = place_item(bomb);
		bomb.x = bomb_loc.x;
		bomb.y = bomb_loc.y;
	} 
}

location place_item( Sprite sprite_to_place ) {
	int max_left = -21 + 6;
	int max_right = LCD_X + 21 - 6;
	int max_top = -12 + 6;
	int max_bottom = LCD_Y - 12 - 6;
	
	bool placing_object = false;

	location new_item_location;

	do {
		placing_object = false;

		sprite_to_place.x = random_number(max_left, max_right);
		sprite_to_place.y = random_number(max_top, max_bottom);

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

location random_door_location( void ) {

	location door_floor_0;
	location door_floor_1;
	location door_floor_2;
	location door_floor_3;

	if (current_floor > 0) {
		door_floor_0.x = -21 + left_right_wallWidthPixels + 2;
		door_floor_0.y = -12 + top_bottom_wallHeightPixels +  2;

		door_floor_1.x = (LCD_X + 21) - door.width  - 4;
		door_floor_1.y = -12 + top_bottom_wallHeightPixels + 2;

		door_floor_2.x = -21 + left_right_wallWidthPixels +  2;
		door_floor_2.y = (LCD_Y + 12) - door.height - 2;

		door_floor_3.x = (LCD_X + 21) - door.width - 4;
		door_floor_3.y = (LCD_Y + 12) - door.height - 2;


		location door_locations[4] = {door_floor_0, door_floor_1, door_floor_2, door_floor_3};

		return door_locations[random_number(0,5)];

	}

	door_floor_0.x = (LCD_X / 2) - (doorWidthPixels / 2);
	door_floor_0.y = (towerHeightPixels - doorHeightPixels) - 12;
	return door_floor_0;
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
				key.y = player.y + player.height + 4;
				key.x = player.x - key.width / 2;
			}
			if (hasShield) {
				shield.y = player.y + player.height + 4;
				shield.x = player.x + key.width + 2;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height + 4;
				bomb.x = player.x + key.width + 2;
			}
		} 
		else 
		{
			if (hasKey) {
				key.y = player.y + player.height + 4;
				key.x = player.x - key.width / 2;
			} else {
				key.y++;
			}
			if (hasShield) {
				shield.y = player.y + player.height + 4;;
				shield.x = player.x + key.width + 2;
			} else {
				shield.y++;
			}
			if (hasBomb) {
				bomb.y = player.y + player.height + 4;
				bomb.x = player.x + key.width + 2;
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
				key.y = player.y - key.height - 2;
				key.x = player.x - key.width / 2;
			}
			if (hasShield) {
				shield.y = player.y - shield.height - 2;
				shield.x = player.x + key.width + 2;
			} else if (hasBomb) {
				bomb.y = player.y - bomb.height - 4;
				bomb.x = player.x + key.width + 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y - key.height - 4;
				key.x = player.x - key.width / 2;
			} else {
				key.y--;
			}
			if (hasShield) {
				shield.y = player.y - shield.height - 4;
				shield.x = player.x + key.width + 2;
			} else {
				shield.y--;
			}
			if (hasBomb) {
				bomb.y = player.y - bomb.height - 4;
				bomb.x = player.x + key.width + 2;
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
				key.y = player.y + player.height - 2;
				key.x = player.x + player.width  + 2;
			}
			if (hasShield) {
				shield.y = player.y;
				shield.x = player.x + player.width + 2;
			}
			if (hasBomb) {
				bomb.y = player.y;
				bomb.x = player.x + player.width + 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y + player.height - 2;
				key.x = player.x + playerWidthPixels + 2;
			} else {
				key.x++;
			}
			if (hasShield) {
				shield.y = player.y;
				shield.x = player.x + player.width + 2;
			} else {
				shield.x++;
			}

			if (hasBomb) {
				bomb.y = player.y;
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
				key.y = player.y + player.height - 2;
				key.x = player.x - key.width - 2;
			}
			if (hasShield) {
				shield.y = player.y;
				shield.x = player.x - shield.width - 2;
			}
			if (hasBomb) {
				bomb.y = player.y;
				bomb.x = player.x - bomb.width - 2;
			}
		} else {
			if (hasKey) {
				key.y = player.y + player.height - 2;
				key.x = player.x - key.width - 2;
			} else {
				key.x--;
			}
			if (hasShield) {
				shield.y = player.y;
				shield.x = player.x - shield.width - 2;
			} else {
				shield.x--;
			}
			if (hasBomb) {
				bomb.y = player.y;
				bomb.x = player.x - bomb.width - 2;
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
			if (collision(&player, &monster[x]) && loading == false && player_respawning == false) {
				if (hasShield) {
					shield.x = -1000;
					monster[x].x = -1000;
					hasShield = false;
					usb_serial_send("Player Used A Shield.");
				} else {
					player_lives--;
					hasKey = false;
					hasShield = false;
					hasBomb = false;
					bomb_detonated = false;
					usb_serial_send("Player Was Killed By A Monster.");
					if (player_lives <= 0) {
						game_over = true;
					} else {
						respawn_player();
					}
				}
			}
		}

	}
	for (int i = 0; i < NUM_OF_TREASURE; i++) {
		if (visability_check(&treasure[i])) {
			if (collision(&player, &treasure[i]) && player_respawning == false) {
				player_score += 10;
				treasure[i].x = -1000;
				usb_serial_send("Player Collected Treasure.");
			}
		}
	}
	if (visability_check(&key)) {
		if (collision(&player, &key) && player_respawning == false) {
			hasKey = true;
			usb_serial_send("Player Collected The Key.");
		}
	}
	if (visability_check(&door)) {
		if (collision(&player, &door) && hasKey  && player_respawning == false) {
			loading = true;
			player_score +=100;
			usb_serial_send("Player Used The Key On The Door To Proceed To The Next Floor.");
		}
	}
	if (visability_check(&tower)) {
		if (collision(&player, &tower)  && player_respawning == false) {
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
		if (collision(&player, &shield) && player_respawning == false) {
			hasShield = true;
			hasBomb = false;
			usb_serial_send("Player Has Picked Up A Shield.");
		}
	}
	if (visability_check(&bomb)) {
		if (collision(&player, &bomb) && !hasBomb  && player_respawning == false) {
			hasShield = false;
			hasBomb = true;
			usb_serial_send("Player Has Picked Up A Bomb.");
		}
	}
}

void respawn_player ( void ) {
	player_respawning = true;
	bool placing_player = false;

	do {
		placing_player= false;

		player.x = random_number(2, 82 - player.width);
		player.y = random_number(2, 46 - player.height);

		int dist_from_x;
		int dist_from_Y;

		if (player.x > LCD_X / 2) {
			dist_from_x = (LCD_X - border_right.x - 2) * -1;

			tower.x -= dist_from_x;
			door.x -= dist_from_x;
			border_top.x -= dist_from_x;
			border_left.x -= dist_from_x;
			border_right.x -= dist_from_x;
			border_bottom.x -= dist_from_x;
			key.x -= dist_from_x;
			bomb.x -= dist_from_x;
			shield.x -= dist_from_x;
			for (int t = 0; t < NUM_OF_TREASURE; t++) {
				treasure[t].x -= dist_from_x;
			}
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				monster[m].x -= dist_from_x;
			}
			for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
				wall_across[a].x -= dist_from_x;
			}
			for (int d = 0; d < NUM_OF_WALLS_DOWN; d++) {
				wall_down[d].x -= dist_from_x;
			}

		}
		if (player.x < LCD_X / 2) {
			dist_from_x = border_left.x * - 1;
			tower.x += dist_from_x;
			door.x += dist_from_x;
			border_top.x += dist_from_x;
			border_left.x += dist_from_x;
			border_right.x += dist_from_x;
			border_bottom.x += dist_from_x;
			key.x += dist_from_x;
			bomb.x += dist_from_x;
			shield.x += dist_from_x;
			for (int t = 0; t < NUM_OF_TREASURE; t++) {
				treasure[t].x += dist_from_x;
			}
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				monster[m].x += dist_from_x;
			}
			for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
				wall_across[a].x += dist_from_x;
			}
			for (int d = 0; d < NUM_OF_WALLS_DOWN; d++) {
				wall_down[d].x += dist_from_x;
			}

		}
		if (player.y > LCD_Y / 2) {
			dist_from_Y = (LCD_Y - border_bottom.y - 2) * -1;
			tower.y -= dist_from_Y;
			door.y -= dist_from_Y;
			border_top.y -= dist_from_Y;
			border_left.y -= dist_from_Y;
			border_right.y -= dist_from_Y;
			border_bottom.y -= dist_from_Y;
			key.y -= dist_from_Y;
			bomb.y -= dist_from_Y;
			shield.y -= dist_from_Y;
			for (int t = 0; t < NUM_OF_TREASURE; t++) {
				treasure[t].y -= dist_from_Y;
			}
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				monster[m].y -= dist_from_Y;
			}
			for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
				wall_across[a].y -= dist_from_Y;
			}
			for (int d = 0; d < NUM_OF_WALLS_DOWN; d++) {
				wall_down[d].y -= dist_from_Y;
			}
		}
		if (player.y < LCD_Y / 2) {
			dist_from_Y = border_top.y * -1;
			tower.y += dist_from_Y;
			door.y += dist_from_Y;
			border_top.y += dist_from_Y;
			border_left.y += dist_from_Y;
			border_right.y += dist_from_Y;
			border_bottom.y += dist_from_Y;
			key.y += dist_from_Y;
			bomb.y += dist_from_Y;
			shield.y += dist_from_Y;
			for (int t = 0; t < NUM_OF_TREASURE; t++) {
				treasure[t].y += dist_from_Y;
			}
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				monster[m].y += dist_from_Y;
			}
			for (int a = 0; a < NUM_OF_WALLS_ACROSS; a++) {
				wall_across[a].y += dist_from_Y;
			}
			for (int d = 0; d < NUM_OF_WALLS_DOWN; d++) {
				wall_down[d].y += dist_from_Y;
			}
		}

		for (int t = 0; t < NUM_OF_TREASURE; t++) {
			for (int m = 0; m < NUM_OF_MONSTERS; m++) {
				for (int i = 0; i < NUM_OF_WALLS_ACROSS; i++) {
					for (int x = 0; x < NUM_OF_WALLS_DOWN; x++) {
						if (collision(&player, &wall_across[i]) ||
							collision(&player, &wall_down[i]) ||
							collision(&player, &door) ||
							collision(&player, &key) ||
							collision(&player, &treasure[t]) ||
							collision(&player, &monster[m]) ||
							collision(&player, &shield) ||
							collision(&player, &bomb) ||
							collision(&player, &tower)) {

							placing_player = true;
						}
					}
				}
			}
		}
	} while( placing_player );

	player_respawning = false;
}
// ---------------------------------------------------------
//	COMBAT FUCNTIONS
// --------------------------------------------------------
void aim( void ) {
	if (hasBomb) {
		int right_adc = adc_read(1);

		int line_end_x = player.x + (player.width / 2);
		int line_end_y = player.y + (player.height / 2);
		int line_length = 15;
		int start_x = line_end_x + (line_length * cos((float)right_adc / (1023 / 12.7)));
		int start_y = line_end_y + (line_length * sin((float)right_adc / (1023 / 12.7)));

		draw_line (start_x,start_y,line_end_x, line_end_y, FG_COLOUR);

		if (BIT_IS_SET(PINF, 5) || BIT_IS_SET(PINF, 6)) {
			if (hasBomb) {
				throw_bomb(start_x, start_y);
				usb_serial_send("Tossed that bomb yo!");
			} 
		}		
	}
}

void throw_bomb (int x, int y) {
	bool can_throw_bomb = false;

	for (int i = 0; i < NUM_OF_WALLS_DOWN; i++) {
		for (int j = 0; j < NUM_OF_WALLS_ACROSS; j++) {
			if (collision(&bomb, &wall_down[i]) ||
				collision(&bomb, &wall_across[j]) ||
				collision(&bomb, &border_top) ||
				collision(&bomb, &border_left) ||
				collision(&bomb, &border_bottom) ||
				collision(&bomb, &border_right)) 
			{	
				break;
			} 
			else {
				can_throw_bomb = true;
			}
		}
	}
	if (can_throw_bomb) {
		bomb.x = x - bomb.width / 2;
		bomb.y = y - bomb.height / 2;
		time_to_boom = get_system_time() + 2.0;
		hasBomb = false;
		bomb_detonated = true;
		usb_serial_send("Player Has Used The Bomb.");
	}
}

void detonate_bomb( void ) {
	if (get_system_time() > time_to_boom) {
		if (bomb_detonated) {
			SET_BIT(PORTB, 2);
			SET_BIT(PORTB, 3);
			_delay_ms(250);
			CLEAR_BIT(PORTB, 2);
			CLEAR_BIT(PORTB, 3);
			_delay_ms(250);
			SET_BIT(PORTB, 2);
			SET_BIT(PORTB, 3);
			_delay_ms(250);
			CLEAR_BIT(PORTB, 2);
			CLEAR_BIT(PORTB, 3);
			for (int x = 0; x < NUM_OF_MONSTERS; x++) {
				if (visability_check(&monster[x])) {
					player_score += 10;
					monster[x].x = -10000;
				}
			}
			bomb.x = -10000;
			bomb_detonated = false;
			usb_serial_send("The Bomb Has Exploded.");
		}
	}
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

	wait_for_input();

	game_over = false;
	main_menu_enabled = true;

	clear_screen();
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
	bomb_detonated = false;

	loading = false;
}

void show_main_menu( void ) {

	clear_screen();
	draw_centred(3, "ANSI-TOWER");
	draw_centred(LCD_Y / 4 + 2, "Greyden Scott");
	draw_centred(LCD_Y / 4 * 2 + 2, "N9935924");
	draw_centred(LCD_Y / 4 * 3 + 2, "Ready?");
	_delay_ms(500);
	show_screen();

	wait_for_input();

	clear_screen();
	draw_centred(LCD_Y / 2, "3");
	show_screen();
	_delay_ms(300);

	clear_screen();
	draw_centred(LCD_Y / 2, "2");
	show_screen();
	_delay_ms(300);

	clear_screen();
	draw_centred(LCD_Y / 2, "1");
	show_screen();
	_delay_ms(300);
	clear_screen();

	player_score = 0;
	player_lives = 3;
	current_floor = 0;

	bomb_detonated = false;

	main_menu_enabled = false;
	overflow_counter = 0.0;
	minutes = 0.0;
	load_level();
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

uint16_t adc_read(int channel) {
	ADMUX = (channel & ((1 << 5) - 1)) | (1 << REFS0);
	ADCSRB = (channel & (1 << 5));

	ADCSRA |= (1 << ADSC);

	while ( ADCSRA & (1 << ADSC) ) {}

	return ADC;
}

void wait_for_input( void ) {
	while (1){

		if (((PINF>>5) & 0b1)||(((PINF>>6) & 0b1))){

			break;
		}
	}
}
// ---------------------------------------------------------
//	Timer overflow stuff.
// ---------------------------------------------------------
ISR(TIMER0_OVF_vect) {
	debug_time_checker();
}

ISR(TIMER1_OVF_vect) {
	ovf_count++;
    overflow_counter++;
}

void debug_time_checker( void ) {
	if ((get_system_time() - prev_system_time) > 0.5) {
		prev_system_time = get_system_time();
		if (game_running && main_menu_enabled == false && game_over == false) {
			sprintf(debug_msg, "[%f] | Score: %0d | Floor: %0d | Player X/Y: (%.2f/%.2f) | Lives: %0d", get_system_time(), player_score, current_floor, player.x, player.y, player_lives);
			usb_serial_send(debug_msg);
		}
	}
}

float get_system_time( void ) {
    return (ovf_count * 65536.0 + TCNT1) * 256.0  / FREQ;
}

float timer ( void ) {
	float time = ( overflow_counter * 65536.0 + TCNT1 ) * 256.0  / FREQ;
	if (time >= 60.00) {
		overflow_counter = 0.0;
		minutes++;
	}
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
