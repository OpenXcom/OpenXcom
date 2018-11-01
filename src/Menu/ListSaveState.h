#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <string>
#include "ListGamesState.h"

namespace OpenXcom
{

class TextEdit;
class TextButton;

/**
 * Save Game screen for listing info on available
 * saved games and saving them.
 */
class ListSaveState : public ListGamesState
{
private:
	TextEdit *_edtSave;
	TextButton *_btnSaveGame;
	std::string _selected;
	int _previousSelectedRow, _selectedRow;
public:
	/// Creates the Save Game state.
	ListSaveState(OptionsOrigin origin);
	/// Cleans up the Save Game state.
	~ListSaveState();
	/// Updates the savegame list.
	void updateList();
	/// Handler for pressing a key on the Save edit.
	void edtSaveKeyPress(Action *action);
	/// Handler for clicking on the Save Game button.
	void btnSaveGameClick(Action *action);
	/// Handler for clicking the Saves list.
	void lstSavesPress(Action *action);
	/// Save game.
	void saveGame();
};

}
