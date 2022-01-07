#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include "board.h"

void error(char *msg) {
	fprintf(stderr, "%s: %s", msg, strerror(errno));
	exit(1);
}

void al_must_init(bool test, const char *description) {
	if (test) return;
	fprintf(stderr, "Couldn't initialize %s", description);
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

typedef struct {
	ALLEGRO_BITMAP* _sheet;

	ALLEGRO_BITMAP* play;
	ALLEGRO_BITMAP* new;
	ALLEGRO_BITMAP* switch_view;
	ALLEGRO_BITMAP* rotate_h;
	ALLEGRO_BITMAP* rotate_v;

	ALLEGRO_BITMAP* bottom;
	ALLEGRO_BITMAP* side;
	ALLEGRO_BITMAP* hit;
	ALLEGRO_BITMAP* miss;
	ALLEGRO_BITMAP* cursor;

	ALLEGRO_BITMAP* s_h_ship;
	ALLEGRO_BITMAP* h_ship;
	ALLEGRO_BITMAP* s_ship;
	ALLEGRO_BITMAP* l_ship;
	ALLEGRO_BITMAP* s_l_ship;
} SPRITES;

SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h) {
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    al_must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init() {
	sprites._sheet = al_load_bitmap("spritessheet.png");
	al_must_init(sprites._sheet, "spritesheet");

	sprites.play = sprite_grab(130, 270, 40, 40);
	sprites.new = sprite_grab(180, 270, 40, 40);
	sprites.switch_view = sprite_grab(80, 270, 40, 40);
	sprites.rotate_h = sprite_grab(150, 220, 40, 40);
	sprites.rotate_v = sprite_grab(210, 220, 40, 40);

	sprites.bottom = sprite_grab(1, 10, 500, 50);
	sprites.side = sprite_grab(531, 10, 50, 500);
	sprites.hit = sprite_grab(280, 270, 40, 40);
	sprites.miss = sprite_grab(230, 270, 40, 40);
	sprites.cursor = sprite_grab(260, 180, 33, 33);

	sprites.s_h_ship = sprite_grab(0, 70, 50, 250);
	sprites.h_ship = sprite_grab(70, 70, 50, 200);
	sprites.s_ship = sprite_grab(190, 70, 50, 150);
	sprites.l_ship = sprite_grab(130, 70, 50, 150);
	sprites.s_l_ship = sprite_grab(250, 70, 50, 100);
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

	al_must_init(al_init_font_addon(), "font addon");
	al_must_init(al_init_primitives_addon(), "primitives addon");

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_mouse_event_source());

	build_board();
	bool done = false;
	bool redraw = true;
	ALLEGRO_EVENT event;

	#define KEY_SEEN     1
	#define KEY_RELEASED 2

	unsigned char key[ALLEGRO_KEY_MAX];
	memset(key, 0, sizeof(key));

	al_start_timer(timer);
	while(1) {
		al_wait_for_event(queue, &event);

		switch(event.type) {
			case ALLEGRO_EVENT_TIMER:
				if(key[ALLEGRO_KEY_UP])
//					y--;
				if(key[ALLEGRO_KEY_DOWN])
//					y++;
				if(key[ALLEGRO_KEY_LEFT])
//					x--;
				if(key[ALLEGRO_KEY_RIGHT])
//					x++;

				if(key[ALLEGRO_KEY_ESCAPE])
					done = true;

				for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
					key[i] &= KEY_SEEN;

				redraw = true;
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (event.mouse.button == 1) {}
					//click event goes here
				break;
			case ALLEGRO_EVENT_KEY_DOWN:
				key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
				break;
			case ALLEGRO_EVENT_KEY_UP:
				key[event.keyboard.keycode] &= KEY_RELEASED;
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
			al_flip_display();
			redraw = false;
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(queue);
	al_destroy_font(font);
	al_destroy_display(display);

	return 0;

}
