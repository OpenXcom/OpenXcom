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
#ifndef OPENXCOM__LISTLOADSTATE_H
#define OPENXCOM__LISTLOADSTATE_H

#include "ListGamesState.h"

namespace OpenXcom
{

/**
 * Load Game screen for listing info on available
 * saved games and loading them.
 */
class ListLoadState : public ListGamesState
{
public:
	/// Creates the Load Game state.
	ListLoadState(Game *game, OptionsOrigin origin);
	/// Cleans up the Load Game state.
	~ListLoadState();
	/// Handler for clicking the Saves list.
	void lstSavesPress(Action *action);
};

}

#endif
