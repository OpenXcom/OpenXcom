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
#ifndef OPENXCOM__SURFACE_H
#define OPENXCOM__SURFACE_H

#include "SDL.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Surface
{
protected:
	SDL_Surface *_surface;
	int _width, _height, _x, _y;
	SDL_Rect _crop;
	bool _visible;
public:
	Surface(int width, int height, int x = 0, int y = 0);
	~Surface();
	void loadScr(string filename);
	void loadSpk(string filename);
	void clear();
	void offset(int off);
	void invert(Uint8 mid);
	virtual void blit(Surface *surface);
	void copy(Surface *surface);
	virtual void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	SDL_Color* getPalette();
	void setX(int x);
	int getX();
	void setY(int y);
	int getY();
	void setCrop(SDL_Rect *crop);
	SDL_Rect *getCrop();
	Uint8 Surface::getPixel(int x, int y);
	SDL_Surface* getSurface();
	int getWidth();
	int getHeight();
	void setVisible(bool visible);
	bool getVisible();
};

#endif