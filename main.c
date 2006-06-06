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
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

Uint32 game_timer(Uint32 interval, void *data);
void draw(struct game_t *g);

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *blits;

enum blitrect {
	BR_CURSOR,
	BR_PRESS_S_TO_START,

	BR_BLOCK1,
	BR_BLOCK2,
	BR_BLOCK3,
	BR_BLOCK4,
	BR_BLOCK5,
	BR_BLOCK6,

	BR_EXPLODING1,
	BR_EXPLODING2,
	BR_EXPLODING3,
	BR_EXPLODING4,
	BR_EXPLODING5,
	BR_EXPLODING6,
		
	BR_BACKGROUND,
	BR_PAUSE,

	BR_CHAR_0,
	BR_CHAR_1,
	BR_CHAR_2,
	BR_CHAR_3,
	BR_CHAR_4,

	BR_CHAR_5,
	BR_CHAR_6,
	BR_CHAR_7,
	BR_CHAR_8,
	BR_CHAR_9,

	BR_GAME_OVER,
};


SDL_Rect blitrect[] = {
	[BR_CURSOR] =    	{   0,   0,  64, 32 },
	[BR_PRESS_S_TO_START]   {  64,   0, 192, 32 },

	[BR_BLOCK1] =    	{   0,  32,  32, 32 },
	[BR_BLOCK2] =    	{  32,  32,  32, 32 },
	[BR_BLOCK3] =    	{  64,  32,  32, 32 },
	[BR_BLOCK4] =    	{  96,  32,  32, 32 },
	[BR_BLOCK5] =    	{ 128,  32,  32, 32 },
	[BR_BLOCK6] =    	{ 160,  32,  32, 32 },
                      	
	[BR_EXPLODING1] =	{   0,  64,  32, 32 },
	[BR_EXPLODING2] =	{  32,  64,  32, 32 },
	[BR_EXPLODING3] =	{  64,  64,  32, 32 },
	[BR_EXPLODING4] =	{  96,  64,  32, 32 },
	[BR_EXPLODING5] =	{ 128,  64,  32, 32 },
	[BR_EXPLODING6] =	{ 160,  64,  32, 32 },
		
	[BR_BACKGROUND] =	{   0,  96,  32, 32 },
	[BR_PAUSE] =     	{  32,  96, 128, 32 },
                      	
	[BR_CHAR_0] =    	{   0, 128,  32, 32 },
	[BR_CHAR_1] =    	{  32, 128,  32, 32 },
	[BR_CHAR_2] =    	{  64, 128,  32, 32 },
	[BR_CHAR_3] =    	{  96, 128,  32, 32 },
	[BR_CHAR_4] =    	{ 128, 128,  32, 32 },
                      	
	[BR_CHAR_5] =    	{   0, 160,  32, 32 },
	[BR_CHAR_6] =    	{  32, 160,  32, 32 },
	[BR_CHAR_7] =    	{  64, 160,  32, 32 },
	[BR_CHAR_8] =    	{  96, 160,  32, 32 },
	[BR_CHAR_9] =    	{ 128, 160,  32, 32 },

	[BR_GAME_OVER] = 	{   0, 192, 192, 64 },
};

enum sample {
	SAMPLE_START,
	SAMPLE_EXPLODE,
	SAMPLE_NEW_BLOCK,
	SAMPLE_FALL,
	SAMPLE_SCORE,
	SAMPLE_BONUS,
	SAMPLE_BONUS2,
	SAMPLE_HURRY,
	SAMPLE_PAUSE,
	SAMPLE_GAME_OVER,

	NUM_SAMPLES
};

struct sample_t {
	char *fname;
	Mix_Chunk *chunk;
};

struct sample_t sample[NUM_SAMPLES] = {
	[SAMPLE_START] = 	{ "wav/start.wav" },
	[SAMPLE_EXPLODE] = 	{ "wav/explode.wav" },
	[SAMPLE_NEW_BLOCK] = 	{ "wav/new_block.wav" },
	[SAMPLE_FALL] = 	{ "wav/fall.wav" },
	[SAMPLE_SCORE] = 	{ "wav/score.wav" },
	[SAMPLE_BONUS] = 	{ "wav/bonus.wav" },
	[SAMPLE_BONUS2] = 	{ "wav/bonus2.wav" },
	[SAMPLE_HURRY] = 	{ "wav/hurry.wav" },
	[SAMPLE_PAUSE] = 	{ "wav/pause.wav" },
	[SAMPLE_GAME_OVER] = 	{ "wav/game_over.wav" },
};

static void game_callback(struct game_t *g, enum game_event event);
static int have_audio = 0;

