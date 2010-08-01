/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__RULESET_H
#define OPENXCOM__RULESET_H

#include <map>
#include <vector>
#include "LangString.h"
#include "SavedGame.h"

using namespace std;

class SoldierNamePool;
class RuleBaseFacility;
class RuleCraft;
class RuleCraftWeapon;
class RuleItem;

/**
 * Set of rules and stats for a game.
 * A ruleset holds all the constant info that never changes
 * throughout a game, like stats of all the in-game items,
 * countries, research tree, soldier names, starting base, etc.
 */
class Ruleset
{
protected:
	vector<SoldierNamePool*> _names;
	map<LangString, RuleBaseFacility*> _facilities;
	map<LangString, RuleCraft*> _crafts;
	map<LangString, RuleCraftWeapon*> _craftWeapons;
	map<LangString, RuleItem*> _items;
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	virtual ~Ruleset();
	/// Generates the starting saved game.
	virtual SavedGame *newSave(GameDifficulty diff);
	/// Gets the pool list for soldier names.
	vector<SoldierNamePool*> *getPools();
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *getBaseFacility(LangString id);
	/// Gets the ruleset for a craft type.
	RuleCraft *getCraft(LangString id);
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *getCraftWeapon(LangString id);
	/// Gets the ruleset for a item type.
	RuleItem *getItem(LangString id);
};

#endif
