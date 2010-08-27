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
#include "ImageButton.h"

/**
 * Sets up an image button with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
ImageButton::ImageButton(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(0)
{
}

/**
 *
 */
ImageButton::~ImageButton()
{
	
}

/**
 * Changes the color for the image button.
 * @param color Color value.
 */
void ImageButton::setColor(Uint8 color)
{
	_color = color;
}

/**
 * Returns the color for the image button.
 * @return Color value.
 */
Uint8 ImageButton::getColor()
{
	return _color;
}

/**
 * Changes the button group this image button belongs to.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void ImageButton::setGroup(ImageButton **group)
{
	_group = group;
	if (_group != 0 && *_group == this)
		invert(_color);
}

/**
 * Sets the button as the pressed button if it's part of a group,
 * and inverts the colors when pressed.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void ImageButton::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_group != 0)
		{
			(*_group)->invert((*_group)->getColor());
			*_group = this;
		}
		invert(_color);
		InteractiveSurface::mousePress(ev, scale, state);
	}
}

/*
 * Sets the button as the released button if it's part of a group.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void ImageButton::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_group == 0)
			invert(_color);
		InteractiveSurface::mouseRelease(ev, scale, state);
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void ImageButton::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}
