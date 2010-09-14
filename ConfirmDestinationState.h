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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM__CONFIRMDESTINATIONSTATE_H
#define OPENXCOM__CONFIRMDESTINATIONSTATE_H

#include "State.h"

class Window;
class Text;
class TextButton;
class Craft;
class Target;

/**
 * Screen that allows the player
 * to confirm a new base on the globe.
 */
class ConfirmDestinationState : public State
{
private:
	Craft *_craft;
	Target *_target;
	Window *_window;
	Text *_txtTarget;
	TextButton *_btnOk, *_btnCancel;
public:
	/// Creates the Confirm Destination state.
	ConfirmDestinationState(Game *game, Craft *craft, Target *target);
	/// Cleans up the Confirm Destination state.
	~ConfirmDestinationState();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
