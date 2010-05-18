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
#include "Screen.h"

/**
 * Initializes a new display screen for the game to render contents to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param bpp Bits-per-pixel.
 */
Screen::Screen(int width, int height, int bpp) : _width(width), _height(height), _bpp(bpp), _scale(1)
{
	_flags = SDL_SWSURFACE|SDL_HWPALETTE;

	_screen = SDL_SetVideoMode(_width, _height, _bpp, _flags);
	if (_screen == 0)
	{
		throw SDL_GetError();
	}

	_surface = new Surface(_width, _height);
}

/**
 * Deletes the buffer from memory. The display screen itself
 * is automatically freed once SDL shuts down.
 */
Screen::~Screen()
{
	delete _surface;
}

/**
 * Returns the screen's internal buffer surface. Any
 * contents that need to be shown will be blitted to this.
 * @return Pointer to the buffer surface.
 */
Surface *Screen::getSurface()
{
	return _surface;
}

/**
 * Renders the buffer's contents onto the screen, applying
 * any necessary filters or conversions in the process.
 * If the scaling factor is bigger than 1, the entire contents
 * of the buffer are resized by that factor (eg. 2 = doubled)
 * before being put on screen.
 */
void Screen::flip()
{
	if (_scale != 1)
	{
		SDL_Surface* zoom = zoomSurface(_surface->getSurface(), _scale, _scale, 0);
		SDL_BlitSurface(zoom, 0, _screen, 0);
		SDL_FreeSurface(zoom);
	}
	else
	{
		SDL_BlitSurface(_surface->getSurface(), 0, _screen, 0);
	}

	if (SDL_Flip(_screen) == -1)
    {
        throw SDL_GetError();
    }
}

/**
 * Clears all the contents out of the internal buffer.
 */
void Screen::clear()
{
	_surface->clear();
	SDL_Rect square;
	square.x = 0;
	square.y = 0;
	square.w = _width;
	square.h = _height;
	SDL_FillRect(_screen, &square, 0);
}

/**
 * Changes the 8bpp palette used to render the screen's contents.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Screen::setPalette(SDL_Color* colors, int firstcolor, int ncolors)
{
	_surface->setPalette(colors, firstcolor, ncolors);
	SDL_SetColors(_screen, colors, firstcolor, ncolors);
}

/**
 * Returns the screen's 8bpp palette.
 * @return Pointer to the palette's colors.
 */
SDL_Color* Screen::getPalette()
{
	return _surface->getPalette();
}

/**
 * Changes the screen's resolution. The display surface
 * and palette have to be reset for this to happen properly.
 * @param width Width in pixels.
 * @param height Height in pixels.
 */
void Screen::setResolution(int width, int height)
{
	_screen = SDL_SetVideoMode(width, height, _bpp, _flags);
	if (_screen == 0)
	{
		throw SDL_GetError();
	}
	_width = width;
	_height = height;
	setPalette(_surface->getPalette());
}

/**
 * Switches the screen between full-screen and/or windowed.
 * The screen has to be reset for this to happen properly.
 * @param full True for full-screen, False for windowed.
 */
void Screen::setFullscreen(bool full)
{
	if (full)
		_flags |= SDL_FULLSCREEN;
	else
		_flags &= ~SDL_FULLSCREEN;
	setResolution(_width, _height);
}

/**
 * Returns the scaling factor used by the screen.
 * @return Scaling factor.
 */
int Screen::getScale()
{
	return _scale;
}

/**
 * Changes the scaling factor used to render the screen,
 * and adapts the resolution respectively.
 * @param amount Scaling factor.
 */
void Screen::setScale(int amount)
{
	_scale = amount;
	_width *= amount;
	_height *= amount;
	setResolution(_width, _height);
}