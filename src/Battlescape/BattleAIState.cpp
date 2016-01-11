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
#include "BattleAIState.h"

namespace OpenXcom
{

/**
 * Sets up a BattleAIState.
 * @param save Pointer to the battle game.
 * @param unit Pointer to the unit.
 */
BattleAIState::BattleAIState(SavedBattleGame *save, BattleUnit *unit) : _save(save), _unit(unit)
{

}

/**
 * Deletes the BattleAIState.
 */
BattleAIState::~BattleAIState()
{

}

/**
 * Loads the AI state from a YAML file.
 * @param node YAML node.
 */
void BattleAIState::load(const YAML::Node &)
{
}

/**
 * Saves the AI state to a YAML file.
 * @return YAML node.
 */
YAML::Node BattleAIState::save() const
{
	return YAML::Node();
}


/**
 * Enters the current AI state.
 */
void BattleAIState::enter()
{

}


/**
 * Exits the current AI state.
 */
void BattleAIState::exit()
{

}

/**
 * Runs any code the state needs to keep updating every
 * AI cycle.
 * @param action (possible) AI action to execute after thinking is done.
 */
void BattleAIState::think(BattleAction *)
{

}

}
