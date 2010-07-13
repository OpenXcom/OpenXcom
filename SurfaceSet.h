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
#ifndef OPENXCOM__SURFACESET_H
#define OPENXCOM__SURFACESET_H

#include <vector>
#include "SDL.h"

class Surface;

using namespace std;

/**
 * Container of a set of surfaces.
 * Used to manage single images that contain series of
 * frames inside, like animated sprites, making them easier
 * to access without constant cropping.
 */
class SurfaceSet
{
private:
	int _width, _height, _nframes;
	vector<Surface*> _frames;
public:
	/// Crates a surface set with frames of the specified size.
	SurfaceSet(int width, int height);
	/// Cleans up the surface set.
	~SurfaceSet();
	/// Loads an X-Com set of PCK/TAB image files.
	void loadPck(string filename);
	/// Loads an X-Com DAT image file.
	void loadDat(string filename);
	/// Gets a particular frame from the set.
	Surface *getFrame(int i);
	/// Gets the width of all frames.
	int getWidth();
	/// Gets the height of all frames.
	int getHeight();
	/// Sets the surface set's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
};

#endif
