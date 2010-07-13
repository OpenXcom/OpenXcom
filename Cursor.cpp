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
#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "Palette.h"

/**
 * Sets up a cursor with the specified size and position
 * and hides the system cursor.
 * @note The size and position don't really matter since
 * it's a 9x13 shape, they're just there for inhertiance.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Cursor::Cursor(int width, int height, int x, int y) : Surface(width, height, x, y), _color(0)
{
	SDL_ShowCursor(SDL_DISABLE);
}

/**
 *
 */
Cursor::~Cursor()
{
	
}

/**
 * Automatically updates the cursor position
 * when the mouse moves.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void Cursor::handle(SDL_Event *ev, int scale)
{
	if (ev->type == SDL_MOUSEMOTION)
	{
		setX(ev->motion.x / scale);
		setY(ev->motion.y / scale);
		draw();
	}
}

/**
 * Changes the cursor's base color.
 * @param color Color value.
 */
void Cursor::setColor(Uint8 color)
{
	_color = color;
	draw();
}

/**
 * Returns the cursor's base color.
 * @return Color value.
 */
Uint8 Cursor::getColor()
{
	return _color;
}

/**
 * Draws a pointer-shaped cursor graphic.
 */
void Cursor::draw()
{
	Uint8 color = _color;
	int x1 = 0, y1 = 0, x2 = _width-1, y2 = _height-1;

	lock();
	for (int i = 0; i < 4; i++)
	{
		lineColor(getSurface(), x1, y1, x1, y2, Palette::getRGBA(this->getPalette(), color));
		lineColor(getSurface(), x1, y1, x2, _width-1, Palette::getRGBA(this->getPalette(), color));
		x1++;
		y1 += 2;
		y2--;
		x2--;
		color++;
	}
	this->setPixel(4, 8, --color);
	unlock();
}
