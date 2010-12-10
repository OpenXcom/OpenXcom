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
#ifndef OPENXCOM_ACTION_H
#define OPENXCOM_ACTION_H

#include "SDL.h"

namespace OpenXcom
{

class InteractiveSurface;

/**
 * Container for all the information associated with a
 * given user action, like mouse clicks, key presses, etc.
 * @note Called action because event is reserved.
 */
class Action
{
private:
	SDL_Event *_ev;
	double _xScale, _yScale;
	InteractiveSurface *_sender;
public:
	/// Creates an action with given event data.
	Action(SDL_Event *ev, double xScale, double yScale);
	/// Cleans up the action.
	~Action();
	/// Gets the screen's X scale.
	double getXScale() const;
	/// Gets the screen's Y scale.
	double getYScale() const;
	/// Gets the sender of the action.
	InteractiveSurface *const getSender() const;
	/// Sets the sender of the action.
	void setSender(InteractiveSurface *sender);
	/// Gets the details of the action.
	SDL_Event *const getDetails() const;
};

}

#endif
