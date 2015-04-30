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
#ifndef OPENXCOM_SURFACESET_H
#define OPENXCOM_SURFACESET_H

#include <vector>
#include <map>
#include <string>
#include <SDL.h>

namespace OpenXcom
{

class Surface;

/**
 * Container of a set of surfaces.
 * Used to manage single images that contain series of
 * frames inside, like animated sprites, making them easier
 * to access without constant cropping.
 */
class SurfaceSet
{
private:
	int _width, _height;
	std::map<int, Surface*> _frames;
public:
	/// Crates a surface set with frames of the specified size.
	SurfaceSet(int width, int height);
	/// Creates a surface set from an existing one.
	SurfaceSet(const SurfaceSet& other);
	/// Cleans up the surface set.
	~SurfaceSet();
	/// Loads an X-Com set of PCK/TAB image files.
	void loadPck(const std::string &pck, const std::string &tab = "");
	/// Loads an X-Com DAT image file.
	void loadDat(const std::string &filename);
	/// Gets a particular frame from the set.
	Surface *getFrame(int i);
	/// Creates a new surface and returns a pointer to it.
	Surface *addFrame(int i);
	/// Gets the width of all frames.
	int getWidth() const;
	/// Gets the height of all frames.
	int getHeight() const;
	/// Gets the total frames in the set.
	size_t getTotalFrames() const;
	/// Sets the surface set's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	std::map<int, Surface*> *getFrames();
};

}

#endif
