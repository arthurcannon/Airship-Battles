/* ships.c
 *
 * Copyright 2022 Arthur Cannon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

SHIP ships[2][SHIP_N];
GRABBED_SHIP grabbed_ship;
SELECTED_SHIP selected_ship;

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

void check_if_sunk(SHIP *s) {
	int hits = 0;
	for (int i = 0; i < s->length; i++) {
		if (s->loc[i]->hit)
			hits++;
	}
	if (hits == s->length) {
		s->is_sunk = true;
		check_for_game_over();
	}
}

void check_for_game_over() {
	int ships_sunk;
	for (int i = 0; i < 2; i++) {
		ships_sunk = 0;
		for (int j = 0; j < SHIP_N; j++) {
			if (ships[i][j].is_sunk)
				ships_sunk++;
		}
		if (ships_sunk == SHIP_N)
			end_game = true;
	}
}

void deselect_ship(bool accept) {
	bool ship_placed;
	int x = -1;
	int y = -1;
	if (selected_ship.active) {
		SHIP *s = selected_ship.ship;
		if (accept) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (s->loc[0] == &boards[1][i][j]) {
						x = j;
						y = i;
						break;
					}
				}
				if (x != -1)
					break;
			}
			ship_placed = place_ship(x, y, s, 1);
		}
		if (!accept || !ship_placed) {
			for (int i = 0; i < s->length; i++) {
				s->loc[i] = selected_ship.loc[i];
			}
			s->rotation = selected_ship.orig_r;
		}
		selected_ship.active = false;
	}
}

void draw_ships() {
	SHIP *s;
	float sx;
	float sy;
	float dx;
	float dy;
	float srx2;
	float sry2;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < SHIP_N; j++) {
			s = &ships[i][j];
			if (i == 1 || s->is_sunk) {
				sx = 1;
				sy = 1;
				dx = s->loc[0]->x;
				dy = s->loc[0]->y;
				if (i == 1 && grabbed_ship.active && grabbed_ship.ship == s) {
					dx = cursor_x - grabbed_ship.offset_x;
					dy = cursor_y - grabbed_ship.offset_y;
				}
				else if (i == 1 && selected_ship.active && selected_ship.ship == s) {
					srx2 = (s->rotation == 0)? 50: 50 * s->length;
					sry2 = (s->rotation == 0)? 50 * s->length: 50;
					al_draw_filled_rectangle(s->loc[0]->x, s->loc[0]->y, s->loc[0]->x + srx2, s->loc[0]->y + sry2, al_map_rgba_f(1, 1, 1, 0.5));
				}
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
				if (s->is_sunk)
					al_draw_tinted_scaled_rotated_bitmap(sprites.ships[s->type], al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 0, 0, dx, dy, sx, sy, deg_to_rad(angle), 0);
				else
					al_draw_scaled_rotated_bitmap(sprites.ships[s->type], 0, 0, dx, dy, sx, sy, deg_to_rad(angle), 0);
			}
		}
	}
}

void drop_ship(float x, float y) {
	if (grabbed_ship.active) for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			GRID_SQUARE *sq = &boards[1][i][j];
			if (collide(x, y, x, y, sq->x, sq->y, sq->x + 50, sq->y + 50)) {
				if (!place_ship(j, i, grabbed_ship.ship, 1))
					grabbed_ship.ship->rotation = grabbed_ship.orig_r;
				grabbed_ship.active = false;
				break;
			}
			if (!grabbed_ship.active)
				break;
		}
	}
}

void grab_ship(float x, float y) {
	if (selected_ship.active) {
		SHIP *s = selected_ship.ship;
		if (collide(x, y, x, y, s->loc[0]->x, s->loc[0]->y, s->loc[s->length - 1]->x + 50, s->loc[s->length - 1]->y + 50)) {
			grabbed_ship.active = true;
			grabbed_ship.offset_x = x - s->loc[0]->x;
			grabbed_ship.offset_y = y - s->loc[0]->y;
			grabbed_ship.orig_r = s->rotation;
			grabbed_ship.ship = s;
		}
	}
}

bool place_ship(int x, int y, SHIP *ship, int board) {
	GRID_SQUARE *sq;
	if (ship->rotation == 0 && y + ship->length > 10)
		return false;
	if (ship->rotation != 0 && x + ship->length > 10)
		return false;
	for (int i = 0; i < ship->length; i++) {
		if (ship->rotation == 0)
			sq = &boards[board][y + i][x];
		else
			sq = &boards[board][y][x + i];
		for (int j = 0; j < SHIP_N; j++) {
			SHIP *s = &ships[board][j];
			if (s == ship)
				continue;
			for (int k = 0; k < s->length; k++) {
				if (s->loc[k] == sq )
					return false;
			}
		}
		ship->loc[i] = sq;
	}
	return true;
}

void rotate_ship() {
	if (grabbed_ship.active) {
		if (grabbed_ship.ship->rotation == 0)
			grabbed_ship.ship->rotation = 1;
		else
			grabbed_ship.ship->rotation = 0;
		grabbed_ship.offset_x = 25;
		grabbed_ship.offset_y = 25;
	}
	else if (selected_ship.active) {
		if (selected_ship.ship->rotation == 0)
			selected_ship.ship->rotation = 1;
		else
			selected_ship.ship->rotation = 0;
	}
}

void select_ship(float x, float y) {
	for (int i = 0; i < SHIP_N; i++) {
		SHIP *s = &ships[1][i];
		if (collide(x, y, x, y, s->loc[0]->x, s->loc[0]->y, s->loc[s->length - 1]->x + 50, s->loc[s->length - 1]->y + 50)) {
			selected_ship.active = true;
			selected_ship.orig_r = s->rotation;
			for (int j = 0; j < s->length; j++) {
				selected_ship.loc[j] = s->loc[j];
			}
			selected_ship.ship = s;
			break;
		}
	}
}
