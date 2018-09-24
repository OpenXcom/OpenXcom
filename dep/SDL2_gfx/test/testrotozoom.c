/*

TestRotozoom.c: test/sample program for SDL_Surface rotozoom functions 

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
#include "SDL2_rotozoom.h"

static SDLTest_CommonState *state;

/* Screen size */
#define WIDTH	DEFAULT_WINDOW_WIDTH
#define HEIGHT	DEFAULT_WINDOW_HEIGHT

/* Done flag */
int done = 0;

/* Custom rotation setup */
double custom_angle=0.0;
double custom_fx=1.0;
double custom_fy=1.0;
int custom_smooth=0;

/* Delay between frames */
int delay;

/* Current message */
char *messageText;

#define POSITION_CENTER		1
#define POSITION_BOTTOMRIGHT	2

void RotatePicture (SDL_Surface *picture, int rotate, int flip, int smooth, int position) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Texture *rotozoom_texture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	double angle = 0.0, zoomf = 1.0, zoomfx = 1.0, zoomfy = 1.0;
	SDL_Renderer *renderer = state->renderers[0];
	SDL_Event event;

	SDL_Log("%s\n", messageText);

	/* Rotate and display the picture */
	framemax=4*360; 
	frameinc=1;
	for (framecount=-360; framecount<framemax && !done; framecount += frameinc) {
		while (SDL_PollEvent(&event)) SDLTest_CommonEvent(state, &event, &done);
		if ((framecount % 360)==0) frameinc++;
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
		zoomf=(float)(framecount+2*360)/(float)framemax;
		zoomf=1.5*zoomf*zoomf;
		
		/* Flip X factor */
		if (flip & 1) {
			zoomfx=-zoomf;
		} else {
			zoomfx=zoomf;
		}
		
		/* Flip Y factor */
		if (flip & 2) {
			zoomfy=-zoomf;
		} else {
			zoomfy=zoomf;
		}

		/* Are we in flipping mode? */
		if (flip) {
			angle=framecount*rotate;
			if (((framecount % 120)==0) || (delay>0)) {
				SDL_Log("  Frame: %i   Rotate: angle=%.2f  Zoom: x=%.2f y=%.2f\n",framecount,angle,zoomfx,zoomfy);
			}
			if ((rotozoom_picture=rotozoomSurfaceXY (picture, angle, zoomfx, zoomfy, smooth))!=NULL) {
				switch (position) {
				case POSITION_CENTER:
					dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;
					dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
					break;
				case POSITION_BOTTOMRIGHT:
					dest.x = (DEFAULT_WINDOW_WIDTH/2) - rotozoom_picture->w;
					dest.y = (DEFAULT_WINDOW_HEIGHT/2) - rotozoom_picture->h;
					break;
				}
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;

				/* Convert to texture and draw */
				rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
				SDL_FreeSurface(rotozoom_picture);
				if (!rotozoom_texture) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
					break;
				}				
				SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
				SDL_DestroyTexture(rotozoom_texture);
			} else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotozoom image\n");
			}
		} else {
			angle=framecount*rotate;
			if ((framecount % 120)==0) {
				SDL_Log("  Frame: %i   Rotate: angle=%.2f  Zoom: f=%.2f \n",framecount,angle,zoomf);
			}
			if ((rotozoom_picture=rotozoomSurface (picture, angle, zoomf, smooth))!=NULL) {
				switch (position) {
				case POSITION_CENTER:
					dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;
					dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
					break;
				case POSITION_BOTTOMRIGHT:
					dest.x = (DEFAULT_WINDOW_WIDTH/2) - rotozoom_picture->w;
					dest.y = (DEFAULT_WINDOW_HEIGHT/2) - rotozoom_picture->h;
					break;
				}
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;

				/* Convert to texture and draw */
				rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
				SDL_FreeSurface(rotozoom_picture);
				if (!rotozoom_texture) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
					break;
				}				
				SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
				SDL_DestroyTexture(rotozoom_texture);
			} else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotozoom image\n");
			}
		}


		stringRGBA(renderer, 8, 8, messageText, 255, 255, 255, 255);

		/* Display */
		SDL_RenderPresent(renderer);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	if (rotate) {
		/* Final display with angle=0 */
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
		if (flip) {
			if ((rotozoom_picture=rotozoomSurfaceXY (picture, 0.01, zoomfx, zoomfy, smooth))!=NULL) {
				dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;;
				dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;

				/* Convert to texture and draw */
				rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
				SDL_FreeSurface(rotozoom_picture);
				if (!rotozoom_texture) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
					return;
				}				
				SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
				SDL_DestroyTexture(rotozoom_texture);
			} else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotozoom image\n");
			}		
		} else {
			if ((rotozoom_picture=rotozoomSurface (picture, 0.01, zoomf, smooth))!=NULL) {
				dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;;
				dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
				dest.w = rotozoom_picture->w;
				dest.h = rotozoom_picture->h;

				/* Convert to texture and draw */
				rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
				SDL_FreeSurface(rotozoom_picture);
				if (!rotozoom_texture) {
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
					return;
				}				
				SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
				SDL_DestroyTexture(rotozoom_texture);
			} else {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotozoom image\n");
			}		
		}

		stringRGBA(renderer, 8, 8, messageText, 255, 255, 255, 255);

		/* Display */
		SDL_RenderPresent(renderer);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

