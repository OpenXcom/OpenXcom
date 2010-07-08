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
#ifndef OPENXCOM__SCREEN_H
#define OPENXCOM__SCREEN_H

#include "SDL.h"
#include "SDL_rotozoom.h"
#include "Surface.h"

/**
 * A display screen, handles rendering onto the game window.
 * In SDL a Screen is treated like a Surface, so this is just
 * a specialized version of a Surface with functionality more
 * relevant for display screens. Contains a Surface buffer
 * where all the contents are kept, so any filters or conversions
 * can be applied before rendering the screen.
 */
class Screen
{
private:
	Surface *_surface;
	SDL_Surface *_screen;
	int _width, _height, _bpp;
	int _scale;
	Uint32 _flags;
public:
	/// Creates a new display screen with the specified resolution.
	Screen(int width, int height, int bpp);
	/// Cleans up the display screen.
	~Screen();
	/// Gets the internal buffer.
	Surface *getSurface();
	/// Renders the screen onto the game window.
	void flip();
	/// Clears the screen.
	void clear();
	/// Sets the screen's 8bpp palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Gets the screen's 8bpp palette.
	SDL_Color* getPalette();
	/// Sets the screen's display resolution.
	void setResolution(int width, int height);
	/// Sets whether the screen is full-screen or windowed.
	void setFullscreen(bool full);
	/// Gets the current scaling factor.
	int getScale();
	/// Sets a new scaling factor.
	void setScale(int amount);
};

#endif
