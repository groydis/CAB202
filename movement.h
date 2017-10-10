#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <sprite.h>

#include "lcd_model.h"
#include "levels.h"
#include "controller.h"
#include "helpers.h"
#include "bitmaps.h"

bool nearBottom (void) {
	if (border_bottom.y == LCD_Y - 2 && joyDown_pressed()) {
		return true;
	}
	return false;
}

bool nearTop( void ) {
	if (border_top.y + border_top.height == 2 && joyUp_pressed()) {
		return true;
	}
	return false;
}

bool nearLeft ( void ) {
	if (border_left.x + border_left.width == 2 && joyLeft_pressed()) {
		return true;
	}
	return false;
}

bool nearRight ( void ) {
	if (border_right.x == LCD_X - 2 && joyRight_pressed()) {
		return true;
	}
	return false;

}

void monster_movement( Sprite enemy ) {
	if (enemy.is_visible) {
	    if (enemy.x < player.x){
            enemy.x += 0.1;
        }
        if (enemy.x > player.x){
            enemy.x -= 0.1;
        }
        if (enemy.y < player.y){
            enemy.y += 0.1;
        }
        if (enemy.y > player.y){
            enemy.y -= 0.1;
        }
	}
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
		    	monster.y++;
		    	tower.y++;
			} else {
				wall_down_1.y++;
				wall_down_2.y++;
				wall_down_3.y++;
				wall_across_1.y++;
				wall_across_2.y++;
				wall_across_3.y++;
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
		    	monster.y--;
		    	tower.y--;
			} else {
				wall_down_1.y--;
				wall_down_2.y--;
				wall_down_3.y--;
				wall_across_1.y--;
				wall_across_2.y--;
				wall_across_3.y--;
			}
		}
	}
	else if (BIT_IS_SET(PINB, 1)&& collision(player, border_left) == false) {

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
		    	monster.x++;
		    	tower.x++;
			} else {
				wall_down_1.x++;
				wall_down_2.x++;
				wall_down_3.x++;
				wall_across_1.x++;
				wall_across_2.x++;
				wall_across_3.x++;
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
		    	monster.x--;
		    	tower.x--;
			} else {
				wall_down_1.x--;
				wall_down_2.x--;
				wall_down_3.x--;
				wall_across_1.x--;
				wall_across_2.x--;
				wall_across_3.x--;
			}
		}
	}
}

void move_sprites( int current_floor, bool hasKey ) {
	if (current_floor == 0) {
		monster_movement(monster);
    	movement(current_floor, hasKey);
	} else {
		movement(current_floor, hasKey);
	}
	draw_level(current_floor);
}

#endif
