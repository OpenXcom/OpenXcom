/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_NEWGAMESTATE_H
#define OPENXCOM_NEWGAMESTATE_H

#include "../Engine/State.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;

/**
 * New Game window that displays a list
 * of possible difficulties for creating
 * a saved game.
 */
class NewGameState : public State
{
private:
	TextButton *_btnBeginner, *_btnExperienced, *_btnVeteran;
	TextButton *_btnGenius, *_btnSuperhuman, *_btnCancel;
	Window *_window;
	Text *_txtTitle;
public:
	/// Creates the New Game state.
	NewGameState(Game *game);
	/// Cleans up the New Game state.
	~NewGameState();
	/// Creates a new game.
	void newGame(GameDifficulty diff);
	/// Handler for clicking the Beginner button.
	void btnBeginnerClick(Action *action);
	/// Handler for clicking the Experienced button.
	void btnExperiencedClick(Action *action);
	/// Handler for clicking the Veteran button.
	void btnVeteranClick(Action *action);
	/// Handler for clicking the Genius button.
	void btnGeniusClick(Action *action);
	/// Handler for clicking the Superhuman button.
	void btnSuperhumanClick(Action *action);
	/// Handler for clicking the cancel button.
	void btnCancelClick(Action *);
};

}

#endif
