typedef struct GRID_SQUARE {
	float x, y;
	bool fired_on;
	bool hit;
	int hit_flash;
} GRID_SQUARE;
void build_board(bool rebuild);
void update_board();
void check_for_hit(GRID_SQUARE *sq, int target);
bool player_fire(int x, int y);
void switch_view();
extern GRID_SQUARE boards[2][10][10];
extern int slanted_board;
