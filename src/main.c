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
#include "main.h"
#include "board.h"
#include "ships.h"

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

SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h) {
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    al_must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init() {
	sprites._sheet = al_load_bitmap("spritesheet.png");
	al_must_init(sprites._sheet, "spritesheet");

	sprites.play = sprite_grab(130, 270, 40, 40);
	sprites.new = sprite_grab(180, 270, 40, 40);
	sprites.switch_view = sprite_grab(80, 270, 40, 40);
	sprites.rotate_h = sprite_grab(150, 220, 40, 40);
	sprites.rotate_v = sprite_grab(210, 220, 40, 40);

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
	al_destroy_bitmap(sprites.play);
	al_destroy_bitmap(sprites.new);
	al_destroy_bitmap(sprites.switch_view);
	al_destroy_bitmap(sprites.rotate_h);
	al_destroy_bitmap(sprites.rotate_v);

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
#define SCALED_HEIGHT		    1100

int main() {
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_DISPLAY *display = NULL;

	al_must_init(al_init(), "allegro");
	al_must_init(al_install_keyboard(), "keyboard");
	al_must_init(al_install_mouse(), "mouse");

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 32.0);
	al_must_init(timer, "timer");

//	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
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

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_mouse_event_source());

	srand(time(NULL));
	sprites_init();
	build_board(false);
	build_ships();
	bool done = false;
	bool redraw = true;
	ALLEGRO_EVENT event;

	int play_stage = 0;
	slanted_board = 0;

	al_start_timer(timer);
	while(1) {
		al_wait_for_event(queue, &event);

		switch(event.type) {
			case ALLEGRO_EVENT_TIMER:

				redraw = true;
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (event.mouse.button == 1 &&
					play_stage == 0 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 270, 550, 770)) {}
//					grab_ship(event.mouse.x, event.mouse.y);
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				if (event.mouse.button == 1 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 800, 100, 850, 150))
					switch_view();
				if (event.mouse.button == 1 &&
					play_stage == 0 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 270, 550, 770)) {}
//						drop_ship(event.mouse.x, event.mouse.y);
				if (event.mouse.button == 1 &&
					play_stage == 1 &&
					slanted_board == 1 &&
					collide(event.mouse.x, event.mouse.y, event.mouse.x, event.mouse.y, 50, 50, 550, 550)) {}
//						fire(event.mouse.x, event.mouse.y);
				break;
			case ALLEGRO_EVENT_MOUSE_AXES:
//				move_cursor(event.mouse.x, event.mouse.y);
				break;
			case ALLEGRO_EVENT_KEY_DOWN:

				break;
			case ALLEGRO_EVENT_KEY_UP:
				if(event.keyboard.keycode == ALLEGRO_KEY_S)
					switch_view();
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
			update_board();
			draw_ships();
			al_flip_display();
			redraw = false;
		}
	}

	sprites_deinit();
	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_font(font);
	al_destroy_display(display);

	return 0;

}
