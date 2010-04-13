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

Surface::Surface(int width, int height, int x, int y) : _width(width), _height(height), _x(x), _y(y), _visible(true)
{
	_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);

	if (_surface == 0)
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
	// Load file and put pixels in surface
	ifstream imgFile (filename.c_str(), ios::in | ios::binary);
	if (!imgFile)
	{
		throw "Failed to load SCR";
	}

	// Lock the surface
	SDL_LockSurface(_surface);
	
	char value;
	int x = 0, y = 0;

	while (imgFile.read(&value, 1))
	{
		setPixelIterative(&x, &y, Uint8(value));
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
	// Load file and put pixels in surface
	ifstream imgFile (filename.c_str(), ios::in | ios::binary);
	if (!imgFile)
	{
		throw "Failed to load SPK";
	}

	// Lock the surface
	SDL_LockSurface(_surface);
	
	Uint16 flag;
	char value;
	int x = 0, y = 0;

	while (imgFile.read((char*)&flag, sizeof(flag)))
	{
		if (flag == 65535)
		{
			imgFile.read((char*)&flag, sizeof(flag));
			for (int i = 0; i < flag*2; i++)
			{
				setPixelIterative(&x, &y, 0);
			}
		}
		else if (flag == 65534)
		{
			imgFile.read((char*)&flag, sizeof(flag));
			for (int i = 0; i < flag*2; i++)
			{
				imgFile.read(&value, 1);
				setPixelIterative(&x, &y, Uint8(value));
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
	
	for (int x = 0, y = 0; x < _surface->w && y < _surface->h;)
	{
		Uint8 pixel = getPixel(x, y);
		if (pixel > 0)
			setPixelIterative(&x, &y, pixel + off);
		else
			setPixelIterative(&x, &y, 0);
	}

	// Unlock the surface
	SDL_UnlockSurface(_surface);
}

void Surface::invert(Uint8 mid)
{
	// Lock the surface
	SDL_LockSurface(_surface);

	for (int x = 0, y = 0; x < _surface->w && y < _surface->h;)
	{
		Uint8 pixel = getPixel(x, y);
		if (pixel > 0)
			setPixelIterative(&x, &y, pixel + 2 * (mid - pixel));
		else
			setPixelIterative(&x, &y, 0);
	}

	// Unlock the surface
	SDL_UnlockSurface(_surface);
}


void Surface::blit(Surface *surface)
{
	if (_visible)
	{
		SDL_Rect* cropper;
		SDL_Rect target;
		if (_crop.w == 0 && _crop.h == 0)
			cropper = 0;
		else
			cropper = &_crop;
		target.x = _x;
		target.y = _y;
		SDL_BlitSurface(_surface, cropper, surface->getSurface(), &target);
	}
}

void Surface::copy(Surface *surface)
{
	SDL_Rect from;
	from.x = _x;
	from.y = _y;
	from.w = _width;
	from.h = _height;
	SDL_BlitSurface(surface->getSurface(), &from, _surface, 0);
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
	if (crop == 0)
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

void Surface::setPixel(int x, int y, Uint8 pixel)
{
    ((Uint8 *)_surface->pixels)[y * _surface->pitch + x * _surface->format->BytesPerPixel] = pixel;
}

void Surface::setPixelIterative(int *x, int *y, Uint8 pixel)
{
    setPixel(*x, *y, pixel);
	(*x)++;
	if (*x == _surface->w)
	{
		(*y)++;
		*x = 0;
	}
}

Uint8 Surface::getPixel(int x, int y)
{
    return ((Uint8 *)_surface->pixels)[y * _surface->pitch + x * _surface->format->BytesPerPixel];
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

void Surface::setVisible(bool visible)
{
	_visible = visible;
}

bool Surface::getVisible()
{
	return _visible;
}