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
class Unit;
class Armor;
class ArticleDefinition;
class RuleInventory;
class RuleResearchProject;
class RuleManufactureInfo;
class AlienRace;
class AlienDeployment;

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
	std::map<std::string, MapDataSet*> _mapDataSets;
	std::map<std::string, RuleSoldier*> _soldiers;
	std::map<std::string, Unit*> _units;
	std::map<std::string, AlienRace*> _alienRaces;
	std::map<std::string, AlienDeployment*> _alienDeployments;
	std::map<std::string, Armor*> _armors;
	std::map<std::string, ArticleDefinition*> _ufopaediaArticles;
	std::map<std::string, RuleInventory*> _invs;
	int _costSoldier, _costEngineer, _costScientist, _timePersonnel;
	std::map<std::string, RuleResearchProject *> _researchProjects;
	std::map<std::string, RuleManufactureInfo *> _manufacture;
	std::vector<std::string> _countriesIndex, _regionsIndex, _facilitiesIndex, _craftsIndex, _craftWeaponsIndex, _itemsIndex, _armorsIndex, _researchIndex, _manufactureIndex;
public:
	/// Creates a blank ruleset.
	Ruleset();
	/// Cleans up the ruleset.
	virtual ~Ruleset();
	/// Loads a ruleset from YAML.
	void load(const std::string &filename);
	/// Saves a ruleset to YAML.
	void save(const std::string &filename) const;
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
	/// Gets the available facilities.
	std::vector<std::string> getBaseFacilitiesList() const;
	/// Gets the ruleset for a craft type.
	RuleCraft *const getCraft(const std::string &id) const;
	/// Gets the available crafts.
	std::vector<std::string> getCraftsList() const;
	/// Gets the ruleset for a craft weapon type.
	RuleCraftWeapon *const getCraftWeapon(const std::string &id) const;
	/// Gets the available craft weapons.
	std::vector<std::string> getCraftWeaponsList() const;
	/// Gets the ruleset for an item type.
	RuleItem *const getItem(const std::string &id) const;
	/// Gets the available items.
	std::vector<std::string> getItemsList() const;
	/// Gets the ruleset for a UFO type.
	RuleUfo *const getUfo(const std::string &id) const;
	/// Gets terrains for battlescape games.
	RuleTerrain *const getTerrain(const std::string &name) const;
	/// Gets mapdatafile for battlescape games.
	MapDataSet *const getMapDataSet(const std::string &name);
	/// Gets soldier unit rules.
	RuleSoldier *const getSoldier(const std::string &name) const;
	/// Gets generated unit rules.
	Unit *const getUnit(const std::string &name) const;
	/// Gets alien race rules.
	AlienRace *const getAlienRace(const std::string &name) const;
	/// Gets deployment rules.
	AlienDeployment *const getDeployment(const std::string &name) const;
	/// Gets armor rules.
	Armor *const getArmor(const std::string &name) const;
	/// Gets Ufopaedia article definition.
	ArticleDefinition *const getUfopaediaArticle(const std::string &name) const;
	/// Gets the inventory list.
	std::map<std::string, RuleInventory*> *const getInventories();
	/// Gets the ruleset for a specific inventory.
	RuleInventory *const getInventory(const std::string &id) const;
	/// Gets the cost of a soldier.
	int getSoldierCost() const;
	/// Gets the cost of an engineer.
	int getEngineerCost() const;
	/// Gets the cost of a scientist.
	int getScientistCost() const;
	/// Gets the transfer time of personnel.
	int getPersonnelTime() const;
	/// Gets the ruleset for a specific research project.
	RuleResearchProject *getResearchProject (const std::string &id) const;
	/// Get the list of all research projects.
	const std::map<std::string, RuleResearchProject *> & getResearchProjects () const;
	/// Gets the ruleset for a specific manufacture project.
	RuleManufactureInfo *getManufactureProject (const std::string &id) const;
	/// Get the list of all manufacture projects.
	const std::map<std::string, RuleManufactureInfo *> & getManufactureProjects () const;
};

}

#endif