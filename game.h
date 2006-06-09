/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#define BOARD_W 6
#define BOARD_H 16

enum game_state {
	GAME_STATE_IDLE,
	GAME_STATE_START,
	GAME_STATE_PLAY,
	GAME_STATE_PAUSE,
	GAME_STATE_GAME_OVER,
};


enum game_action_id {
	GAME_ACTION_START,
	GAME_ACTION_TICK,
	GAME_ACTION_UP,
	GAME_ACTION_DOWN,
	GAME_ACTION_LEFT,
	GAME_ACTION_RIGHT,
	GAME_ACTION_FLIP,
	GAME_ACTION_PAUSE,
	GAME_ACTION_EARTHQUAKE,
	GAME_ACTION_SET_CURSOR,
};

struct game_action_set_cursor {
	int x;
	int y;
};

struct game_action {
	enum game_action_id id;
	union {
		struct game_action_set_cursor set_cursor;
	} data;
};


enum game_event_id {
	GAME_EVENT_START,
	GAME_EVENT_EXPLODING,
	GAME_EVENT_SCORE_UPDATE,
	GAME_EVENT_NEW_BLOCK,
	GAME_EVENT_FALL,
	GAME_EVENT_GAME_OVER,
	GAME_EVENT_HURRY,
	GAME_EVENT_EARTHQUAKE,
};

struct game_event_exploding {
	int points;
	int blocks;
	int x;
	int y;
};

struct game_event {
	enum game_event_id id;
	union {
		struct game_event_exploding exploding;
	} data;
};

struct cell_t {
	int contents;
	int falling;
	int prev_falling;
	int fallen;
	int exploding;
	int same;
};

struct game_t {

	enum game_state state;
	struct cell_t cell[BOARD_W][BOARD_H];

	int num_blocks;

	int cursor_x;
	int cursor_y;

	int score;
	int score_counter;

	int time;
	int earthquake_available;
	int earthquake_counter;

	void (*callback)(struct game_t *g, struct game_event *event);
};


struct game_t *game_new(void);
int game_do(struct game_t *g, struct game_action *action);
void game_register_callback(struct game_t *g, void (*callback)(struct game_t *g, struct game_event *event));

/*
 * End
 */

