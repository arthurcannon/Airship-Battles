/* main.h
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

typedef struct SPRITES {
	ALLEGRO_BITMAP* _sheet;

	ALLEGRO_BITMAP* icon;

	ALLEGRO_BITMAP* play;
	ALLEGRO_BITMAP* new;
	ALLEGRO_BITMAP* switch_view;
	ALLEGRO_BITMAP* rotate_h;
	ALLEGRO_BITMAP* rotate_v;
	ALLEGRO_BITMAP* checkmark;

	ALLEGRO_BITMAP* top;
	ALLEGRO_BITMAP* side;
	ALLEGRO_BITMAP* hit;
	ALLEGRO_BITMAP* miss;
	ALLEGRO_BITMAP* cursor;

	ALLEGRO_BITMAP* ships[5];
} SPRITES;
void error(char *msg);
void al_must_init(bool test, const char *description);
bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
float deg_to_rad(float deg);
int between(int lo, int hi);
float between_f(float lo, float hi);
ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);
void sprites_init();
void start_game();
void new_game();
void render_cursor(float x, float y, ALLEGRO_DISPLAY *display);
extern SPRITES sprites;
extern ALLEGRO_MOUSE_CURSOR *peg_cursor;
extern bool game_over;
extern bool end_game;
extern int play_stage;
extern int taking_turn;
extern float cursor_x;
extern float cursor_y;
