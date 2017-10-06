#ifndef GAME_SCREENS_H_
#define GAME_SCREENS_H_

#include <stdlib.h>
#include <math.h>
#include <graphics.h>
#include <sprite.h>
#include <util/delay.h>

#include "lcd_model.h"
#include "timer.h"

char *flr_msg = "Floor: %02d";
char *lives_msg = "Lives: %02d";
char *score_msg = "Score: %02d";

void draw_boarder() {
	for (int h = 0; h < 48; h++){
		set_pixel(0,h,FG_COLOUR);\
		set_pixel(1,h,FG_COLOUR);
		set_pixel(83,h, FG_COLOUR);
		set_pixel(82,h,FG_COLOUR);
	}
	for (int w = 0; w < 84; w++){
		set_pixel(w,0,FG_COLOUR);
		set_pixel(w,1,FG_COLOUR);
		set_pixel(w,47, FG_COLOUR);
		set_pixel(w,46,FG_COLOUR);
	}
}

void draw_main_menu(char *countdown, int countdown_x_pos) {
	
	draw_boarder();

    draw_string(17, 2, "ANSI-TOWER", FG_COLOUR);
    draw_string(8, LCD_Y / 4 + 2, "Greyden Scott", FG_COLOUR);
    draw_string(19, LCD_Y / 4 * 2 + 2, "N9935924", FG_COLOUR);
    draw_string(countdown_x_pos, LCD_Y / 4 * 3 + 2, countdown, FG_COLOUR);
}

void draw_pause_screen(int current_floor, int player_lives, int player_score) {

	draw_boarder();

	sprintf(flr_msg, "Floor: %d", current_floor);
	sprintf(lives_msg, "Lives: %d", player_lives);
	sprintf(score_msg, "Score: %d", player_score);


    drawCentred(2, "P A U S E D");
	drawCentred(LCD_Y / 5 + 2, flr_msg);
	display_time(15,LCD_Y / 5 * 2 + 2);

	drawCentred(LCD_Y / 5 * 3 + 2, score_msg);
	drawCentred(LCD_Y / 5 * 4 + 2, lives_msg);

}



#endif