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

Screen::Screen(int width, int height, int bpp) : _width(width), _height(height), _bpp(bpp), _scale(1)
{
	_flags = SDL_DOUBLEBUF|SDL_HWPALETTE;

	_screen = SDL_SetVideoMode(_width, _height, _bpp, _flags);
	if (_screen == NULL)
	{
		throw SDL_GetError();
	}

	_surface = new Surface(_width, _height);
}

Screen::~Screen()
{
	delete _surface;
}

Surface *Screen::getSurface()
{
	return _surface;
}

void Screen::flip()
{
	if (_scale != 1)
	{
		SDL_Surface* zoom = zoomSurface(_surface->getSurface(), _scale, _scale, 0);
		SDL_BlitSurface(zoom, NULL, _screen, NULL);
		SDL_FreeSurface(zoom);
	}
	else
	{
		SDL_BlitSurface(_surface->getSurface(), NULL, _screen, NULL);
	}

	if (SDL_Flip(_screen) == -1)
    {
        throw SDL_GetError();
    }
}

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

void Screen::setPalette(SDL_Color* colors, int firstcolor, int ncolors)
{
	_surface->setPalette(colors, firstcolor, ncolors);
	SDL_SetColors(_screen, colors, firstcolor, ncolors);
}

SDL_Color* Screen::getPalette()
{
	return _surface->getPalette();
}

void Screen::setResolution(int width, int height)
{
	_screen = SDL_SetVideoMode(width, height, _bpp, _flags);
	if (_screen == NULL)
	{
		throw SDL_GetError();
	}
	_width = width;
	_height = height;
	setPalette(_surface->getPalette());
}

void Screen::setFullscreen(bool full)
{
	if (full)
		_flags = SDL_DOUBLEBUF|SDL_HWPALETTE|SDL_FULLSCREEN;
	else
		_flags = SDL_DOUBLEBUF|SDL_HWPALETTE;
	setResolution(_width, _height);
}

int Screen::getScale()
{
	return _scale;
}

void Screen::setScale(int amount)
{
	_scale = amount;
	_width *= amount;
	_height *= amount;
	setResolution(_width, _height);
}