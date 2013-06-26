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

#include "EndBattleBState.h"
#include "BattlescapeState.h"

namespace OpenXcom
{

/**
 * Sets up an EndBattleBState.
 * this just queues up an "end battle" action
 */
EndBattleBState::EndBattleBState(BattlescapeGame *parent, int liveSoldiers, BattlescapeState *battle) : BattleState(parent), _liveSoldiers(liveSoldiers), _battle(battle)
{
	// nothing to see here
}

/**
 * Deletes the EndBattleBState.
 */
EndBattleBState::~EndBattleBState()
{
}

void EndBattleBState::init()
{
}

/*
 * Think!
 */
void EndBattleBState::think()
{
	_battle->finishBattle(false, _liveSoldiers);
}

void EndBattleBState::cancel()
{
}

}
