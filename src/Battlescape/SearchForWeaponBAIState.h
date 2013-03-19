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
#ifndef OPENXCOM_SEARCHFORWEAPONBAISTATE_H
#define OPENXCOM_SEARCHFORWEAPONBAISTATE_H

#include "BattleAIState.h"

namespace OpenXcom
{

class BattlescapeGame;
class BattleUnit;
class BattlescapeState;
class Node;

/**
 * This is the special AI state of units, walking around and looking for a weapon to pick up.
 */
class SearchForWeaponBAIState : public BattleAIState
{
public:
	/// Creates a new BattleAIState linked to the game and a certain unit.
	SearchForWeaponBAIState(SavedBattleGame *game, BattleUnit *unit);
	/// Cleans up the BattleAIState.
	~SearchForWeaponBAIState();
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
	/// check through all the items on the ground and pick one
	BattleItem *surveyItems(BattleAction *action);
	/// evaluate if it's worth while to take this item
	bool worthTaking(BattleItem* item, BattleAction *action);
	/// pick the item up from the ground
	int takeItemFromGround(BattleItem* item, BattleAction *action);
	/// assign the item to a slot (stolen from battlescapeGenerator::addItem())
	bool takeItem(BattleItem* item, BattleAction *action);

};

}

#endif
