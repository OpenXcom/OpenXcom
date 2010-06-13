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

/**
 * Sets up a button with the specified size and position.
 * The text is centered on the button.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Button::Button(Font *big, Font *small, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _color(0), _group(0)
{
	_text = new Text(big, small, width, small->getHeight(), 0, (int)ceil((double)(height - small->getHeight()) / 2.0));
	_text->setSmall();
	_text->setAlign(ALIGN_CENTER);
}

/**
 * Deletes the contained Text.
 */
Button::~Button()
{
	delete _text;
}

/**
 * Changes the color for the button and text.
 * @param color Color value.
 */
void Button::setColor(Uint8 color)
{
	_color = color;
	_text->setColor(_color-3);
	draw();
}

/**
 * Returns the color for the button and text.
 * @return Color value.
 */
Uint8 Button::getColor()
{
	return _color;
}

/**
 * Changes the text of the button label.
 * @param text Text string.
 */
void Button::setText(string text)
{
	_text->setText(text);
	draw();
}

/**
 * Returns the text of the button label.
 * @return Text string.
 */
string Button::getText()
{
	return _text->getText();
}

/**
 * Changes the button group this button belongs to.
 * @param group Pointer to the pressed button pointer in the group.
 * Null makes it a regular button.
 */
void Button::setGroup(Button **group)
{
	_group = group;
}

/**
 * Replaces a certain amount of colors in the surface's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Button::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Draws the labelled button.
 * The colors are inverted if the button is pressed.
 */
void Button::draw()
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
	if (_group == 0)
		press = _isPressed;
	else
		press = (*_group == this);

	if (press)
	{
		this->invert(_color);
	}
	_text->setInvert(press);

	_text->draw();
	_text->blit(this);
}

/**
 * Sets the button as the pressed button if it's part of a group.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void Button::mousePress(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		if (_group != 0)
			*_group = this;

		InteractiveSurface::mousePress(ev, scale, state);
		draw();
	}
}

/**
 * Sets the button as the released button.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void Button::mouseRelease(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseRelease(ev, scale, state);
		draw();
	}
}

/**
 * Only accepts left clicks.
 * @param ev Pointer to a SDL_Event.
 * @param scale Current screen scale (used to correct mouse input).
 * @param state State that the event handlers belong to.
 */
void Button::mouseClick(SDL_Event *ev, int scale, State *state)
{
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		InteractiveSurface::mouseClick(ev, scale, state);
	}
}