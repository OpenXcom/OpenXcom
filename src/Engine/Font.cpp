/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "DosFont.h"
#include "Surface.h"
#include "FileMap.h"
#include "Language.h"

namespace OpenXcom
{

/**
 * Initializes the font with a blank surface.
 */
Font::Font() : _width(0), _height(0), _spacing(0), _monospace(false)
{
}

/**
 * Deletes the font's surface.
 */
Font::~Font()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		delete (*i);
	}
}

/**
 * Loads the font from a YAML file.
 * @param node YAML node.
 */
void Font::load(const YAML::Node &node)
{
	_width = node["width"].as<int>(_width);
	_height = node["height"].as<int>(_height);
	_spacing = node["spacing"].as<int>(_spacing);
	_monospace = node["monospace"].as<bool>(_monospace);
	for (YAML::const_iterator i = node["images"].begin(); i != node["images"].end(); ++i)
	{
		std::string image = "Language/" + (*i)["file"].as<std::string>();
		std::wstring chars = Language::utf8ToWstr((*i)["chars"].as<std::string>());
		Surface *surface = new Surface(_width, _height);
		surface->loadImage(FileMap::getFilePath(image));
		_surfaces.push_back(surface);
		init(_surfaces.size() - 1, chars);
	}
}

/**
 * Generates a pre-defined Codepage 437 (MS-DOS terminal) font.
 */
void Font::loadTerminal()
{
	_width = 9;
	_height = 16;
	_spacing = 0;
	_monospace = true;

	SDL_RWops *rw = SDL_RWFromConstMem(dosFont, DOSFONT_SIZE);
	SDL_Surface *s = SDL_LoadBMP_RW(rw, 0);
	SDL_FreeRW(rw);
	Surface *surface = new Surface(s->w, s->h);
	SDL_Color terminal[2] = {{0, 0, 0, 0}, {185, 185, 185, 255}};
	surface->setPalette(terminal, 0, 2);
	SDL_BlitSurface(s, 0, surface->getSurface(), 0);
	SDL_FreeSurface(s);
	_surfaces.push_back(surface);

	init(0, L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
}


/**
 * Calculates the real size and position of each character in
 * the surface and stores them in SDL_Rect's for future use
 * by other classes.
 * @param index The index of the surface to use.
 * @param str A string of characters to map to the surface.
 */
void Font::init(size_t index, const std::wstring &str)
{
	Surface *surface = _surfaces[index];
	surface->lock();
	int length = (surface->getWidth() / _width);
	if (_monospace)
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			SDL_Rect rect;
			int startX = i % length * _width;
			int startY = i / length * _height;
			rect.x = startX;
			rect.y = startY;
			rect.w = _width;
			rect.h = _height;
			_chars[str[i]] = std::make_pair(index, rect);
		}
	}
	else
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			SDL_Rect rect;
			int left = -1, right = -1;
			int startX = i % length * _width;
			int startY = i / length * _height;
			for (int x = startX; x < startX + _width; ++x)
			{
				for (int y = startY; y < startY + _height && left == -1; ++y)
				{
					Uint8 pixel = surface->getPixel(x, y);
					if (pixel != 0)
					{
						left = x;
					}
				}
			}
			for (int x = startX + _width - 1; x >= startX; --x)
			{
				for (int y = startY + _height; y-- != startY && right == -1;)
				{
					Uint8 pixel = surface->getPixel(x, y);
					if (pixel != 0)
					{
						right = x;
					}
				}
			}
			rect.x = left;
			rect.y = startY;
			rect.w = right - left + 1;
			rect.h = _height;

			_chars[str[i]] = std::make_pair(index, rect);
		}
	}
	surface->unlock();
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
		return 0;
	}
	Surface *surface = _surfaces[_chars[c].first];
	*surface->getCrop() = _chars[c].second;
	return surface;
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
 * Returns the spacing for any character in the font.
 * @return Spacing in pixels.
 * @note This does not refer to character spacing within the surface,
 * but to the spacing used between multiple characters in a line.
 */
int Font::getSpacing() const
{
	return _spacing;
}

/**
 * Returns the dimensions of a particular character in the font.
 * @param c Font character.
 * @return Width and Height dimensions (X and Y are ignored).
 */
SDL_Rect Font::getCharSize(wchar_t c)
{
	SDL_Rect size = { 0, 0, 0, 0 };
	if (c != 1 && !isLinebreak(c) && !isSpace(c))
	{
		size.w = _chars[c].second.w + _spacing;
		size.h = _chars[c].second.h + _spacing;
	}
	else
	{
		if (_monospace)
			size.w = _width + _spacing;
		else if (isNonBreakableSpace(c))
			size.w = _width / 4;
		else
			size.w = _width / 2;
		size.h = _height + _spacing;
	}
	// In case anyone mixes them up
	size.x = size.w;
	size.y = size.h;
	return size;
}

/**
 * Returns the font's 8bpp palette.
 * @return Pointer to the palette's colors.
 */
SDL_Color *Font::getPalette() const
{
	return _surfaces[0]->getPalette();
}

/**
 * Replaces a certain amount of colors in the font's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Font::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setPalette(colors, firstcolor, ncolors);
	}
}

}
