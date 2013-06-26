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
#ifndef OPENXCOM_ENDBATTLEBSTATE_H
#define OPENXCOM_ENDBATTLEBSTATE_H

#include "BattleState.h"

namespace OpenXcom
{

class BattleUnit;

class EndBattleBState : public BattleState
{
private:
	int _liveSoldiers;
	BattlescapeState *_battle;
public:
	/// Creates a new UnitPanicBState class
	EndBattleBState(BattlescapeGame *parent, int liveSoldiers, BattlescapeState *battle);
	/// Cleans up the UnitPanicBState.
	~EndBattleBState();
	/// Initializes the state.
	void init();
	/// Handles a cancels request.
	void cancel();
	/// Runs state functionality every cycle.
	void think();
};

}

#endif
