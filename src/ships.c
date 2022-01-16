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

SHIP ships[2][SHIP_N];

void build_ships() {
	SHIP *s;
	int x;
	int y;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < SHIP_N; j++) {
			s = &ships[i][j];
			s->type = j;
			s->rotation = between(0, 2);
			s->in_movment = false;
			s->is_sunk = false;
			switch (j) {
				case S_H_SHIP:
					s->length = 5;
					break;
				case H_SHIP:
					s->length = 4;
					break;
				case S_SHIP:
					s->length = 3;
					break;
				case L_SHIP:
					s->length = 3;
					break;
				case S_L_SHIP:
					s->length = 2;
					break;
			}
			do {
				x = between(0, 10);
				y = between(0, 10);
			} while(!place_ship(x, y, s, i));
		}
	}
}

void draw_ships() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < SHIP_N; j++) {
			SHIP *s = &ships[i][j];
//			if (i == 1 || s->is_sunk) {
				float sy = 1;
				float sx = 1;
				float dx = s->loc[0]->x;
				float dy = s->loc[0]->y;
				int angle;
				if (s->rotation == 0) {
					angle = 0;
					if (i == slanted_board)
						sy = 0.4;
				}
				else {
					angle = 270;
					if (i == slanted_board)
						sx = 0.4;
					dy += 50 * sx;
				}
				al_draw_scaled_rotated_bitmap(sprites.ships[s->type], 0, 0, dx, dy, sx, sy, deg_to_rad(angle), 0);
//			}
		}
	}
}

bool place_ship(int x, int y, SHIP *ship, int board) {
	GRID_SQUARE *sq;
	if (ship->rotation == 0 && y + ship->length > 9)
		return false;
	if (ship->rotation != 0 && x + ship->length > 9)
		return false;
	for (int i = 0; i < ship->length; i++) {
		if (ship->rotation == 0)
			sq = &boards[board][y + i][x];
		else
			sq = &boards[board][y][x + i];
		for (int j = 0; j < SHIP_N; j++) {
			SHIP *s = &ships[board][j];
			for (int k = 0; k < s->length; k++) {
				if (s->loc[k] == sq)
					return false;
			}
		}
		ship->loc[i] = sq;
	}
	return true;
}
