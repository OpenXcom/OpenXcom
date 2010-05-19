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
#include "Bar.h"

/**
 * Sets up a blank bar with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Bar::Bar(int width, int height, int x, int y) : Surface(width, height, x, y), _color(0), _scale(0), _max(0), _value(0)
{
}

/**
 *
 */
Bar::~Bar()
{
}

/**
 * Changes the color used to draw the border and contents.
 * @param color Color value.
 */
void Bar::setColor(Uint8 color)
{
	_color = color;
}

/**
 * Returns the color used to draw the bar.
 * @return Color value.
 */
Uint8 Bar::getColor()
{
	return _color;
}

/**
 * Changes the scale factor used to draw the bar values.
 * @param scale Scale in pixels/unit.
 */
void Bar::setScale(double scale)
{
	_scale = scale;
}

/**
 * Returns the scale factor used to draw the bar values.
 * @return Scale in pixels/unit.
 */
double Bar::getScale()
{
	return _scale;
}

/**
 * Changes the maximum value used to draw the outer border.
 * @param max Maximum value.
 */
void Bar::setMax(double max)
{
	_max = max;
}

/**
 * Returns the maximum value used to draw the outer border.
 * @return Maximum value.
 */
double Bar::getMax()
{
	return _max;
}

/**
 * Changes the value used to draw the inner contents.
 * @param value Current value.
 */
void Bar::setValue(double value)
{
	_value = value;
}

/**
 * Returns the value used to draw the inner contents.
 * @return Current value.
 */
double Bar::getValue()
{
	return _value;
}

/**
 * Blits the bar graphic onto another surface.
 * @param surface Pointer to surface to blit onto.
 */
void Bar::blit(Surface *surface)
{
	SDL_Rect square;

	square.x = 0;
	square.y = 0;
	square.w = (Uint16)(_scale * _max) + 2;
	square.h = _height;

	SDL_FillRect(_surface, &square, _color + 4);

	square.x++;
	square.y++;
	square.w -= 2;
	square.h -= 2;

	SDL_FillRect(_surface, &square, 0);
	
	square.w = (Uint16)(_scale * _value);

	SDL_FillRect(_surface, &square, _color);

	Surface::blit(surface);
}