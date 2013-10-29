/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include <fstream>
#include "../aresame.h"
#include "../Engine/Options.h"
#include "../Engine/Exception.h"
#include "../Engine/CrossPlatform.h"
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
#include "Unit.h"
#include "AlienRace.h"
#include "AlienDeployment.h"
#include "Armor.h"
#include "ArticleDefinition.h"
#include "RuleInventory.h"
#include "RuleResearch.h"
#include "RuleManufacture.h"
#include "ExtraSprites.h"
#include "ExtraSounds.h"
#include "ExtraStrings.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Region.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/AlienStrategy.h"
#include "../Savegame/GameTime.h"
#include "UfoTrajectory.h"
#include "RuleAlienMission.h"
#include "City.h"
#include "MCDPatch.h"
#include "../Engine/Logger.h"
#include <algorithm>

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _costSoldier(0), _costEngineer(0), _costScientist(0), _timePersonnel(0), _initialFunding(0), _startingTime(6, 1, 1, 1999, 12, 0, 0), _modIndex(0), _facilityListOrder(0), _craftListOrder(0), _itemListOrder(0), _researchListOrder(0),  _manufactureListOrder(0), _ufopaediaListOrder(0)
{
    // Check in which data dir the folder is stored
    std::string path = CrossPlatform::getDataFolder("SoldierName/");
	// Add soldier names
	std::vector<std::string> names = CrossPlatform::getFolderContents(path, "nam");

	for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		std::string file = CrossPlatform::noExt(*i);
		SoldierNamePool *pool = new SoldierNamePool();
		pool->load(file);
		_names.push_back(pool);
	}
}

/**
 * Deletes all the contained rules from memory.
 */
