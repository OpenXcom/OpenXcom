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
#include "CustomButton.h"

CustomButton::CustomButton(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(NULL)
{
}

CustomButton::~CustomButton()
{
	
}

void CustomButton::setColor(Uint8 color)
{
	_color = color;
}

Uint8 CustomButton::getColor()
{
	return _color;
}

void CustomButton::setGroup(CustomButton **group)
{
	_group = group;
	if (_group != NULL && *_group == this)
		invert(_color);
}

void CustomButton::blit(Surface *surface)
{
	Surface::blit(surface);
}

void CustomButton::handle(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
		InteractiveSurface::handle(ev, scale, state);
}

void CustomButton::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_group != NULL)
	{
		(*_group)->invert((*_group)->getColor());
		*_group = this;
	}
	invert(_color);
	InteractiveSurface::mousePress(ev, scale, state);
}

void CustomButton::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (_group == NULL)
		invert(_color);
	InteractiveSurface::mouseRelease(ev, scale, state);
}