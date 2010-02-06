/*
 * Copyright 2010 Daniel Albano
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM__SCREEN_H
#define OPENXCOM__SCREEN_H

#include "SDL.h"
#include "SDL_rotozoom.h"
#include "Surface.h"

class Screen
{
private:
	Surface *_surface;
	SDL_Surface *_screen;
	int _width, _height, _bpp;
	int _scale;
	Uint32 _flags;
public:
	Screen(int width, int height, int bpp);
	~Screen();
	Surface *getSurface();
	void flip();
	void clear();
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	SDL_Color* getPalette();
	void setResolution(int width, int height);
	void setFullscreen(bool full);
	int getScale();
	void setScale(int amount);
};

#endif