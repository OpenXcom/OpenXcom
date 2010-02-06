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
#ifndef OPENXCOM__PALETTE_H
#define OPENXCOM__PALETTE_H

#include <string>
#include <fstream>
#include "SDL.h"

using namespace std;

class Palette
{
private:
	SDL_Color *_colors;
	int _ncolors;
public:
	Palette();
	~Palette();
	void loadDat(string filename, int ncolors, int offset = 0);
	SDL_Color *getColors(int offset = 0);

	// Position of each palette in PALETTES.DAT
	static inline int palOffset(int palette) { return palette*(768+6); }
	// Position of each color block in a palette
	static inline int blockOffset(int block) { return block*16; }
	// Position of the background block in a peltte
	static const int backPos = 223;
};

#endif