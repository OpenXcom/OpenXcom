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
#include "State_Interactive.h"

InteractiveSurface::InteractiveSurface(int width, int height, int x, int y) : Surface(width, height, x, y), _click(NULL), _press(NULL), _release(NULL), _in(NULL), _out(NULL), _isPressed(false), _isHovered(false)
{

}

InteractiveSurface::~InteractiveSurface()
{
}

void InteractiveSurface::handle(SDL_Event *ev, int scale, State *state)
{
	if ((ev->button.x >= _x * scale && ev->button.x < (_x + _width) * scale) &&
		(ev->button.y >= _y * scale && ev->button.y < (_y + _height) * scale))
	{
		if (!_isHovered)
		{
			_isHovered = true;
			mouseIn(ev, scale, state);
		}
	}
	else
	{
		if (_isHovered)
		{
			_isHovered = false;
			mouseOut(ev, scale, state);
		}
	}

	if (!_isPressed && ev->type == SDL_MOUSEBUTTONDOWN)
	{
		if (_isHovered)
		{
			_isPressed = true;
			mousePress(ev, scale, state);
		}
	}
	else if (_isPressed && ev->type == SDL_MOUSEBUTTONUP)
	{
		_isPressed = false;
		mouseRelease(ev, scale, state);
		if (_isHovered)
		{
			mouseClick(ev, scale, state);
		}
	}
}

void InteractiveSurface::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_press != NULL)
		(state->*_press)(ev, scale);
}

void InteractiveSurface::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (_release != NULL)
		(state->*_release)(ev, scale);
}

void InteractiveSurface::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (_click != NULL)
		(state->*_click)(ev, scale);
}

void InteractiveSurface::mouseIn(SDL_Event *ev, int scale, State *state)
{
	if (_in != NULL)
		(state->*_in)(ev, scale);
}

void InteractiveSurface::mouseOut(SDL_Event *ev, int scale, State *state)
{
	if (_out != NULL)
		(state->*_out)(ev, scale);
}

void InteractiveSurface::onMouseClick(EventHandler handler)
{
	_click = handler;
}

void InteractiveSurface::onMousePress(EventHandler handler)
{
	_press = handler;
}

void InteractiveSurface::onMouseRelease(EventHandler handler)
{
	_release = handler;
}

void InteractiveSurface::onMouseIn(EventHandler handler)
{
	_in = handler;
}

void InteractiveSurface::onMouseOut(EventHandler handler)
{
	_out = handler;
}