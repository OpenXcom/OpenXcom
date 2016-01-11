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
#include "../Engine/State.h"
#include "OptionsBaseState.h"
#include <vector>
#include <string>
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class ArrowButton;

/**
 * Base class for saved game screens which
 * provides the common layout and listing.
 */
class ListGamesState : public State
{
protected:
	TextButton *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtDate, *_txtDelete, *_txtDetails;
	TextList *_lstSaves;
	ArrowButton *_sortName, *_sortDate;
	OptionsOrigin _origin;
	std::vector<SaveInfo> _saves;
	unsigned int _firstValidRow;
	bool _autoquick, _sortable;

	void updateArrows();
public:
	/// Creates the Saved Game state.
	ListGamesState(OptionsOrigin origin, int firstValidRow, bool autoquick);
	/// Cleans up the Saved Game state.
	virtual ~ListGamesState();
	/// Sets up the saves list.
	void init();
	/// Sorts the savegame list.
	void sortList(SaveSort sort);
	/// Updates the savegame list.
	virtual void updateList();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for moving the mouse over a list item.
	void lstSavesMouseOver(Action *action);
	/// Handler for moving the mouse outside the list borders.
	void lstSavesMouseOut(Action *action);
	/// Handler for clicking the Saves list.
	virtual void lstSavesPress(Action *action);
	/// Handler for clicking the Name arrow.
	void sortNameClick(Action *action);
	/// Handler for clicking the Date arrow.
	void sortDateClick(Action *action);
	/// disables the sort buttons.
	void disableSort();
};

}
