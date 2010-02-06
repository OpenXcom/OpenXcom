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
#include "Font.h"

#define FIRST_CHAR '!'

using namespace std;

Font::Font(int width, int height, int nchar, int spacing) : _width(width), _height(height), _nchar(nchar), _chars(), _spacing(spacing)
{
	_surface = new Surface(width, height*nchar);
}

Font::~Font()
{
	delete _surface;
}

void Font::load()
{
	for (unsigned char i = FIRST_CHAR; i < FIRST_CHAR + _nchar; i++)
	{
		SDL_Rect rect;
		int left = -1, right = -1;
		for (int x = 0; x < _width; x++)
		{
			for (int y = (i - FIRST_CHAR) * _height; (y < (i + 1 - FIRST_CHAR) * _height) && (left == -1); y++)
			{
				Uint8 pixel = _surface->getPixel(x, y);
				if (pixel != 0)
					left = x;
			}
		}
		for (int x = _width - 1; x >= 0; x--)
		{
			for (int y = (i + 1 - FIRST_CHAR) * _height - 1; (y >= (i - FIRST_CHAR) * _height) && (right == -1); y--)
			{
				Uint8 pixel = _surface->getPixel(x, y);
				if (pixel != 0)
					right = x;
			}
		}
		rect.x = left;
		rect.y = (i - FIRST_CHAR) * _height;
		rect.w = right - left + 1;
		rect.h = _height;

		_chars[i] = rect;
	}
}

Surface *Font::getChar(char c)
{
	_surface->setCrop(&_chars[c]);
	return _surface;
}

int Font::getWidth()
{
	return _width;
}

int Font::getHeight()
{
	return _height;
}

int Font::getSpacing()
{
	return _spacing;
}

Surface* Font::getSurface()
{
	return _surface;
}