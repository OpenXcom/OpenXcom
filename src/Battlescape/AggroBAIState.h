/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_AGGROBAISTATE_H
#define OPENXCOM_AGGROBAISTATE_H

#include "BattleAIState.h"
#include <vector>


namespace OpenXcom
{

class SavedBattleGame;
class BattleUnit;
class BattlescapeState;

/**
 * This class is used by the BattleUnit AI.
 */
class AggroBAIState : public BattleAIState
{
protected:
	BattleUnit *_aggroTarget;
	BattleUnit *_lastKnownTarget;
	Position _lastKnownPosition;
	int _timesNotSeen;
	static std::vector<Position> _randomTileSearch;
	static int _randomTileSearchAge;
	bool _charge;
	
public:
	/// Creates a new AggroBAIState linked to the game and a certain unit.
	AggroBAIState(SavedBattleGame *game, BattleUnit *unit);
	/// Cleans up the AggroBAIState.
	~AggroBAIState();
	/// Loads the AI state from YAML.
	void load(const YAML::Node& node);
	/// Saves the AI state to YAML.
	void save(YAML::Emitter& out) const;
	/// Enters the state.
	void enter();
	/// Exits the state.
	void exit();
	/// Runs state functionality every AI cycle.
	void think(BattleAction *action);
	/// Sets aggro target, triggered by reaction fire.
	void setAggroTarget(BattleUnit *unit);
	/// Get the aggro target, for savegame
	BattleUnit *getAggroTarget() const { return _aggroTarget; }
	/// Get the last known location of target, for turning
	Position getLastKnownPosition() const { return _lastKnownPosition; }
	/// decide if we should throw a grenade/launch a missile to this position
	bool explosiveEfficacy(Position targetPos, BattleUnit *attackingUnit, int radius, int diff);
};

}

#endif
