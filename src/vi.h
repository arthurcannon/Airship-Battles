typedef struct {
	int x;
	int y;
	bool hit;
	SHIP *ship;
} SHOT;
typedef struct VI_STATE {
	int timer;
	SHOT shots_fired[100];
	int shot_count;
	bool last_shot_was_hit;
	bool ship_search;
	SHOT *search_center;
	int dir_searched;
	int search_dir;
	int add_to_dir;
} VI_STATE;
void init_vi();
void vi_change_direction();
bool vi_fire(int x, int y);
SHOT vi_increment_direction();
void vi_take_turn();
extern VI_STATE vi_state;
