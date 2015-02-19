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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_CONFIRMLANDINGSTATE_H
#define OPENXCOM_CONFIRMLANDINGSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Window;
class Text;
class TextButton;
class Craft;

/**
 * Window that allows the player
 * to confirm a craft landing at its destination.
 */
class ConfirmLandingState : public State
{
private:
	Craft *_craft;
	Window *_window;
	int _texture, _shade;
	Text *_txtMessage, *_txtBegin;
	TextButton *_btnYes, *_btnNo;
public:
	/// Creates the Confirm Landing state.
	ConfirmLandingState(Craft *craft, int texture, int shade);
	/// Cleans up the Confirm Landing state.
	~ConfirmLandingState();
	/// initialize the state, make a sanity check.
	void init();
	/// Handler for clicking the Yes button.
	void btnYesClick(Action *action);
	/// Handler for clicking the No button.
	void btnNoClick(Action *action);
};

}

#endif
