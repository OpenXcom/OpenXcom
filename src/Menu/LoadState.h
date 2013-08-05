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
#ifndef OPENXCOM__LOADSTATE_H
#define OPENXCOM__LOADSTATE_H

#include "SavedGameState.h"

namespace OpenXcom
{

/**
 * Load Game screen for listing info on available
 * saved games and loading them.
 */
class LoadState : public SavedGameState
{
private:
public:
	/// Creates the Load Game state.
	LoadState(Game *game, bool geo);
	/// Creates the Quick Load Game state.
	LoadState(Game *game, bool geo, bool showMsg);
	/// Cleans up the Load Game state.
	~LoadState();
	/// Handler for clicking the Saves list.
	void lstSavesPress(Action *action);
	/// Quick load game.
	void quickLoad(const std::wstring &filename16);
};

}

#endif
