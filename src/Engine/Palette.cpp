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
#include "Palette.h"
#include <fstream>
#include "Exception.h"

namespace OpenXcom
{

/**
 * Initializes a brand new palette.
 */
Palette::Palette() : _colors(0)
{
}

/**
 * Deletes any colors contained within.
 */
Palette::~Palette()
{
	delete[] _colors;
}

/**
 * Loads an X-Com palette from a file. X-Com palettes are just a set
 * of RGB colors in a row, on a 0-63 scale, which have to be adjusted
 * for modern computers (0-255 scale).
 * @param filename Filename of the palette.
 * @param ncolors Number of colors in the palette.
 * @param offset Position of the palette in the file (in bytes).
 * @sa http://www.ufopaedia.org/index.php?title=PALETTES.DAT
 */
void Palette::loadDat(const std::string &filename, int ncolors, int offset)
{
	if(_colors != 0)
		throw Exception("loadDat can be run only once");
	_colors = new SDL_Color[ncolors];

	// Load file and put colors in pallete
	std::ifstream palFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!palFile)
	{
		throw Exception(filename + " not found");
	}

	// Move pointer to proper pallete
	palFile.seekg(offset, std::ios::beg);

	Uint8 value[3];

	for (int i = 0; i < ncolors && palFile.read((char*)value, 3); ++i)
	{
		// Correct X-Com colors to RGB colors
		_colors[i].r = value[0] * 4;
		_colors[i].g = value[1] * 4;
		_colors[i].b = value[2] * 4;
	}

	palFile.close();
}

/**
 * Provides access to colors contained in the palette.
 * @param offset Offset to a specific color.
 * @return Pointer to the requested SDL_Color.
 */
SDL_Color *Palette::getColors(int offset) const
{
	return _colors + offset;
}

/**
 * Converts an SDL_Color struct into an hexadecimal RGBA color value.
 * Mostly used for operations with SDL_gfx that require colors in this format.
 * @param pal Requested palette.
 * @param color Requested color in the palette.
 * @return Hexadecimal RGBA value.
 */
Uint32 Palette::getRGBA(SDL_Color* pal, Uint8 color)
{
	return ((Uint32) pal[color].r << 24) | ((Uint32) pal[color].g << 16) | ((Uint32) pal[color].b << 8) | (Uint32) 0xFF;
}

}
