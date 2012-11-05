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
#ifndef OPENXCOM_UNITTURNBSTATE_H
#define OPENXCOM_UNITTURNBSTATE_H

#include "BattleState.h"
#include "Position.h"

namespace OpenXcom
{

class BattleUnit;

class UnitTurnBState : public BattleState
{
private:
	BattleUnit *_unit;
	bool _turret;
public:
	/// Creates a new UnitTurnBState class
	UnitTurnBState(BattlescapeGame *parent, BattleAction action);
	/// Cleans up the UnitTurnBState.
	~UnitTurnBState();
	/// Initializes the state.
	void init();
	/// Handles a cancels request.
	void cancel();
	/// Runs state functionality every cycle.
	void think();
	/// Get the result of the state.
	std::string getResult() const;
};

}

#endif
