/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <SDL.h>
#include <SDL_mixer.h>
#include "../fmath.h"
#include "../Engine/Timer.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

const double Window::POPUP_SPEED = 0.05;

Sound *Window::soundPopup[3] = {0, 0, 0};

/**
 * Sets up a blank window with the specified size and position.
 * @param state Pointer to state the window belongs to.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 * @param popup Popup animation.
 */
Window::Window(State *state, int width, int height, int x, int y, WindowPopup popup) : Surface(width, height, x, y), _dx(-x), _dy(-y), _bg(0), _color(0), _popup(popup), _popupStep(0.0), _state(state), _contrast(false), _screen(false), _thinBorder(false)
{
	_timer = new Timer(10);
	_timer->onTimer((SurfaceHandler)&Window::popup);

	if (_popup == POPUP_NONE)
	{
		_popupStep = 1.0;
	}
	else
	{
		setHidden(true);
		_timer->start();
		if (_state != 0)
		{
			_screen = state->isScreen();
			if (_screen)
				_state->toggleScreen();
		}
	}
}

/**
 * Deletes timers.
 */
Window::~Window()
{
	delete _timer;
}

/**
 * Changes the surface used to draw the background of the window.
 * @param bg New background.
 */
void Window::setBackground(Surface *bg)
{
	_bg = bg;
	_redraw = true;
}

/**
 * Changes the color used to draw the shaded border.
 * @param color Color value.
 */
void Window::setColor(Uint8 color)
{
	_color = color;
	_redraw = true;
}

/**
 * Returns the color used to draw the shaded border.
 * @return Color value.
 */
Uint8 Window::getColor() const
{
	return _color;
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape UI.
 * @param contrast High contrast setting.
 */
void Window::setHighContrast(bool contrast)
{
	_contrast = contrast;
	_redraw = true;
}

/**
 * Keeps the animation timers running.
 */
void Window::think()
{
	if (_hidden && _popupStep < 1.0)
	{
		_state->hideAll();
		setHidden(false);
	}

	_timer->think(0, this);
}

/**
 * Plays the window popup animation.
 */
void Window::popup()
{
	if (AreSame(_popupStep, 0.0))
	{
		int sound = RNG::seedless(0,2);
		if (soundPopup[sound] != 0)
		{
			soundPopup[sound]->play(Mix_GroupAvailable(0));
		}
	}
	if (_popupStep < 1.0)
	{
		_popupStep += POPUP_SPEED;
	}
	else
	{
		if (_screen)
		{
			_state->toggleScreen();
		}
		_state->showAll();
		_popupStep = 1.0;
		_timer->stop();
	}
	_redraw = true;
}

/**
 * Draws the bordered window with a graphic background.
 * The background never moves with the window, it's
 * always aligned to the top-left corner of the screen
 * and cropped to fit the inside area.
 */
void Window::draw()
{
	Surface::draw();
	SDL_Rect square;

	if (_popup == POPUP_HORIZONTAL || _popup == POPUP_BOTH)
	{
		square.x = (int)((getWidth() - getWidth() * _popupStep) / 2);
		square.w = (int)(getWidth() * _popupStep);
	}
	else
	{
		square.x = 0;
		square.w = getWidth();
	}
	if (_popup == POPUP_VERTICAL || _popup == POPUP_BOTH)
	{
		square.y = (int)((getHeight() - getHeight() * _popupStep) / 2);
		square.h = (int)(getHeight() * _popupStep);
	}
	else
	{
		square.y = 0;
		square.h = getHeight();
	}

	int mul = 1;
	if (_contrast)
	{
		mul = 2;
	}
	Uint8 color = _color + 3 * mul;

	if (_thinBorder)
	{
		color = _color + 1 * mul;
		for (int i = 0; i < 5; ++i)
		{
			drawRect(&square, color);

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
				color = _color + 5 * mul;
				setPixel(square.w, 0, color);
				break;
			case 1:
				color = _color + 2 * mul;
				break;
			case 2:
				color = _color + 4 * mul;
				setPixel(square.w+1, 1, color);
				break;
			case 3:
				color = _color + 3 * mul;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 5; ++i)
		{
			drawRect(&square, color);
			if (i < 2)
				color -= 1 * mul;
			else
				color += 1 * mul;
			square.x++;
			square.y++;
			if (square.w >= 2)
				square.w -= 2;
			else
				square.w = 1;

			if (square.h >= 2)
				square.h -= 2;
			else
				square.h = 1;
		}
	}

	if (_bg != 0)
	{
		_bg->getCrop()->x = square.x - _dx;
		_bg->getCrop()->y = square.y - _dy;
		_bg->getCrop()->w = square.w ;
		_bg->getCrop()->h = square.h ;
		_bg->setX(square.x);
		_bg->setY(square.y);
		_bg->blit(this);
	}
}

/**
 * Changes the horizontal offset of the surface in the X axis.
 * @param dx X position in pixels.
 */
void Window::setDX(int dx)
{
	_dx = dx;
}

/**
 * Changes the vertical offset of the surface in the Y axis.
 * @param dy Y position in pixels.
 */
void Window::setDY(int dy)
{
	_dy = dy;
}

/**
 * Changes the window to have a thin border.
 */
void Window::setThinBorder()
{
	_thinBorder = true;
}
}
