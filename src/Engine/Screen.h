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
#ifndef OPENXCOM_SCREEN_H
#define OPENXCOM_SCREEN_H

#include <SDL.h>
#include <string>
#include "OpenGL.h"

namespace OpenXcom
{

class Surface;
class Action;

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
public:
	static int BASE_WIDTH;
	static int BASE_HEIGHT;

private:
	SDL_Surface *_screen;
	void *_misalignedPixelBuffer;
	int _bpp;
	double _scaleX, _scaleY;
	Uint32 _flags;
	bool _fullscreen;
	int _zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy);
	SDL_Color deferredPalette[256];
	int _numColors, _firstColor;
	bool _pushPalette;
	OpenGL glOutput;
	Surface *_surface;
public:
	/// Creates a new display screen with the specified resolution.
	Screen(int width, int height, int bpp, bool fullscreen, int windowedModePositionX, int windowedModePositionY);
	/// Cleans up the display screen.
	~Screen();
	/// Gets the internal buffer.
	Surface *getSurface();
	/// Handles keyboard events.
	void handle(Action *action);
	/// Renders the screen onto the game window.
	void flip();
	/// Clears the screen.
	void clear();
	/// Sets the screen's 8bpp palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256, bool immediately = false);
	/// Gets the screen's 8bpp palette.
	SDL_Color *getPalette() const;
	/// Gets the screen's width.
	int getWidth() const;
	/// Gets the screen's height.
	int getHeight() const;
	/// Sets the screen's display resolution.
	void setResolution(int width, int height);
	/// Sets whether the screen is full-screen or windowed.
	void setFullscreen(bool full);
	/// Gets the screen's X scale.
	double getXScale() const;
	/// Gets the screen's Y scale.
	double getYScale() const;
	/// Takes a screenshot.
	void screenshot(const std::string &filename) const;
	/// Checks whether HQX is requested and works for the selected resolution
	static bool isHQXEnabled();
	/// Checks whether OpenGL output is requested
	static bool isOpenGLEnabled();
	/// Sets the _flags and _bpp variables based on game options; needed in more than one place now
	void makeVideoFlags();
};

}

#endif
