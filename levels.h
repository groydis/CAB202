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

Sprite wall_down_1;
Sprite wall_down_2;
Sprite wall_down_3;

Sprite wall_across_1;
Sprite wall_across_2;
Sprite wall_across_3;

typedef struct 
{
    int x;
    int y;

} Location;

int rand_number( int min, int max)
{
   return rand() % (min - max + 1) + min;
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
	sprite_init(&door, x, y, doorWidthPixels, doorHeightPixels, doorBitmaps);
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
	sprite_init(&border_top, x, y, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
}
void setup_left_wall( void ) {
	int x = -21;
	int y = -36;
	sprite_init(&border_left, x, y, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
}
void setup_right_wall( void ) {
	int x = LCD_X + 21 - left_right_wallWidthPixels;
	int y = -36;
	sprite_init(&border_right, x, y, left_right_wallWidthPixels, left_right_wallHeightPixels, left_right_wallBitmaps);
}
void setup_bottom_wall( void ) {
	int x = -21;
	int y = LCD_Y + 12;
	sprite_init(&border_bottom, x, y, top_bottom_wallWidthPixels, top_bottom_wallHeightPixels, top_bottom_wallBitmaps);
}

void setup_wall_across_1(int x, int y) {
	sprite_init(&wall_across_1, x, y, wall_acrossWidthPixels, wall_acrossHeightPixels, wall_acrossBitmaps);
}

void setup_wall_across_2(int x, int y) {
	sprite_init(&wall_across_2, x, y, wall_acrossWidthPixels, wall_acrossHeightPixels, wall_acrossBitmaps);
}

void setup_wall_across_3(int x, int y) {
	sprite_init(&wall_across_3, x, y, wall_acrossWidthPixels, wall_acrossHeightPixels, wall_acrossBitmaps);
}

void setup_wall_down_1(int x, int y) {
	sprite_init(&wall_down_1, x, y, wall_downWidthPixels, wall_downHeightPixels, wall_downBitmaps);
}

void setup_wall_down_2(int x, int y) {
	sprite_init(&wall_down_2, x, y, wall_downWidthPixels, wall_downHeightPixels, wall_downBitmaps);
}

void setup_wall_down_3(int x, int y) {
	sprite_init(&wall_down_3, x, y, wall_downWidthPixels, wall_downHeightPixels, wall_downBitmaps);
}

void load_level(int level) {
	if (level == 0) {
		Location door_loc = random_door_location(level);
		Location key_loc = random_key_location(level);

		setup_player();
	    setup_monster((LCD_X + 10) - monsterWidthPixels - 2, 0);
		
		setup_key(key_loc.x, key_loc.y);
		setup_door(door_loc.x, door_loc.y);
		
		setup_tower();
		setup_top_wall();
		setup_left_wall();
		setup_right_wall();
		setup_bottom_wall();
	} else {
		Location door_loc = random_door_location(level);
		Location key_loc = random_key_location(level);

		setup_player();

		setup_door(door_loc.x, door_loc.y);
		setup_key(key_loc.x, key_loc.y);

		setup_top_wall();
		setup_left_wall();
		setup_right_wall();
		setup_bottom_wall();

		int max_left = border_left.x + border_left.width + 2 + door.width + 2;
		int max_right = border_right.x - 2 - door.width - 2;
		int max_top = border_top.y + border_top.height + 2 + door.height + 2;
		int max_bottom = border_bottom.y - 2 - door.height - 2 - 2;

		int magical_random_map = rand_number(0, 10);
		if (magical_random_map == 0) {
			setup_wall_across_1(border_left.x + border_left.width, max_top);
			setup_wall_down_1(max_left, wall_across_1.y);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(max_right, wall_across_2.y);
			setup_wall_down_3(LCD_X / 2, border_top.y + border_top.height);
			setup_wall_across_3(wall_down_1.x, wall_down_1.y + wall_downHeightPixels);
		} else if (magical_random_map == 1) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(max_left, wall_across_1.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(max_right, wall_across_2.y);
			setup_wall_down_3(LCD_X / 2, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_3(wall_down_1.x, wall_down_1.y);
		} else if (magical_random_map == 2) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(max_left, wall_across_1.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(wall_across_2.x, wall_across_2.y);
			setup_wall_down_3(LCD_X / 2, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_3(wall_down_1.x, wall_down_1.y);
		} else if (magical_random_map == 3) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(max_left, wall_across_1.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_bottom);
			setup_wall_down_2(wall_across_2.x, wall_across_2.y - wall_downHeightPixels);
			setup_wall_down_3(LCD_X / 2, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_3(wall_down_1.x, wall_down_1.y);
		} else if (magical_random_map == 4) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(max_left, wall_across_1.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_bottom);
			setup_wall_down_2(wall_across_2.x, wall_across_2.y - wall_downHeightPixels);
			setup_wall_down_3(LCD_X / 2, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_3(wall_down_2.x - wall_acrossWidthPixels, wall_down_2.y);
		} else if (magical_random_map == 5) {
			setup_wall_down_1(max_left, border_top.y + border_top.height);
			setup_wall_across_1(wall_down_1.x, wall_down_1.y + wall_downHeightPixels);
			setup_wall_down_2(max_left, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_2(wall_down_2.x, wall_down_2.y);
			setup_wall_down_3(max_right - wall_downWidthPixels, border_top.y + border_top.height);
			setup_wall_across_3(border_right.x - wall_acrossWidthPixels, max_bottom);
		} else if (magical_random_map == 6) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(max_left, wall_across_1.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(wall_across_1.x + wall_acrossWidthPixels - 3, wall_across_2.y);
			setup_wall_down_3(LCD_X /2, border_top.y + border_top.height);
			setup_wall_across_3(border_right.x - wall_acrossWidthPixels, max_bottom);
		} else if (magical_random_map == 7) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(LCD_X / 2, border_top.y + border_top.height);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_bottom);
			setup_wall_down_2(wall_across_2.x, wall_across_2.y - wall_downHeightPixels);
			setup_wall_down_3(max_right, wall_across_2.y - wall_downHeightPixels);
			setup_wall_across_3(border_left.x + border_left.width, max_top);
		} else if (magical_random_map == 8) {
			setup_wall_across_1(border_left.x + border_left.width, max_bottom);
			setup_wall_down_1(LCD_X / 2, border_bottom.y - wall_downHeightPixels);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(wall_across_2.x, wall_across_2.y);
			setup_wall_down_3(max_right, wall_across_2.y);
			setup_wall_across_3(border_left.x + border_left.width, max_top);
		} else if (magical_random_map == 9) {
			setup_wall_across_1(border_left.x + border_left.width, max_top);
			setup_wall_down_1(max_left, wall_across_1.y);
			setup_wall_across_2(border_right.x - wall_acrossWidthPixels, max_top);
			setup_wall_down_2(wall_across_1.x + wall_acrossWidthPixels - 3, wall_across_2.y);
			setup_wall_down_3(LCD_X /2, border_top.y + border_top.height);
			setup_wall_across_3(border_right.x - wall_acrossWidthPixels, max_bottom);
		}

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

	   	sprite_draw(&wall_down_1);
	    sprite_draw(&wall_down_2);
	    sprite_draw(&wall_down_3);

	    sprite_draw(&wall_across_1);
	    sprite_draw(&wall_across_2);
	    sprite_draw(&wall_across_3);

	    sprite_draw(&door);
	    sprite_draw(&key);
	    sprite_draw(&player);

	}

}

#endif 