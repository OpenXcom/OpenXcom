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
#ifndef OPENXCOM_CIVILIANBAISTATE_H
#define OPENXCOM_CIVILIANBAISTATE_H

#include "BattleAIState.h"
#include "Position.h"
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class BattleUnit;
class SavedBattleGame;
class Node;
struct BattleAction;

/**
 * This is the intial AI state of units, walking around and looking for intruders.
 */
class CivilianBAIState : public BattleAIState
{
private:
	BattleAction *_escapeAction, *_patrolAction;
	BattleUnit *_aggroTarget;
	int _escapeTUs, _AIMode, _visibleEnemies, _spottingEnemies;
	bool _traceAI;
protected:
	Node *_fromNode, *_toNode;
public:
	/// Creates a new BattleAIState linked to the game and a certain unit.
	CivilianBAIState(SavedBattleGame *save, BattleUnit *unit, Node *node);
	/// Cleans up the BattleAIState.
	~CivilianBAIState();
	/// Loads the AI state from YAML.
	void load(const YAML::Node& node);
	/// Saves the AI state to YAML.
	YAML::Node save() const;
	/// Enters the state.
	void enter();
	/// Exits the state.
	void exit();
	/// Runs state functionality every AI cycle.
	void think(BattleAction *action);
	int getSpottingUnits(Position pos) const;
	int selectNearestTarget();
	void setupEscape();
	void setupPatrol();
	void evaluateAIMode();
};

}

#endif