void ZoomPicture (SDL_Surface *picture, int smooth) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Texture *rotozoom_texture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	double zoomxf,zoomyf;
	SDL_Renderer *renderer = state->renderers[0];
	SDL_Event event;

	SDL_Log("%s\n", messageText);

	/* Zoom and display the picture */
	framemax=4*360; frameinc=1;
	for (framecount=360; framecount<framemax && !done; framecount += frameinc) {
		while (SDL_PollEvent(&event)) SDLTest_CommonEvent(state, &event, &done);
		if ((framecount % 360)==0) frameinc++;
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
		zoomxf=(float)framecount/(float)framemax;
		zoomxf=1.5*zoomxf*zoomxf;
		zoomyf=0.5+fabs(1.0*sin((double)framecount/80.0));
		if ((framecount % 120)==0) {
			SDL_Log("  Frame: %i   Zoom: x=%.2f y=%.2f\n",framecount,zoomxf,zoomyf);
		}
		if ((rotozoom_picture=zoomSurface (picture, zoomxf, zoomyf, smooth))!=NULL) {
			dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;;
			dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
			dest.w = rotozoom_picture->w;
			dest.h = rotozoom_picture->h;

			/* Convert to texture and draw */
			rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
			SDL_FreeSurface(rotozoom_picture);
			if (!rotozoom_texture) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
				return;
			}				
			SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
			SDL_DestroyTexture(rotozoom_texture);
		} else {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't zoom image\n");
		}

		stringRGBA(renderer, 8, 8, messageText, 255, 255, 255, 255);

		/* Display */
		SDL_RenderPresent(renderer);

		/* Maybe delay */
		if (delay>0) {
			SDL_Delay(delay);
		}
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

void RotatePicture90Degrees (SDL_Surface *picture) 
{
	SDL_Surface *rotozoom_picture;
	SDL_Texture *rotozoom_texture;
	SDL_Rect dest;
	int framecount, framemax, frameinc;
	int numClockwiseTurns;
	SDL_Renderer *renderer = state->renderers[0];
	SDL_Event event;

	SDL_Log("%s\n", messageText);

	/* Rotate and display the picture */
	framemax = 21;
	frameinc = 1;
	numClockwiseTurns = -4;
	for (framecount=0; framecount<framemax && !done; framecount += frameinc) {
		while (SDL_PollEvent(&event)) SDLTest_CommonEvent(state, &event, &done);
		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
		SDL_RenderClear(renderer);
		SDL_Log("  Frame: %i   Rotate90: %i clockwise turns\n",framecount,numClockwiseTurns);
		if ((rotozoom_picture = rotateSurface90Degrees(picture, numClockwiseTurns))!=NULL) {
			dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;;
			dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
			dest.w = rotozoom_picture->w;
			dest.h = rotozoom_picture->h;

			/* Convert to texture and draw */
			rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
			SDL_FreeSurface(rotozoom_picture);
			if (!rotozoom_texture) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
				return;
			}				
			SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
			SDL_DestroyTexture(rotozoom_texture);
		} else {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotate image: %s\n", SDL_GetError());
		}

		stringRGBA(renderer, 8, 8, messageText, 255, 255, 255, 255);

		/* Display */
		SDL_RenderPresent(renderer);

		/* Always delay */
		SDL_Delay(500);
		if (delay>0) {
			SDL_Delay(delay);
		}

		numClockwiseTurns++;
	}

	/* Pause for a sec */
	SDL_Delay(1000);
}

