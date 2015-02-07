/*
 * Copyright 2010-2014 OpenXcom Developers.
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

class Font;
class Language;

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
	SDL_Rect _crop, _clear;
	bool _visible, _hidden, _redraw;
	void *_alignedBuffer;
	std::string _tooltip;

	void resize(int width, int height);
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
	/// Loads a TFTD BDY graphic.
	void loadBdy(const std::string &filename);
	/// Loads a general image file.
	void loadImage(const std::string &filename);
	/// Clears the surface's contents eith a specified colour.
	void clear(Uint32 color = 0);
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
	/// Initializes the surface's various text resources.
	virtual void initText(Font *, Font *, Language *) {};
	/// Copies a portion of another surface into this one.
	void copy(Surface *surface);
    /// Draws a filled rectangle on the surface.
    void drawRect(SDL_Rect *rect, Uint8 color);
	/// Draws a filled rectangle on the surface.
	void drawRect(Sint16 x, Sint16 y, Sint16 w, Sint16 h, Uint8 color);
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
	/**
	 * Returns the surface's 8bpp palette.
	 * @return Pointer to the palette's colors.
	 */
	SDL_Color *getPalette() const
	{
		return _surface->format->palette->colors;
	}
	/// Sets the X position of the surface.
	virtual void setX(int x);
	/**
	 * Returns the position of the surface in the X axis.
	 * @return X position in pixels.
	 */
	int getX() const
	{
		return _x;
	}
	/// Sets the Y position of the surface.
	virtual void setY(int y);
	/**
	 * Returns the position of the surface in the Y axis.
	 * @return Y position in pixels.
	 */
	int getY() const
	{
		return _y;
	}
	/// Sets the surface's visibility.
	void setVisible(bool visible);
	/// Gets the surface's visibility.
	bool getVisible() const;
	/// Resets the cropping rectangle for the surface.
	void resetCrop();
	/// Gets the cropping rectangle for the surface.
	SDL_Rect *getCrop();
	/**
	 * Changes the color of a pixel in the surface, relative to
	 * the top-left corner of the surface.
	 * @param x X position of the pixel.
	 * @param y Y position of the pixel.
	 * @param pixel New color for the pixel.
	 */
	void setPixel(int x, int y, Uint8 pixel)
	{
		if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
		{
			return;
		}
		((Uint8 *)_surface->pixels)[y * _surface->pitch + x * _surface->format->BytesPerPixel] = pixel;
	}
	/**
	 * Changes the color of a pixel in the surface and returns the
	 * next pixel position. Useful when changing a lot of pixels in
	 * a row, eg. loading images.
	 * @param x Pointer to the X position of the pixel. Changed to the next X position in the sequence.
	 * @param y Pointer to the Y position of the pixel. Changed to the next Y position in the sequence.
	 * @param pixel New color for the pixel.
	 */
	void setPixelIterative(int *x, int *y, Uint8 pixel)
	{
		setPixel(*x, *y, pixel);
		(*x)++;
		if (*x == getWidth())
		{
			(*y)++;
			*x = 0;
		}
	}
	/**
	 * Returns the color of a specified pixel in the surface.
	 * @param x X position of the pixel.
	 * @param y Y position of the pixel.
	 * @return Color of the pixel.
	 */
	Uint8 getPixel(int x, int y) const
	{
		if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight())
		{
			return 0;
		}
		return ((Uint8 *)_surface->pixels)[y * _surface->pitch + x * _surface->format->BytesPerPixel];
	}
	/**
	 * Returns the internal SDL_Surface for SDL calls.
	 * @return Pointer to the surface.
	 */
	SDL_Surface *getSurface() const
	{
		return _surface;
	}
	/**
	 * Returns the width of the surface.
	 * @return Width in pixels.
	 */
	int getWidth() const
	{
		return _surface->w;
	}
	/// Sets the width of the surface.
	virtual void setWidth(int width);
	/**
	 * Returns the height of the surface.
	 * @return Height in pixels
	 */
	int getHeight() const
	{
		return _surface->h;
	}
	/// Sets the height of the surface.
	virtual void setHeight(int height);
	/// Sets the surface's special hidden flag.
	void setHidden(bool hidden);
	/// Locks the surface.
	void lock();
	/// Unlocks the surface.
	void unlock();
	/// Specific blit function to blit battlescape terrain data in different shades in a fast way.
	void blitNShade(Surface *surface, int x, int y, int off, bool half = false, int newBaseColor = 0);
	/// Invalidate the surface: force it to be redrawn
	void invalidate(bool valid = true);
	/// Gets the tooltip of the surface.
	std::string getTooltip() const;
	/// Sets the tooltip of the surface.
	void setTooltip(const std::string &tooltip);
	/// Sets the color of the surface.
	virtual void setColor(Uint8 color) { /* empty by design */ };
	/// Sets the secondary color of the surface.
	virtual void setSecondaryColor(Uint8 color) { /* empty by design */ };
	/// Sets the border colour of the surface.
	virtual void setBorderColor(Uint8 color) { /* empty by design */ };

};

}

#endif
