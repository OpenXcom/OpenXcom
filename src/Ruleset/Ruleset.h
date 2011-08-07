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
class ArticleDefinition;
class RuleInventory;

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
	std::map<std::string, ArticleDefinition*> _ufopaediaArticles;
	std::map<std::string, RuleInventory*> _invs;
	int _costSoldier, _costEngineer, _costScientist, _timePersonnel;
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	virtual ~Ruleset();
	/// Generates the starting saved game.
	virtual SavedGame *newSave(GameDifficulty diff) const;
	/// Gets the pool list for soldier names.
	std::vector<SoldierNamePool*> *const getPools();
	/// Gets the ruleset for a country type.
	RuleCountry *const getCountry(const std::string &id) const;
	/// Gets the ruleset for a region type.
	RuleRegion *const getRegion(const std::string &id) const;
	/// Gets the ruleset for a facility type.
	RuleBaseFacility *const getBaseFacility(const std::string &id) const;
	/// Gets the ruleset for a craft type.
	RuleCraft *const getCraft(const std::string &id) const;
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *const getCraftWeapon(const std::string &id) const;
	/// Gets the ruleset for an item type.
	RuleItem *const getItem(const std::string &id) const;
	/// Gets the ruleset for a UFO type.
	RuleUfo *const getUfo(const std::string &id) const;
	/// Gets terrains for battlescape games.
	RuleTerrain *const getTerrain(const std::string &name) const;
	/// Gets mapdatafile for battlescape games.
	MapDataSet *const getMapDataSet(const std::string &name) const;
	/// Gets soldier unit rules.
	RuleSoldier *const getSoldier(const std::string &name) const;
	/// Gets alien unit rules.
	RuleAlien *const getAlien(const std::string &name) const;
	/// Gets armor rules.
	RuleArmor *const getArmor(const std::string &name) const;
	/// Gets Ufopaedia article definition.
	ArticleDefinition *const getUfopaediaArticle(const std::string &name) const;
	/// Gets the inventory list.
	std::map<std::string, RuleInventory*> *const getInventories();
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
