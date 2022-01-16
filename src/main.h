typedef struct SPRITES {
	ALLEGRO_BITMAP* _sheet;

	ALLEGRO_BITMAP* play;
	ALLEGRO_BITMAP* new;
	ALLEGRO_BITMAP* switch_view;
	ALLEGRO_BITMAP* rotate_h;
	ALLEGRO_BITMAP* rotate_v;

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
extern SPRITES sprites;
