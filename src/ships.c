#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>

typedef enum {
	S_H_SHIP = 0,
	H_SHIP,
	S_SHIP,
	L_SHIP,
	S_L_SHIP,
	SHIP_N
} SHIPS;

typedef struct {
	int x, y;
} SHIP;
