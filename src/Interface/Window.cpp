/*
 * Copyright 2010 OpenXcom Developers.
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
#include "SDL.h"
#include "../Engine/Timer.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"

#define POPUP_SPEED 0.075

namespace OpenXcom
{

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
Window::Window(State *state, int width, int height, int x, int y, WindowPopup popup) : Surface(width, height, x, y), _bg(0), _color(0), _popup(popup), _popupStep(0.0), _state(state)
{
	_timer = new Timer(10);
	_timer->onTimer((SurfaceHandler)&Window::popup);

	if (_popup == POPUP_NONE)
	{
		_popupStep = 1.0;
	}
	else
	{
		_timer->start();
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
	if (_popupStep < 1.0)
	{
		for (std::vector<Surface*>::iterator i = _state->getSurfaces()->begin(); i < _state->getSurfaces()->end(); i++)
			if ((*i) != this)
				(*i)->hide();
	}

	_bg = bg;
	draw();
}

/**
 * Changes the color used to draw the shaded border.
 * @param color Color value.
 */
void Window::setColor(Uint8 color)
{
	if (_popupStep < 1.0)
	{
		for (std::vector<Surface*>::iterator i = _state->getSurfaces()->begin(); i < _state->getSurfaces()->end(); i++)
			if ((*i) != this)
				(*i)->hide();
	}

	_color = color;
	draw();
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
 * Keeps the animation timers running.
 */
void Window::think()
{
	_timer->think(0, this);
}

/**
 * Plays the window popup animation.
 */
void Window::popup()
{
	if (_popupStep == 0.0)
	{
		int sound = RNG::generate(0, 2);
		if (soundPopup[sound] != 0)
			soundPopup[sound]->play();
	}
	if (_popupStep < 1.0)
	{
		_popupStep += POPUP_SPEED;
	}
	else
	{
		for (std::vector<Surface*>::iterator i = _state->getSurfaces()->begin(); i < _state->getSurfaces()->end(); i++)
			if ((*i) != this)
				(*i)->show();
		_popupStep = 1.0;
		_timer->stop();
	}
	draw();
}

/**
 * Draws the bordered window with a graphic background.
 * The background never moves with the window, it's
 * always aligned to the top-left corner of the screen
 * and cropped to fit the inside area.
 */
void Window::draw()
{
	SDL_Rect square;
	Uint8 color = _color;
	
	clear();
	
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

	for (int i = 0; i < 5; i++)
	{
		drawRect(&square, color);
		if (i < 2)
			color--;
		else
			color++;
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
		
	if (_bg != 0)
	{
		_bg->getCrop()->x = getX() + square.x;
		_bg->getCrop()->y = getY() + square.y;
		_bg->getCrop()->w = square.w;
		_bg->getCrop()->h = square.h;
		_bg->setX(square.x);
		_bg->setY(square.y);
		_bg->blit(this);
	}
}

}
