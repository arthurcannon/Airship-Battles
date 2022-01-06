#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>

void error(char *msg) {
	fprintf(stderr, "%s: %s", msg, strerror(errno));
	exit(1);
}

void al_must_init(bool test, const char *description) {
	if (test) return;
	fprintf(stderr, "Couldn't initialize %s", description);
	error("Aleggro initialization error");
}

float deg_to_rad(float deg) {
	return (deg * M_PI) / 180.0;
}

#define SCALED_WIDTH		    900
#define SCALED_HEIGHT		    900

int main(void) {

	ALLEGRO_FONT *font = NULL;
	ALLEGRO_DISPLAY *display = NULL;

	al_must_init(al_init(), "allegro");

//	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

	display = al_create_display(SCALED_WIDTH, SCALED_HEIGHT);
	al_must_init(display, "display");

	al_must_init(al_init_font_addon(), "font addon");

	al_must_init(al_init_primitives_addon(), "primitives addon");

	al_clear_to_color(al_color_html("#80B2FF"));

	al_flip_display();

	al_destroy_font(font);
	al_destroy_display(display);

	return 0;

}
