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
#include "BattleState.h"

namespace OpenXcom
{

/**
 * Sets up a BattleState.
 * @param parent Pointer to the parent state.
 * @param action Struct containing info about the action.
 */
BattleState::BattleState(BattlescapeGame *parent, BattleAction action) : _parent(parent), _action(action)
{

}

/**
 * Sets up a BattleState.
 * @param parent Pointer to the parent state.
 */
BattleState::BattleState(BattlescapeGame *parent) : _parent(parent)
{
	_action.result = "";
	_action.targeting = false;
	_action.TU = 0;
}


/**
 * Deletes the BattleState.
 */
BattleState::~BattleState()
{

}

/**
 * Start the current BattleState.
 */
void BattleState::init()
{

}

/**
 * Cancels the current BattleState.
 */
void BattleState::cancel()
{

}

/**
 * Runs any code the state needs to keep updating every
 * game cycle.
 */
void BattleState::think()
{

}

/**
 * Gets the action result. Returns error messages or an empty string when everything went fine.
 * @return Error or empty string when everything is fine.
 */
BattleAction BattleState::getAction() const
{
	return _action;
}

}
