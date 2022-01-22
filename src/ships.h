/* ships.h
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

typedef enum SHIP_TYPES {
	S_H_SHIP = 0,
	H_SHIP,
	S_SHIP,
	L_SHIP,
	S_L_SHIP,
	SHIP_N
} SHIP_TYPES;

typedef struct SHIP {
	GRID_SQUARE *loc[5];
	SHIP_TYPES type;
	int length;
	int rotation;
	bool in_movment;
  bool is_sunk;
} SHIP;

typedef struct GRABBED_SHIP {
	bool active;
	float offset_x;
	float offset_y;
	int orig_r;
	SHIP *ship;
} GRABBED_SHIP;

typedef struct SELECTED_SHIP {
	bool active;
	int orig_r;
	GRID_SQUARE *loc[5];
	SHIP *ship;
} SELECTED_SHIP;

void build_ships();
void check_if_sunk(SHIP *s);
void check_for_game_over();
void deselect_ship(bool accept);
void draw_ships();
void drop_ship(float x, float y);
void grab_ship(float x, float y);
bool place_ship(int x, int y, SHIP *ship, int board);
void rotate_ship();
void select_ship(float x, float y);
extern SHIP ships[2][SHIP_N];
extern GRABBED_SHIP grabbed_ship;
extern SELECTED_SHIP selected_ship;
