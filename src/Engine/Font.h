/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_FONT_H
#define OPENXCOM_FONT_H

#include <map>
#include <string>
#include <SDL.h>

namespace OpenXcom
{

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
	static std::wstring _index;
	Surface *_surface;
	int _width, _height;
	std::map<wchar_t, SDL_Rect> _chars;
	int _spacing; // For some reason the X-Com small font is smooshed together by one pixel...
public:
	/// Creates a font with a blank surface.
	Font(int width, int height, int spacing);
	/// Cleans up the font.
	~Font();
	/// Determines the size and position of each character in the font.
	void load();
	/// Loads the character index for every font.
	static void loadIndex(const std::string &filename);
	/// Gets a particular character from the font, with its real size.
	Surface *getChar(wchar_t c);
	/// Gets the font's character width.
	int getWidth() const;
	/// Gets the font's character height.
	int getHeight() const;
	/// Gets the horizontal spacing between characters.
	int getSpacing() const;
	/// Gets the font's surface.
	Surface *getSurface() const;
};

}

#endif
