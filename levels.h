#ifndef LEVELS_H_
#define LEVELS_H_

#include <sprite.h>
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
Sprite boarder_top;
Sprite boarder_left;
Sprite boarder_right;
Sprite boarder_bottom;

void setup_player(void) {
	int x = (LCD_X / 2) - playerWidthPixels / 2;
	int y = (LCD_Y / 2);
	sprite_init(&player, x, y, playerWidthPixels, playerHeightPixels, playerBitmaps);
}

void setup_monster(void) {
	int x = (LCD_X + 10) - monsterWidthPixels - 2;
	int y = (0);
	sprite_init(&monster, x, y, monsterWidthPixels, monsterHeightPixels, monsterBitmaps);
}
void setup_key(void) {
	int x = (-10);
	int y = (0);
	sprite_init(&key, x, y, keyWidthPixels, keyHeightPixels, keyBitmaps);
}
void setup_door(void) {
	int x = (LCD_X / 2) - levelOneDoorWidthPixels / 2;
	int y = (towerHeightPixels - levelOneDoorWidthPixels) - 12;
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
	int y = -36;
	sprite_init(&boarder_top, x, y, wall_t_bWidthPixels, wall_t_bHeightPixels, wall_t_bBitmaps);
}
void setup_left_wall( void ) {
	int x = -63;
	int y = -36;
	sprite_init(&boarder_left, x, y, wall_l_rWidthPixels, wall_l_rHeightPixels, wall_l_rBitmaps);
}
void setup_right_wall( void ) {
	int x = LCD_X + 21;
	int y = -36;
	sprite_init(&boarder_right, x, y, wall_l_rWidthPixels, wall_l_rHeightPixels, wall_l_rBitmaps);
}
void setup_bottom_wall( void ) {
	int x = -21;
	int y = LCD_Y + 12;
	sprite_init(&boarder_bottom, x, y, wall_t_bWidthPixels, wall_t_bHeightPixels, wall_t_bBitmaps);
}

void load_level(int level) {
	if (level == 0) {
		setup_player();
	    setup_monster();
		setup_key();
		setup_door();
		setup_tower();
		setup_top_wall();
		setup_left_wall();
		setup_right_wall();
		setup_bottom_wall();
	}
}

void draw_level(int level) {
	if (level == 0) {
		sprite_draw(&monster);
	    sprite_draw(&key);
	    sprite_draw(&tower);
	    sprite_draw(&door);
	    sprite_draw(&player);
	    sprite_draw(&boarder_top);
	    sprite_draw(&boarder_left);
	    sprite_draw(&boarder_right);
	    sprite_draw(&boarder_bottom);
	}
}

#endif 