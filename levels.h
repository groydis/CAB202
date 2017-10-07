#ifndef LEVELS_H_
#define LEVELS_H_

#include <sprite.h>
#include <math.h>

#include "bitmaps.h"
#include "lcd_model.h"
#include "helpers.h"

Sprite player;
Sprite monster;
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

typedef struct 
{
    int x;
    int y;

} Location;

int rand_number( int min, int max)
{
    const unsigned int range = 1 + max - min;
    const unsigned int a = RAND_MAX / range;
    const unsigned int limit = a * range;
    int s;
    do{
        s = rand();
    } while (s >= limit);
 
    return min + (s / a);
}

Location random_location(Sprite sprite_spawned, int floor) {

	Location door_floor_0;
	Location door_floor_1;
	Location door_floor_2;
	Location door_floor_3;

	if (floor > 0) {
		door_floor_0.x = -21;
		door_floor_0.y = -12;
		door_floor_1.x = (LCD_X + 21) - door.width;
		door_floor_1.y = -12;
		door_floor_2.x = -21;
		door_floor_2.y = (LCD_Y - 12) - door.height;
		door_floor_3.x = (LCD_X + 21) - door.width;
		door_floor_3.y = (LCD_Y - 12) - door.height;

		Location door_locations[4] = {door_floor_0, door_floor_1, door_floor_2, door_floor_3};

		if (sprite_spawned.bitmap == levelOneDoorBitmaps) {
			return door_locations[rand_number(0,3)];
		}
	}

	door_floor_0.x = (LCD_X / 2) - levelOneDoorWidthPixels / 2;
	door_floor_0.y = (towerHeightPixels - levelOneDoorWidthPixels) - 12;
	return door_floor_0;
}

void setup_player(void) {
	int x = (LCD_X / 2) - playerWidthPixels / 2;
	int y = (LCD_Y / 2);
	sprite_init(&player, x, y, playerWidthPixels, playerHeightPixels, playerBitmaps);
}

void setup_monster(int x, int y) {
	sprite_init(&monster, x, y, monsterWidthPixels, monsterHeightPixels, monsterBitmaps);
}

void setup_key(int x, int y) {
	sprite_init(&key, x, y, keyWidthPixels, keyHeightPixels, keyBitmaps);
}
void setup_door(int x, int y) {
	sprite_init(&door, x, y, levelOneDoorWidthPixels, levelOneDoorHeightPixels, levelOneDoorBitmaps);
}
void setup_tower(void) {
	int x = (LCD_X / 2) - towerWidthPixels / 2;
	int y = -12;
	sprite_init(&tower, x, y, towerWidthPixels, towerHeightPixels, towerBitmaps);
}

//void setup_shield(void) {
//	int x = (LCD_X / 2);
//	int y = (LCD_Y / 2);
//	sprite_init(&shield, x, y, shieldWidthPixels, shieldHeightPixels, shieldBitmaps);
//}

//void setup_bomb(void) {
//	int x = (LCD_X / 2);
//	int y = (LCD_Y / 2);
//	sprite_init(&bomb, x, y, bombWidthPixels, bombHeightPixels, bombBitmaps);
//}

//void setup_bowarrow(void) {
//	int x = (LCD_X / 2);
//	int y = (LCD_Y / 2);
//	sprite_init(&bowarrow, x, y, bowarrowWidthPixels, bowarrowHeightPixels, bowarrowBitmaps);
//}

void setup_top_wall( void ) {
	int x = -21;
	int y = -12;
	sprite_init(&border_top, x, y, wall_t_bWidthPixels, wall_t_bHeightPixels, wall_t_bBitmaps);
}
void setup_left_wall( void ) {
	int x = -21;
	int y = -36;
	sprite_init(&border_left, x, y, wall_l_rWidthPixels, wall_l_rHeightPixels, wall_l_rBitmaps);
}
void setup_right_wall( void ) {
	int x = LCD_X + 21;
	int y = -36;
	sprite_init(&border_right, x, y, wall_l_rWidthPixels, wall_l_rHeightPixels, wall_l_rBitmaps);
}
void setup_bottom_wall( void ) {
	int x = -21;
	int y = LCD_Y + 12;
	sprite_init(&border_bottom, x, y, wall_t_bWidthPixels, wall_t_bHeightPixels, wall_t_bBitmaps);
}

void load_level(int level) {
	if (level == 0) {
		Location door_loc = random_location(door, level);

		setup_player();
	    setup_monster((LCD_X + 10) - monsterWidthPixels - 2, 0);
		
		setup_key(-10, 0);
		setup_door(door_loc.x, door_loc.y);
		
		setup_tower();
		setup_top_wall();
		setup_left_wall();
		setup_right_wall();
		setup_bottom_wall();
	} else {
		Location door_loc = random_location(door, level);

		setup_player();

		setup_door(door_loc.x, door_loc.y);

		setup_top_wall();
		setup_left_wall();
		setup_right_wall();
		setup_bottom_wall();
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
	    sprite_draw(&monster);
	    sprite_draw(&key);
	} else {
		sprite_draw(&border_top);
	    sprite_draw(&border_left);
	    sprite_draw(&border_right);
	    sprite_draw(&border_bottom);

	    sprite_draw(&door);
	    sprite_draw(&player);
	}

}

#endif 