#define ROTATE_OFF	0
#define ROTATE_ON	1

#define FLIP_OFF	0
#define FLIP_X		1
#define FLIP_Y		2
#define FLIP_XY		3

void CustomTest(SDL_Surface *picture, double a, double x, double y, int smooth){
	SDL_Surface *rotozoom_picture;
	SDL_Texture *rotozoom_texture;
	SDL_Rect dest;
	SDL_Renderer *renderer = state->renderers[0];

	SDL_Log("%s\n", messageText);
	SDL_Log("  Frame: C   Rotate: angle=%.2f  Zoom: fx=%.2f fy=%.2f \n",a,x,y);

	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);
	if ((rotozoom_picture=rotozoomSurfaceXY (picture, a, x, y, smooth))!=NULL) {
		dest.x = (DEFAULT_WINDOW_WIDTH - rotozoom_picture->w)/2;;
		dest.y = (DEFAULT_WINDOW_HEIGHT - rotozoom_picture->h)/2;
		dest.w = rotozoom_picture->w;
		dest.h = rotozoom_picture->h;

		/* Convert to texture and draw */
		rotozoom_texture = SDL_CreateTextureFromSurface(renderer, rotozoom_picture);
		SDL_FreeSurface(rotozoom_picture);
		if (!rotozoom_texture) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
			return;
		}				
		SDL_RenderCopy(renderer, rotozoom_texture, NULL, &dest);				
		SDL_DestroyTexture(rotozoom_texture);
	} else {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't rotozoom image\n");
	}

	/* Display */
	SDL_RenderPresent(renderer);

	/* Maybe delay */
	if (delay>0) {
		SDL_Delay(delay);
	}

	SDL_Delay(1000);		
}


