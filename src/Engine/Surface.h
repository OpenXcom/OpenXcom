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
#ifndef OPENXCOM_SURFACE_H
#define OPENXCOM_SURFACE_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

namespace OpenXcom
{

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
	int _x, _y;
	SDL_Rect _crop;
	bool _visible, _hidden, _redraw;
	SDL_Color *_originalColors;
	void *_misalignedPixelBuffer, *_alignedBuffer;
public:
	/// Creates a new surface with the specified size and position.
	Surface(int width, int height, int x = 0, int y = 0, int bpp = 8);
	/// Creates a new surface from an existing one.
	Surface(const Surface& other);
	/// Cleans up the surface.
	virtual ~Surface();
	/// Loads an X-Com SCR graphic.
	void loadScr(const std::string &filename);
	/// Loads an X-Com SPK graphic.
	void loadSpk(const std::string &filename);
	/// Loads a general image file.
	void loadImage(const std::string &filename);
	/// Clears the surface's contents.
	void clear();
	/// Offsets the surface's colors by a set amount.
	void offset(int off, int min = -1, int max = -1, int mul = 1);
	/// Inverts the surface's colors.
	void invert(Uint8 mid);
	/// Runs surface functionality every cycle
	virtual void think();
	/// Draws the surface's graphic.
	virtual void draw();
	/// Blits this surface onto another one.
	virtual void blit(Surface *surface);
	/// Copies a portion of another surface into this one.
	void copy(Surface *surface);
    /// Draws a filled rectangle on the surface.
    void drawRect(SDL_Rect *rect, Uint8 color);
    /// Draws a line on the surface.
    void drawLine(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 color);
    /// Draws a filled circle on the surface.
    void drawCircle(Sint16 x, Sint16 y, Sint16 r, Uint8 color);
    /// Draws a filled polygon on the surface.
    void drawPolygon(Sint16 *x, Sint16 *y, int n, Uint8 color);
    /// Draws a textured polygon on the surface.
    void drawTexturedPolygon(Sint16 *x, Sint16 *y, int n, Surface *texture, int dx, int dy);
    /// Draws a string on the surface.
    void drawString(Sint16 x, Sint16 y, const char *s, Uint8 color);
	/// Sets the surface's palette.
	virtual void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Gets the surface's palette.
	SDL_Color *getPalette() const;
	/// Sets the X position of the surface.
	void setX(int x);
	/// Gets the X position of the surface.
	int getX() const;
	/// Sets the Y position of the surface.
	void setY(int y);
	/// Gets the Y position of the surface.
	int getY() const;
	/// Sets the surface's visibility.
	void setVisible(bool visible);
	/// Gets the surface's visibility.
	bool getVisible() const;
	/// Resets the cropping rectangle for the surface.
	void resetCrop();
	/// Gets the cropping rectangle for the surface.
	SDL_Rect *getCrop();
	/// Changes a pixel in the surface.
	void setPixel(int x, int y, Uint8 pixel);
	/// Changes a pixel in the surface and returns the next one.
	void setPixelIterative(int *x, int *y, Uint8 pixel);
	/// Gets a pixel of the surface.
	Uint8 getPixel(int x, int y) const;
	/// Gets the internal SDL surface.
	SDL_Surface *getSurface() const;
	/// Gets the surface's width.
	int getWidth() const;
	/// Gets the surface's height.
	int getHeight() const;
	/// Sets the surface's special hidden flag.
	void setHidden(bool hidden);
	/// Locks the surface.
	void lock();
	/// Unlocks the surface.
	void unlock();
	/// Offsets and optionally inverts the surface palette's colors by a set amount.
	void paletteShift(int off, int mul, int mid = 0);
	/// Restores the original palette.
	void paletteRestore();
	/// Specific blit function to blit battlescape terrain data in different shades in a fast way.
	void blitNShade(Surface *surface, int x, int y, int off, bool half = false, int newBaseColor = 0);
	/// Invalidate the surface: force it to be redrawn
	void invalidate();
};

}

#endif