Ruleset::~Ruleset()
{
	for (std::vector<SoldierNamePool*>::iterator i = _names.begin(); i != _names.end(); ++i)
	{
		delete *i;
	}
	for (std::map<std::string, RuleCountry*>::iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleRegion*>::iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleBaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraft*>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleCraftWeapon*>::iterator i = _craftWeapons.begin(); i != _craftWeapons.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleUfo*>::iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleTerrain*>::iterator i = _terrains.begin(); i != _terrains.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, MapDataSet*>::iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Unit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienRace*>::iterator i = _alienRaces.begin(); i != _alienRaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, AlienDeployment*>::iterator i = _alienDeployments.begin(); i != _alienDeployments.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Armor*>::iterator i = _armors.begin(); i != _armors.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, ArticleDefinition*>::iterator i = _ufopaediaArticles.begin(); i != _ufopaediaArticles.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleInventory*>::iterator i = _invs.begin(); i != _invs.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleResearch *>::const_iterator i = _research.begin (); i != _research.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleManufacture *>::const_iterator i = _manufacture.begin (); i != _manufacture.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, UfoTrajectory *>::const_iterator i = _ufoTrajectories.begin (); i != _ufoTrajectories.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.begin (); i != _alienMissions.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, MCDPatch *>::const_iterator i = _MCDPatches.begin (); i != _MCDPatches.end (); ++i)
	{
		delete i->second;
	}
	for (std::vector< std::pair<std::string, ExtraSprites *> >::const_iterator i = _extraSprites.begin (); i != _extraSprites.end (); ++i)
	{
		delete i->second;
	}
	for (std::vector< std::pair<std::string, ExtraSounds *> >::const_iterator i = _extraSounds.begin (); i != _extraSounds.end (); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, ExtraStrings *>::const_iterator i = _extraStrings.begin (); i != _extraStrings.end (); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads a ruleset's contents from the given source.
 * @param source The source to use.
 */
void Ruleset::load(const std::string &source)
{
	std::string dirname = CrossPlatform::getDataFolder("Ruleset/" + source + '/');
	if (!CrossPlatform::folderExists(dirname))
		loadFile(CrossPlatform::getDataFile("Ruleset/" + source + ".rul"));
	else
		loadFiles(dirname);
}

/**
 * Loads a ruleset's contents from a YAML file.
 * Rules that match pre-existing rules overwrite them.
 * @param filename YAML filename.
 */
void Ruleset::loadFile(const std::string &filename)
{
	YAML::Node doc = YAML::LoadFile(filename);

	for (YAML::const_iterator i = doc["countries"].begin(); i != doc["countries"].end(); ++i)
	{
		RuleCountry *rule = loadRule(*i, &_countries, &_countriesIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["regions"].begin(); i != doc["regions"].end(); ++i)
	{
		RuleRegion *rule = loadRule(*i, &_regions, &_regionsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["facilities"].begin(); i != doc["facilities"].end(); ++i)
	{
		RuleBaseFacility *rule = loadRule(*i, &_facilities, &_facilitiesIndex);
		if (rule != 0)
		{
			_facilityListOrder += 100;
			rule->load(*i, _modIndex, _facilityListOrder);
		}
	}
 	for (YAML::const_iterator i = doc["crafts"].begin(); i != doc["crafts"].end(); ++i)
	{
		RuleCraft *rule = loadRule(*i, &_crafts, &_craftsIndex);
		if (rule != 0)
		{
			_craftListOrder += 100;
			rule->load(*i, this, _modIndex, _craftListOrder);
		}
	}
 	for (YAML::const_iterator i = doc["craftWeapons"].begin(); i != doc["craftWeapons"].end(); ++i)
	{
		RuleCraftWeapon *rule = loadRule(*i, &_craftWeapons, &_craftWeaponsIndex);
		if (rule != 0)
		{
			rule->load(*i, _modIndex);
		}
	}
 	for (YAML::const_iterator i = doc["items"].begin(); i != doc["items"].end(); ++i)
	{
		RuleItem *rule = loadRule(*i, &_items, &_itemsIndex);
		if (rule != 0)
		{
			_itemListOrder += 100;
			rule->load(*i, _modIndex, _itemListOrder);
		}
	}
 	for (YAML::const_iterator i = doc["ufos"].begin(); i != doc["ufos"].end(); ++i)
	{
		RuleUfo *rule = loadRule(*i, &_ufos, &_ufosIndex);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
 	for (YAML::const_iterator i = doc["invs"].begin(); i != doc["invs"].end(); ++i)
	{
		RuleInventory *rule = loadRule(*i, &_invs, 0, "id");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["terrains"].begin(); i != doc["terrains"].end(); ++i)
	{
		RuleTerrain *rule = loadRule(*i, &_terrains, &_terrainIndex, "name");
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
 	for (YAML::const_iterator i = doc["armors"].begin(); i != doc["armors"].end(); ++i)
	{
		Armor *rule = loadRule(*i, &_armors, &_armorsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["soldiers"].begin(); i != doc["soldiers"].end(); ++i)
	{
		RuleSoldier *rule = loadRule(*i, &_soldiers);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["units"].begin(); i != doc["units"].end(); ++i)
	{
		Unit *rule = loadRule(*i, &_units);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["alienRaces"].begin(); i != doc["alienRaces"].end(); ++i)
	{
		AlienRace *rule = loadRule(*i, &_alienRaces, &_aliensIndex, "id");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["alienDeployments"].begin(); i != doc["alienDeployments"].end(); ++i)
	{
		AlienDeployment *rule = loadRule(*i, &_alienDeployments, &_deploymentsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["research"].begin(); i != doc["research"].end(); ++i)
	{
		RuleResearch *rule = loadRule(*i, &_research, &_researchIndex, "name");
		if (rule != 0)
		{
			_researchListOrder += 100;
			rule->load(*i, _researchListOrder);
		}
	}
 	for (YAML::const_iterator i = doc["manufacture"].begin(); i != doc["manufacture"].end(); ++i)
	{
		RuleManufacture *rule = loadRule(*i, &_manufacture, &_manufactureIndex, "name");
		if (rule != 0)
		{
			_manufactureListOrder += 100;
			rule->load(*i, _manufactureListOrder);
		}
	}
 	for (YAML::const_iterator i = doc["ufopaedia"].begin(); i != doc["ufopaedia"].end(); ++i)
	{
		std::string id = (*i)["id"].as<std::string>();
		ArticleDefinition *rule;
		if (_ufopaediaArticles.find(id) != _ufopaediaArticles.end())
		{
			rule = _ufopaediaArticles[id];
		}
		else
		{
			UfopaediaTypeId type = (UfopaediaTypeId)(*i)["type_id"].as<int>();
			switch (type)
			{
			case UFOPAEDIA_TYPE_CRAFT: rule = new ArticleDefinitionCraft(); break;
			case UFOPAEDIA_TYPE_CRAFT_WEAPON: rule = new ArticleDefinitionCraftWeapon(); break;
			case UFOPAEDIA_TYPE_VEHICLE: rule = new ArticleDefinitionVehicle(); break;
			case UFOPAEDIA_TYPE_ITEM: rule = new ArticleDefinitionItem(); break;
			case UFOPAEDIA_TYPE_ARMOR: rule = new ArticleDefinitionArmor(); break;
			case UFOPAEDIA_TYPE_BASE_FACILITY: rule = new ArticleDefinitionBaseFacility(); break;
			case UFOPAEDIA_TYPE_TEXTIMAGE: rule = new ArticleDefinitionTextImage(); break;
			case UFOPAEDIA_TYPE_TEXT: rule = new ArticleDefinitionText(); break;
			case UFOPAEDIA_TYPE_UFO: rule = new ArticleDefinitionUfo(); break;
			default: rule = 0; break;
			}
			_ufopaediaArticles[id] = rule;
			_ufopaediaIndex.push_back(id);
		}
		_ufopaediaListOrder += 100;
		rule->load(*i, _ufopaediaListOrder);
	}
 	//_startingBase->load(i->second, 0);
	if (doc["startingBase"])
		_startingBase = YAML::Node(doc["startingBase"]);
 	_startingTime.load(doc["startingTime"]);
 	_costSoldier = doc["costSoldier"].as<int>(_costSoldier);
 	_costEngineer = doc["costEngineer"].as<int>(_costEngineer);
 	_costScientist = doc["costScientist"].as<int>(_costScientist);
 	_timePersonnel = doc["timePersonnel"].as<int>(_timePersonnel);
 	_initialFunding = doc["initialFunding"].as<int>(_initialFunding);
 	for (YAML::const_iterator i = doc["ufoTrajectories"].begin(); i != doc["ufoTrajectories"].end(); ++i)
	{
		UfoTrajectory *rule = loadRule(*i, &_ufoTrajectories, 0, "id");
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	for (YAML::const_iterator i = doc["alienMissions"].begin(); i != doc["alienMissions"].end(); ++i)
	{
		RuleAlienMission *rule = loadRule(*i, &_alienMissions, &_alienMissionsIndex);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
 	_alienItemLevels = doc["alienItemLevels"].as< std::vector< std::vector<int> > >(_alienItemLevels);
 	for (YAML::const_iterator i = doc["MCDPatches"].begin(); i != doc["MCDPatches"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (_MCDPatches.find(type) != _MCDPatches.end())
		{
			_MCDPatches[type]->load(*i);
		}
		else
		{
			std::auto_ptr<MCDPatch> patch(new MCDPatch());
			patch->load(*i);
			_MCDPatches[type] = patch.release();
			_MCDPatchesIndex.push_back(type);
		}
	}
 	for (YAML::const_iterator i = doc["extraSprites"].begin(); i != doc["extraSprites"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::auto_ptr<ExtraSprites> extraSprites(new ExtraSprites());
		extraSprites->load(*i, _modIndex);
		_extraSprites.push_back(std::make_pair(type, extraSprites.release()));
		_extraSpritesIndex.push_back(type);
	}
 	for (YAML::const_iterator i = doc["extraSounds"].begin(); i != doc["extraSounds"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::auto_ptr<ExtraSounds> extraSounds(new ExtraSounds());
		extraSounds->load(*i, _modIndex);
		_extraSounds.push_back(std::make_pair(type, extraSounds.release()));
		_extraSoundsIndex.push_back(type);
	}
 	for (YAML::const_iterator i = doc["extraStrings"].begin(); i != doc["extraStrings"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (_extraStrings.find(type) != _extraStrings.end())
		{
			_extraStrings[type]->load(*i);
		}
		else
		{
			std::auto_ptr<ExtraStrings> extraStrings(new ExtraStrings());
			extraStrings->load(*i);
			_extraStrings[type] = extraStrings.release();
			_extraStringsIndex.push_back(type);
		}
	}

  // refresh _psiRequirements for psiStrengthEval
	for (std::vector<std::string>::const_iterator i = _facilitiesIndex.begin(); i != _facilitiesIndex.end(); ++i)
	{
		RuleBaseFacility *rule = getBaseFacility(*i);
		if (0 < rule->getPsiLaboratories())
		{
			_psiRequirements = rule->getRequirements();
			break;
		}
	}

	_modIndex += 1000;
}

/**
 * Loads the contents of all the rule files in the given directory.
 * @param dirname The name of an existing directory containing rule files.
 */
void Ruleset::loadFiles(const std::string &dirname)
{
	std::vector<std::string> names = CrossPlatform::getFolderContents(dirname, "rul");

	for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
	{
		loadFile(dirname + *i);
	}
}

/**
 *
 */
template <typename T>
T *Ruleset::loadRule(const YAML::Node &node, std::map<std::string, T*> *map, std::vector<std::string> *index, const std::string &key)
{
	T *rule = 0;
	if (node[key])
	{
		std::string type = node[key].as<std::string>();
		typename std::map<std::string, T*>::iterator i = map->find(type);
		if (i != map->end())
		{
			rule = i->second;
		}
		else
		{
			rule = new T(type);
			(*map)[type] = rule;
			if (index != 0)
			{
				index->push_back(type);
			}
		}
	}
	else if (node["delete"])
	{
		std::string type = node["delete"].as<std::string>();
		typename std::map<std::string, T*>::iterator i = map->find(type);
		if (i != map->end())
		{
			map->erase(i);
		}
	}

	return rule;
}

/**
 * Generates a brand new saved game with starting data.
 * @return A new saved game.
 */
SavedGame *Ruleset::newSave() const
{
	SavedGame *save = new SavedGame();

	// Add countries
	for (std::vector<std::string>::const_iterator i = _countriesIndex.begin(); i != _countriesIndex.end(); ++i)
	{
		save->getCountries()->push_back(new Country(getCountry(*i)));
	}
	// Adjust funding to total $6M
	int missing = ((_initialFunding - save->getCountryFunding()/1000) / (int)save->getCountries()->size()) * 1000;
	for (std::vector<Country*>::iterator i = save->getCountries()->begin(); i != save->getCountries()->end(); ++i)
	{
		(*i)->setFunding((*i)->getFunding().back() + missing);
	}
	save->setFunds(save->getCountryFunding());

	// Add regions
	for (std::vector<std::string>::const_iterator i = _regionsIndex.begin(); i != _regionsIndex.end(); ++i)
	{
		save->getRegions()->push_back(new Region(getRegion(*i)));
	}

	// Set up starting base
	Base *base = new Base(this);
	base->load(_startingBase, save, true);

	// Correct IDs
	for (std::vector<Craft*>::const_iterator i = base->getCrafts()->begin(); i != base->getCrafts()->end(); ++i)
	{
		save->getId((*i)->getRules()->getType());
	}

	// Generate soldiers
	int soldiers = _startingBase["randomSoldiers"].as<int>(0);
	for (int i = 0; i < soldiers; ++i)
	{
		Soldier *soldier = new Soldier(getSoldier("XCOM"), getArmor("STR_NONE_UC"), &_names, save->getId("STR_SOLDIER"));
		soldier->setCraft(base->getCrafts()->front());
		base->getSoldiers()->push_back(soldier);
	}

	save->getBases()->push_back(base);
	// Setup alien strategy
	save->getAlienStrategy().init(this);
	save->setTime(_startingTime);

	return save;
}

/**
 * Returns the list of soldier name pools.
 * @return Pointer to soldier name pool list.
 */
const std::vector<SoldierNamePool*> &Ruleset::getPools() const
{
	return _names;
}

/**
 * Returns the rules for the specified country.
 * @param id Country type.
 * @return Rules for the country.
 */
RuleCountry *Ruleset::getCountry(const std::string &id) const
{
	std::map<std::string, RuleCountry*>::const_iterator i = _countries.find(id);
	if (_countries.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all countries
 * provided by the ruleset.
 * @return List of countries.
 */
const std::vector<std::string> &Ruleset::getCountriesList() const
{
	return _countriesIndex;
}

/**
 * Returns the rules for the specified region.
 * @param id Region type.
 * @return Rules for the region.
 */
RuleRegion *Ruleset::getRegion(const std::string &id) const
{
	std::map<std::string, RuleRegion*>::const_iterator i = _regions.find(id);
	if (_regions.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all regions
 * provided by the ruleset.
 * @return List of regions.
 */
const std::vector<std::string> &Ruleset::getRegionsList() const
{
	return _regionsIndex;
}

/**
 * Returns the rules for the specified base facility.
 * @param id Facility type.
 * @return Rules for the facility.
 */
RuleBaseFacility *Ruleset::getBaseFacility(const std::string &id) const
{
	std::map<std::string, RuleBaseFacility*>::const_iterator i = _facilities.find(id);
	if (_facilities.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all base facilities
 * provided by the ruleset.
 * @return List of base faciliies.
 */
const std::vector<std::string> &Ruleset::getBaseFacilitiesList() const
{
	return _facilitiesIndex;
}

/**
 * Returns the rules for the specified craft.
 * @param id Craft type.
 * @return Rules for the craft.
 */
RuleCraft *Ruleset::getCraft(const std::string &id) const
{
	std::map<std::string, RuleCraft*>::const_iterator i = _crafts.find(id);
	if (_crafts.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all crafts
 * provided by the ruleset.
 * @return List of crafts.
 */
const std::vector<std::string> &Ruleset::getCraftsList() const
{
	return _craftsIndex;
}

/**
 * Returns the rules for the specified craft weapon.
 * @param id Craft weapon type.
 * @return Rules for the craft weapon.
 */
RuleCraftWeapon *Ruleset::getCraftWeapon(const std::string &id) const
{
	std::map<std::string, RuleCraftWeapon*>::const_iterator i = _craftWeapons.find(id);
	if (_craftWeapons.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all craft weapons
 * provided by the ruleset.
 * @return List of craft weapons.
 */
const std::vector<std::string> &Ruleset::getCraftWeaponsList() const
{
	return _craftWeaponsIndex;
}

/**
 * Returns the rules for the specified item.
 * @param id Item type.
 * @return Rules for the item, or 0 when the item is not found.
 */
RuleItem *Ruleset::getItem(const std::string &id) const
{
	if (_items.find(id) != _items.end())
		return _items.find(id)->second;
	else
		return 0;
}

/**
 * Returns the list of all items
 * provided by the ruleset.
 * @return List of items.
 */
const std::vector<std::string> &Ruleset::getItemsList() const
{
	return _itemsIndex;
}

/**
 * Returns the rules for the specified UFO.
 * @param id UFO type.
 * @return Rules for the UFO.
 */
RuleUfo *Ruleset::getUfo(const std::string &id) const
{
	std::map<std::string, RuleUfo*>::const_iterator i = _ufos.find(id);
	if (_ufos.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all ufos
 * provided by the ruleset.
 * @return List of ufos.
 */
const std::vector<std::string> &Ruleset::getUfosList() const
{
	return _ufosIndex;
}

/**
 * Returns the list of all terrains
 * provided by the ruleset.
 * @return List of terrains.
 */
const std::vector<std::string> &Ruleset::getTerrainList() const
{
	return _terrainIndex;
}

/**
 * Returns the rules for the specified terrain.
 * @param name Terrain name.
 * @return Rules for the terrain.
 */
RuleTerrain *Ruleset::getTerrain(const std::string &name) const
{
	std::map<std::string, RuleTerrain*>::const_iterator i = _terrains.find(name);
	if (_terrains.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific map data file.
 * @param name Datafile name.
 * @return Rules for the datafile.
 */
MapDataSet *Ruleset::getMapDataSet(const std::string &name)
{
	std::map<std::string, MapDataSet*>::iterator map = _mapDataSets.find(name);
	if (map == _mapDataSets.end())
	{
		MapDataSet *set = new MapDataSet(name);
		_mapDataSets[name] = set;
		return set;
	}
	else
	{
		return map->second;
	}
}

/**
 * Returns the info about a specific unit.
 * @param name Unit name.
 * @return Rules for the units.
 */
RuleSoldier *Ruleset::getSoldier(const std::string &name) const
{
	std::map<std::string, RuleSoldier*>::const_iterator i = _soldiers.find(name);
	if (_soldiers.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific unit.
 * @param name Unit name.
 * @return Rules for the units.
 */
Unit *Ruleset::getUnit(const std::string &name) const
{
	std::map<std::string, Unit*>::const_iterator i = _units.find(name);
	if (_units.end() != i) return i->second; else return 0;
}

/**
 * Returns the info about a specific alien race.
 * @param name Race name.
 * @return Rules for the race.
 */
AlienRace *Ruleset::getAlienRace(const std::string &name) const
{
	std::map<std::string, AlienRace*>::const_iterator i = _alienRaces.find(name);
	if (_alienRaces.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all alien races.
 * provided by the ruleset.
 * @return List of alien races.
 */
const std::vector<std::string> &Ruleset::getAlienRacesList() const
{
	return _aliensIndex;
}

/**
 * Returns the info about a specific deployment.
 * @param name Deployment name.
 * @return Rules for the deployment.
 */
AlienDeployment *Ruleset::getDeployment(const std::string &name) const
{
	std::map<std::string, AlienDeployment*>::const_iterator i = _alienDeployments.find(name);
	if (_alienDeployments.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all alien deployments
 * provided by the ruleset.
 * @return List of alien deployments.
 */
const std::vector<std::string> &Ruleset::getDeploymentsList() const
{
	return _deploymentsIndex;
}

/**
 * Returns the info about a specific armor.
 * @param name Armor name.
 * @return Rules for the armor.
 */
Armor *Ruleset::getArmor(const std::string &name) const
{
	std::map<std::string, Armor*>::const_iterator i = _armors.find(name);
	if (_armors.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all armors
 * provided by the ruleset.
 * @return List of armors.
 */
const std::vector<std::string> &Ruleset::getArmorsList() const
{
	return _armorsIndex;
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

/**
 * Returns the article definition for a given name.
 * @param name Article name.
 * @return Article definition.
 */
ArticleDefinition *Ruleset::getUfopaediaArticle(const std::string &name) const
{
	std::map<std::string, ArticleDefinition*>::const_iterator i = _ufopaediaArticles.find(name);
	if (_ufopaediaArticles.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of all articles
 * provided by the ruleset.
 * @return List of articles.
 */
const std::vector<std::string> &Ruleset::getUfopaediaList() const
{
	return _ufopaediaIndex;
}

/**
 * Returns the list of inventories.
 * @return Pointer to inventory list.
 */
std::map<std::string, RuleInventory*> *Ruleset::getInventories()
{
	return &_invs;
}

/**
 * Returns the rules for a specific inventory.
 * @param id Inventory type.
 * @return Inventory ruleset.
 */
RuleInventory *Ruleset::getInventory(const std::string &id) const
{
	std::map<std::string, RuleInventory*>::const_iterator i = _invs.find(id);
	if (_invs.end() != i) return i->second; else return 0;
}

/**
 * Returns the rules for the specified research project.
 * @param id Research project type.
 * @return Rules for the research project.
 */
RuleResearch *Ruleset::getResearch (const std::string &id) const
{
	std::map<std::string, RuleResearch *>::const_iterator i = _research.find(id);
	if (_research.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of research projects.
 * @return The list of research projects.
 */
const std::vector<std::string> &Ruleset::getResearchList () const
{
	return _researchIndex;
}

/**
 * Returns the rules for the specified manufacture project.
 * @param id Manufacture project type.
 * @return Rules for the manufacture project.
 */
RuleManufacture *Ruleset::getManufacture (const std::string &id) const
{
	std::map<std::string, RuleManufacture *>::const_iterator i = _manufacture.find(id);
	if (_manufacture.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of manufacture projects.
 * @return The list of manufacture projects.
 */
const std::vector<std::string> &Ruleset::getManufactureList () const
{
	return _manufactureIndex;
}

/**
 * Generates and returns a list of facilities for custom bases.
 * The list contains all the facilities that are listed in the 'startingBase'
 * part of the ruleset.
 * @return The list of facilities for custom bases.
 */
std::vector<OpenXcom::RuleBaseFacility*> Ruleset::getCustomBaseFacilities() const
{
	std::vector<OpenXcom::RuleBaseFacility*> PlaceList;

	for (YAML::const_iterator i = _startingBase["facilities"].begin(); i != _startingBase["facilities"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (type != "STR_ACCESS_LIFT")
		{
			PlaceList.push_back(getBaseFacility(type));
		}
	}
	return PlaceList;
}

/**
 * Returns the data for the specified ufo trajectory.
 * @param id Ufo trajectory id.
 * @return A pointer to the data for the specified ufo trajectory.
 */
const UfoTrajectory *Ruleset::getUfoTrajectory(const std::string &id) const
{
	std::map<std::string, UfoTrajectory *>::const_iterator i = _ufoTrajectories.find(id);
	if (_ufoTrajectories.end() != i) return i->second; else return 0;
}

/**
 * Returns the rules for the specified alien mission.
 * @param id Alien mission type.
 * @return Rules for the alien mission.
 */
const RuleAlienMission *Ruleset::getAlienMission(const std::string &id) const
{
	std::map<std::string, RuleAlienMission *>::const_iterator i = _alienMissions.find(id);
	if (_alienMissions.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of alien mission types.
 * @return The list of alien mission types.
 */
const std::vector<std::string> &Ruleset::getAlienMissionList() const
{
	return _alienMissionsIndex;
}

#define CITY_EPSILON 0.00000000000001 // compensate for slight coordinate change

/**
 * @brief Match a city based on coordinates.
 * This function object compares a city's coordinates with the stored coordinates.
 */
class EqualCoordinates: std::unary_function<const City *, bool>
{
public:
	/// Remembers the coordinates.
	EqualCoordinates(double lon, double lat) : _lon(lon), _lat(lat) { /* Empty by design */ }
	/// Compares with stored coordinates.
	//bool operator()(const City *city) const { return AreSame(city->getLongitude(), _lon) && AreSame(city->getLatitude(), _lat); }
	bool operator()(const City *city) const { return (fabs(city->getLongitude() - _lon) < CITY_EPSILON) &&
	                                                 (fabs(city->getLatitude() - _lat) < CITY_EPSILON); }
private:
	double _lon, _lat;
};

/**
 * Finds the city at coordinates @a lon, @a lat.
 * The search will only match exact coordinates.
 * @param lon The longtitude.
 * @param lat The latitude.
 * @return A pointer to the city information, or 0 if no city was found.
 */
const City *Ruleset::locateCity(double lon, double lat) const
{
	for (std::map<std::string, RuleRegion*>::const_iterator rr = _regions.begin(); rr != _regions.end(); ++rr)
	{
		const std::vector<City*> &cities = *rr->second->getCities();
		std::vector<City *>::const_iterator citer = std::find_if(cities.begin(), cities.end(), EqualCoordinates(lon, lat));
		if (citer != cities.end())
		{
			return *citer;
		}
	}
	return 0;
}

/**
 * Gets the alien item level table.
 * @return A deep array containing the alien item levels.
 */
const std::vector<std::vector<int> > &Ruleset::getAlienItemLevels() const
{
	return _alienItemLevels;
}

/**
 * Gets the Defined starting base.
 * @return The starting base definition.
 */
const YAML::Node &Ruleset::getStartingBase()
{
	return _startingBase;
}

/**
 * Gets an MCDPatch.
 * @param id The ID of the MCDPatch we want.
 * @return The MCDPatch based on ID, or 0 if none defined.
 */
MCDPatch *Ruleset::getMCDPatch(const std::string id) const
{
	std::map<std::string, MCDPatch*>::const_iterator i = _MCDPatches.find(id);
	if (_MCDPatches.end() != i) return i->second; else return 0;
}

/**
 * Gets the list of external sprites.
 * @return The list of external sprites.
 */
std::vector<std::pair<std::string, ExtraSprites *> > Ruleset::getExtraSprites() const
{
	return _extraSprites;
}

/**
 * Gets the list of external sounds.
 * @return The list of external sounds.
 */
std::vector<std::pair<std::string, ExtraSounds *> > Ruleset::getExtraSounds() const
{
	return _extraSounds;
}
/**
 * Gets the list of external strings.
 * @return The list of external strings.
 */
std::map<std::string, ExtraStrings *> Ruleset::getExtraStrings() const
{
	return _extraStrings;
}

/**
 * Sorts all our lists according to their weight.
 */
void Ruleset::sortLists()
{
	std::map<int, std::string> list;
	int offset = 0;

	for (std::vector<std::string>::const_iterator i = _itemsIndex.begin(); i != _itemsIndex.end(); ++i)
	{
		while (list.find(getItem(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getItem(*i)->getListOrder() + offset] = *i;
	}
	_itemsIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_itemsIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;

	for (std::vector<std::string>::const_iterator i = _craftsIndex.begin(); i != _craftsIndex.end(); ++i)
	{
		while (list.find(getCraft(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getCraft(*i)->getListOrder() + offset] = *i;
	}
	_craftsIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_craftsIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	for (std::vector<std::string>::const_iterator i = _facilitiesIndex.begin(); i != _facilitiesIndex.end(); ++i)
	{
		while (list.find(getBaseFacility(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getBaseFacility(*i)->getListOrder() + offset] = *i;
	}
	_facilitiesIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_facilitiesIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	for (std::vector<std::string>::const_iterator i = _craftWeaponsIndex.begin(); i != _craftWeaponsIndex.end(); ++i)
	{
		while (list.find(getItem(getCraftWeapon(*i)->getLauncherItem())->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getItem(getCraftWeapon(*i)->getLauncherItem())->getListOrder() + offset] = *i;
	}
	_craftWeaponsIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_craftWeaponsIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	int alternateEntry = 0;
	for (std::vector<std::string>::const_iterator i = _armorsIndex.begin(); i != _armorsIndex.end(); ++i)
	{
		if (getItem(getArmor(*i)->getStoreItem()))
		{
			while (list.find(getItem(getArmor(*i)->getStoreItem())->getListOrder() + offset) != list.end())
			{
				++offset;
			}
			list[getItem(getArmor(*i)->getStoreItem())->getListOrder() + offset] = *i;
		}
		else
		{
			list[alternateEntry] = *i;
			alternateEntry += 1;
		}
	}
	_armorsIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_armorsIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	for (std::vector<std::string>::const_iterator i = _ufopaediaIndex.begin(); i != _ufopaediaIndex.end(); ++i)
	{
		while (list.find(getUfopaediaArticle(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getUfopaediaArticle(*i)->getListOrder() + offset] = *i;
	}
	_ufopaediaIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_ufopaediaIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	for (std::vector<std::string>::const_iterator i = _researchIndex.begin(); i != _researchIndex.end(); ++i)
	{
		while (list.find(getResearch(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getResearch(*i)->getListOrder() + offset] = *i;
	}
	_researchIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_researchIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
	
	for (std::vector<std::string>::const_iterator i = _manufactureIndex.begin(); i != _manufactureIndex.end(); ++i)
	{
		while (list.find(getManufacture(*i)->getListOrder() + offset) != list.end())
		{
			++offset;
		}
		list[getManufacture(*i)->getListOrder() + offset] = *i;
	}
	_manufactureIndex.clear();
	for (std::map<int, std::string>::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		_manufactureIndex.push_back(i->second);
	}
	list.clear();
	offset = 0;
}

/**
 * Gets the research-requirements for Psi-Lab (it's a cache for psiStrengthEval)
 */
std::vector<std::string> Ruleset::getPsiRequirements()
{
	return _psiRequirements;
}

}
