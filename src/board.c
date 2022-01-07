#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>

typedef struct {
	float x, y;
	bool hit;
	bool fired_on;
} GRID_SQUARE;

GRID_SQUARE boards[2][10][10];

void build_board() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 10; j++) {
			float y = (550 * i) + (50 * j);
			for (int k = 0; k < 10; k++) {
				float x = 50 * (k + 1);
				GRID_SQUARE *sq = &boards[i][j][k];
				sq->x = x;
				sq->y = y;
				sq->hit = false;
				sq->fired_on = false;
			}
		}
	}
}

void update_board() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 10; j++) {
			float y = (550 * i) + (50 * j);
			al_draw_line(50, y, 550, y, al_color_html("#FFCC1F"), 2);
			for (int k = 0; k < 10; k++) {
				float x = 50.0 * (k + 1);
				GRID_SQUARE *sq = &boards[i][j][k];
				if (j == 0)
					al_draw_line(x, y, x, y + 500, al_color_html("#FFCC1F"), 2);
				if (sq->fired_on && sq->hit) {}
					//draw hit
				else if (sq->fired_on && !sq->hit) {}
					//draw miss
			}
		}
	}
	al_draw_line(50, 500, 550, 500, al_color_html("#FFCC1F"), 2);
	al_draw_line(50, 1050, 550, 1050, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, 0, 550, 500, al_color_html("#FFCC1F"), 2);
	al_draw_line(550, 550, 550, 1050, al_color_html("#FFCC1F"), 2);
}
