/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_PALETTE_H
#define OPENXCOM_PALETTE_H

#include <string>
#include <SDL.h>

namespace OpenXcom
{

/**
 * Container for palettes (sets of 8bpp colors).
 * Works as an encapsulation for SDL's SDL_Color struct and
 * provides shortcuts for common tasks to make code more readable.
 */
class Palette
{
private:
	SDL_Color *_colors;
	int _count;
public:
	/// Creates a blank palette.
	Palette();
	/// Cleans up the palette.
	~Palette();
	/// Loads the colors from an X-Com palette.
	void loadDat(const std::string &filename, int ncolors, int offset = 0);
	// Gets a certain color from the palette.
	SDL_Color *getColors(int offset = 0) const;

	void savePal(const std::string &file) const;
	void setColors(SDL_Color* pal, int ncolors);
	/// Converts a given color into a RGBA color value.
	static Uint32 getRGBA(SDL_Color* pal, Uint8 color);
	/// Gets the position of a given palette.
	/**
	 * Returns the position of a palette inside an X-Com palette file (each is a 768-byte chunks).
	 * Handy for loading the palettes from the game files.
	 * @param palette Requested palette.
	 * @return Palette position in bytes.
	 */
	static inline int palOffset(int palette) { return palette*(768+6); }
	/// Gets the position of a certain color block in a palette.
	/**
	 * Returns the position of a certain color block in an X-Com palette (they're usually split in 16-color gradients).
	 * Makes setting element colors a lot easier than determining the exact color position.
	 * @param block Requested block.
	 * @return Color position.
	 */
	static inline Uint8 blockOffset(Uint8 block) { return block*16; }
	/// Position of the background colors block in an X-Com palette (used for background images in screens).
	static const int backPos = 224;
};

}

#endif
