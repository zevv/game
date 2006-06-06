
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


#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"

static int game_start(struct game_t *g);
static int game_tick(struct game_t *g);


struct game_t *game_new(void)
{
	struct game_t *g;
	
	srand(time(NULL));

	g = calloc(sizeof(*g), 1);
	g->state = GAME_STATE_IDLE;
	g->cursor_x = BOARD_W / 2;
	g->cursor_y = BOARD_H / 2;
	return g;
}

static void game_callback(struct game_t *g, enum game_event ev);

int game_do(struct game_t *g, enum game_action action)
{
	struct cell_t cell;
	
	if(g->cursor_x < 0) g->cursor_x = 0;
	if(g->cursor_x > BOARD_W-2) g->cursor_x = BOARD_W-2;
	if(g->cursor_y < 0) g->cursor_y = 0;
	if(g->cursor_y > BOARD_H-1) g->cursor_y = BOARD_H-1;
	
	switch(action) {

		case GAME_ACTION_START:
			return game_start(g);
			break;

		case GAME_ACTION_PAUSE:
			if(g->state == GAME_STATE_PLAY) {
				g->state = GAME_STATE_PAUSE;
			} else if(g->state == GAME_STATE_PAUSE) {
				g->state = GAME_STATE_PLAY;
			}
			break;

		case GAME_ACTION_UP:
			if(g->cursor_y > 0) g->cursor_y --;
			break;

		case GAME_ACTION_DOWN:
			if(g->cursor_y < BOARD_H-1) g->cursor_y ++;
			break;
		
		case GAME_ACTION_LEFT:
			if(g->cursor_x > 0) g->cursor_x --;
			break;

		case GAME_ACTION_RIGHT:
			if(g->cursor_x < BOARD_W-2) g->cursor_x ++;
			break;

		default:
			break;
	}

	if(g->state == GAME_STATE_PLAY) {

		switch(action) {

			case GAME_ACTION_START:
				return game_start(g);
				break;

			case GAME_ACTION_FLIP:
				cell = g->cell[g->cursor_x][g->cursor_y];
				g->cell[g->cursor_x][g->cursor_y] = g->cell[g->cursor_x+1][g->cursor_y];
				g->cell[g->cursor_x+1][g->cursor_y] = cell;
				
				break;

			case GAME_ACTION_TICK:
				game_tick(g);
				break;

			case GAME_ACTION_EARTHQUAKE:
				if(g->earthquake_available) {
					g->earthquake_available = 0;
					g->earthquake_counter = 20;
					game_callback(g, GAME_EVENT_EARTHQUAKE);
				}
				break;

			default:
				break;
		}
	}


	return 0;
}



static int game_start(struct game_t *g)
{
	int x;
	int y;
	struct cell_t *cell;

	g->score = 0;
	g->score_counter = 0;
	g->num_blocks = 6;
	g->state = GAME_STATE_PLAY;
	g->time = 0;
	g->earthquake_available = 1;

	for(y=0; y<BOARD_H; y++) {
		for(x=0; x<BOARD_W; x++) {
			cell = &g->cell[x][y];

			memset(cell, 0, sizeof(*cell));

			if((y>3) && ((rand() % 2) == 0)) {
				cell->contents = (rand() % g->num_blocks) + 1;
			} else {
				cell->contents = 0;
			}
		}
	}

	game_callback(g, GAME_EVENT_START);

	return 0;
}


static void explode_same_cells(struct game_t *g)
{
	int x, y;
	struct cell_t *cell;
	int same_count = 0;
	
	for(x=0; x<BOARD_W; x++) {
		for(y=0; y<BOARD_H; y++) {
			cell = &g->cell[x][y];
			if(cell->same) {
				if(!cell->exploding) cell->exploding = 1;
				same_count ++;
				cell->same = 0;
			}
		}
	}

	if(same_count >= 5) {
		game_callback(g, GAME_EVENT_BONUS2);
	}

	else if(same_count >= 4) {
		game_callback(g, GAME_EVENT_BONUS);
	}

	else if(same_count >= 5) {
		game_callback(g, GAME_EVENT_EXPLODING);
	}

	g->score += (same_count - 2) * 3;
}
	

	

