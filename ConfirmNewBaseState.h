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
#ifndef OPENXCOM__CONFIRMNEWBASESTATE_H
#define OPENXCOM__CONFIRMNEWBASESTATE_H

#include "State.h"

class Base;
class Window;
class Text;
class TextButton;

/**
 * Screen that allows the player
 * to confirm a new base on the globe.
 */
class ConfirmNewBaseState : public State
{
private:
	Base *_base;
	Window *_window;
	Text *_txtCost, *_txtArea;
	TextButton *_btnOk, *_btnCancel;
	int _cost;
public:
	/// Creates the Confirm New Base state.
	ConfirmNewBaseState(Game *game, Base *base);
	/// Cleans up the Confirm New Base state.
	~ConfirmNewBaseState();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
