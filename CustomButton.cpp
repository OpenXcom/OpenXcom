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

/**
 * Sets up a custom button with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
CustomButton::CustomButton(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(0)
{
}

/**
 *
 */
CustomButton::~CustomButton()
{
	
}

/**
 * Changes the color for the custom button.
 * @param color Color value.
 */
void CustomButton::setColor(Uint8 color)
{
	_color = color;
}

/**
 * Returns the color for the custom button.
 * @return Color value.
 */
Uint8 CustomButton::getColor()
{
	return _color;
}

/**
 * Changes the button group this button belongs to.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void CustomButton::setGroup(CustomButton **group)
{
	_group = group;
	if (_group != 0 && *_group == this)
		invert(_color);
}

/**
 * Blits the custom button onto another surface.
 * @param surface Pointer to surface to blit onto.
 */
void CustomButton::blit(Surface *surface)
{
	Surface::blit(surface);
}

/**
 * Ignores any mouse clicks that aren't the left mouse button.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void CustomButton::handle(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
		InteractiveSurface::handle(ev, scale, state);
}

/**
 * Sets the button as the pressed button if it's part of a group,
 * and inverts the colors when pressed.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void CustomButton::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (_group != 0)
	{
		(*_group)->invert((*_group)->getColor());
		*_group = this;
	}
	invert(_color);
	InteractiveSurface::mousePress(ev, scale, state);
}

/*
 * Sets the button as the released button if it's part of a group.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void CustomButton::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (_group == 0)
		invert(_color);
	InteractiveSurface::mouseRelease(ev, scale, state);
}