static int game_tick(struct game_t *g)
{
	int x, y;
	static int fall_counter = 0;
	static int newblock_counter = 0;
	static int explode_counter = 0;
	static int hurry_counter = 0;
	int same_count = 0;
	int t;
	int i;
	struct cell_t *cell = NULL;
	struct cell_t *pcell = NULL;
	struct cell_t *tcell;

	/*
	 * Check for earthquake
	 */

	if(g->earthquake_counter) {
		cell = &g->cell[rand() % BOARD_W][BOARD_H-1];
		memset(cell, 0, sizeof(*cell));
		g->earthquake_counter --;
	}

	/*
	 * Check for falling blocks
	 */
		
	for(x=0; x<BOARD_W; x++) {
		for(y=0; y<BOARD_H; y++) {
			g->cell[x][y].fallen = 0;
		}
	}

	if(fall_counter++ == 2) {

		for(x=0; x<BOARD_W; x++) {
			for(y=1; y<BOARD_H; y++) {
				if(!g->cell[x][y-1].fallen) {
					if((g->cell[x][y].contents == 0) && (g->cell[x][y-1].contents != 0)) {
						g->cell[x][y] = g->cell[x][y-1];
						g->cell[x][y].fallen = 1;
						g->cell[x][y].falling = 1;
						g->cell[x][y-1].contents = 0;
					} else {
						g->cell[x][y].falling = 0;
					}
				}
			}
		}


		fall_counter = 0;
	}

	/*
	 * Check for 'hurry' state
	 */

	if(hurry_counter++ >= 70) {
		t=0 ;
		for(x=0; x<BOARD_W; x++) {
			if(g->cell[x][2].contents && !g->cell[x][2].falling) t=1;
		}
		if(t) game_callback(g, GAME_EVENT_HURRY);
		hurry_counter = 0;
	}


	/*
	 * Add a new block every now and then. If there is a block
	 * at the place we want to insert the new block, the game
	 * is over.
	 */

	g->time ++;

	t = 40 - (g->time/400);
	if(t < 4) t = 4;
	if(newblock_counter++ >= t) {
		x = rand() % BOARD_W;
		if(g->cell[x][0].contents) {
			g->state = GAME_STATE_GAME_OVER;
			game_callback(g, GAME_EVENT_GAME_OVER);
		}
		g->cell[x][0].contents = (rand() % g->num_blocks) + 1;
		newblock_counter = 0;
		game_callback(g, GAME_EVENT_NEW_BLOCK);
	}


	/*
	 * Check for exploding blocks, and remove if exploded
	 */

	if(explode_counter++ > 1) {
		for(x=0; x<BOARD_W; x++) {
			for(y=0; y<BOARD_H; y++) {
				if(g->cell[x][y].contents && (g->cell[x][y].exploding > 0)) {
					g->cell[x][y].exploding ++;
					if(g->cell[x][y].exploding > 5) {
						cell = &g->cell[x][y];
						memset(cell, 0, sizeof(*cell));
						g->score ++;
					}
				}
			}
		}
		explode_counter = 0;
	}
	
	/*
	 * Check rows of 3 or longer the same
	 */
	

	for(y=0; y<BOARD_H; y++) {
		same_count = 1;
		cell = pcell = NULL;
		for(x=0; x<=BOARD_W; x++) {
			pcell = cell;
			cell = (x<BOARD_W) ? &g->cell[x][y] : NULL;

			if(cell && pcell && (cell->contents) && (cell->contents == pcell->contents) && !cell->exploding) {
				 same_count ++;
			} else {
				if(same_count >= 3) {
					for(i=0; i<same_count; i++) {
						tcell = &g->cell[x-i-1][y];
						tcell->same =1;
					}
					explode_same_cells(g);
				}
				same_count = 1;
			}
		}
	}
				
	for(x=0; x<BOARD_W; x++) {
		same_count = 1;
		cell = pcell = NULL;
		for(y=0; y<BOARD_H+1; y++) {
			pcell = cell;
			cell = (y<BOARD_H) ? &g->cell[x][y] : NULL;

			if(cell && pcell && (cell->contents) && (cell->contents == pcell->contents) && !cell->exploding) {
				 same_count ++;
			} else {
				if(same_count >= 3) {
					for(i=0; i<same_count; i++) {
						tcell = &g->cell[x][y-i-1];
						tcell->same = 1;
					}
					explode_same_cells(g);
				}
				same_count = 1;
			}
		}
	}


	/*
	 * Update score counter
	 */

	if(g->score_counter < g->score) {
		g->score_counter ++;	
		game_callback(g, GAME_EVENT_SCORE_UPDATE);
	}

	return 0;
}

void game_register_callback(struct game_t *g, void (*callback)(struct game_t *g, enum game_event event))
{
	g->callback = callback;
}

static void game_callback(struct game_t *g, enum game_event ev)
{
	if(g->callback) g->callback(g, ev);
}


/*
 * End
 */

