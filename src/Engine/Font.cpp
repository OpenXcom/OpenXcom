/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <fstream>
#include <iostream>
#include "Exception.h"
#include "Surface.h"
#include "Language.h"

namespace OpenXcom
{

std::wstring Font::_index = L"";

/**
 * Initializes the font with a blank surface sized big enough to
 * contain all the characters.
 * @param width Width in pixels of each character.
 * @param height Height in pixels of each character.
 * @param spacing Horizontal spacing between each character.
 */
Font::Font(int width, int height, int spacing) : _width(width), _height(height), _chars(), _spacing(spacing)
{
	_surface = new Surface(width, height * _index.length());
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
	for (unsigned int i = 0; i < _index.length(); ++i)
	{
		SDL_Rect rect;
		int left = -1, right = -1;
		for (int x = 0; x < _width; ++x)
		{
			for (unsigned int y = i * _height; y < (i + 1) * _height && left == -1; ++y)
			{
				Uint8 pixel = _surface->getPixel(x, y);
				if (pixel != 0)
				{
					left = x;
				}
			}
		}
		for (int x = _width - 1; x >= 0; --x)
		{
			for (unsigned int y = (i + 1) * _height; y-- != i * _height && right == -1;)
			{
				Uint8 pixel = _surface->getPixel(x, y);
				if (pixel != 0)
				{
					right = x;
				}
			}
		}
		rect.x = left;
		rect.y = i * _height;
		rect.w = right - left + 1;
		rect.h = _height;

		_chars[_index[i]] = rect;
	}
	_surface->unlock();
}

/**
 * Loads the characters contained in each font
 * from a UTF-8 file to use as the index.
 * @param filename Filename of the index file.
 */
void Font::loadIndex(const std::string &filename)
{
	_index = L"";

	std::ifstream txtFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!txtFile)
	{
		throw Exception(filename + " not found");
	}

	char value;
	std::string buffer;
	while (txtFile.read(&value, 1))
	{
		buffer += value;
	}

	if (!txtFile.eof())
	{
		throw Exception("Invalid font index");
	}

	_index = Language::utf8ToWstr(buffer);
	if (sizeof(_index[0]) == 1)
		_index = _index.substr(0, 256);
	txtFile.close();
}

/**
 * Returns a particular character from the set stored in the font.
 * @param c Character to use for size/position.
 * @return Pointer to the font's surface with the respective
 * cropping rectangle set up.
 */
Surface *Font::getChar(wchar_t c)
{
	if (_chars.find(c) == _chars.end())
	{
		c = '?';
	}
	_surface->getCrop()->x = _chars[c].x;
	_surface->getCrop()->y = _chars[c].y;
	_surface->getCrop()->w = _chars[c].w;
	_surface->getCrop()->h = _chars[c].h;
	return _surface;
}
/**
 * Returns the maximum width for any character in the font.
 * @return Width in pixels.
 */
int Font::getWidth() const
{
	return _width;
}

/**
 * Returns the maximum height for any character in the font.
 * @return Height in pixels.
 */
int Font::getHeight() const
{
	return _height;
}

/**
 * Returns the horizontal spacing for any character in the font.
 * @return Spacing in pixels.
 * @note This does not refer to character spacing within the surface,
 * but to the spacing used when drawing a series of characters.
 */
int Font::getSpacing() const
{
	return _spacing;
}

/**
 * Returns the surface stored within the font. Used for loading the
 * actual graphic into the font.
 * @return Pointer to the internal surface.
 */
Surface *Font::getSurface() const
{
	return _surface;
}

}
