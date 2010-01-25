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
#ifndef __FONT_H__
#define __FONT_H__

#include <map>
#include "SDL.h"
#include "Surface.h"

using namespace std;

class Font
{
private:
	Surface *_surface;
	int _width, _height, _nchar;
	map<int, SDL_Rect> _chars;
	// For some reason the X-Com small font is smooshed together by one pixel...
	int _spacing;
public:
	Font(int width, int height, int nchar, int spacing = 0);
	~Font();
	void load();
	Surface *getChar(char c);
	int getWidth();
	int getHeight();
	int getSpacing();
	Surface *getSurface();
};

#endif