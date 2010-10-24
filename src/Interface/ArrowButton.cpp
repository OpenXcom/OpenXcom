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
#include "ArrowButton.h"
#include "TextList.h"
#include "../Engine/Timer.h"

/**
 * Sets up an arrow button with the specified size and position.
 * @param shape Shape of the arrow.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
ArrowButton::ArrowButton(ArrowShape shape, int width, int height, int x, int y) : ImageButton(width, height, x, y), _shape(shape), _list(0)
{
	_validButton = SDL_BUTTON_LEFT;

	_timer = new Timer(50);
	_timer->onTimer((SurfaceHandler)&ArrowButton::scroll);
}

/**
 *
 */
ArrowButton::~ArrowButton()
{
	
}

/**
 * Changes the color for the arrow button.
 * @param color Color value.
 */
void ArrowButton::setColor(Uint8 color)
{
	ImageButton::setColor(color);
	draw();
}

/**
 * Changes the text associated with the arrow button.
 * This makes the button scroll that list.
 * @param list Pointer to text list.
 */
void ArrowButton::setTextList(TextList *list)
{
	_list = list;
}

/**
 * Draws the button with the specified arrow shape.
 */
void ArrowButton::draw()
{
	lock();

	// Draw button
	SDL_Rect square;
	int color = _color - 1;

	square.x = 0;
	square.y = 0;
	square.w = _width - 1;
	square.h = _height - 1;

	SDL_FillRect(_surface, &square, color);

	square.x++;
	square.y++;
	color = _color + 2;

	SDL_FillRect(_surface, &square, color);

	square.w--;
	square.h--;
	color = _color + 1;

	SDL_FillRect(_surface, &square, color);

	setPixel(0, 0, _color - 2);
	setPixel(0, _height - 1, _color + 1);
	setPixel(_width - 1, 0, _color + 1);

	color = _color - 2;

	if (_shape == ARROW_BIG_UP)
	{
		// Draw arrow square
		square.x = 5;
		square.y = 8;
		square.w = 3;
		square.h = 3;

		SDL_FillRect(_surface, &square, color);

		// Draw arrow triangle
		square.x = 2;
		square.y = 7;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			SDL_FillRect(_surface, &square, color);
			square.x++;
			square.y--;
		}
		SDL_FillRect(_surface, &square, color);
	}
	else if (_shape == ARROW_BIG_DOWN)
	{
		// Draw arrow square
		square.x = 5;
		square.y = 3;
		square.w = 3;
		square.h = 3;

		SDL_FillRect(_surface, &square, color);

		// Draw arrow triangle
		square.x = 2;
		square.y = 6;
		square.w = 9;
		square.h = 1;

		for (; square.w > 1; square.w -= 2)
		{
			SDL_FillRect(_surface, &square, color);
			square.x++;
			square.y++;
		}
		SDL_FillRect(_surface, &square, color);
	}

	unlock();
}

/**
 * Keeps the scrolling timers running.
 */
void ArrowButton::think()
{
	_timer->think(0, this);
}

/**
 * Scrolls the list.
 */
void ArrowButton::scroll()
{
	if (_shape == ARROW_BIG_UP)
	{
		_list->scrollUp();
	}
	else if (_shape == ARROW_BIG_DOWN)
	{
		_list->scrollDown();
	}
}

/**
 * Starts scrolling the associated list.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ArrowButton::mousePress(Action *action, State *state)
{
	ImageButton::mousePress(action, state);
	if (_list != 0)
	{
		_timer->start();
	}
}

/*
 * Stops scrolling the associated list.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ArrowButton::mouseRelease(Action *action, State *state)
{
	ImageButton::mouseRelease(action, state);
	if (_list != 0)
	{
		_timer->stop();
	}
}
