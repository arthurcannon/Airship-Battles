#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
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

VI_STATE vi_state;

void init_vi() {
	vi_state.timer = 0;
	vi_state.shot_count = 0;
	vi_state.ship_search = false;
	vi_state.last_shot_was_hit = false;
	for (int i = 0; i < 100; i++) {
		SHOT *shot = &vi_state.shots_fired[i];
		shot->x = -1;
		shot->y = -1;
		shot->hit = false;
		shot->ship = NULL;
	}
}

void vi_change_direction() {
	puts("change direction called");
	if (vi_state.search_dir == 0 && (vi_state.dir_searched == 1 || vi_state.dir_searched == 3)) {
		vi_state.search_dir = 2;
		vi_state.dir_searched++;
	}
	else if ((vi_state.search_dir == 0 || vi_state.search_dir == 2) && vi_state.dir_searched == 2) {
		do {
			vi_state.search_dir = between(0, 4);
		} while (vi_state.search_dir == 0 || vi_state.search_dir == 2);
		vi_state.dir_searched++;
	}
	else if (vi_state.search_dir == 2 && (vi_state.dir_searched == 1 || vi_state.dir_searched == 3)) {
		vi_state.search_dir = 0;
		vi_state.dir_searched++;
	}
	else if (vi_state.search_dir == 1 && (vi_state.dir_searched == 1 || vi_state.dir_searched == 3)) {
		vi_state.search_dir = 3;
		vi_state.dir_searched++;
	}
	else if ((vi_state.search_dir == 1 || vi_state.search_dir == 3) && vi_state.dir_searched == 2) {
		do {
			vi_state.search_dir = between(0, 4);
		} while (vi_state.search_dir == 1 || vi_state.search_dir == 3);
		vi_state.dir_searched++;
	}
	else if (vi_state.search_dir == 3 && (vi_state.dir_searched == 1 || vi_state.dir_searched == 3)) {
		vi_state.search_dir = 1;
		vi_state.dir_searched++;
	}
	vi_state.add_to_dir = 1;
	printf("new direction %i\ndirections searched %i\n", vi_state.search_dir, vi_state.dir_searched);
}

bool vi_fire(int x, int y) {
	puts("firing");
	if (x > 9 || x < 0 || y > 9 || y < 0)
		return false;
	GRID_SQUARE *sq = &boards[1][y][x];
	if (sq) {
		if (sq->fired_on)
			return false;
		sq->fired_on = true;
		sq->hit_flash = 30;
		check_for_hit(sq, 1);
		vi_state.shots_fired[vi_state.shot_count].x = x;
		vi_state.shots_fired[vi_state.shot_count].y = y;
		vi_state.shots_fired[vi_state.shot_count].hit = sq->hit;
		vi_state.last_shot_was_hit = sq->hit;
		if (sq->hit) {
			for (int i = 0; i < SHIP_N; i++) {
				SHIP *s = &ships[1][i];
				for (int j = 0; j < s->length; j++) {
					if (s->loc[j] == sq) {
						printf("ship should set and type is %i\n", s->type);
						vi_state.shots_fired[vi_state.shot_count].ship = s;
						break;
					}
				}
				if (vi_state.shots_fired[vi_state.shot_count].ship == s)
					break;
			}
		}
		vi_state.shot_count++;
		return true;
	}
	return false;
}

SHOT vi_increment_direction() {
	puts("increment direction called");
	SHOT shot = *vi_state.search_center;
	switch (vi_state.search_dir) {
		case 0:
			shot.x += vi_state.add_to_dir;
			break;
		case 1:
			shot.y += vi_state.add_to_dir;
			break;
		case 2:
			shot.x -= vi_state.add_to_dir;
			break;
		case 3:
			shot.y -= vi_state.add_to_dir;
			break;
	}
	if (shot.x < 10 && shot.x > -1 && shot.y < 10 && shot.y > -1)
		shot.hit = boards[1][shot.y][shot.x].hit;
	else
		shot.hit = false;
	int hit;
	if (shot.hit) hit = 1;
	else hit = 0;
	printf("Results:\nx: %i\ny: %i\nhit: %i\n", shot.x, shot.y, hit);
	return shot;
}

void vi_take_turn() {
	int x;
	int y;
	SHOT shot;
	if (
		!(vi_state.ship_search && !vi_state.search_center->ship->is_sunk) &&
		!(vi_state.last_shot_was_hit && !vi_state.shots_fired[vi_state.shot_count - 1].ship->is_sunk)
	) {
		puts("Entering the lost shot finding logic");
		for (int i = vi_state.shot_count - 1; i > -1; i--) {
			printf("loop %i\n", i);
			SHOT *shot = &vi_state.shots_fired[i];
			if (shot->hit && !shot->ship->is_sunk) {
				vi_state.ship_search = true;
				vi_state.search_center = shot;
				vi_state.add_to_dir = 0;
				vi_state.dir_searched = 1;
				vi_state.search_dir = between(0, 4);
				printf("x: %i\ny: %i\nsearch direction: %i\ndirections searched: %i\nshot count: %i\n", vi_state.search_center->x, vi_state.search_center->y, vi_state.search_dir, vi_state.dir_searched, vi_state.shot_count);
				break;
			}
		}
	}
	if (vi_state.ship_search && !vi_state.search_center->ship->is_sunk) {
		puts("performing ship search");
		printf("x: %i\ny: %i\nsearch direction: %i\ndirections searched: %i\nshot count: %i\n", vi_state.search_center->x, vi_state.search_center->y, vi_state.search_dir, vi_state.dir_searched, vi_state.shot_count);
		if (vi_state.last_shot_was_hit) {
			puts("last time was a hit");
			vi_state.add_to_dir++;
			shot = vi_increment_direction();
		}
		else {
			puts("last time was a flop");
			vi_change_direction();
			shot = vi_increment_direction();
		}
		while (!vi_fire(shot.x, shot.y)) {
			puts("fire loop");
			if (!shot.hit)
				vi_change_direction();
			else {
				puts("it was a hit");
				vi_state.add_to_dir++;
			}
			shot = vi_increment_direction();
		}
		if (vi_state.search_center->ship->is_sunk)
			vi_state.ship_search = false;
	}
	else if  (vi_state.last_shot_was_hit && !vi_state.shots_fired[vi_state.shot_count - 1].ship->is_sunk) {
		puts("hey we hit somthing last turn");
		vi_state.ship_search = true;
		vi_state.search_center = &vi_state.shots_fired[vi_state.shot_count - 1];
		vi_state.add_to_dir = 1;
		vi_state.dir_searched = 1;
		do {
			vi_state.search_dir = between(0, 4);
			shot = vi_increment_direction();
			puts("fire loop");
		} while (!vi_fire(shot.x, shot.y));
		if (vi_state.last_shot_was_hit && vi_state.search_center->ship->is_sunk)
			vi_state.ship_search = false;
	}
	else {
		puts("rand shot");
		do {
			x = between(0, 10);
			y = between(0, 10);
			printf("x: %i\ny: %i\n", x, y);
		} while (!vi_fire(x, y));
	}
}
