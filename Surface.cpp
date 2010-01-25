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
#include "Surface.h"

Surface::Surface(int width, int height, int x, int y) : _width(width), _height(height), _x(x), _y(y)
{
	_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 8, 0, 0, 0, 0);

	if (_surface == NULL)
	{
		throw SDL_GetError();
	}

	SDL_SetColorKey(_surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0);

	_crop.w = 0;
	_crop.h = 0;
	_crop.x = 0;
	_crop.y = 0;
}

Surface::~Surface()
{
	SDL_FreeSurface(_surface);
}

void Surface::loadScr(string filename)
{
	// Lock the surface
	SDL_LockSurface(_surface);

	Uint8 *index = (Uint8 *)_surface->pixels;
	
	// Load file and put pixels in surface
	ifstream imgFile (filename.c_str(), ios::in | ios::binary);
	if (!imgFile)
	{
		throw "Failed to load SCR";
	}
	
	char value;

	while (imgFile.read(&value, 1))
	{
		*index = Uint8(value);
		index++;
	}

	if (!imgFile.eof())
	{
		throw "Invalid data from file";
	}

	// Unlock the surface
	SDL_UnlockSurface(_surface);

	imgFile.close();
}

void Surface::loadSpk(string filename)
{
	// Lock the surface
	SDL_LockSurface(_surface);

	Uint8 *index = (Uint8 *)_surface->pixels;
	
	// Load file and put pixels in surface
	ifstream imgFile (filename.c_str(), ios::in | ios::binary);
	if (!imgFile)
	{
		throw "Failed to load SPK";
	}
	
	Uint16 flag;
	char value;

	while (imgFile.read((char*)&flag, sizeof(flag)))
	{
		if (flag == 65535)
		{
			imgFile.read((char*)&flag, sizeof(flag));
			for (int i = 0; i < flag*2; i++)
			{
				*index = 0;
				index++;
			}
		}
		else if (flag == 65534)
		{
			imgFile.read((char*)&flag, sizeof(flag));
			for (int i = 0; i < flag*2; i++)
			{
				imgFile.read(&value, 1);
				*index = Uint8(value);
				index++;
			}
		}
		else if (flag == 65533)
		{
			break;
		}
	}
	/*
	if (!imgFile.eof())
	{
		throw "Invalid data from file";
	}
	*/

	// Unlock the surface
	SDL_UnlockSurface(_surface);

	imgFile.close();
}

void Surface::clear()
{
	SDL_Rect square;
	square.x = 0;
	square.y = 0;
	square.w = _width;
	square.h = _height;
	SDL_FillRect(_surface, &square, 0);
}

void Surface::offset(int off)
{
	// Lock the surface
	SDL_LockSurface(_surface);
	
	Uint8 *index = (Uint8 *)_surface->pixels;

	for (int i = 0; i < _surface->w * _surface->h; i++)
	{
		if (*index > 0)
			*index += off;
		index++;
	}

	// Unlock the surface
	SDL_UnlockSurface(_surface);
}

void Surface::invert(Uint8 mid)
{
	// Lock the surface
	SDL_LockSurface(_surface);
	
	Uint8 *index = (Uint8 *)_surface->pixels;

	for (int i = 0; i < _surface->w * _surface->h; i++)
	{
		if (*index > 0)
			*index += 2 * (mid - *index);
		index++;
	}

	// Unlock the surface
	SDL_UnlockSurface(_surface);
}


void Surface::blit(Surface *surface)
{
	SDL_Rect* cropper;
	SDL_Rect target;
	if (_crop.w == 0 && _crop.h == 0)
		cropper = NULL;
	else
		cropper = &_crop;
	target.x = _x;
	target.y = _y;
	SDL_BlitSurface(_surface, cropper, surface->getSurface(), &target);
}

void Surface::setX(int x)
{
	_x = x;
}

int Surface::getX()
{
	return _x;
}

void Surface::setY(int y)
{
	_y = y;
}

int Surface::getY()
{
	return _y;
}

void Surface::setCrop(SDL_Rect *crop)
{
	if (crop == NULL)
	{
		_crop.w = 0;
		_crop.h = 0;
		_crop.x = 0;
		_crop.y = 0;
	}
	else
	{
		_crop.w = crop->w;
		_crop.h = crop->h;
		_crop.x = crop->x;
		_crop.y = crop->y;
	}
}

SDL_Rect *Surface::getCrop()
{
	return &_crop;
}

void Surface::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	SDL_SetColors(_surface, colors, firstcolor, ncolors);
}

SDL_Color* Surface::getPalette()
{
	return _surface->format->palette->colors;
}

Uint8 Surface::getPixel(int x, int y)
{
    Uint8 *pixels = (Uint8 *)_surface->pixels;

    return pixels[(y * _width) + x];
}

SDL_Surface* Surface::getSurface()
{
	return _surface;
}

int Surface::getWidth()
{
	return _width;
}

int Surface::getHeight()
{
	return _height;
}