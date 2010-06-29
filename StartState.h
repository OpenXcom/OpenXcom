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
#ifndef OPENXCOM__STARTSTATE_H
#define OPENXCOM__STARTSTATE_H

#include "State_Interactive.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "MainMenuState.h"

/**
 * Initializes the game and displays the Language
 * window that lets the player pick a language.
 */
class StartState : public State
{
private:
	TextButton *_btnEnglish, *_btnGerman, *_btnFrench;
	Window *_window;
public:
	/// Creates the Start state.
	StartState(Game *game);
	/// Cleans up the Start state.
	~StartState();
	/// Handler for clicking the English button.
	void btnEnglishClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Deutsche button.
	void btnGermanClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Francais button.
	void btnFrenchClick(SDL_Event *ev, int scale);
};

#endif
