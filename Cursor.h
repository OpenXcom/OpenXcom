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
#ifndef OPENXCOM__CURSOR_H
#define OPENXCOM__CURSOR_H

#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "Surface.h"

class Cursor : public Surface
{
private:
	Uint8 _color;

public:
	Cursor(int width, int height, int x = 0, int y = 0);
	~Cursor();
	void handle(SDL_Event *ev, int scale);
	void setColor(Uint8 color);
	Uint8 getColor();
	void blit(Surface *surface);
};

#endif