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

Bar::Bar(int width, int height, int x, int y) : Surface(width, height, x, y), _color(0), _scale(0), _max(0), _value(0)
{
}

Bar::~Bar()
{
}

void Bar::setColor(Uint8 color)
{
	_color = color;
}

Uint8 Bar::getColor()
{
	return _color;
}

void Bar::setScale(double scale)
{
	_scale = scale;
}

double Bar::getScale()
{
	return _scale;
}

void Bar::setMax(double max)
{
	_max = max;
}

double Bar::getMax()
{
	return _max;
}

void Bar::setValue(double value)
{
	_value = value;
}

double Bar::getValue()
{
	return _value;
}

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