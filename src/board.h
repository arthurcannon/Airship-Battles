typedef struct GRID_SQUARE {
	float x, y;
	bool fired_on;
  bool hit;
} GRID_SQUARE;
void build_board(bool rebuild);
void update_board();
void fire(int x, int y);
void switch_view();
extern GRID_SQUARE boards[2][10][10];
extern int slanted_board;
