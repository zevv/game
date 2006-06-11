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
static SDL_Surface *load_img(char *fname);

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *help;
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

	BR_BONUS_1,
	BR_BONUS_2,
	BR_BONUS_3,
		
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
	
	[BR_BONUS_1] =		{ 160,  64,  32, 32 },
	[BR_BONUS_2] =		{ 192,  64,  32, 32 },
	[BR_BONUS_3] =		{ 224,  64,  32, 32 },
		
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
	SAMPLE_BONUS_1,
	SAMPLE_NEW_BLOCK,
	SAMPLE_FALL,
	SAMPLE_SCORE,
	SAMPLE_BONUS_2,
	SAMPLE_BONUS_3,
	SAMPLE_HURRY,
	SAMPLE_PAUSE,
	SAMPLE_GAME_OVER,
	SAMPLE_EARTHQUAKE,

	NUM_SAMPLES
};

struct sample_t {
	char *fname;
	Mix_Chunk *chunk;
};

struct sample_t sample_list[NUM_SAMPLES] = {
	[SAMPLE_START] = 	{ "wav/start.wav" },
	[SAMPLE_NEW_BLOCK] = 	{ "wav/new_block.wav" },
	[SAMPLE_FALL] = 	{ "wav/fall.wav" },
	[SAMPLE_SCORE] = 	{ "wav/score.wav" },
	[SAMPLE_BONUS_1] = 	{ "wav/explode.wav" },
	[SAMPLE_BONUS_2] = 	{ "wav/bonus.wav" },
	[SAMPLE_BONUS_3] = 	{ "wav/bonus2.wav" },
	[SAMPLE_HURRY] = 	{ "wav/hurry.wav" },
	[SAMPLE_PAUSE] = 	{ "wav/pause.wav" },
	[SAMPLE_GAME_OVER] = 	{ "wav/game_over.wav" },
	[SAMPLE_EARTHQUAKE] = 	{ "wav/earthquake.wav" },
};


struct floating_score {
	int x;
	int y;
	int dx;
	int dy;
	int points;
	int visible;
};

static struct floating_score floating_score;
static void game_callback(struct game_t *g, struct game_event *event);
static int have_audio = 0;

