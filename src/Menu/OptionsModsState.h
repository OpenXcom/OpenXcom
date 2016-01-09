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
#include "OptionsBaseState.h"
#include "../Engine/ModInfo.h"
#include <vector>
#include <string>
#include <utility>

namespace OpenXcom
{

class TextList;
class ComboBox;

/**
 * Options window that displays the
 * available mods.
 */
class OptionsModsState : public OptionsBaseState
{
private:
	Text     *_txtMaster;
	ComboBox *_cbxMasters;
	TextList *_lstMods;
	std::vector<const ModInfo *> _masters;
	std::string _curMasterId;
	std::vector< std::pair<std::string, bool> > _mods;
public:
	/// Creates the Advanced state.
	OptionsModsState(OptionsOrigin origin);
	/// Cleans up the Advanced state.
	~OptionsModsState();
	std::wstring makeTooltip(const ModInfo &modInfo);
	void cbxMasterHover(Action *action);
	void cbxMasterChange(Action *action);
	void lstModsRefresh(size_t scrollLoc);
	void lstModsHover(Action *action);
	/// Handler for clicking an item on the menu.
	void lstModsClick(Action *action);
	/// Handler for clicking the left reordering button.
	void lstModsLeftArrowClick(Action *action);
	/// Moves a mod up.
	void moveModUp(Action *action, unsigned int row, bool max = false);
	/// Handler for clicking the right reordering button.
	void lstModsRightArrowClick(Action *action);
	/// Moves a mod down.
	void moveModDown(Action *action, unsigned int row, bool max = false);
	/// Handler for pressing-down a mouse-button in the list.
	void lstModsMousePress(Action *action);
};

}