int main(int argc, char **argv)
{
	struct game_t *g;
	SDL_Event ev;
	SDL_Surface *tmp;
	int i;
	int r;

	/*
	 * Init SDL
	 */

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	atexit(SDL_Quit);

	/*
	 * Init video
	 */

	screen = SDL_SetVideoMode(BOARD_W*32+3, BOARD_H*32+3, 0, 0);
	SDL_EnableKeyRepeat(100, 40);

	tmp = IMG_Load("img/game.png");
	blits = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);
	
	tmp = IMG_Load("img/background.png");
	background = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);

	/*
	 * Init audio
	 */

	r = Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 1024);
	if(r != 0) {
		printf("No audio: %s\n", Mix_GetError());
	} else {
		have_audio = 1;
	}

	if(have_audio) {
		Mix_AllocateChannels(16);
		for(i=0; i<NUM_SAMPLES; i++) {
			sample[i].chunk = Mix_LoadWAV(sample[i].fname);
			if(sample[i].chunk == NULL) {
				printf("Error loading wav: %s\n", Mix_GetError());
				exit(1);
			}
			Mix_VolumeChunk(sample[i].chunk, 64);
		}
	}

	/*
	 * Register timer
	 */

	SDL_AddTimer(40, game_timer, NULL);

	/*
	 * Start game
	 */

	g = game_new();
	game_register_callback(g, game_callback);

	while(SDL_WaitEvent(&ev)) {

		switch(ev.type) {

			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEMOTION:

				g->cursor_x = (ev.motion.x-16) / 32;
				g->cursor_y = ev.motion.y / 32;
				break;

			case SDL_MOUSEBUTTONDOWN:
				game_do(g, GAME_ACTION_FLIP);
				break;


			case SDL_KEYDOWN:

				if(ev.key.state == SDL_PRESSED) {

					switch(ev.key.keysym.sym) {

						case SDLK_ESCAPE:
							exit(0);
							break;

						case 'p':
							game_do(g, GAME_ACTION_PAUSE);
							break;

						case SDLK_UP:
						case 'k':
							game_do(g, GAME_ACTION_UP);
							break;

						case SDLK_DOWN:
						case 'j':
							game_do(g, GAME_ACTION_DOWN);
							break;

						case SDLK_LEFT:
						case 'h':
							game_do(g, GAME_ACTION_LEFT);
							break;

						case SDLK_RIGHT:
						case 'l':
							game_do(g, GAME_ACTION_RIGHT);
							break;

						case SDLK_SPACE:
							game_do(g, GAME_ACTION_FLIP);
							break;
						
						case 's':
							game_do(g, GAME_ACTION_START);
							break;

						default:
							break;
					}
				}
				break;

			case SDL_USEREVENT:

				game_do(g, GAME_ACTION_TICK);
				draw(g);
				break;

			default:
				break;

		}
	}

	return 0;
}


static void blit(enum blitrect id, int x, int y)
{
	SDL_Rect r;
	r.x = x;
	r.y = y;
	SDL_BlitSurface(blits, &blitrect[id], screen, &r);
}


void draw(struct game_t *g)
{
	struct cell_t *c;
	int x, y;
	static int blink_counter = 0;
	int blink;
	int i;
	char score[BOARD_W+1];

	if(blink_counter++ == 15) blink_counter = 0;
	blink = (blink_counter < 10);

	/*
	 * Clear screen
	 */

	SDL_FillRect(screen, NULL, 0);
	SDL_BlitSurface(background, NULL, screen, NULL); 

	/*
	 * Draw cells 
	 */

	for(y=0; y<BOARD_H; y++) {
		for(x=0; x<BOARD_W; x++) {

			c = &g->cell[x][y];

			if((c->contents > 0) && (c->contents <= g->num_blocks) && !(g->state == GAME_STATE_PAUSE)) {
				blit(c->contents -  1 + BR_BLOCK1, x*32, y*32);
				if(c->exploding) blit(c->exploding - 1 + BR_EXPLODING1, x*32, y*32);

			} else {
				blit(BR_BACKGROUND, x*32, y*32);
			}
				

		}
	}

	/*
	 * Draw cursor
	 */

	for(i=0; i<blink+1; i++) blit(BR_CURSOR, g->cursor_x * 32, g->cursor_y * 32);

	/*
	 * Draw score
	 */
	
	snprintf(score, sizeof(score), "%*d", BOARD_W, g->score_counter);
	for(x=0; x<strlen(score); x++) {
		if(score[x] != ' ') blit(BR_CHAR_0 + score[x] - '0', x*32, 0);
	}
	
	if(g->state == GAME_STATE_IDLE) blit(BR_PRESS_S_TO_START, 0, 32*6);

	if(g->state == GAME_STATE_GAME_OVER) {
		blit(BR_GAME_OVER, 0, 32*4);
		blit(BR_PRESS_S_TO_START, 0, 32*6);
	}
	
	if(g->state == GAME_STATE_PAUSE) {
		blit(BR_PAUSE, 32*1, 32*4);
		if(have_audio) {
			if(! Mix_Playing(15)) Mix_PlayChannel(15, sample[SAMPLE_PAUSE].chunk, 0);
		}
	}	


	SDL_UpdateRect(screen, 0, 0, 0, 0);

}


Uint32 game_timer(Uint32 interval, void *data)
{
	SDL_Event ev;

	ev.user.type = SDL_USEREVENT;
	ev.user.code = 0;
	SDL_PushEvent(&ev);

	return interval;
}

static void game_callback(struct game_t *g, enum game_event event)
{
	enum sample s[] = {
		[GAME_EVENT_START] = SAMPLE_START,
		[GAME_EVENT_EXPLODING ] =SAMPLE_EXPLODE,
		[GAME_EVENT_NEW_BLOCK] = SAMPLE_NEW_BLOCK,
		[GAME_EVENT_FALL] = SAMPLE_FALL,
		[GAME_EVENT_SCORE_UPDATE] = SAMPLE_SCORE,
		[GAME_EVENT_BONUS] = SAMPLE_BONUS,
		[GAME_EVENT_BONUS2] = SAMPLE_BONUS2,
		[GAME_EVENT_HURRY] = SAMPLE_HURRY,
		[GAME_EVENT_GAME_OVER] = SAMPLE_GAME_OVER,
	};

	if(have_audio) Mix_PlayChannel(-1, sample[s[event]].chunk, 0);
}

/*
 * End
 */

