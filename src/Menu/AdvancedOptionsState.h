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
#ifndef OPENXCOM_ADVANCEDOPTIONSSTATE_H
#define OPENXCOM_ADVANCEDOPTIONSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class InteractiveSurface;

/**
 * Options window that displays all
 * the settings the player can configure.
 */
class AdvancedOptionsState : public State
{
private:
	Window *_window;
	Text *_txtTitle, *_txtDescription;
	TextList *_lstOptions;
	size_t _boolQuantity;
	TextButton *_btnOk, *_btnCancel, *_btnDefault;
	// intentionally avoiding using a map here, to avoid auto-sorting.
	std::vector<std::pair<std::string, bool> > _settingBoolSet;
	std::vector<std::pair<std::string, int> > _settingIntSet;
public:
	/// Creates the Options state.
	AdvancedOptionsState(Game *game);
	/// Cleans up the Options state.
	~AdvancedOptionsState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Restore Defaults button.
	void btnDefaultClick(Action *action);
	/// Handler for clicking an item on the menu.
	void lstOptionsClick(Action *action);
	/// Handler for moving the mouse over a menu item.
	void lstOptionsMouseOver(Action *action);
	/// Handler for moving the mouse outside the menu borders.
	void lstOptionsMouseOut(Action *action);
	/// special function to sub out strings for pathfinding settings
	std::wstring updatePathString(int sel);

};

}

#endif
