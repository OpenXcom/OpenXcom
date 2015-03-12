/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_OPTIONSMODSSTATE_H
#define OPENXCOM_OPTIONSMODSSTATE_H

#include "OptionsBaseState.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class TextList;

/**
 * Options window that displays the
 * available mods.
 */
class OptionsModsState : public OptionsBaseState
{
private:
	TextList *_lstMods;
	std::vector<std::string> _mods;
public:
	/// Creates the Advanced state.
	OptionsModsState(OptionsOrigin origin);
	/// Cleans up the Advanced state.
	~OptionsModsState();
	/// Handler for clicking an item on the menu.
	void lstModsClick(Action *action);

};

}

#endif
