#pragma once
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
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <SDL.h>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Surface;
class Palette;

struct FontImage
{
	int width, height, spacing;
	Surface *surface;
};

/**
 * Takes care of loading and storing each character in a sprite font.
 * Sprite fonts consist of a set of characters split in fixed-size regions.
 * @note The characters don't all need to be the same size, they can
 * have blank space and will be automatically lined up properly.
 */
class Font
{
private:
	std::vector<FontImage> _images;
	std::map< wchar_t, std::pair<size_t, SDL_Rect> > _chars;
	bool _monospace;
	/// Determines the size and position of each character in the font.
	void init(size_t index, const std::wstring &str);
public:
	/// Creates a blank font.
	Font();
	/// Cleans up the font.
	~Font();
	/// Checks if a character is a linebreak.
	static inline bool isLinebreak(wchar_t c) { return (c == L'\n' || c == L'\x02'); }
	/// Checks if a character is a blank space (includes non-breaking spaces).
	static inline bool isSpace(wchar_t c) { return (c == L' ' || c == L'\xA0'); }
	/// Checks if a character is a word separator.
	static inline bool isSeparator(wchar_t c) { return (c == L'-' || c == '/'); }
	/// Checks if a character is a non-breaking space.
	static inline bool isNonBreakableSpace(wchar_t c) { return (c == L'\xA0'); }
	/// Loads the font from YAML.
	void load(const YAML::Node& node);
	/// Generate the terminal font.
	void loadTerminal();
	/// Gets a particular character from the font, with its real size.
	Surface *getChar(wchar_t c);
	/// Gets the font's character width.
	int getWidth() const;
	/// Gets the font's character height.
	int getHeight() const;
	/// Gets the spacing between characters.
	int getSpacing() const;
	/// Gets the size of a particular character;
	SDL_Rect getCharSize(wchar_t c);
	/// Gets the font's palette.
	SDL_Color *getPalette() const;
	/// Sets the font's palette.
	void setPalette(SDL_Color *colors, int firstcolor, int ncolors);
};

}
