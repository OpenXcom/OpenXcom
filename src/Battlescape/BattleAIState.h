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
#ifndef OPENXCOM_BATTLEAISTATE_H
#define OPENXCOM_BATTLEAISTATE_H

#include <yaml-cpp/yaml.h>
#include "BattlescapeGame.h"

namespace OpenXcom
{

class SavedBattleGame;
class BattleUnit;

/**
 * This class is used by the BattleUnit AI.
 */
class BattleAIState
{
protected:
	SavedBattleGame *_game;
	BattleUnit *_unit;
public:
	/// Creates a new BattleAIState linked to the game and a certain unit.
	BattleAIState(SavedBattleGame *game, BattleUnit *unit);
	/// Cleans up the BattleAIState.
	virtual ~BattleAIState();
	/// Loads the AI state from YAML.
	virtual void load(const YAML::Node& node);
	/// Saves the AI state to YAML.
	virtual YAML::Node save() const;
	/// Enters the state.
	virtual void enter();
	/// Exits the state.
	virtual void exit();
	/// Runs state functionality every AI cycle.
	virtual void think(BattleAction *action);
};

}

#endif
