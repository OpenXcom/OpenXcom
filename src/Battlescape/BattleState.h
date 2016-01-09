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
#include "BattlescapeGame.h"

namespace OpenXcom
{

class BattlescapeGame;

/**
 * This class sets the battlescape in a certain sub-state.
 * These states can be triggered by the player or the AI.
 */
class BattleState
{
protected:
	BattlescapeGame *_parent;
	BattleAction _action;
public:
	/// Creates a new BattleState linked to the game.
	BattleState(BattlescapeGame *parent, BattleAction action);
	/// Creates a new BattleState linked to the game.
	BattleState(BattlescapeGame *parent);
	/// Cleans up the BattleState.
	virtual ~BattleState();
	/// Initializes the state.
	virtual void init();
	/// Handles a cancel request.
	virtual void cancel();
	/// Runs state functionality every cycle.
	virtual void think();
	/// Gets a copy of the action.
	BattleAction getAction() const;
};

}
