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

#include "Action.h"

namespace OpenXcom
{

/**
 * Creates a new action.
 * @param scaleX Screen's X scaling factor.
 * @param scaleY Screen's Y scaling factor.
 * @param topBlackBand Screen's top black band height.
 * @param leftBlackBand Screen's left black band width.
 * @param ev Pointer to SDL_event.
 */
Action::Action(SDL_Event *ev, double scaleX, double scaleY, int topBlackBand, int leftBlackBand) : _ev(ev), _scaleX(scaleX), _scaleY(scaleY), _topBlackBand(topBlackBand), _leftBlackBand(leftBlackBand), _mouseX(-1), _mouseY(-1), _surfaceX(-1), _surfaceY(-1), _sender(0)
{
}

Action::~Action()
{
}

/**
 * Returns the X scaling factor used by the screen
 * when this action was fired (used to correct mouse input).
 * @return Screen's X scaling factor.
 */
double Action::getXScale() const
{
	return _scaleX;
}

/**
 * Returns the Y scaling factor used by the screen
 * when this action was fired (used to correct mouse input).
 * @return Screen's Y scaling factor.
 */
double Action::getYScale() const
{
	return _scaleY;
}

/**
 * Sets this action as a mouse action with
 * the respective mouse properties.
 * @param mouseX Mouse's X position.
 * @param mouseY Mouse's Y position.
 * @param surfaceX Surface's X position.
 * @param surfaceY Surface's Y position.
 */
void Action::setMouseAction(int mouseX, int mouseY, int surfaceX, int surfaceY)
{
	_mouseX = mouseX - _leftBlackBand;
	_mouseY = mouseY - _topBlackBand;
	_surfaceX = surfaceX;
	_surfaceY = surfaceY;
}

bool Action::isMouseAction() const
{
	return (_mouseX != -1);
}

/**
 * Returns the height in pixel of the
 * top black band if any.
 * @return Screen's top black band.
 */
int Action::getTopBlackBand() const
{
	return _topBlackBand;
}

/**
 * Returns the width in pixel of the
 * left black band if any.
 * @return Screen's left black band.
 */
int Action::getLeftBlackBand() const
{
	return _leftBlackBand;
}

/**
 * Returns the X position of the
 * mouse cursor relative to the game window,
 * or -1 if this isn't a mouse-related action.
 * @return Mouse's X position.
 */
int Action::getXMouse() const
{
	return _mouseX;
}

/**
 * Returns the Y position of the
 * mouse cursor relative to the game window,
 * or -1 if this isn't a mouse-related action.
 * @return Mouse's Y position.
 */
int Action::getYMouse() const
{
	return _mouseY;
}

/**
 * Returns the absolute X position of the
 * mouse cursor relative to the game window,
 * corrected for screen scaling.
 * @return Mouse's absolute X position.
 */
double Action::getAbsoluteXMouse() const
{
	if (_mouseX == -1)
		return -1;
	return _mouseX / _scaleX;
}

/**
 * Returns the absolute Y position of the
 * mouse cursor relative to the game window,
 * corrected for screen scaling.
 * @return Mouse's absolute X position.
 */
double Action::getAbsoluteYMouse() const
{
	if (_mouseY == -1)
		return -1;
	return _mouseY / _scaleY;
}

/**
 * Returns the relative X position of the
 * mouse cursor relative to the surface that
 * triggered the action, corrected for screen scaling.
 * @return Mouse's relative X position.
 */
double Action::getRelativeXMouse() const
{
	if (_mouseX == -1)
		return -1;
	return _mouseX - _surfaceX * _scaleX;
}

/**
 * Returns the relative X position of the
 * mouse cursor relative to the surface that
 * triggered the action, corrected for screen scaling.
 * @return Mouse's relative X position.
 */
double Action::getRelativeYMouse() const
{
	if (_mouseY == -1)
		return -1;
	return _mouseY - _surfaceY * _scaleY;
}

/**
 * Returns the interactive surface that triggered
 * this action (the sender).
 * @return Pointer to interactive surface.
 */
InteractiveSurface *Action::getSender() const
{
	return _sender;
}

/**
 * Changes the interactive surface that triggered
 * this action (the sender).
 * @param sender Pointer to interactive surface.
 */
void Action::setSender(InteractiveSurface *sender)
{
	_sender = sender;
}

/**
 * Returns the details about this action.
 * @return Pointer to SDL_event.
 */
SDL_Event *Action::getDetails() const
{
	return _ev;
}

}