void Draw (int start, int end)
{
	SDL_Surface *picture, *picture_again;
	char *bmpfile;
	SDL_Renderer *renderer = state->renderers[0];

	/* Define masking bytes */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000; 
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00; 
	Uint32 amask = 0x000000ff;
#else
	Uint32 amask = 0xff000000; 
	Uint32 bmask = 0x00ff0000;
	Uint32 gmask = 0x0000ff00; 
	Uint32 rmask = 0x000000ff;
#endif

	/* --------- 8 bit tests -------- */

	if (start<=6) {

		/* Message */
		SDL_Log("8 bit tests ...\n");

		/* Load the image into a surface */
		bmpfile = "sample8.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		if (start <= 1) {
			SDL_snprintf(messageText, 1024, "1.  rotozoom: Rotating and zooming");
			RotatePicture(picture,ROTATE_ON,FLIP_OFF,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done8bit;
		}
		if (end == 1) goto done8bit;

		if (start <= 2) {
			SDL_snprintf(messageText, 1024, "2.a  rotozoom: Just zooming (angle=0, center)");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done8bit;
			SDL_snprintf(messageText, 1024, "2.b  rotozoom: Just zooming (angle=0, bottomright)");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_OFF,POSITION_BOTTOMRIGHT);
			if (done) goto done8bit;
		}
		if (end == 2) goto done8bit;

		if (start <= 3) {
			SDL_snprintf(messageText, 1024, "3.  zoom: Just zooming");
			ZoomPicture(picture,SMOOTHING_OFF);
			if (done) goto done8bit;
		}
		if (end == 3) goto done8bit;

		if (start <= 4) {
			SDL_snprintf(messageText, 1024, "4.  rotozoom: Rotating and zooming, interpolation on but unused");
			RotatePicture(picture,ROTATE_ON,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done8bit;
		}
		if (end == 4) goto done8bit;

		if (start <= 5) {
			SDL_snprintf(messageText, 1024, "5.a  rotozoom: Just zooming (angle=0, center), interpolation on but unused");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done8bit;
			SDL_snprintf(messageText, 1024, "5.b  rotozoom: Just zooming (angle=0, bottomright), interpolation on but unused");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_BOTTOMRIGHT);
			if (done) goto done8bit;
		}
		if (end == 5) goto done8bit;

		if (start <= 6) {
			SDL_snprintf(messageText, 1024, "6.  zoom: Just zooming, interpolation on but unused");
			ZoomPicture(picture,SMOOTHING_ON);
			if (done) goto done8bit;
		}
		if (end == 6) goto done8bit;

done8bit:

		/* Free the picture */
		SDL_FreeSurface(picture);
		if (done) return;
		if (end <= 6) return;
	}

	/* -------- 24 bit test --------- */

	if (start<=12) {

		/* Message */
		SDL_Log("24 bit tests ...\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* Add white frame */
		rectangleColor(renderer, 0, 0, picture->w-1, picture->h-1, 0xffffffff);

		if (start <= 7) {
			SDL_snprintf(messageText, 1024, "7.  rotozoom: Rotating and zooming, no interpolation");
			RotatePicture(picture,ROTATE_ON,FLIP_OFF,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done24bit;			
		}
		if (end == 7) goto done24bit;

		if (start <= 8) {
			SDL_snprintf(messageText, 1024, "8a.  rotozoom: Just zooming (angle=0), no interpolation, centered");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "8b.  rotozoom: Just zooming (angle=0), no interpolation, corner");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_OFF,POSITION_BOTTOMRIGHT);
			if (done) goto done24bit;			

			SDL_snprintf(messageText, 1024, "8c.  rotozoom: Just zooming (angle=0), X flip, no interpolation, centered");
			RotatePicture(picture,ROTATE_OFF,FLIP_X,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "8d.  rotozoom: Just zooming (angle=0), Y flip, no interpolation, centered");
			RotatePicture(picture,ROTATE_OFF,FLIP_Y,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done24bit;			
						
			SDL_snprintf(messageText, 1024, "8e.  rotozoom: Just zooming (angle=0), XY flip, no interpolation, centered");
			RotatePicture(picture,ROTATE_OFF,FLIP_XY,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto done24bit;			
		}
		if (end == 8) goto done24bit;

		if (start <= 9) {
			SDL_snprintf(messageText, 1024, "9.  zoom: Just zooming, no interpolation");
			ZoomPicture(picture,SMOOTHING_OFF);
			if (done) goto done24bit;			
		}
		if (end == 9) goto done24bit;

		if (start <= 10) {
			SDL_snprintf(messageText, 1024, "10. rotozoom: Rotating and zooming, with interpolation");
			RotatePicture(picture,ROTATE_ON,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done24bit;			
		}
		if (end == 10) goto done24bit;

		if (start <= 11) {
			SDL_snprintf(messageText, 1024, "11a. rotozoom: Just zooming (angle=0), with interpolation, centered");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "11b. rotozoom: Just zooming (angle=0), with interpolation, corner");
			RotatePicture(picture,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_BOTTOMRIGHT);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "11c. rotozoom: Just zooming (angle=0), X flip, with interpolation, corner");
			RotatePicture(picture,ROTATE_OFF,FLIP_X,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "11d. rotozoom: Just zooming (angle=0), Y flip, with interpolation, corner");
			RotatePicture(picture,ROTATE_OFF,FLIP_Y,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done24bit;			
			
			SDL_snprintf(messageText, 1024, "11e. rotozoom: Just zooming (angle=0), XY flip, with interpolation, corner");
			RotatePicture(picture,ROTATE_OFF,FLIP_XY,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done24bit;			
		}
		if (end == 11) goto done24bit;

		if (start <= 12) {
			SDL_snprintf(messageText, 1024, "12. zoom: Just zooming, with interpolation");
			ZoomPicture(picture,SMOOTHING_ON);
			if (done) goto done24bit;			
		}
		if (end == 12) goto done24bit;

done24bit:

		/* Free the picture */
		SDL_FreeSurface(picture);
                if (done) return;			
		if (end <= 12) return;
	}

	/* -------- 32 bit test --------- */

	if (start<=16) {

		/* Message */
		SDL_Log("32 bit tests ...\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* New source surface is 32bit with defined RGBA ordering */
		/* Much faster to do this once rather than the routine on the fly */
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, rmask, gmask, bmask, amask);
		if (picture_again == NULL) goto done32bit;		
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		if (start <= 13) {
			SDL_snprintf(messageText, 1024, "13. Rotating and zooming, with interpolation (RGBA source)");
			RotatePicture(picture_again,ROTATE_ON,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done32bit;
		}
		if (end == 13) goto done32bit;

		if (start <= 14) {
			SDL_snprintf(messageText, 1024, "14.a Just zooming (angle=0, center), with interpolation (RGBA source)");
			RotatePicture(picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done32bit;
			SDL_snprintf(messageText, 1024, "14.b Just zooming (angle=0, bottomright), with interpolation (RGBA source)");
			RotatePicture(picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_BOTTOMRIGHT);
			if (done) goto done32bit;
		}
		if (end == 14) goto done32bit;

		SDL_FreeSurface(picture_again);
		picture_again=NULL;

		/* New source surface is 32bit with defined ABGR ordering */
		/* Much faster to do this once rather than the routine on the fly */
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Converting 24bit image into 32bit ABGR surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, amask, bmask, gmask, rmask);
		if (picture_again == NULL) goto done32bit;		
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		if (start <= 15) {
			SDL_snprintf(messageText, 1024, "15. Rotating and zooming, with interpolation (ABGR source)");
			RotatePicture(picture_again,ROTATE_ON,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done32bit;
		}
		if (end == 15) goto done32bit;

		if (start <= 16) {
			SDL_snprintf(messageText, 1024, "16.a Just zooming (angle=0,center), with interpolation (ABGR source)");
			RotatePicture(picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto done32bit;
			SDL_snprintf(messageText, 1024, "16.b Just zooming (angle=0,bottomright), with interpolation (ABGR source)");
			RotatePicture(picture_again,ROTATE_OFF,FLIP_OFF,SMOOTHING_ON,POSITION_BOTTOMRIGHT);
			if (done) goto done32bit;
		}
		if (end == 16) goto done32bit;

done32bit:

		/* Free the picture */
		SDL_FreeSurface(picture);
		if (picture_again) SDL_FreeSurface(picture_again);
		if (done) return;
		if (end <= 16) return;
	}

	/* -------- 32 bit flip test --------- */

	if (start<=22) {

		/* Message */
		SDL_Log("32 bit flip tests ...\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* Excercise flipping functions on 32bit RGBA */
		SDL_Log("Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, rmask, gmask, bmask, amask);
		if (picture_again == NULL) goto doneflip;
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		if (start <= 17) {
			SDL_snprintf(messageText, 1024, "17. Rotating with x-flip, no interpolation (RGBA source)");
			RotatePicture(picture_again,ROTATE_ON,FLIP_X,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 17) goto doneflip;

		if (start <= 18) {
			SDL_snprintf(messageText, 1024, "18. Rotating with y-flip, no interpolation");
			RotatePicture(picture_again,ROTATE_ON,FLIP_Y,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 18) goto doneflip;

		if (start <= 19) {
			SDL_snprintf(messageText, 1024, "19. Rotating with xy-flip, no interpolation");
			RotatePicture(picture_again,ROTATE_ON,FLIP_XY,SMOOTHING_OFF,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 19) goto doneflip;

		if (start <= 20) {
			SDL_snprintf(messageText, 1024, "20. Rotating with x-flip, with interpolation");
			RotatePicture(picture_again,ROTATE_ON,FLIP_X,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 20) goto doneflip;

		if (start <= 21) {
			SDL_snprintf(messageText, 1024, "21. Rotating with y-flip, with interpolation");
			RotatePicture(picture_again,ROTATE_ON,FLIP_Y,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 21) goto doneflip;

		if (start <= 22) {
			SDL_snprintf(messageText, 1024, "22. Rotating with xy-flip, with interpolation");
			RotatePicture(picture_again,ROTATE_ON,FLIP_XY,SMOOTHING_ON,POSITION_CENTER);
			if (done) goto doneflip;
		}
		if (end == 22) goto doneflip;

doneflip:

		/* Free the pictures */
		SDL_FreeSurface(picture);
		if (picture_again) SDL_FreeSurface(picture_again);
		if (done) return;
		if (end <= 22) return;
	}

	if (start<=24) {

		/* Message */
		SDL_Log("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* Excercise flipping functions on 32bit RGBA */
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, rmask, gmask, bmask, amask);
		if (picture_again == NULL) goto donecustom32bit;
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		SDL_snprintf(messageText, 1024, "23. CustomTest, values from commandline (32bit)");
		CustomTest(picture_again, custom_angle, custom_fx, custom_fy, custom_smooth);

donecustom32bit:

		/* Free the picture */
		SDL_FreeSurface(picture);
		if (picture_again) SDL_FreeSurface(picture_again);
		if (done) return;

		/* Message */
		SDL_Log("Loading 8bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample8.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if (picture == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		SDL_snprintf(messageText, 1024, "24. CustomTest, values from commandline (8bit)");
		CustomTest(picture, custom_angle, custom_fx, custom_fy, custom_smooth);

		/* Free the picture */
		SDL_FreeSurface(picture);
		if (done) return;
		if (end <= 24) return;
	}

	if (start<=25) {

		/* Message */
		SDL_Log("Loading 24bit image\n");

		/* Load the image into a surface */
		bmpfile = "sample24.bmp";
		SDL_Log("Loading picture: %s\n", bmpfile);
		picture = SDL_LoadBMP(bmpfile);
		if ( picture == NULL ) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", bmpfile, SDL_GetError());
			return;
		}

		/* New source surface is 32bit with defined RGBA ordering */
		SDL_Log("Converting 24bit image into 32bit RGBA surface ...\n");
		picture_again = SDL_CreateRGBSurface(SDL_SWSURFACE, picture->w, picture->h, 32, rmask, gmask, bmask, amask);
		if (picture_again == NULL) goto donerotate90;
		SDL_BlitSurface(picture,NULL,picture_again,NULL);

		/* Excercise rotate90 function on 32bit RGBA */
		SDL_snprintf(messageText, 1024, "25.  rotate90: Rotate 90 degrees clockwise (32bit)");
		RotatePicture90Degrees(picture_again);

donerotate90:

		/* Free the pictures */
		SDL_FreeSurface(picture);
		if (picture_again) SDL_FreeSurface(picture_again);
		if (done) return;
		if (end <= 25) return;
	}

	return;
}


/* ====== Main */

int main(int argc, char *argv[])
{
	int i;
	int testStart = 0;
	int testEnd = 25;
	SDL_Event event;
	Uint32 then, now, frames;

	/* Initialize test framework */
	state = SDLTest_CommonCreateState(argv, SDL_INIT_VIDEO);
	if (!state) {
		return 1;
	}

    SDL_Log("SDL2_gfx %i.%i.%i: testrotozoom", SDL2_GFXPRIMITIVES_MAJOR, SDL2_GFXPRIMITIVES_MINOR, SDL2_GFXPRIMITIVES_MICRO);
    SDL_Log("Platform: %s", SDL_GetPlatform());

	for (i = 1; i < argc;) {
		int consumed;

		consumed = SDLTest_CommonArg(state, i);
		if (consumed == 0) {
                   consumed = -1;
                   if (SDL_strcasecmp(argv[i], "--start") == 0) {
                      if (argv[i + 1]) {
                         testStart = SDL_atoi(argv[i + 1]);
                         if (testStart < 0) testStart = 0;
                         consumed = 2;
                      }
                   }
                   else if (SDL_strcasecmp(argv[i], "--end") == 0) {
                      if (argv[i + 1]) {
                         testEnd = SDL_atoi(argv[i + 1]);
                         if (testEnd < 0) testEnd = 0;
                         consumed = 2;
                      }
                   }
                }
                
		if (consumed < 0) {
			SDL_Log("Usage: %s %s [--start #] [--end #]\n",
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
                Draw(testStart, testEnd);
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
