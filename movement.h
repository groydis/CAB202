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
	if (monster_visble(monster)) {
		if (monster.x < player.x) {
			monster.x++;
		}
		if (monster.x > player.x) {
			monster.x--;
		}
		if (monster.y < player.y) {
			monster.y++;
		}
		if (monster.y > player.y) {
			monster.y--;
		}
	}
}

void movement( int level, bool hasKey ) {
	if (joyUp_pressed() && collision(player, boarder_top) == false) {
		if (hasKey) {
			key.y = (player.y / 2) + (playerHeightPixels + keyHeightPixels - 1);
			key.x = (player.x / 2);
		}

		if (nearTop() || nearBottom()) {
			
			player.y--;

		}

		if (player.y != LCD_Y / 2) {

			player.y--;

		} else {

			key.y++;
			door.y++;

			boarder_top.y++;
			boarder_left.y++;
			boarder_right.y++;
			boarder_bottom.y++;

			if (level == 0) {
		    	monster.y++;
		    	tower.y++;
			}
		}
	}
	else if (joyDown_pressed() && collision(player, boarder_bottom) == false) {

		if (hasKey) {
			key.y = (player.y / 2) - (keyHeightPixels + 2);
			key.x = (player.x / 2);
    	}

		if (nearBottom() || nearTop()) {
			
			player.y++;

		} 
		if (player.y != LCD_Y / 2) {

			player.y++;

		} else {
			key.y--;
	    	door.y--;

			boarder_top.y--;
			boarder_left.y--;
			boarder_right.y--;
			boarder_bottom.y--;

			if (level == 0) {
		    	monster.y--;
		    	tower.y--;
			}
		}
	}
	else if (joyLeft_pressed() && collision(player, boarder_left) == false) {
		if (hasKey) {
			key.y = (player.y / 2);
			key.x = (player.x / 2) + playerWidthPixels;
    	}

		if (nearLeft() || nearRight()) {
			
			player.x--;

		} 
		if ( player.x != (LCD_X / 2) - playerWidthPixels / 2) {
			
			player.x--;

		} else {
			key.x++;
			door.x++;

			boarder_top.x++;
			boarder_left.x++;
			boarder_right.x++;
			boarder_bottom.x++;

			if (level == 0) {
		    	monster.x++;
		    	tower.x++;
			}
		}
	}

	else if (joyRight_pressed() && collision(player, boarder_right) == false) {
		if (hasKey) {
			key.y = (player.y / 2);
			key.x = (player.x / 2) - (playerWidthPixels + keyWidthPixels);
		}

		if (nearRight() || nearRight()) {
			
			player.x++;

		} 
		if ( player.x != (LCD_X / 2) - playerWidthPixels / 2) {

			player.x++;

		} else {
			key.x--;
	    	door.x--;

			boarder_top.x--;
			boarder_left.x--;
			boarder_right.x--;
			boarder_bottom.x--;

			if (level == 0) {
		    	monster.x--;
		    	tower.x--;
			}
		}
	}
}


#endif
