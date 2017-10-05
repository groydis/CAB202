#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <sprite.h>
#include "lcd_model.h"
#include "levels.h"
#include "controller.h"
#include "helpers.h"
#include "bitmaps.h"

bool nearBottom (void) {
	if (boarder_bottom.y == LCD_Y - 2 && joyDown_pressed()) {
		return true;
	}
	return false;
}

bool nearTop( void ) {
	if (boarder_top.y + wall_t_bHeightPixels == 2 && joyUp_pressed()) {
		return true;
	}
	return false;
}

bool nearLeft ( void ) {
	if (boarder_left.x + wall_l_rWidthPixels == 2 && joyLeft_pressed()) {
		return true;
	}
	return false;
}

bool nearRight ( void ) {
	if (boarder_right.x == LCD_X - 2 && joyRight_pressed()) {
		return true;
	}
	return false;

}


void monster_movement(Sprite monster) {
//	if (monster_visble(monster)) {
//        if (monster.x < player.x){
//            monster.x += 0.1;
//        }
//        if (monster.x > player.x){
//            monster.x -= 0.1;
//        }
//        if (monster.y < player.y){
//            monster.y += 0.1;
//        }
//        if (monster.y > player.y){
//            monster.y -= 0.1;
//        }
//    }

    if (monster.x <= 83 && monster.x >= 0){
        if (monster.y <= 47 && monster.y >= 0){
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
    }
}

void movement( int level, bool hasKey ) {
	if (joyUp_pressed() && collision(player, boarder_top) == false) {
		if (hasKey) {
			key.y = player.y + player.height;
			key.x = player.x - key.width;
		}

		if (nearTop() || nearBottom()) {
			
			player.y--;

		}

		if (player.y != LCD_Y / 2) {

			player.y--;

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y++;
			}
			door.y++;

			boarder_top.y++;
			boarder_left.y++;
			boarder_right.y++;
			boarder_bottom.y++;

			if (level == 0) {
				if (monster_visble(monster)) {

				} else {
		    		monster.y++;
				}
		    	tower.y++;
			}
		}
	}
	else if (joyDown_pressed() && collision(player, boarder_bottom) == false) {

		if (nearBottom() || nearTop()) {
			
			player.y++;

		} 
		if (player.y != LCD_Y / 2) {

			player.y++;

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.y--;
			}
	    	door.y--;

			boarder_top.y--;
			boarder_left.y--;
			boarder_right.y--;
			boarder_bottom.y--;

			if (level == 0) {
				if (monster_visble(monster)) {

				} else {
		    		monster.y--;
				}
		    	tower.y--;
			}
		}
	}
	else if (joyLeft_pressed() && collision(player, boarder_left) == false) {

		if (nearLeft() || nearRight()) {
			
			player.x--;

		} 
		if ( player.x != (LCD_X / 2) - playerWidthPixels / 2) {
			
			player.x--;

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x++;
			}
			door.x++;

			boarder_top.x++;
			boarder_left.x++;
			boarder_right.x++;
			boarder_bottom.x++;

			if (level == 0) {
				if (monster_visble(monster)) {

				} else {
		    		monster.x++;
		    	}
		    	tower.x++;
			}
		}
	}

	else if (joyRight_pressed() && collision(player, boarder_right) == false) {

		if (nearRight() || nearRight()) {
			
			player.x++;

		} 
		if ( player.x != (LCD_X / 2) - playerWidthPixels / 2) {

			player.x++;

		} else {
			if (hasKey) {
				key.y = player.y + player.height;
				key.x = player.x - key.width;
			} else {
				key.x--;
			}
	    	door.x--;

			boarder_top.x--;
			boarder_left.x--;
			boarder_right.x--;
			boarder_bottom.x--;

			if (level == 0) {
				if (monster_visble(monster)) {

				} else {
		    		monster.x--;
				}
		    	tower.x--;
			}
		}
	}
}


#endif
