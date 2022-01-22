#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include "main.h"
#include "board.h"
#include "ships.h"
#include "vi.h"

int slanted_board = 0;

GRID_SQUARE boards[2][10][10];

void build_board(bool rebuild) {
	int box_height;
	int table_height = (slanted_board == 0)? 220: 550;
	for (int i = 0; i < 2; i++) {
		if (slanted_board == i)
			box_height = 20;
		else
			box_height = 50;
		for (int j = 0; j < 10; j++) {
			float y = (table_height * i) + (box_height * (j + 1));
			for (int k = 0; k < 10; k++) {
				float x = 50 * (k + 1);
				GRID_SQUARE *sq = &boards[i][j][k];
				sq->x = x;
				sq->y = y;
				if (!rebuild) {
					sq->hit = false;
					sq->fired_on = false;
					sq->hit_flash = -1;
				}
			}
		}
	}
}

void update_board() {
	int box_height;
	int top_dy;
	int side_dy;
	int table_height = (slanted_board == 0)? 220: 550;
	int table_2_height = (slanted_board == 1)? 200: 500;
	int table_1_start = (slanted_board == 0)? 20: 50;
	int table_2_start = (slanted_board == 0)? 270: 570;
	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			top_dy = 0;
			side_dy = table_1_start;
		}
		else {
			top_dy = table_height;
			side_dy = table_2_start;
		}
		if (slanted_board == i)
			box_height = 20;
		else
			box_height = 50;
		al_draw_scaled_bitmap(sprites.top, 0, 0, 500, 50, 50, top_dy, 500, box_height, 0);
		al_draw_scaled_bitmap(sprites.side, 0, 0, 50, 500, 0, side_dy, 50, box_height * 10, 0);
		for (int j = 0; j < 10; j++) {
			float y = (table_height * i) + (box_height * (j + 1));
			al_draw_line(50, y, 550, y, al_color_html("#FFCC1F"), 2);
			for (int k = 0; k < 10; k++) {
				float x = 50.0 * (k + 1);
				GRID_SQUARE *sq = &boards[i][j][k];
				if (j == 0)
					al_draw_line(x, y, x, y + (box_height * 10), al_color_html("#FFCC1F"), 2);
				if (sq->hit_flash > -1) {
					if ((sq->hit_flash % 5) == 0)
						al_draw_filled_rectangle(sq->x + 2, sq->y + 2, sq->x + 48, sq->y + box_height - 2, al_map_rgba_f(1, 1, 1, 0.5));
					sq->hit_flash--;
				}
				if (sq->fired_on && sq->hit)
					al_draw_scaled_bitmap(sprites.hit, 0, 0, 40, 40, sq->x + 5, sq->y + ((box_height * 0.2)/2), 40, box_height * 0.8, 0);
				else if (sq->fired_on && !sq->hit)
					al_draw_scaled_bitmap(sprites.miss, 0, 0, 40, 40, sq->x + 5, sq->y + ((box_height * 0.2)/2), 40, box_height * 0.8, 0);
//
			}
		}
	}
	al_draw_line(50, table_height, 550, table_height, al_color_html("#FFCC1F"), 2);
	al_draw_line(50, 770, 550, 770, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, table_1_start, 550, table_height, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, table_2_start, 550, table_2_start + table_2_height, al_color_html("#FFCC1F"), 2);
	al_draw_filled_rounded_rectangle(560, 10, 880, 780, 30, 30, al_color_name("grey"));
	al_draw_rounded_rectangle(560, 10, 880, 780, 30, 30, al_color_name("black"), 4);
	al_draw_rounded_rectangle(567, 17, 873, 773, 30, 30, al_color_name("black"), 4);
	al_draw_filled_rectangle(590, 100, 640, 150, al_color_html("#80B2FF"));
	al_draw_bitmap(sprites.new, 595, 105, 0);
	al_draw_filled_rectangle(660, 100, 710, 150, al_color_html("#80B2FF"));
	if (play_stage == 0 && !selected_ship.active)
		al_draw_bitmap(sprites.play, 665, 105, 0);
	else
		al_draw_tinted_bitmap(sprites.play, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 665, 105, 0);
	al_draw_filled_rectangle(730, 100, 780, 150, al_color_html("#80B2FF"));
	if (selected_ship.active && selected_ship.ship->rotation == 1)
		al_draw_bitmap(sprites.rotate_h, 735, 105, 0);
	else if (selected_ship.active && selected_ship.ship->rotation == 0)
		al_draw_bitmap(sprites.rotate_v, 735, 105, 0);
	else
		al_draw_tinted_bitmap(sprites.rotate_h, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 735, 105, 0);
	al_draw_filled_rectangle(800, 100, 850, 150, al_color_html("#80B2FF"));
	if (!selected_ship.active)
		al_draw_bitmap(sprites.switch_view, 805, 105, 0);
	else
		al_draw_tinted_bitmap(sprites.switch_view, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 805, 105, 0);
	if (selected_ship.active) {
		al_draw_filled_rectangle(660, 170, 710, 220, al_color_html("#80B2FF"));
		al_draw_bitmap(sprites.hit, 665, 175, 0);
		al_draw_filled_rectangle(730, 170, 780, 220, al_color_html("#80B2FF"));
		al_draw_bitmap(sprites.checkmark, 735, 175, 0);
	}
}

void check_for_hit(GRID_SQUARE *sq, int target) {
	for (int i = 0; i < SHIP_N; i++) {
		SHIP *s = &ships[target][i];
		for (int j = 0; j < s->length; j++) {
			if (s->loc[j] == sq) {
				sq->hit = true;
				check_if_sunk(s);
			}
		}
	}
}

bool player_fire(int x, int y) {
	int grid_height = (slanted_board == 0)? 20: 50;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			GRID_SQUARE *sq = &boards[0][i][j];
			if (collide(x, y, x, y, sq->x, sq->y, sq->x + 50, sq->y + grid_height)){
				if (sq->fired_on)
					return false;
				sq->fired_on = true;
				sq->hit_flash = 30;
				check_for_hit(sq, 0);
				return true;
			}
		}
	}
	return false;
}

void switch_view() {
	if (slanted_board == 0)
		slanted_board = 1;
	else
		slanted_board = 0;
	build_board(true);
}
