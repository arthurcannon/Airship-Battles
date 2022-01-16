typedef enum SHIP_TYPES {
	S_H_SHIP = 0,
	H_SHIP,
	S_SHIP,
	L_SHIP,
	S_L_SHIP,
	SHIP_N
} SHIP_TYPES;

typedef struct SHIP {
	GRID_SQUARE *loc[5];
	SHIP_TYPES type;
	int length;
	int rotation;
	bool in_movment;
  bool is_sunk;
} SHIP;

void build_ships();
void draw_ships();
bool place_ship(int x, int y, SHIP *ship, int board);
