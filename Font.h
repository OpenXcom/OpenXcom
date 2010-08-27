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
#ifndef OPENXCOM__FONT_H
#define OPENXCOM__FONT_H

#include <map>
#include "SDL.h"

using namespace std;

class Surface;

/**
 * Takes care of loading and storing each character in a sprite font.
 * Sprite fonts consist of a set of fixed-size characters all lined up
 * in one column in a surface.
 * @note The characters don't all need to be the same size, they can
 * have blank space and will be automatically lined up properly.
 */
class Font
{
private:
	Surface *_surface;
	int _width, _height, _nchar;
	map<unsigned char, SDL_Rect> _chars;
	// For some reason the X-Com small font is smooshed together by one pixel...
	int _spacing;
public:
	/// Creates a font with a blank surface.
	Font(int width, int height, int nchar, int spacing = 0);
	/// Cleans up the font.
	~Font();
	/// Determines the size and position of each character in the font.
	void load();
	/// Gets a particular character from the font, with its real size.
	Surface *getChar(char c);
	/// Gets the font's character width.
	int getWidth();
	/// Gets the font's character height.
	int getHeight();
	/// Gets the horizontal spacing between characters.
	int getSpacing();
	/// Gets the font's surface.
	Surface *getSurface();
};

#endif
