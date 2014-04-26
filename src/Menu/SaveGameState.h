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
#ifndef OPENXCOM__SAVEGAMESTATE
#define OPENXCOM__SAVEGAMESTATE

#include "../Engine/State.h"
#include <string>
#include "OptionsBaseState.h"

namespace OpenXcom
{

class Text;

/**
 * Saves the current game, with an optional message.
 */
class SaveGameState : public State
{
private:
	OptionsOrigin _origin;
	Text *_txtStatus;
	std::string _filename;
public:
	/// Creates the Save Game state.
	SaveGameState(Game *game, OptionsOrigin origin, const std::string &filename, bool showMsg = true);
	/// Cleans up the Save Game state.
	~SaveGameState();
	/// Saves the game.
	void init();
};

}

#endif
