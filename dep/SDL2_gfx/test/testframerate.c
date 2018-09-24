/* 

TestFramerate.c: test/sample program for framerate manager 

Copyright (C) 2012-2014 Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "SDL_test_common.h"

#include "SDL2_gfxPrimitives.h"
#include "SDL2_framerate.h"

static SDLTest_CommonState *state;

/* Screen size */
#define WIDTH	DEFAULT_WINDOW_WIDTH
#define HEIGHT	DEFAULT_WINDOW_HEIGHT

/* Pause flag */
int pause = 0;

/* Done flag */
int done = 0;

/* Current message */
char *messageText;

void ClearScreen(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderClear(renderer);
}

/* Global draw state */
int timeout = 0;
int x = WIDTH / 2;
int y = HEIGHT / 2;
int dx = 7;
int dy = 5;
int r = 255; 
int g = 255;
int b = 255;
Uint32 time_passed = 0;

void Draw(SDL_Renderer *renderer, FPSmanager *fpsm)
{
	int newRate;
	int currentRate;

		/* FPSmanager: get current rate */
		currentRate = SDL_getFramerate(fpsm);

		/* Set/switch framerate */
		timeout--;
		if (timeout < 0) {
			/* New rate */
			newRate = 5 + 5 * (rand() % 10);

			/* Update timeout */
			timeout = 2 * newRate;

			/* New Color */
			r = rand() & 255;
			g = rand() & 255;
			b = rand() & 255;

			/* FPSmanager: set new rate */
			SDL_setFramerate(fpsm, newRate);
		}

		/* Black screen */
		ClearScreen(renderer);

		/* Move */
		x += dx;
		y += dy;

		/* Reflect */
		if ((x<0) || (x>WIDTH)) { dx=-dx; }
		if ((y<0) || (y>HEIGHT)) { dy=-dy; }

		/* Draw */
		filledCircleRGBA (renderer,x,y,30,r,g,b,255);
		circleRGBA(renderer,x,y,30,255,255,255,255);

		/* Report set rate and current delay (from last iteration) */
		if (time_passed > 0) {
			SDL_snprintf(messageText, 1024, "Set rate: %4iHz   Last delay=%4ims / Calc.Rate=%4iHz", currentRate, time_passed, 1000 / time_passed); 
			stringRGBA (renderer, WIDTH/2 - 4*strlen(messageText),HEIGHT-24,messageText,255,255,255,255);
		}

		/* Simulate some random delay from drawing, business logic, network interaction, etc. */
		SDL_Delay(1 + rand() % 10);

		/* Display */
		SDL_RenderPresent(renderer);

		/* FPSmanager: Delay to a fixed rate */                   
		time_passed = SDL_framerateDelay(fpsm);  
}



/* ====== Main */

int main(int argc, char *argv[])
{
	int i;
	SDL_Event event;
	Uint32 then, now, frames;
	FPSmanager fpsm;

	/* Initialize test framework */
	state = SDLTest_CommonCreateState(argv, SDL_INIT_VIDEO);
	if (!state) {
		return 1;
	}

    SDL_Log("SDL2_gfx %i.%i.%i: testframerate", SDL2_GFXPRIMITIVES_MAJOR, SDL2_GFXPRIMITIVES_MINOR, SDL2_GFXPRIMITIVES_MICRO);
    SDL_Log("Platform: %s", SDL_GetPlatform());

	for (i = 1; i < argc;) {
		int consumed;

		consumed = SDLTest_CommonArg(state, i);
		if (consumed == 0) {
                   consumed = -1;
                   if (SDL_strcasecmp(argv[i], "--x") == 0) {
                      if (argv[i + 1]) {
						  x = SDL_atoi(argv[i + 1]);
                         if (x < 0) x = 0;
                         if (x >= WIDTH) x = WIDTH - 1;
                         consumed = 2;
                      }
				   }
                   else if (SDL_strcasecmp(argv[i], "--y") == 0) {
                      if (argv[i + 1]) {
                         y = SDL_atoi(argv[i + 1]);
                         if (y < 0) y = 0;
                         if (y >= HEIGHT) y = HEIGHT - 1;
                         consumed = 2;
                      }
                   }
                }
                
		if (consumed < 0) {
			SDL_Log("Usage: %s %s [--x #] [--y #]\n",
				argv[0], SDLTest_CommonUsage(state));
			return 1;
		}
		i += consumed;
	}
	if (!SDLTest_CommonInit(state)) {
		return 2;
	}

	/* Create the windows and initialize the renderers */
	for (i = 0; i < state->num_windows; ++i) {
		SDL_Renderer *renderer = state->renderers[i];
                SDL_RendererInfo info;
                SDL_GetRendererInfo(state->renderers[i], &info);		                
                SDL_Log("Renderer %i: %s %s", i, info.name, (info.flags | SDL_RENDERER_ACCELERATED) ? "(Accelerated)" : "");		
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
	}

	/* Initialize random number generator */
	srand((unsigned int)time(NULL));

	/* Create common message buffer */
	messageText = (char *)SDL_malloc(1024);
	if (messageText == NULL) {
		return -1;
	}

	/* Initialize framerate manager */
	SDL_initFramerate(&fpsm);

	/* Main render loop */
	frames = 0;
	then = SDL_GetTicks();
	done = 0;
	while (!done) {
		/* Check for events */
		++frames;
		while (SDL_PollEvent(&event) && !done) {
			SDLTest_CommonEvent(state, &event, &done);
		}

        /* Do all the drawing work */
		Draw(state->renderers[0], &fpsm);
	}

	/* Print out some timing information */
	now = SDL_GetTicks();
	if (now > then) {
		double fps = ((double) frames * 1000) / (now - then);
		SDL_Log("%2.2f frames per second\n", fps);
	}

	/* Free common message buffer */
	if (messageText) {
		free(messageText);
	}

	/* Shutdown SDL */
	SDLTest_CommonQuit(state);

	return 0;
}

