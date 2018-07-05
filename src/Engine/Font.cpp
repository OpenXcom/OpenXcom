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
Font::Font() : _monospace(false)
{
}

/**
 * Deletes the font's surface.
 */
Font::~Font()
{
	for (std::vector<FontImage>::iterator i = _images.begin(); i != _images.end(); ++i)
	{
		delete (*i).surface;
	}
}

/**
 * Loads the font from a YAML file.
 * @param node YAML node.
 */
void Font::load(const YAML::Node &node)
{
	int width = node["width"].as<int>(0);
	int height = node["height"].as<int>(0);
	int spacing = node["spacing"].as<int>(0);
	_monospace = node["monospace"].as<bool>(_monospace);
	for (YAML::const_iterator i = node["images"].begin(); i != node["images"].end(); ++i)
	{
		FontImage image;
		image.width = (*i)["width"].as<int>(width);
		image.height = (*i)["height"].as<int>(height);
		image.spacing = (*i)["spacing"].as<int>(spacing);
		std::string file = "Language/" + (*i)["file"].as<std::string>();
		std::wstring chars = Language::utf8ToWstr((*i)["chars"].as<std::string>());
		image.surface = new Surface(image.width, image.height);
		image.surface->loadImage(FileMap::getFilePath(file));
		_images.push_back(image);
		init(_images.size() - 1, chars);
	}
}

/**
 * Generates a pre-defined Codepage 437 (MS-DOS terminal) font.
 */
void Font::loadTerminal()
{
	FontImage image;
	image.width = 9;
	image.height = 16;
	image.spacing = 0;
	_monospace = true;

	SDL_RWops *rw = SDL_RWFromConstMem(dosFont, DOSFONT_SIZE);
	SDL_Surface *s = SDL_LoadBMP_RW(rw, 0);
	SDL_FreeRW(rw);
	image.surface = new Surface(s->w, s->h);
	SDL_Color terminal[2] = {{0, 0, 0, 0}, {185, 185, 185, 255}};
	image.surface->setPalette(terminal, 0, 2);
	SDL_BlitSurface(s, 0, image.surface->getSurface(), 0);
	SDL_FreeSurface(s);
	_images.push_back(image);

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
	FontImage *image = &_images[index];
	Surface *surface = image->surface;
	surface->lock();
	int length = (surface->getWidth() / image->width);
	if (_monospace)
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			SDL_Rect rect;
			int startX = i % length * image->width;
			int startY = i / length * image->height;
			rect.x = startX;
			rect.y = startY;
			rect.w = image->width;
			rect.h = image->height;
			_chars[str[i]] = std::make_pair(index, rect);
		}
	}
	else
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			SDL_Rect rect;
			int left = -1, right = -1;
			int startX = i % length * image->width;
			int startY = i / length * image->height;
			for (int x = startX; x < startX + image->width; ++x)
			{
				for (int y = startY; y < startY + image->height && left == -1; ++y)
				{
					Uint8 pixel = surface->getPixel(x, y);
					if (pixel != 0)
					{
						left = x;
					}
				}
			}
			for (int x = startX + image->width - 1; x >= startX; --x)
			{
				for (int y = startY + image->height; y-- != startY && right == -1;)
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
			rect.h = image->height;

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
	Surface *surface = _images[_chars[c].first].surface;
	*surface->getCrop() = _chars[c].second;
	return surface;
}

/**
 * Returns the maximum width for any character in the font.
 * @return Width in pixels.
 */
int Font::getWidth() const
{
	return _images[0].width;
}

/**
 * Returns the maximum height for any character in the font.
 * @return Height in pixels.
 */
int Font::getHeight() const
{
	return _images[0].height;
}

/**
 * Returns the spacing for any character in the font.
 * @return Spacing in pixels.
 * @note This does not refer to character spacing within the surface,
 * but to the spacing used between multiple characters in a line.
 */
int Font::getSpacing() const
{
	return _images[0].spacing;
}

/**
 * Returns the dimensions of a particular character in the font.
 * @param c Font character.
 * @return Width and Height dimensions (X and Y are ignored).
 */
SDL_Rect Font::getCharSize(wchar_t c)
{
	SDL_Rect size = { 0, 0, 0, 0 };
	if (c != TOK_FLIP_COLORS && !isLinebreak(c) && !isSpace(c))
	{
		if (_chars.find(c) == _chars.end()) 
			c = L'?';

		FontImage *image = &_images[_chars[c].first];
		size.w = _chars[c].second.w + image->spacing;
		size.h = _chars[c].second.h + image->spacing;
	}
	else
	{
		if (_monospace)
			size.w = getWidth() + getSpacing();
		else if (isNonBreakableSpace(c))
			size.w = getWidth() / 4;
		else
			size.w = getWidth() / 2;
		size.h = getHeight() + getSpacing();
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
	return _images[0].surface->getPalette();
}

/**
 * Replaces a certain amount of colors in the font's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Font::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::vector<FontImage>::iterator i = _images.begin(); i != _images.end(); ++i)
	{
		(*i).surface->setPalette(colors, firstcolor, ncolors);
	}
}

}
