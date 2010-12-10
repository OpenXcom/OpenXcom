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
#include "../Ruleset/Ruleset.h"
#include "../Resource/SoldierNamePool.h"
#include "RuleBaseFacility.h"
#include "RuleCraft.h"
#include "RuleCraftWeapon.h"
#include "RuleItem.h"
#include "RuleUfo.h"
#include "RuleTerrain.h"
#include "MapDataFile.h"
#include "RuleUnitSprite.h"

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _names(), _facilities(), _crafts(), _craftWeapons(), _items(), _ufos(), _terrains(), _mapDataFiles()
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
	for (std::map<std::string, MapDataFile*>::iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleUnitSprite*>::iterator i = _unitSprites.begin(); i != _unitSprites.end(); i++)
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
 * Generates a battlescape saved game based on terrain type, mission type, time of day, place on earth.
 * @param res the ResourcePack.
 * @param save The base SavedGame.
 * @param texture The texture of the globe polygon we are on.
 * @param craft The XCom craft.
 * @param ufo The UFO.
 * @return New saved game.
 */
SavedBattleGame *Ruleset::newBattleSave(ResourcePack *res, SavedGame *save, int texture, Craft *craft, Ufo* ufo)
{
	SavedBattleGame *bsave = 0;

	return bsave;
}

/**
 * Handles the end battle stuff
 */
void Ruleset::endBattle(SavedGame *save)
{
	delete save->getBattleGame();
	save->setBattleGame(0);
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
	return _items.at(id);
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
RuleTerrain *Ruleset::getTerrain(std::string name)
{
	return _terrains[name];
}

/**
 * Returns the info about a specific map data file
 * @param name datafile name.
 * @return Rules for the datafile.
 */
MapDataFile *Ruleset::getMapDataFile(std::string name)
{
	return _mapDataFiles[name];
}

/**
 * Returns the info about a specific unit to draw it
 * @param name datafile name.
 * @return Rules for the datafile.
 */
RuleUnitSprite *Ruleset::getUnitSprites(std::string name)
{
	return _unitSprites[name];
}

}
