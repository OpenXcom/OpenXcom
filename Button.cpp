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

Button::Button(Font *big, Font *small, int width, int height, int x, int y) : Surface(width, height, x, y), _color(0), _state(STATE_NORMAL), _click(NULL), _press(NULL), _release(NULL)
{
	_text = new Text(big, small, width, small->getHeight(), 0, (height - small->getHeight())/2);
	_text->setSmall();
	_text->setAlign(ALIGN_CENTER);
}

Button::~Button()
{
	delete _text;

	Surface::~Surface();
}

void Button::handle(SDL_Event *ev, int scale, State *state)
{
	if (_state == STATE_NORMAL && ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT)
	{
		if ((ev->button.x >= _x * scale && ev->button.x < (_x + _width) * scale) &&
			(ev->button.y >= _y * scale && ev->button.y < (_y + _height) * scale))
		{
			_state = STATE_PRESSED;
			if (_press != NULL)
				(state->*_press)(ev);
		}
	}
	else if (_state == STATE_PRESSED && ev->type == SDL_MOUSEBUTTONUP && ev->button.button == SDL_BUTTON_LEFT)
	{
		_state = STATE_NORMAL;
		if (_release != NULL)
			(state->*_release)(ev);
		if ((ev->button.x >= _x * scale && ev->button.x < (_x + _width) * scale) &&
			(ev->button.y >= _y * scale && ev->button.y < (_y + _height) * scale))
		{
			if (_click != NULL)
				(state->*_click)(ev);
		}
	}
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
	
	if (_state == STATE_PRESSED)
	{
		this->invert(_color);
		_text->setInvert(true);
	}
	else
	{
		_text->setInvert(false);
	}

	_text->blit(this);

	Surface::blit(surface);
}

void Button::onClick(EventHandler handler)
{
	_click = handler;
}

void Button::onPress(EventHandler handler)
{
	_press = handler;
}

void Button::onRelease(EventHandler handler)
{
	_release = handler;
}