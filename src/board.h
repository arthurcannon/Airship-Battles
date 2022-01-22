/* board.h
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

typedef struct GRID_SQUARE {
	float x, y;
	bool fired_on;
	bool hit;
	int hit_flash;
} GRID_SQUARE;
void build_board(bool rebuild);
void update_board();
void check_for_hit(GRID_SQUARE *sq, int target);
bool player_fire(int x, int y);
void switch_view();
extern GRID_SQUARE boards[2][10][10];
extern int slanted_board;
