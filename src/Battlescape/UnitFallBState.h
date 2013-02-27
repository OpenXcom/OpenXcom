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
#ifndef OPENXCOM_UNITFALLBSTATE_H
#define OPENXCOM_UNITFALLBSTATE_H

#include "BattleState.h"
#include "Position.h"

namespace OpenXcom
{

class TileEngine;

class UnitFallBState : public BattleState
{
private:
	TileEngine *_terrain;
public:
	/// Creates a new UnitWalkBState class
	UnitFallBState(BattlescapeGame *parent);
	/// Cleans up the UnitWalkBState.
	~UnitFallBState();
	/// Initializes the state.
	void init();
	/// Runs state functionality every cycle. Returns false when finished.
	void think();
};

}

#endif
