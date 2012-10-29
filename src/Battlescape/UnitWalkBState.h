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
#ifndef OPENXCOM_UNITWALKBSTATE_H
#define OPENXCOM_UNITWALKBSTATE_H

#include "BattleState.h"
#include "Position.h"

namespace OpenXcom
{

class BattleUnit;
class Pathfinding;
class TileEngine;

class UnitWalkBState : public BattleState
{
private:
	Position _target;
	BattleUnit *_unit;
	Pathfinding *_pf;
	TileEngine *_terrain;
	void postPathProcedures();
	void setNormalWalkSpeed();
	void playMovementSound();
public:
	/// Creates a new UnitWalkBState class
	UnitWalkBState(BattlescapeGame *parent, BattleAction _action);
	/// Cleans up the UnitWalkBState.
	~UnitWalkBState();
	/// Set the target to walk to.
	void setTarget(Position target);
	/// Initializes the state.
	void init();
	/// Handles a cancels request.
	void cancel();
	/// Runs state functionality every cycle. Returns false when finished.
	void think();
};

}

#endif
