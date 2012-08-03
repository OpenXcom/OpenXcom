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
#ifndef OPENXCOM_NEWBATTLESTATE_H
#define OPENXCOM_NEWBATTLESTATE_H

#include "../Engine/State.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Selector;

/**
 * New Game window that displays a list
 * of possible difficulties for creating
 * a saved game.
 */
class NewBattleState : public State
{
private:
	Window *_window;
	Text *_txtTitle, *_txtMissionType, *_txtMissionOption, *_txtAlien, *_txtSunlight, *_txtCraft;
	Selector *_selMissionType, *_selMissionOption, *_selAlienRace, *_selSunlight, *_selCraft;
	TextButton *_btnOk, *_btnCancel, *_btnCraft;
public:
	/// Creates the New Game state.
	NewBattleState(Game *game);
	/// Cleans up the New Game state.
	~NewBattleState();
	/// Initializes a blank savegame.
	void initSave();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
