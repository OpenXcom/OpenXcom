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
#include "Ruleset.h"
#include "SoldierNamePool.h"
#include "RuleCountry.h"
#include "RuleRegion.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "RuleTerrain.h"
#include "MapDataSet.h"
#include "RuleSoldier.h"
#include "RuleAlien.h"

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _names(), _countries(), _regions(), _facilities(), _crafts(), _craftWeapons(), _items(), _ufos(), _terrains(), _mapDataFiles(), _soldiers(), _aliens(), _costSoldier(0), _costEngineer(0), _costScientist(0)
{
}

/**
 * Deletes all the contained rules from memory.
 */
Ruleset::~Ruleset()
{
	for (std::vector<SoldierNamePool*>::iterator i = _names.begin(); i != _names.end(); i++)
	{
		delete *i;
	}
	for (std::map<std::string, RuleCountry*>::iterator i = _countries.begin(); i != _countries.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleRegion*>::iterator i = _regions.begin(); i != _regions.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraft*>::iterator i = _crafts.begin(); i != _crafts.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraftWeapon*>::iterator i = _craftWeapons.begin(); i != _craftWeapons.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleItem*>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleUfo*>::iterator i = _ufos.begin(); i != _ufos.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleTerrain*>::iterator i = _terrains.begin(); i != _terrains.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, MapDataSet*>::iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleAlien*>::iterator i = _aliens.begin(); i != _aliens.end(); i++)
	{
		delete i->second;
	}
}

/**
 * Generates a brand new blank saved game.
 * @param diff Difficulty for the save.
 * @return New saved game.
 */
SavedGame *Ruleset::newSave(GameDifficulty diff)
{
	SavedGame *save = new SavedGame(diff);

	return save;
}

/**
 * Returns the list of soldier name pools.
 * @return Pointer to soldier name pool list.
 */
std::vector<SoldierNamePool*> *const Ruleset::getPools()
{
	return &_names;
}

/**
 * Returns the rules for the specified country.
 * @param id Country type.
 * @return Rules for the country.
 */
RuleCountry *const Ruleset::getCountry(std::string id)
{
	return _countries[id];
}

/**
 * Returns the rules for the specified region.
 * @param id Region type.
 * @return Rules for the region.
 */
RuleRegion *const Ruleset::getRegion(std::string id)
{
	return _regions[id];
}

/**
 * Returns the rules for the specified base facility.
 * @param id Facility type.
 * @return Rules for the facility.
 */
RuleBaseFacility *const Ruleset::getBaseFacility(std::string id)
{
	return _facilities[id];
}

/**
 * Returns the rules for the specified craft.
 * @param id Craft type.
 * @return Rules for the craft.
 */
RuleCraft *const Ruleset::getCraft(std::string id)
{
	return _crafts[id];
}

/**
 * Returns the rules for the specified craft weapon.
 * @param id Craft weapon type.
 * @return Rules for the craft weapon.
 */
RuleCraftWeapon *const Ruleset::getCraftWeapon(std::string id)
{
	return _craftWeapons[id];
}
/**
 * Returns the rules for the specified item.
 * @param id Item type.
 * @return Rules for the item.
 */
RuleItem *const Ruleset::getItem(std::string id)
{
	return _items[id];
}

/**
 * Returns the rules for the specified UFO.
 * @param id UFO type.
 * @return Rules for the UFO.
 */
RuleUfo *const Ruleset::getUfo(std::string id)
{
	return _ufos[id];
}

/**
 * Returns the rules for the specified terrain.
 * @param name terrain name.
 * @return Rules for the terrain.
 */
RuleTerrain *const Ruleset::getTerrain(std::string name)
{
	return _terrains[name];
}

/**
 * Returns the info about a specific map data file
 * @param name datafile name.
 * @return Rules for the datafile.
 */
MapDataSet *const Ruleset::getMapDataSet(std::string name)
{
	return _mapDataFiles[name];
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleSoldier *const Ruleset::getSoldier(std::string name)
{
	return _soldiers[name];
}

/**
 * Returns the info about a specific unit
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleAlien *const Ruleset::getAlien(std::string name)
{
	return _aliens[name];
}

/**
 * Returns the info about a specific armor
 * @param name Armor name.
 * @return Rules for the armor.
 */
RuleArmor *const Ruleset::getArmor(std::string name)
{
	return _armors[name];
}

/**
 * Returns the cost of an individual soldier
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getSoldierCost() const
{
	return _costSoldier;
}

/**
 * Returns the cost of an individual engineer
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getEngineerCost() const
{
	return _costEngineer;
}

/**
 * Returns the cost of an individual scientist
 * for purchase/maintenance.
 * @return Cost.
 */
int Ruleset::getScientistCost() const
{
	return _costScientist;
}

/**
 * Returns the time it takes to transfer personnel
 * between bases.
 * @return Time in hours.
 */
int Ruleset::getPersonnelTime() const
{
	return _timePersonnel;
}

}
