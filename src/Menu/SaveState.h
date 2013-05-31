/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM__SAVESTATE_H
#define OPENXCOM__SAVESTATE_H

#include <string>
#include "SavedGameState.h"

namespace OpenXcom
{

class TextEdit;

/**
 * Save Game screen for listing info on available
 * saved games and saving them.
 */
class SaveState : public SavedGameState
{
private:
	TextEdit *_edtSave;
	std::wstring _selected;
	int _previousSelectedRow, _selectedRow;
public:
	/// Creates the Save Game state.
	SaveState(Game *game, bool geo);
	/// Creates the Quick Save Game state.
	SaveState(Game *game, bool geo, bool showMsg);
	/// Cleans up the Save Game state.
	~SaveState();
	/// Updates the savegame list.
	void updateList();
	/// Handler for pressing a key on the Save edit.
	void edtSaveKeyPress(Action *action);
	/// Handler for clicking the Saves list.
	void lstSavesPress(Action *action);
	/// Quick save game.
	void quickSave(const std::wstring &filename16);
};

}

#endif
