/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_DELAYEDSAVESTATE_H
#define OPENXCOM_DELAYEDSAVESTATE_H

#include "BattleState.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class BattlescapeGame;
class Game;

class DelayedSaveState : public BattleState
{
private:
	Game *_game;
	SaveType _type;
	bool _firstRun;
public:
	/// Constructor for the delayed save state.
	DelayedSaveState(BattlescapeGame *parent, Game *game, SaveType type);
	/// Deletes the state.
	~DelayedSaveState();
	/// Initializes the state.
	void init();
	/// Runs state functionality every cycle.
	void think();
};

}

#endif
