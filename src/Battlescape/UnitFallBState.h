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
#include "BattleState.h"

namespace OpenXcom
{

class BattlescapeGame;
class TileEngine;
class Tile;
class BattleUnit;

/**
 * State for falling units.
 */
class UnitFallBState : public BattleState
{
private:
	TileEngine *_terrain;
	std::vector<Tile*> tilesToFallInto;
	std::vector<BattleUnit*> unitsToMove;
public:
	/// Creates a new UnitWalkBState class
	UnitFallBState(BattlescapeGame *parent);
	/// Cleans up the UnitWalkBState.
	~UnitFallBState();
	/// Initializes the state.
	void init();
	/// Runs state functionality every cycle. Returns when finished.
	void think();
};

}
