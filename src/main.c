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

void error(char *msg) {
	fprintf(stderr, "%s: %s\r\n", msg, strerror(errno));
	exit(1);
}

void al_must_init(bool test, const char *description) {
	if (test) return;
	fprintf(stderr, "Couldn't initialize %s\r\n", description);
	error("Aleggro initialization error");
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
    if(ax1 > bx2) return false;
    if(ax2 < bx1) return false;
    if(ay1 > by2) return false;
    if(ay2 < by1) return false;

    return true;
}

float deg_to_rad(float deg) {
	return (deg * M_PI) / 180.0;
}

int between(int lo, int hi) {
	return lo + (rand() % (hi - lo));
}

float between_f(float lo, float hi) {
	return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

void start_game() {
	play_stage = 1;
	taking_turn = 1;
	if (slanted_board == 0)
		switch_view();
}

void new_game(bool *redraw, bool *turn_ended, int *end_of_turn_wait) {
	*redraw = true;
	game_over = false;
	play_stage = 0;
	slanted_board = 0;
	*turn_ended = false;
	*end_of_turn_wait = 0;
	grabbed_ship.active = false;
	build_board(false);
	build_ships();
	init_vi();
}

void render_cursor(float x, float y, ALLEGRO_DISPLAY *display) {
	cursor_x = x;
	cursor_y = y;
	if (
		play_stage == 1 &&
		taking_turn == 1 &&
		(
			(slanted_board == 1 && collide(x, y, x, y, 50, 50, 550, 550)) ||
			(slanted_board == 0 && collide(x, y, x, y, 50, 20, 550, 220))
		)
	) {
		al_set_mouse_cursor(display, peg_cursor);
	}
	else if (
		(
			play_stage == 0 &&
			slanted_board == 0 &&
			!selected_ship.active &&
			collide(x, y, x, y, 50, 270, 550, 770) &&
			(
				collide(x, y, x, y, ships[1][0].loc[0]->x, ships[1][0].loc[0]->y, ships[1][0].loc[4]->x + 50, ships[1][0].loc[4]->y + 50) ||
			 	collide(x, y, x, y, ships[1][1].loc[0]->x, ships[1][1].loc[0]->y, ships[1][1].loc[3]->x + 50, ships[1][1].loc[3]->y + 50) ||
			 	collide(x, y, x, y, ships[1][2].loc[0]->x, ships[1][2].loc[0]->y, ships[1][2].loc[2]->x + 50, ships[1][2].loc[2]->y + 50) ||
			 	collide(x, y, x, y, ships[1][3].loc[0]->x, ships[1][3].loc[0]->y, ships[1][3].loc[2]->x + 50, ships[1][3].loc[2]->y + 50) ||
			 	collide(x, y, x, y, ships[1][4].loc[0]->x, ships[1][4].loc[0]->y, ships[1][4].loc[1]->x + 50, ships[1][4].loc[1]->y + 50)
			)
		) ||
	 	(
	 		collide(x, y, x, y, 590, 100, 640, 150) ||
			(play_stage == 0 && !selected_ship.active && collide(x, y, x, y, 660, 100, 710, 150)) ||
			(selected_ship.active && collide(x, y, x, y, 730, 100, 780, 150)) ||
			(!selected_ship.active && collide(x, y, x, y, 800, 100, 850, 150)) ||
			(selected_ship.active && collide(x, y, x, y, 660, 170, 710, 220)) ||
			(selected_ship.active && collide(x, y, x, y, 730, 170, 780, 220))
	 	)
	) {
		al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
	}
	else if (
		selected_ship.active &&
		slanted_board == 0 &&
		collide(x, y, x, y, selected_ship.ship->loc[0]->x, selected_ship.ship->loc[0]->y, selected_ship.ship->loc[selected_ship.ship->length - 1]->x + 50, selected_ship.ship->loc[selected_ship.ship->length - 1]->y + 50)
	) {
		al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
	}
	else {
		al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
	}
}

SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h) {
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    al_must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init() {
	sprites._sheet = al_load_bitmap("spritesheet.png");
	al_must_init(sprites._sheet, "spritesheet");

	sprites.icon = sprite_grab(0, 340, 200, 200);

	sprites.play = sprite_grab(130, 270, 40, 40);
	sprites.new = sprite_grab(180, 270, 40, 40);
	sprites.switch_view = sprite_grab(80, 270, 40, 40);
	sprites.rotate_h = sprite_grab(150, 220, 40, 40);
	sprites.rotate_v = sprite_grab(210, 220, 40, 40);
	sprites.checkmark = sprite_grab(340, 270, 40, 40);

	sprites.top = sprite_grab(0, 0, 500, 50);
	sprites.side = sprite_grab(530, 0, 50, 500);
	sprites.hit = sprite_grab(280, 270, 40, 40);
	sprites.miss = sprite_grab(230, 270, 40, 40);
	sprites.cursor = sprite_grab(260, 180, 33, 33);

	sprites.ships[0] = sprite_grab(0, 70, 50, 250);
	sprites.ships[1] = sprite_grab(70, 70, 50, 200);
	sprites.ships[2] = sprite_grab(190, 70, 50, 150);
	sprites.ships[3] = sprite_grab(130, 70, 50, 150);
	sprites.ships[4] = sprite_grab(250, 70, 50, 100);

}

void sprites_deinit() {
	al_destroy_bitmap(sprites.icon);

	al_destroy_bitmap(sprites.play);
	al_destroy_bitmap(sprites.new);
	al_destroy_bitmap(sprites.switch_view);
	al_destroy_bitmap(sprites.rotate_h);
	al_destroy_bitmap(sprites.rotate_v);
	al_destroy_bitmap(sprites.checkmark);

	al_destroy_bitmap(sprites.top);
	al_destroy_bitmap(sprites.side);
	al_destroy_bitmap(sprites.hit);
	al_destroy_bitmap(sprites.miss);
	al_destroy_bitmap(sprites.cursor);

	al_destroy_bitmap(sprites.ships[0]);
	al_destroy_bitmap(sprites.ships[1]);
	al_destroy_bitmap(sprites.ships[2]);
	al_destroy_bitmap(sprites.ships[3]);
	al_destroy_bitmap(sprites.ships[4]);

	al_destroy_bitmap(sprites._sheet);
}

#define SCALED_WIDTH		    900
#define SCALED_HEIGHT		    800
ALLEGRO_MOUSE_CURSOR *peg_cursor;
float cursor_x;
float cursor_y;
int play_stage;
int taking_turn;
bool game_over;

int main() {
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_DISPLAY *display = NULL;

	al_must_init(al_init(), "allegro");
	al_must_init(al_install_keyboard(), "keyboard");
	al_must_init(al_install_mouse(), "mouse");

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 32.0);
	al_must_init(timer, "timer");

	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	display = al_create_display(SCALED_WIDTH, SCALED_HEIGHT);
	al_must_init(display, "display");

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	al_must_init(queue, "event queue");

	al_must_init(al_init_image_addon(), "Image addon");
	al_must_init(al_init_font_addon(), "font addon");
	al_must_init(al_init_primitives_addon(), "primitives addon");
	al_must_init(al_init_native_dialog_addon(), "dialog addon");

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_mouse_event_source());

	srand(time(NULL));
	sprites_init();

	al_set_display_icon(display, sprites.icon);

	build_board(false);
	build_ships();
	init_vi();
	bool done = false;
	bool redraw = true;
	ALLEGRO_EVENT event;
	peg_cursor = al_create_mouse_cursor(sprites.cursor, 7, 3);

	game_over = false;
	play_stage = 0;
	slanted_board = 0;
	bool turn_ended = false;
	int end_of_turn_wait = 0;
	selected_ship.active = false;
	grabbed_ship.active = false;

	al_start_timer(timer);
	while(1) {
		al_wait_for_event(queue, &event);

		switch(event.type) {
			case ALLEGRO_EVENT_TIMER:
				if (
					!game_over &&
					play_stage == 1 &&
					turn_ended &&
					end_of_turn_wait > 0
				)
						end_of_turn_wait--;
				if (
					!game_over &&
					play_stage == 1 &&
					turn_ended &&
					end_of_turn_wait == 0
				) {
					turn_ended = false;
					if (taking_turn == 0) {
						taking_turn = 1;
						if (slanted_board == 0)
							switch_view();
					}
					else {
						taking_turn = 0;
						if (slanted_board == 1)
							switch_view();
					}
				}
				if (
					!game_over &&
					play_stage == 1 &&
					taking_turn == 0 &&
					!turn_ended
				) {
					if (vi_state.timer < 5 * 30)
						vi_state.timer++;
					else {
						vi_state.timer = 0;
						vi_take_turn();
						end_of_turn_wait = 30;
						turn_ended = true;
					}
				}
				redraw = true;
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (
					event.mouse.button == 1 &&
					play_stage == 0 &&
					selected_ship.active &&
					slanted_board == 0 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, selected_ship.ship->loc[0]->x, selected_ship.ship->loc[0]->y, selected_ship.ship->loc[selected_ship.ship->length - 1]->x + 50, selected_ship.ship->loc[selected_ship.ship->length - 1]->y + 50)
				)
					grab_ship(event.mouse.x, event.mouse.y);
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				if (
					event.mouse.button == 1 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 590, 100, 640, 150)
				)
					new_game(&redraw, &turn_ended, &end_of_turn_wait);
				if (
					play_stage == 0 &&
					event.mouse.button == 1 &&
					!selected_ship.active &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 660, 100, 710, 150)
				)
					start_game();
				if (
					selected_ship.active &&
					event.mouse.button == 1 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 730, 100, 780, 150)
				)
					rotate_ship();
				if (
					event.mouse.button == 1 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 800, 100, 850, 150)
				)
					switch_view();
				if (
					selected_ship.active &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 660, 170, 710, 220)
				)
					deselect_ship(false);
				if (
					selected_ship.active &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 730, 170, 780, 220)
				)
					deselect_ship(true);
				if (
					play_stage == 0 &&
					slanted_board == 0 &&
					!selected_ship.active &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 270, 550, 770)
				)
					select_ship(event.mouse.x, event.mouse.y);
				if (
					event.mouse.button == 1 &&
					slanted_board == 0 &&
					grabbed_ship.active &&
					play_stage == 0 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 270, 550, 770)
				)
					drop_ship((event.mouse.x - grabbed_ship.offset_x) + 10, (event.mouse.y - grabbed_ship.offset_y) + 10);
				if (
					!game_over &&
					event.mouse.button == 1 &&
					play_stage == 1 &&
					taking_turn == 1 &&
					!turn_ended &&
					(
						(slanted_board == 1 && collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 50, 550, 550)) ||
						(slanted_board == 0 && collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 20, 550, 220))
					)
				)
					if (player_fire(event.mouse.x, event.mouse.y)) {
						turn_ended = true;
						end_of_turn_wait = 30;
					}
				break;
			case ALLEGRO_EVENT_MOUSE_AXES:
				render_cursor(event.mouse.x, event.mouse.y, display);
				break;
			case ALLEGRO_EVENT_KEY_DOWN:

				break;
			case ALLEGRO_EVENT_KEY_UP:
				if(event.keyboard.keycode == ALLEGRO_KEY_S)
					switch_view();
				if (grabbed_ship.active && event.keyboard.keycode == ALLEGRO_KEY_R)
					rotate_ship();
				if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					done = true;
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				done = true;
				break;
		}

		if (done)
			break;

		if (redraw && al_is_event_queue_empty(queue)){
			al_clear_to_color(al_color_html("#80B2FF"));
			draw_ships();
			update_board();
			al_flip_display();
			redraw = false;
		}

		if (game_over) {
			int answer = al_show_native_message_box(
				display,
				"Game Over",
				(taking_turn == 1)? "You Win": "You Lose",
				"Would you like to play again?",
				NULL,
				ALLEGRO_MESSAGEBOX_YES_NO
			);
			switch (answer) {
				case 1:
					new_game(&redraw, &turn_ended, &end_of_turn_wait);
					break;
				case 2:
				default:
					done = true;
					break;
			}
		}
	}

	sprites_deinit();
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_font(font);
	al_destroy_display(display);

	return 0;

}
