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
#include "Cursor.h"

Cursor::Cursor(int width, int height, int x, int y) : Surface(width, height, x, y), _color(0)
{
	SDL_ShowCursor(SDL_DISABLE);
}

Cursor::~Cursor()
{
	Surface::~Surface();
}

void Cursor::handle(SDL_Event *ev, int scale)
{
	if (ev->type == SDL_MOUSEMOTION)
	{
		setX(ev->motion.x / scale);
		setY(ev->motion.y / scale);
	}
}

void Cursor::setColor(Uint8 color)
{
	_color = color;
}

Uint8 Cursor::getColor()
{
	return _color;
}

void Cursor::blit(Surface *surface)
{
	Uint8 color = _color;
	int x1 = 0, y1 = 0, x2 = _width-1, y2 = _height-1;

	for (int i = 0; i < 5; i++)
	{
		lineColor(getSurface(), x1, y1, x1, y2, color);
		lineColor(getSurface(), x1, y1, x2, _width-1, color);
		x1++;
		y1 += 2;
		y2--;
		x2--;
		color++;
	}

	Surface::blit(surface);
}