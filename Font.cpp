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
#include "Surface.h"

// Fonts start with this character
#define FIRST_CHAR '!'

/**
 * Initializes the font with a blank surface sized big enough to
 * contain all the characters.
 * @param width Width in pixels of each character.
 * @param height Height in pixels of each character.
 * @param nchar Number of characters the font contains.
 * @param spacing Horizontal spacing between each character.
 */
Font::Font(int width, int height, int nchar, int spacing) : _width(width), _height(height), _nchar(nchar), _chars(), _spacing(spacing)
{
	_surface = new Surface(width, height*nchar);
}

/**
 * Deletes the font's surface.
 */
Font::~Font()
{
	delete _surface;
}

/**
 * Calculates the real size and position of each character in
 * the surface and stores them in SDL_Rect's for future use
 * by other classes.
 */
void Font::load()
{
	_surface->lock();
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
	_surface->unlock();
}

/**
 * Returns a particular character from the set stored in the font.
 * @param c Character to use for size/position.
 * @return Pointer to the font's surface with the respective
 * cropping rectangle set up.
 */
Surface *Font::getChar(char c)
{
	_surface->setCrop(&_chars[c]);
	return _surface;
}
/**
 * Returns the maximum width for any character in the font.
 * @return Width in pixels.
 */
int Font::getWidth()
{
	return _width;
}

/**
 * Returns the maximum height for any character in the font.
 * @return Height in pixels.
 */
int Font::getHeight()
{
	return _height;
}

/**
 * Returns the horizontal spacing for any character in the font.
 * @return Spacing in pixels.
 * @note This does not refer to character spacing within the surface,
 * but to the spacing used when drawing a series of characters.
 */
int Font::getSpacing()
{
	return _spacing;
}

/**
 * Returns the surface stored within the font. Used for loading the
 * actual graphic into the font.
 * @return Pointer to the internal surface.
 */
Surface* Font::getSurface()
{
	return _surface;
}
