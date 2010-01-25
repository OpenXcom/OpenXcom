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
#ifndef __SURFACESET_H__
#define __SURFACESET_H__

#include <map>
#include "SDL.h"
#include "Surface.h"

using namespace std;

class SurfaceSet
{
private:
	Surface *_surface;
	int _width, _height, _nframe;
	map<int, SDL_Rect> _frames;
public:
	SurfaceSet(int width, int height);
	~SurfaceSet();
	void loadPck(string filename);
	void loadDat(string filename);
	Surface *getFrame(int i);
	int getWidth();
	int getHeight();
	Surface *getSurface();
};

#endif