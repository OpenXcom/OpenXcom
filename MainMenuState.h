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
#ifndef OPENXCOM__MAINMENUSTATE_H
#define OPENXCOM__MAINMENUSTATE_H

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "NewGameState.h"

/**
 * Main Menu window displayed when first
 * starting the game.
 */
class MainMenuState : public State
{
private:
	TextButton *_btnNew, *_btnLoad, *_btnQuit;
	Window *_window;
	Text *_txtTitle, *_txtVersion;
public:
	/// Creates the Main Menu state.
	MainMenuState(Game *game);
	/// Cleans up the Main Menu state.
	~MainMenuState();
	/// Handler for clicking the New Game button.
	void btnNewClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Load Saved Game button.
	void btnLoadClick(SDL_Event *ev, int scale);
	/// Handler for clicking the Quit button.
	void btnQuitClick(SDL_Event *ev, int scale);
};

#endif
