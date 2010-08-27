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
#ifndef OPENXCOM__SURFACE_H
#define OPENXCOM__SURFACE_H

#include "SDL.h"
#include <string>

using namespace std;

/**
 * Element that is blit (rendered) onto the screen.
 * Mainly an encapsulation for SDL's SDL_Surface struct, so it
 * borrows a lot of its terminology. Takes care of all the common
 * rendering tasks and color effects, while serving as the base
 * class for more specialized screen elements.
 */
class Surface
{
protected:
	SDL_Surface *_surface;
	int _width, _height, _x, _y;
	SDL_Rect _crop;
	bool _visible, _hidden;
public:
	/// Creates a new surface with the specified size and position.
	Surface(int width, int height, int x = 0, int y = 0);
	/// Creates a new surface from an existing one.
	Surface(const Surface& other);
	/// Cleans up the surface.
	virtual ~Surface();
	/// Loads an X-Com SCR graphic.
	void loadScr(string filename);
	/// Loads an X-Com SPK graphic.
	void loadSpk(string filename);
	/// Clears the surface's contents.
	void clear();
	/// Offsets the surface's colors by a set amount.
	void offset(int off, int min = -1, int max = -1);
	/// Inverts the surface's colors.
	void invert(Uint8 mid);
	/// Runs surface functionality every cycle.
	virtual void think();
	/// Draws the surface's graphic.
	virtual void draw();
	/// Blits this surface onto another one.
	virtual void blit(Surface *surface);
	/// Copies a portion of another surface into this one.
	void copy(Surface *surface);
	/// Sets the surface's palette.
	virtual void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Gets the surface's palette.
	SDL_Color* getPalette();
	/// Sets the X position of the surface.
	void setX(int x);
	/// Gets the X position of the surface.
	int getX();
	/// Sets the Y position of the surface.
	void setY(int y);
	/// Gets the Y position of the surface.
	int getY();
	/// Sets the cropping rectangle for the surface.
	void setCrop(SDL_Rect *crop);
	/// Gets the cropping rectangle for the surface.
	SDL_Rect *getCrop();
	/// Changes a pixel in the surface.
	void setPixel(int x, int y, Uint8 pixel);
	/// Changes a pixel in the surface and returns the next one.
	void setPixelIterative(int *x, int *y, Uint8 pixel);
	/// Gets a pixel of the surface.
	Uint8 getPixel(int x, int y);
	/// Gets the internal SDL surface.
	SDL_Surface* getSurface();
	/// Gets the surface's width.
	int getWidth();
	/// Gets the surface's height.
	int getHeight();
	/// Sets the surface's visibility.
	void setVisible(bool visible);
	/// Gets the surface's visibility.
	bool getVisible();
	/// Hides the surface.
	void hide();
	/// Shows the surface.
	void show();
	/// Locks the surface.
	void lock();
	/// Unlocks the surface.
	void unlock();
};

#endif
