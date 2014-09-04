/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include <algorithm>
#include "../fmath.h"
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
#include "RuleScriptedEvent.h"
#include "RuleMissionSequence.h"
#include "ExtraSprites.h"
#include "ExtraSounds.h"
#include "ExtraStrings.h"
#include "RuleInterface.h"
#include "SoundDefinition.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Region.h"
#include "../Savegame/Base.h"
#include "../Savegame/Country.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Transfer.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Savegame/AlienStrategy.h"
#include "../Savegame/GameTime.h"
#include "UfoTrajectory.h"
#include "RuleAlienMission.h"
#include "City.h"
#include "MCDPatch.h"
#include "../Engine/Logger.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "StatString.h"
#include "RuleGlobe.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

/**
 * Creates a ruleset with blank sets of rules.
 */
Ruleset::Ruleset() : _costSoldier(0), _costEngineer(0), _costScientist(0), _timePersonnel(0), _initialFunding(0), _alienFuel(""), _startingTime(6, 1, 1, 1999, 12, 0, 0), _modIndex(0), _facilityListOrder(0), _craftListOrder(0), _itemListOrder(0), _researchListOrder(0),  _manufactureListOrder(0), _ufopaediaListOrder(0), _invListOrder(0)
{
	_globe = new RuleGlobe();

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
	delete _globe;
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
	for (std::map<std::string, RuleScriptedEvent*>::iterator i = _scriptedEvents.begin(); i != _scriptedEvents.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, RuleMissionSequence*>::iterator i = _missionSequences.begin(); i != _missionSequences.end(); ++i)
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
	for (std::map<std::string, RuleInterface *>::const_iterator i = _interfaces.begin (); i != _interfaces.end (); ++i)
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
		RuleInventory *rule = loadRule(*i, &_invs, &_invsIndex, "id");
		if (rule != 0)
		{
			_invListOrder += 10;
			rule->load(*i, _invListOrder);
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
 	for (YAML::const_iterator i = doc["scriptedEvents"].begin(); i != doc["scriptedEvents"].end(); ++i)
	{
		RuleScriptedEvent *rule = loadRule(*i, &_scriptedEvents, &_scriptedEventsIndex);
		if (rule != 0)
		{
			rule->load(*i, this);
		}
	}
 	for (YAML::const_iterator i = doc["missionSequences"].begin(); i != doc["missionSequences"].end(); ++i)
	{
		RuleMissionSequence *rule = loadRule(*i, &_missionSequences, &_missionSequencesIndex);
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
			rule->load(*i, _modIndex);
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
		if ((*i)["id"])
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
				case UFOPAEDIA_TYPE_TFTD: rule = new ArticleDefinitionTFTD(); break;
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
		else if ((*i)["delete"])
		{
			std::string type = (*i)["delete"].as<std::string>();
			std::map<std::string, ArticleDefinition*>::iterator i = _ufopaediaArticles.find(type);
			if (i != _ufopaediaArticles.end())
			{
				_ufopaediaArticles.erase(i);
			}
			std::vector<std::string>::iterator idx = std::find(_ufopaediaIndex.begin(), _ufopaediaIndex.end(), type);
			if (idx != _ufopaediaIndex.end())
			{
				_ufopaediaIndex.erase(idx);
			}
		}
	}
 	// Bases can't be copied, so for savegame purposes we store the node instead
	YAML::Node base = doc["startingBase"];
	if (base)
	{
		for (YAML::const_iterator i = base.begin(); i != base.end(); ++i)
		{
			_startingBase[i->first.as<std::string>()] = YAML::Node(i->second);
		}
	}
	if (doc["startingTime"])
	{
		_startingTime.load(doc["startingTime"]);
	}
 	_costSoldier = doc["costSoldier"].as<int>(_costSoldier);
 	_costEngineer = doc["costEngineer"].as<int>(_costEngineer);
 	_costScientist = doc["costScientist"].as<int>(_costScientist);
 	_timePersonnel = doc["timePersonnel"].as<int>(_timePersonnel);
 	_initialFunding = doc["initialFunding"].as<int>(_initialFunding);
	_alienFuel = doc["alienFuel"].as<std::string>(_alienFuel);
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
		// doesn't support modIndex
		if (type != "TEXTURE.DAT")
			extraSprites->load(*i, _modIndex);
		else
			extraSprites->load(*i, 0);
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

	for (YAML::const_iterator i = doc["statStrings"].begin(); i != doc["statStrings"].end(); ++i)
	{
		StatString *statString = new StatString();
		statString->load(*i);
		_statStrings.push_back(statString);
	}

	for (YAML::const_iterator i = doc["interfaces"].begin(); i != doc["interfaces"].end(); ++i)
	{
		RuleInterface *rule = loadRule(*i, &_interfaces);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	for (YAML::const_iterator i = doc["soundDefs"].begin(); i != doc["soundDefs"].end(); ++i)
	{
		SoundDefinition *rule = loadRule(*i, &_soundDefs);
		if (rule != 0)
		{
			rule->load(*i);
		}
	}
	if (doc["globe"])
	{
		_globe->load(doc["globe"]);
	}
	for (YAML::const_iterator i = doc["soundRefs"].begin(); i != doc["soundRefs"].end(); ++i)
	{
		ResourcePack::DOOR_OPEN = (*i)["doorSound"].as<int>(ResourcePack::DOOR_OPEN);
		ResourcePack::SLIDING_DOOR_OPEN = (*i)["slidingDoorSound"].as<int>(ResourcePack::SLIDING_DOOR_OPEN);
		ResourcePack::SLIDING_DOOR_CLOSE = (*i)["slidingDoorClose"].as<int>(ResourcePack::SLIDING_DOOR_CLOSE);
		ResourcePack::SMALL_EXPLOSION = (*i)["smallExplosion"].as<int>(ResourcePack::SMALL_EXPLOSION);
		ResourcePack::LARGE_EXPLOSION = (*i)["largeExplosion"].as<int>(ResourcePack::LARGE_EXPLOSION);
		ResourcePack::ITEM_DROP = (*i)["itemDrop"].as<int>(ResourcePack::ITEM_DROP);
		ResourcePack::ITEM_THROW = (*i)["itemThrow"].as<int>(ResourcePack::ITEM_THROW);
		ResourcePack::ITEM_RELOAD = (*i)["itemReload"].as<int>(ResourcePack::ITEM_RELOAD);
		ResourcePack::WALK_OFFSET = (*i)["walkOffset"].as<int>(ResourcePack::WALK_OFFSET);
		ResourcePack::FLYING_SOUND = (*i)["flyingSound"].as<int>(ResourcePack::FLYING_SOUND);
		if ((*i)["maleScream"])
		{
			int k = 0;
			for (YAML::const_iterator j = (*i)["maleScream"].begin(); j != (*i)["maleScream"].end() && k < 3; ++j, ++k)
			{
				ResourcePack::MALE_SCREAM[k] = (*j).as<int>(ResourcePack::MALE_SCREAM[k]);
			}
		}
		if ((*i)["femaleScream"])
		{
			int k = 0;
			for (YAML::const_iterator j = (*i)["femaleScream"].begin(); j != (*i)["femaleScream"].end() && k < 3; ++j, ++k)
			{
				ResourcePack::FEMALE_SCREAM[k] = (*j).as<int>(ResourcePack::FEMALE_SCREAM[k]);
			}
		}
		ResourcePack::BUTTON_PRESS = (*i)["buttonPress"].as<int>(ResourcePack::BUTTON_PRESS);
		if ((*i)["windowPopup"])
		{
			int k = 0;
			for (YAML::const_iterator j = (*i)["windowPopup"].begin(); j != (*i)["windowPopup"].end() && k < 3; ++j, ++k)
			{
				ResourcePack::WINDOW_POPUP[k] = (*j).as<int>(ResourcePack::WINDOW_POPUP[k]);
			}
		}
		ResourcePack::UFO_FIRE = (*i)["ufoFire"].as<int>(ResourcePack::UFO_FIRE);
		ResourcePack::UFO_HIT = (*i)["ufoHit"].as<int>(ResourcePack::UFO_HIT);
		ResourcePack::UFO_CRASH = (*i)["ufoCrash"].as<int>(ResourcePack::UFO_CRASH);
		ResourcePack::UFO_EXPLODE = (*i)["ufoExplode"].as<int>(ResourcePack::UFO_EXPLODE);
		ResourcePack::INTERCEPTOR_HIT = (*i)["intterceptorHit"].as<int>(ResourcePack::INTERCEPTOR_HIT);
		ResourcePack::INTERCEPTOR_EXPLODE = (*i)["interceptorExplode"].as<int>(ResourcePack::INTERCEPTOR_EXPLODE);
	}
	// refresh _psiRequirements for psiStrengthEval
	for (std::vector<std::string>::const_iterator i = _facilitiesIndex.begin(); i != _facilitiesIndex.end(); ++i)
	{
		RuleBaseFacility *rule = getBaseFacility(*i);
		if (rule->getPsiLaboratories() > 0)
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
 * Loads a rule element, adding/removing from vectors as necessary.
 * @param node YAML node.
 * @param map Map associated to the rule type.
 * @param index Index vector for the rule type.
 * @param key Rule key name.
 * @return Pointer to new rule if one was created, or NULL if one was removed.
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
		if (index != 0)
		{
			std::vector<std::string>::iterator idx = std::find(index->begin(), index->end(), type);
			if (idx != index->end())
			{
				index->erase(idx);
			}
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
		int funding = (*i)->getFunding().back() + missing;
		if (funding < 0)
		{
			funding = (*i)->getFunding().back();
		}
		(*i)->setFunding(funding);
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
		Soldier *soldier = genSoldier(save);
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
 * Returns the rules for the specified scripted event
 * @param name Event name.
 * @return Rules for this scripted event
 */
RuleScriptedEvent *Ruleset::getScriptedEvent(const std::string &name) const
{
	std::map<std::string, RuleScriptedEvent*>::const_iterator i = _scriptedEvents.find(name);
	if (_scriptedEvents.end() != i) return i->second; else return 0;
}

/**
 * Returns the enitre list of rules for scripted events
 * @return List of rules for all scripted events 
 */
const std::vector<std::string> Ruleset::getScriptedEventList() const
{
	return _scriptedEventsIndex;
}

/**
 * Returns the rules for the specified mission sequence
 * @param name Sequence name
 * @return Rules for display screens for this scripted 
 */
RuleMissionSequence *Ruleset::getMissionSequence(const std::string &name) const
{
	std::map<std::string, RuleMissionSequence*>::const_iterator i = _missionSequences.find(name);
	if (_missionSequences.end() != i) return i->second; else return 0;
}

/**
 * Returns the enitre list of rules for scripted events
 * @return List of rules for all scripted events 
 */
const std::vector<std::string> Ruleset::getMissionSequenceList() const
{
	return _missionSequencesIndex;
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
 * Returns the list of inventories.
 * @return The list of inventories.
 */
const std::vector<std::string> &Ruleset::getInvsList () const
{
	return _invsIndex;
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
 * Gets the list of StatStrings.
 * @return The list of StatStrings.
 */
std::vector<StatString *> Ruleset::getStatStrings() const
{
	return _statStrings;
}

/**
 * Compares rules based on their list orders.
 */
template <typename T>
struct compareRule : public std::binary_function<const std::string&, const std::string&, bool>
{
	Ruleset *_ruleset;
	typedef T*(Ruleset::*RuleLookup)(const std::string &id);
	RuleLookup _lookup;

	compareRule(Ruleset *ruleset, RuleLookup lookup) : _ruleset(ruleset), _lookup(lookup)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		T *rule1 = (_ruleset->*_lookup)(r1);
		T *rule2 = (_ruleset->*_lookup)(r2);
		return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Craft weapons use the list order of their launcher item.
 */
template <>
struct compareRule<RuleCraftWeapon> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Ruleset *_ruleset;

	compareRule(Ruleset *ruleset) : _ruleset(ruleset)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		RuleItem *rule1 = _ruleset->getItem(_ruleset->getCraftWeapon(r1)->getLauncherItem());
		RuleItem *rule2 = _ruleset->getItem(_ruleset->getCraftWeapon(r2)->getLauncherItem());
		return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Armor uses the list order of their store item.
 * Itemless armor comes before all else.
 */
template <>
struct compareRule<Armor> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Ruleset *_ruleset;

	compareRule(Ruleset *ruleset) : _ruleset(ruleset)
	{
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		Armor* armor1 = _ruleset->getArmor(r1);
		Armor* armor2 = _ruleset->getArmor(r2);
		RuleItem *rule1 = _ruleset->getItem(armor1->getStoreItem());
		RuleItem *rule2 = _ruleset->getItem(armor2->getStoreItem());
		if (!rule1 && !rule2)
			return (armor1 < armor2); // tiebreaker, don't care about order, pointers are as good as any
		else if (!rule1)
			return true;
		else if (!rule2)
			return false;
		else
			return (rule1->getListOrder() < rule2->getListOrder());
	}
};

/**
 * Ufopaedia articles use section and list order.
 */
template <>
struct compareRule<ArticleDefinition> : public std::binary_function<const std::string&, const std::string&, bool>
{
	Ruleset *_ruleset;
	static std::map<std::string, int> _sections;

	compareRule(Ruleset *ruleset) : _ruleset(ruleset)
	{
		_sections[UFOPAEDIA_XCOM_CRAFT_ARMAMENT] = 0;
		_sections[UFOPAEDIA_HEAVY_WEAPONS_PLATFORMS] = 1;
		_sections[UFOPAEDIA_WEAPONS_AND_EQUIPMENT] = 2;
		_sections[UFOPAEDIA_ALIEN_ARTIFACTS] = 3;
		_sections[UFOPAEDIA_BASE_FACILITIES] = 4;
		_sections[UFOPAEDIA_ALIEN_LIFE_FORMS] = 5;
		_sections[UFOPAEDIA_ALIEN_RESEARCH] = 6;
		_sections[UFOPAEDIA_UFO_COMPONENTS] = 7;
		_sections[UFOPAEDIA_UFOS] = 8;
		_sections[UFOPAEDIA_NOT_AVAILABLE] = 9;
	}

	bool operator()(const std::string &r1, const std::string &r2) const
	{
		ArticleDefinition *rule1 = _ruleset->getUfopaediaArticle(r1);
		ArticleDefinition *rule2 = _ruleset->getUfopaediaArticle(r2);
		if (_sections[rule1->section] == _sections[rule2->section])
			return (rule1->getListOrder() < rule2->getListOrder());
		else
			return (_sections[rule1->section] < _sections[rule2->section]);
	}
};
std::map<std::string, int> compareRule<ArticleDefinition>::_sections;

/**
 * Sorts all our lists according to their weight.
 */
void Ruleset::sortLists()
{
	std::sort(_itemsIndex.begin(), _itemsIndex.end(), compareRule<RuleItem>(this, (compareRule<RuleItem>::RuleLookup)&Ruleset::getItem));
	std::sort(_craftsIndex.begin(), _craftsIndex.end(), compareRule<RuleCraft>(this, (compareRule<RuleCraft>::RuleLookup)&Ruleset::getCraft));
	std::sort(_facilitiesIndex.begin(), _facilitiesIndex.end(), compareRule<RuleBaseFacility>(this, (compareRule<RuleBaseFacility>::RuleLookup)&Ruleset::getBaseFacility));
	std::sort(_researchIndex.begin(), _researchIndex.end(), compareRule<RuleResearch>(this, (compareRule<RuleResearch>::RuleLookup)&Ruleset::getResearch));
	std::sort(_manufactureIndex.begin(), _manufactureIndex.end(), compareRule<RuleManufacture>(this, (compareRule<RuleManufacture>::RuleLookup)&Ruleset::getManufacture));
	std::sort(_invsIndex.begin(), _invsIndex.end(), compareRule<RuleInventory>(this, (compareRule<RuleInventory>::RuleLookup)&Ruleset::getInventory));
	// special cases
	std::sort(_craftWeaponsIndex.begin(), _craftWeaponsIndex.end(), compareRule<RuleCraftWeapon>(this));
	std::sort(_armorsIndex.begin(), _armorsIndex.end(), compareRule<Armor>(this));
	std::sort(_ufopaediaIndex.begin(), _ufopaediaIndex.end(), compareRule<ArticleDefinition>(this));
}

/**
 * Gets the research-requirements for Psi-Lab (it's a cache for psiStrengthEval)
 */
std::vector<std::string> Ruleset::getPsiRequirements() const
{
	return _psiRequirements;
}

/**
 * Creates a new randomly-generated soldier.
 * @param save Saved game the soldier belongs to.
 * @return Newly generated soldier.
 */
Soldier *Ruleset::genSoldier(SavedGame *save) const
{
	Soldier *soldier = 0;
	int newId = save->getId("STR_SOLDIER");

	// Check for duplicates
	// Original X-COM gives up after 10 tries so might as well do the same here
	bool duplicate = true;
	for (int i = 0; i < 10 && duplicate; i++)
	{
		delete soldier;
		soldier = new Soldier(getSoldier("XCOM"), getArmor("STR_NONE_UC"), &_names, newId);
		duplicate = false;
		for (std::vector<Base*>::iterator i = save->getBases()->begin(); i != save->getBases()->end() && !duplicate; ++i)
		{
			for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end() && !duplicate; ++j)
			{
				if ((*j)->getName() == soldier->getName())
				{
					duplicate = true;
				}
			}
			for (std::vector<Transfer*>::iterator k = (*i)->getTransfers()->begin(); k != (*i)->getTransfers()->end() && !duplicate; ++k)
			{
				if ((*k)->getType() == TRANSFER_SOLDIER && (*k)->getSoldier()->getName() == soldier->getName())
				{
					duplicate = true;
				}
			}
		}
	}

	// calculate new statString
	soldier->calcStatString(getStatStrings(), (Options::psiStrengthEval && save->isResearched(getPsiRequirements())));

	return soldier;
}

/**
 * Gets the name of the item to be used as alien fuel.
 * @return the name of the fuel.
 */
const std::string Ruleset::getAlienFuel() const
{
	return _alienFuel;
}

/**
 * Gets information on an interface.
 * @param id the interface we want info on.
 * @return the interface.
 */
RuleInterface *Ruleset::getInterface(const std::string id) const
{
	std::map<std::string, RuleInterface*>::const_iterator i = _interfaces.find(id);
	if (_interfaces.end() != i) return i->second; else return 0;
}

/**
 * Gets the rules for the Geoscape globe.
 * @return Pointer to globe rules.
 */
RuleGlobe *Ruleset::getGlobe() const
{
	return _globe;
}

const std::map<std::string, SoundDefinition *> *Ruleset::getSoundDefinitions() const
{
	return &_soundDefs;
}

}
