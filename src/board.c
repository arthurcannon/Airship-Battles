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
#include "main.h"
#include "board.h"
#include "ships.h"

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
				if (sq->fired_on && sq->hit) {}
					//draw hit
				else if (sq->fired_on && !sq->hit) {}
					//draw miss
//				al_draw_filled_rectangle(sq->x + 2, sq->y + 2, sq->x + 48, sq->y + box_height - 2, al_map_rgba_f(1, 1, 1, 0.5));
			}
		}
	}
	al_draw_line(50, table_height, 550, table_height, al_color_html("#FFCC1F"), 2);
	al_draw_line(50, 770, 550, 770, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, table_1_start, 550, table_height, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, table_2_start, 550, table_2_start + table_2_height, al_color_html("#FFCC1F"), 2);
	al_draw_filled_rounded_rectangle(570, 20, 870, 770, 50, 50, al_color_name("grey"));
	al_draw_filled_rectangle(800, 100, 850, 150, al_color_html("#80B2FF"));
	al_draw_bitmap(sprites.switch_view, 805, 105, 0);
}

void fire(int x, int y) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			GRID_SQUARE *sq = &boards[0][i][j];
			if (collide(x - 5, y - 5, x + 5, y + 5, sq->x, sq->y, sq->x + 50, sq->y + 50))
				sq->fired_on = true;
		}
	}
}

void switch_view() {
	if (slanted_board == 0)
		slanted_board = 1;
	else
		slanted_board = 0;
	build_board(true);
}
