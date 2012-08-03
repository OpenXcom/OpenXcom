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
#ifndef OPENXCOM__LOADGAMESTATE_H
#define OPENXCOM__LOADGAMESTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

/**
 * Load Game screen for listing info on available
 * saved games and loading them.
 */
class LoadGameState : public State
{
private:
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtTime, *_txtDate;
	TextList *_lstSaves;
	bool _geo;
public:
	/// Creates the Load Game state.
	LoadGameState(Game *game, bool geo);
	/// Cleans up the Load Game state.
	~LoadGameState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Saves list.
	void lstSavesClick(Action *action);
};

}

#endif
