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
#include "Button.h"

Button::Button(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(NULL)
{
	_text = new Text(big, small, width, small->getHeight(), 0, (int)ceil((double)(height - small->getHeight()) / 2));
	_text->setSmall();
	_text->setAlign(ALIGN_CENTER);
}

Button::~Button()
{
	delete _text;

	Surface::~Surface();
}

void Button::setColor(Uint8 color)
{
	_color = color;
	_text->setColor(_color-3);
}

Uint8 Button::getColor()
{
	return _color;
}

void Button::setText(string text)
{
	_text->setText(text);
}

string Button::getText()
{
	return _text->getText();
}

void Button::setGroup(Button **group)
{
	_group = group;
}

void Button::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

void Button::blit(Surface *surface)
{
	SDL_Rect square;
	int color = _color - 2;

	square.x = 0;
	square.y = 0;
	square.w = _width;
	square.h = _height;

	for (int i = 0; i < 5; i++)
	{
		SDL_FillRect(_surface, &square, color);
		
		if (i % 2 == 0)
		{
			square.x++;
			square.y++;
		}
		square.w--;
		square.h--;

		switch (i)
		{
		case 0:
			color = _color + 2;
			break;
		case 1:
			color = _color - 1;
			break;
		case 2:
			color = _color + 1;
			break;
		case 3:
			color = _color;
			break;
		}
	}
	
	bool press;
	if (_group == NULL)
		press = _isPressed;
	else
		press = (*_group == this);

	if (press)
	{
		this->invert(_color);
	}
	_text->setInvert(press);

	_text->blit(this);

	Surface::blit(surface);
}

void Button::handle(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
		InteractiveSurface::handle(ev, scale, state);
}

void Button::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_group != NULL)
		*_group = this;

	InteractiveSurface::mousePress(ev, scale, state);
}

void Button::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	InteractiveSurface::mouseRelease(ev, scale, state);
}