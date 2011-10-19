/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM__SAVEGAMESTATE_H
#define OPENXCOM__SAVEGAMESTATE_H

#include <string>
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class TextEdit;

/**
 * Save Game screen for listing info on available
 * saved games and saving them.
 */
class SaveGameState : public State
{
private:
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtTime, *_txtDate;
	TextList *_lstSaves;
	TextEdit *_edtSave;
	std::string _selected;
	bool _geo;
        int _previousSelectedRow, _selectedRow;
public:
	/// Creates the Save Game state.
	SaveGameState(Game *game, bool geo);
	/// Cleans up the Save Game state.
	~SaveGameState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for pressing a key on the Save edit.
	void edtSaveKeyPress(Action *action);
	/// Handler for clicking the Saves list.
	void lstSavesClick(Action *action);
};

}

#endif
