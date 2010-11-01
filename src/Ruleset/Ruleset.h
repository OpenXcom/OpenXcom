/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_RULESET_H
#define OPENXCOM_RULESET_H

#include <map>
#include <vector>
#include "../Resource/LangString.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"

class SoldierNamePool;
class RuleBaseFacility;
class RuleCraft;
class RuleCraftWeapon;
class RuleItem;
class RuleUfo;
class Terrain;

/**
 * Set of rules and stats for a game.
 * A ruleset holds all the constant info that never changes
 * throughout a game, like stats of all the in-game items,
 * countries, research tree, soldier names, starting base, etc.
 */
class Ruleset
{
protected:
	std::vector<SoldierNamePool*> _names;
	std::map<LangString, RuleBaseFacility*> _facilities;
	std::map<LangString, RuleCraft*> _crafts;
	std::map<LangString, RuleCraftWeapon*> _craftWeapons;
	std::map<LangString, RuleItem*> _items;
	std::map<LangString, RuleUfo*> _ufos;
	std::map<std::string, Terrain*> _terrains;
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	virtual ~Ruleset();
	/// Generates the starting saved game.
	virtual SavedGame *newSave(GameDifficulty diff);
	/// Generates the starting saved game.
	virtual SavedBattleGame *newBattleSave(SavedGame *save, int texture, Craft *craft, Ufo* ufo);
	virtual void endBattle(SavedBattleGame *bsave, SavedGame *save);
	/// Gets the pool list for soldier names.
	std::vector<SoldierNamePool*> *const getPools();
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *const getBaseFacility(LangString id);
	/// Gets the ruleset for a craft type.
	RuleCraft *const getCraft(LangString id);
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *const getCraftWeapon(LangString id);
	/// Gets the ruleset for an item type.
	RuleItem *const getItem(LangString id);
	/// Gets the ruleset for a UFO type.
	RuleUfo *const getUfo(LangString id);
	/// Gets terrains for battlescape games
	Terrain *getTerrain(std::string name);
};

#endif
