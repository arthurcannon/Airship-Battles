/* vi.h
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

typedef struct {
	int x;
	int y;
	bool hit;
	SHIP *ship;
} SHOT;
typedef struct VI_STATE {
	int timer;
	SHOT shots_fired[100];
	int shot_count;
	bool last_shot_was_hit;
	bool ship_search;
	SHOT *search_center;
	int dir_searched;
	int search_dir;
	int add_to_dir;
} VI_STATE;
void init_vi();
void vi_change_direction();
bool vi_fire(int x, int y);
SHOT vi_increment_direction();
void vi_take_turn();
extern VI_STATE vi_state;
