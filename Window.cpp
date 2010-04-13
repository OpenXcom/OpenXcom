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
#include "Window.h"

Window::Window(int width, int height, int x, int y) : Surface(width, height, x, y), _bg(0), _color(0)
{
}

Window::~Window()
{
	
}

void Window::setBg(Surface *bg)
{
	_bg = bg;
}

void Window::setColor(Uint8 color)
{
	_color = color;
}

Uint8 Window::getColor()
{
	return _color;
}

void Window::blit(Surface *surface)
{
	SDL_Rect cropper;
	SDL_Rect square;
	int color = _color;

	square.x = 0;
	square.y = 0;
	square.w = _width;
	square.h = _height;

	for (int i = 0; i < 5; i++)
	{
		SDL_FillRect(_surface, &square, color);
		if (i < 2)
			color--;
		else
			color++;
		square.x++;
		square.y++;
		square.w -= 2;
		square.h -= 2;
	}

	cropper.x = _x + square.x;
	cropper.y = _y + square.y;
	cropper.w = square.w;
	cropper.h = square.h;
	
	_bg->setCrop(&cropper);
	_bg->setX(square.x);
	_bg->setY(square.y);
	_bg->blit(this);

	Surface::blit(surface);
}