int main(int argc, char **argv)
{
	struct game_t *g;
	SDL_Event ev;
	SDL_Surface *icon;
	int i;
	int r;
	Mix_Music *music;
	int volume;
	struct game_action action;

	/*
	 * Init SDL
	 */

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	atexit(SDL_Quit);
	SDL_WM_SetCaption("Game", "Game");

	/*
	 * Set icon and init video
	 */

	icon = IMG_Load("img/icon.png");
	if(icon) {
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}

	screen = SDL_SetVideoMode(BOARD_W*32+3, BOARD_H*32+3, 0, 0);
	SDL_EnableKeyRepeat(100, 40);

	blits      = load_img("img/game.png");
	background = load_img("img/background.png");
	help       = load_img("img/help.png");

	/*
	 * Init audio
	 */

	r = Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 1024);
	if(r != 0) {
		printf("No audio: %s\n", Mix_GetError());
	} else {
		have_audio = 1;

		music = Mix_LoadMUS("mp3/track-01.mp3");
		if(music) {
			Mix_VolumeMusic(64);
			Mix_PlayMusic(music, -1);
		} else {
			printf("Can't load music: %s\n", Mix_GetError());
		}

		Mix_AllocateChannels(16);
		for(i=0; i<NUM_SAMPLES; i++) {
			sample_list[i].chunk = Mix_LoadWAV(sample_list[i].fname);
			if(sample_list[i].chunk == NULL) {
				fprintf(stderr, "Error loading wav: %s\n", Mix_GetError());
				exit(1);
			}
			Mix_VolumeChunk(sample_list[i].chunk, 128);
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
	if(g == NULL) exit(1);
	game_register_callback(g, game_callback);

	while(SDL_WaitEvent(&ev)) {

		switch(ev.type) {

			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEMOTION:

				action.id = GAME_ACTION_SET_CURSOR;
				action.data.set_cursor.x = (ev.motion.x-16) / 32;
				action.data.set_cursor.y = ev.motion.y / 32;
				game_do(g, &action);
				break;

			case SDL_MOUSEBUTTONDOWN:
				action.id = GAME_ACTION_FLIP;
				game_do(g, &action);
				break;


			case SDL_KEYDOWN:

				if(ev.key.state == SDL_PRESSED) {

					switch(ev.key.keysym.sym) {

						case SDLK_ESCAPE:
							exit(0);
							break;

						case 'p':
							action.id = GAME_ACTION_PAUSE; 
							game_do(g, &action);
							break;

						case SDLK_UP:
						case 'k':
							action.id = GAME_ACTION_UP; 
							game_do(g, &action);
							break;

						case SDLK_DOWN:
						case 'j':
							action.id = GAME_ACTION_DOWN; 
							game_do(g, &action);
							break;

						case SDLK_LEFT:
						case 'h':
							action.id = GAME_ACTION_LEFT; 
							game_do(g, &action);
							break;

						case SDLK_RIGHT:
						case 'l':
							action.id = GAME_ACTION_RIGHT; 
							game_do(g, &action);
							break;

						case SDLK_SPACE:
							action.id = GAME_ACTION_FLIP; 
							game_do(g, &action);
							break;
						
						case 's':
							action.id = GAME_ACTION_START; 
							game_do(g, &action);
							break;

						case 'e':
							action.id = GAME_ACTION_EARTHQUAKE; 
							game_do(g, &action);
							break;
						
						case 'm':
							Mix_VolumeMusic(0);
							break;

						case '-':
							volume = Mix_VolumeMusic(-1) - 10;
							if(volume < 0) volume = 0;
							Mix_VolumeMusic(volume);
							break;

						case '=':
							volume = Mix_VolumeMusic(-1) + 10;
							if(volume > 128) volume = 128;
							Mix_VolumeMusic(volume);
							break;


						default:
							break;
					}
				}
				break;

			case SDL_USEREVENT:

				action.id = GAME_ACTION_TICK; 
				game_do(g, &action);
				draw(g);
				break;

			default:
				break;

		}
	}

	return 0;
}


static void blit(struct game_t *g, enum blitrect id, int x, int y)
{
	SDL_Rect r;
	int delta;
	if(g->earthquake_counter) {
		delta = g->earthquake_counter / 2;
		if(delta < 1) delta = 1;
		x += (rand() % delta) - delta/2;
		y += (rand() % delta) - delta/2;
	}
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
				blit(g, c->contents -  1 + BR_BLOCK1, x*32, y*32);
				if(c->exploding) blit(g, c->exploding - 1 + BR_EXPLODING1, x*32, y*32);

			} else {
				blit(g, BR_BACKGROUND, x*32, y*32);
			}
				

		}
	}

	/*
	 * Draw cursor
	 */

	for(i=0; i<blink+1; i++) blit(g, BR_CURSOR, g->cursor_x * 32, g->cursor_y * 32);

	/*
	 * Draw score
	 */
	
	snprintf(score, sizeof(score), "%*d", BOARD_W, g->score_counter);
	for(x=0; x<strlen(score); x++) {
		if(score[x] != ' ') blit(g, BR_CHAR_0 + score[x] - '0', x*32, 0);
	}
	
	if(g->state == GAME_STATE_IDLE) {
		blit(g, BR_PRESS_S_TO_START, 0, 32*6);
		SDL_BlitSurface(help, NULL, screen, NULL); 
	}

	if(g->state == GAME_STATE_GAME_OVER) {
		blit(g, BR_GAME_OVER, 0, 32*4);
		blit(g, BR_PRESS_S_TO_START, 0, 32*6);
	}
	
	if(g->state == GAME_STATE_PAUSE) {
		blit(g, BR_PAUSE, 32*1, 32*4);
		if(have_audio) {
			if(! Mix_Playing(15)) Mix_PlayChannel(15, sample_list[SAMPLE_PAUSE].chunk, 0);
		}
	}	

	/*
	 * Draw floating score
	 */

	if(floating_score.visible) {
		if(floating_score.points ==  5) blit(g, BR_BONUS_1, floating_score.x, floating_score.y);
		if(floating_score.points == 10) blit(g, BR_BONUS_2, floating_score.x, floating_score.y);
		if(floating_score.points == 15) blit(g, BR_BONUS_3, floating_score.x, floating_score.y);
		floating_score.x += floating_score.dx;
		floating_score.y += floating_score.dy;

		if((floating_score.x < 0) || (floating_score.y <0)) {
			floating_score.visible = 0;
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

static void game_callback(struct game_t *g, struct game_event *event)
{
	struct sample_t *sample = NULL;

	switch(event->id) {
		
		case GAME_EVENT_START:
			sample = &sample_list[SAMPLE_START];
			break;

		case GAME_EVENT_EXPLODING :
			if(event->data.exploding.blocks == 3) sample = &sample_list[SAMPLE_BONUS_1];
			if(event->data.exploding.blocks == 4) sample = &sample_list[SAMPLE_BONUS_2];
			if(event->data.exploding.blocks == 5) sample = &sample_list[SAMPLE_BONUS_3];
			if(event->data.exploding.blocks == 6) sample = &sample_list[SAMPLE_BONUS_3];

			floating_score.x = event->data.exploding.x * 32;
			floating_score.y = event->data.exploding.y * 32;
			floating_score.points = event->data.exploding.points;
			floating_score.dx = -(floating_score.x - BOARD_W*32/2) / 10;
			floating_score.dy = -(rand() % 16) - 12;
			floating_score.visible = 1;

			break;

		case GAME_EVENT_NEW_BLOCK:
			sample = &sample_list[SAMPLE_NEW_BLOCK];
			break;

		case GAME_EVENT_FALL:
			sample = &sample_list[SAMPLE_FALL];
			break;

		case GAME_EVENT_SCORE_UPDATE:
			sample = &sample_list[SAMPLE_SCORE];
			break;

		case GAME_EVENT_HURRY:
			sample = &sample_list[SAMPLE_HURRY];
			break;

		case GAME_EVENT_GAME_OVER:
			sample = &sample_list[SAMPLE_GAME_OVER];
			break;

		case GAME_EVENT_EARTHQUAKE:
			sample = &sample_list[SAMPLE_EARTHQUAKE];
			break;

	};

	if(have_audio && sample) Mix_PlayChannel(-1, sample->chunk, 0);
}


/*
 * Load image and convert to display format
 */

static SDL_Surface *load_img(char *fname)
{
	SDL_Surface *surf1, *surf2;

	surf1 = IMG_Load(fname);
	if(surf1 == NULL) {
		fprintf(stderr, "Can't load image '%s': %s\n", fname, SDL_GetError());
		exit(1);
	}

	surf2 = SDL_DisplayFormatAlpha(surf1);
	if(surf2 == NULL) {
		fprintf(stderr, "Can't convert image to display format: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_FreeSurface(surf1);

	return surf2;
}

/*
 * End
 */

