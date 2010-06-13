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
#ifndef OPENXCOM__BASESCAPEERRORSTATE_H
#define OPENXCOM__BASESCAPEERRORSTATE_H

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"

/**
 * Generic window used to display error messages
 * when the player is on the Basescape.
 */
class BasescapeErrorState : public State
{
private:
	Button *_btnOk;
	Window *_window;
	Text *_txtError;
public:
	/// Creates the Basescape Error state.
	BasescapeErrorState(Game *game, LangString string);
	/// Cleans up the Basescape Error state.
	~BasescapeErrorState();
	/// Handler for clicking the OK button.
	void btnOkClick(SDL_Event *ev, int scale);
};

#endif
