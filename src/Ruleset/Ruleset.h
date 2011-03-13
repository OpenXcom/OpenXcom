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
#include <string>
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

class SoldierNamePool;
class RuleCountry;
class RuleRegion;
class RuleBaseFacility;
class RuleCraft;
class RuleCraftWeapon;
class RuleItem;
class RuleUfo;
class RuleTerrain;
class MapDataSet;
class ResourcePack;
class RuleSoldier;
class RuleAlien;
class RuleArmor;

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
	std::map<std::string, RuleCountry*> _countries;
	std::map<std::string, RuleRegion*> _regions;
	std::map<std::string, RuleBaseFacility*> _facilities;
	std::map<std::string, RuleCraft*> _crafts;
	std::map<std::string, RuleCraftWeapon*> _craftWeapons;
	std::map<std::string, RuleItem*> _items;
	std::map<std::string, RuleUfo*> _ufos;
	std::map<std::string, RuleTerrain*> _terrains;
	std::map<std::string, MapDataSet*> _mapDataFiles;
	std::map<std::string, RuleSoldier*> _soldiers;
	std::map<std::string, RuleAlien*> _aliens;
	std::map<std::string, RuleArmor*> _armors;
	int _costSoldier, _costEngineer, _costScientist, _timePersonnel;
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	virtual ~Ruleset();
	/// Generates the starting saved game.
	virtual SavedGame *newSave(GameDifficulty diff);
	/// Gets the pool list for soldier names.
	std::vector<SoldierNamePool*> *const getPools();
	/// Gets the ruleset for a country type.
	RuleCountry *const getCountry(std::string id);
	/// Gets the ruleset for a region type.
	RuleRegion *const getRegion(std::string id);
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *const getBaseFacility(std::string id);
	/// Gets the ruleset for a craft type.
	RuleCraft *const getCraft(std::string id);
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *const getCraftWeapon(std::string id);
	/// Gets the ruleset for an item type.
	RuleItem *const getItem(std::string id);
	/// Gets the ruleset for a UFO type.
	RuleUfo *const getUfo(std::string id);
	/// Gets terrains for battlescape games.
	RuleTerrain *const getTerrain(std::string name);
	/// Gets mapdatafile for battlescape games.
	MapDataSet *const getMapDataSet(std::string name);
	/// Gets unit rules.
	RuleSoldier *const getSoldier(std::string name);
	/// Gets unit rules.
	RuleAlien *const getAlien(std::string name);
	/// Gets armor rules.
	RuleArmor *const getArmor(std::string name);
	/// Gets the cost of a soldier.
	int getSoldierCost() const;
	/// Gets the cost of an engineer.
	int getEngineerCost() const;
	/// Gets the cost of a scientist.
	int getScientistCost() const;
	/// Gets the transfer time of personnel.
	int getPersonnelTime() const;
};

}

#endif
