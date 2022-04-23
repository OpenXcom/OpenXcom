/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "SavedGame.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "../version.h"
#include "../Engine/Logger.h"
#include "../Mod/Mod.h"
#include "../Engine/RNG.h"
#include "../Engine/Unicode.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include "SavedBattleGame.h"
#include "SerializationHelper.h"
#include "GameTime.h"
#include "Country.h"
#include "Base.h"
#include "Craft.h"
#include "Region.h"
#include "Ufo.h"
#include "Waypoint.h"
#include "../Mod/RuleResearch.h"
#include "ResearchProject.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "Transfer.h"
#include "../Mod/RuleManufacture.h"
#include "Production.h"
#include "MissionSite.h"
#include "AlienBase.h"
#include "AlienStrategy.h"
#include "AlienMission.h"
#include "../Mod/RuleRegion.h"
#include "MissionStatistics.h"
#include "SoldierDeath.h"

namespace OpenXcom
{

const std::string SavedGame::AUTOSAVE_GEOSCAPE = "_autogeo_.asav",
				  SavedGame::AUTOSAVE_BATTLESCAPE = "_autobattle_.asav",
				  SavedGame::QUICKSAVE = "_quick_.asav";

struct findRuleResearch : public std::unary_function<ResearchProject *,
								bool>
{
	RuleResearch * _toFind;
	findRuleResearch(RuleResearch * toFind);
	bool operator()(const ResearchProject *r) const;
};

findRuleResearch::findRuleResearch(RuleResearch * toFind) : _toFind(toFind)
{
}

bool findRuleResearch::operator()(const ResearchProject *r) const
{
	return _toFind == r->getRules();
}

struct equalProduction : public std::unary_function<Production *,
							bool>
{
	RuleManufacture * _item;
	equalProduction(RuleManufacture * item);
	bool operator()(const Production * p) const;
};

equalProduction::equalProduction(RuleManufacture * item) : _item(item)
{
}

bool equalProduction::operator()(const Production * p) const
{
	return p->getRules() == _item;
}

/**
 * Initializes a brand new saved game according to the specified difficulty.
 */
SavedGame::SavedGame() : _difficulty(DIFF_BEGINNER), _end(END_NONE), _ironman(false), _globeLon(0.0), _globeLat(0.0), _globeZoom(0), _battleGame(0), _debug(false), _warned(false), _monthsPassed(-1), _selectedBase(0)
{
	_time = new GameTime(6, 1, 1, 1999, 12, 0, 0);
	_alienStrategy = new AlienStrategy();
	_funds.push_back(0);
	_maintenance.push_back(0);
	_researchScores.push_back(0);
	_incomes.push_back(0);
	_expenditures.push_back(0);
	_lastselectedArmor="STR_NONE_UC";
}

/**
 * Deletes the game content from memory.
 */
SavedGame::~SavedGame()
{
	delete _time;
	for (std::vector<Country*>::iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Region*>::iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Base*>::iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Ufo*>::iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Waypoint*>::iterator i = _waypoints.begin(); i != _waypoints.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<MissionSite*>::iterator i = _missionSites.begin(); i != _missionSites.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<AlienBase*>::iterator i = _alienBases.begin(); i != _alienBases.end(); ++i)
	{
		delete *i;
	}
	delete _alienStrategy;
	for (std::vector<AlienMission*>::iterator i = _activeMissions.begin(); i != _activeMissions.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Soldier*>::iterator i = _deadSoldiers.begin(); i != _deadSoldiers.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<MissionStatistics*>::iterator i = _missionStatistics.begin(); i != _missionStatistics.end(); ++i)
	{
		delete *i;
	}

	delete _battleGame;
}

/**
 * Removes version number from a mod name, if any.
 * @param name Mod id from a savegame.
 * @return Sanitized mod name.
 */
std::string SavedGame::sanitizeModName(const std::string &name)
{
	size_t versionInfoBreakPoint = name.find(" ver: ");
	if (versionInfoBreakPoint == std::string::npos)
	{
		return name;
	}
	else
	{
		return name.substr(0, versionInfoBreakPoint);
	}
}

static bool _isCurrentGameType(const SaveInfo &saveInfo, const std::string &curMaster)
{
	bool matchMasterMod = false;
	if (saveInfo.mods.empty())
	{
		// if no mods listed in the savegame, this is an old-style
		// savegame.  assume "xcom1" as the game type.
		matchMasterMod = (curMaster == "xcom1");
	}
	else
	{
		for (std::vector<std::string>::const_iterator i = saveInfo.mods.begin(); i != saveInfo.mods.end(); ++i)
		{
			std::string name = SavedGame::sanitizeModName(*i);
			if (name == curMaster)
			{
				matchMasterMod = true;
				break;
			}
		}
	}

	if (!matchMasterMod)
	{
		Log(LOG_DEBUG) << "skipping save from inactive master: " << saveInfo.fileName;
	}

	return matchMasterMod;
}

/**
 * Gets all the info of the saves found in the user folder.
 * @param lang Loaded language.
 * @param autoquick Include autosaves and quicksaves.
 * @return List of saves info.
 */
std::vector<SaveInfo> SavedGame::getList(Language *lang, bool autoquick)
{
	std::vector<SaveInfo> info;
	std::string curMaster = Options::getActiveMaster();
	std::vector<std::string> saves = CrossPlatform::getFolderContents(Options::getMasterUserFolder(), "sav");

	if (autoquick)
	{
		std::vector<std::string> asaves = CrossPlatform::getFolderContents(Options::getMasterUserFolder(), "asav");
		saves.insert(saves.begin(), asaves.begin(), asaves.end());
	}
	for (std::vector<std::string>::iterator i = saves.begin(); i != saves.end(); ++i)
	{
		try
		{
			SaveInfo saveInfo = getSaveInfo(*i, lang);
			if (!_isCurrentGameType(saveInfo, curMaster))
			{
				continue;
			}
			info.push_back(saveInfo);
		}
		catch (Exception &e)
		{
			Log(LOG_ERROR) << (*i) << ": " << e.what();
			continue;
		}
		catch (YAML::Exception &e)
		{
			Log(LOG_ERROR) << (*i) << ": " << e.what();
			continue;
		}
	}

	return info;
}

/**
 * Gets the info of a specific save file.
 * @param file Save filename.
 * @param lang Loaded language.
 */
SaveInfo SavedGame::getSaveInfo(const std::string &file, Language *lang)
{
	std::string fullname = Options::getMasterUserFolder() + file;
	YAML::Node doc = YAML::LoadFile(fullname);
	SaveInfo save;

	save.fileName = file;

	if (save.fileName == QUICKSAVE)
	{
		save.displayName = lang->getString("STR_QUICK_SAVE_SLOT");
		save.reserved = true;
	}
	else if (save.fileName == AUTOSAVE_GEOSCAPE)
	{
		save.displayName = lang->getString("STR_AUTO_SAVE_GEOSCAPE_SLOT");
		save.reserved = true;
	}
	else if (save.fileName == AUTOSAVE_BATTLESCAPE)
	{
		save.displayName = lang->getString("STR_AUTO_SAVE_BATTLESCAPE_SLOT");
		save.reserved = true;
	}
	else
	{
		if (doc["name"])
		{
			save.displayName = doc["name"].as<std::string>();
		}
		else
		{
			save.displayName = Unicode::convPathToUtf8(CrossPlatform::noExt(file));
		}
		save.reserved = false;
	}

	save.timestamp = CrossPlatform::getDateModified(fullname);
	std::pair<std::string, std::string> str = CrossPlatform::timeToString(save.timestamp);
	save.isoDate = str.first;
	save.isoTime = str.second;
	save.mods = doc["mods"].as<std::vector< std::string> >(std::vector<std::string>());

	std::ostringstream details;
	if (doc["turn"])
	{
		details << lang->getString("STR_BATTLESCAPE") << ": " << lang->getString(doc["mission"].as<std::string>()) << ", ";
		details << lang->getString("STR_TURN").arg(doc["turn"].as<int>());
	}
	else
	{
		GameTime time = GameTime(6, 1, 1, 1999, 12, 0, 0);
		time.load(doc["time"]);
		details << lang->getString("STR_GEOSCAPE") << ": ";
		details << time.getDayString(lang) << " " << lang->getString(time.getMonthString()) << " " << time.getYear() << ", ";
		details << time.getHour() << ":" << std::setfill('0') << std::setw(2) << time.getMinute();
	}
	if (doc["ironman"].as<bool>(false))
	{
		details << " (" << lang->getString("STR_IRONMAN") << ")";
	}
	save.details = details.str();

	return save;
}

/**
 * Loads a saved game's contents from a YAML file.
 * @note Assumes the saved game is blank.
 * @param filename YAML filename.
 * @param mod Mod for the saved game.
 */
void SavedGame::load(const std::string &filename, Mod *mod)
{
	std::string s = Options::getMasterUserFolder() + filename;
	std::vector<YAML::Node> file = YAML::LoadAllFromFile(s);
	if (file.empty())
	{
		throw Exception(filename + " is not a vaild save file");
	}

	// Get brief save info
	YAML::Node brief = file[0];
	_time->load(brief["time"]);
	if (brief["name"])
	{
		_name = brief["name"].as<std::string>();
	}
	else
	{
		_name = Unicode::convPathToUtf8(filename);
	}
	_ironman = brief["ironman"].as<bool>(_ironman);

	// Get full save data
	YAML::Node doc = file[1];
	_difficulty = (GameDifficulty)doc["difficulty"].as<int>(_difficulty);
	_end = (GameEnding)doc["end"].as<int>(_end);
	if (doc["rng"] && (_ironman || !Options::newSeedOnLoad))
		RNG::setSeed(doc["rng"].as<uint64_t>());
	_monthsPassed = doc["monthsPassed"].as<int>(_monthsPassed);
	_graphRegionToggles = doc["graphRegionToggles"].as<std::string>(_graphRegionToggles);
	_graphCountryToggles = doc["graphCountryToggles"].as<std::string>(_graphCountryToggles);
	_graphFinanceToggles = doc["graphFinanceToggles"].as<std::string>(_graphFinanceToggles);
	_funds = doc["funds"].as< std::vector<int64_t> >(_funds);
	_maintenance = doc["maintenance"].as< std::vector<int64_t> >(_maintenance);
	_researchScores = doc["researchScores"].as< std::vector<int> >(_researchScores);
	_incomes = doc["incomes"].as< std::vector<int64_t> >(_incomes);
	_expenditures = doc["expenditures"].as< std::vector<int64_t> >(_expenditures);
	_warned = doc["warned"].as<bool>(_warned);
	_globeLon = doc["globeLon"].as<double>(_globeLon);
	_globeLat = doc["globeLat"].as<double>(_globeLat);
	_globeZoom = doc["globeZoom"].as<int>(_globeZoom);
	_ids = doc["ids"].as< std::map<std::string, int> >(_ids);

	for (YAML::const_iterator i = doc["countries"].begin(); i != doc["countries"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (mod->getCountry(type))
		{
			Country *c = new Country(mod->getCountry(type), false);
			c->load(*i);
			_countries.push_back(c);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load country " << type;
		}
	}

	for (YAML::const_iterator i = doc["regions"].begin(); i != doc["regions"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (mod->getRegion(type))
		{
			Region *r = new Region(mod->getRegion(type));
			r->load(*i);
			_regions.push_back(r);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load region " << type;
		}
	}

	// Alien bases must be loaded before alien missions
	for (YAML::const_iterator i = doc["alienBases"].begin(); i != doc["alienBases"].end(); ++i)
	{
		std::string deployment = (*i)["deployment"].as<std::string>("STR_ALIEN_BASE_ASSAULT");
		if (mod->getDeployment(deployment))
		{
			AlienBase *b = new AlienBase(mod->getDeployment(deployment));
			b->load(*i);
			_alienBases.push_back(b);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load deployment for alien base " << deployment;
		}
	}

	// Missions must be loaded before UFOs.
	const YAML::Node &missions = doc["alienMissions"];
	for (YAML::const_iterator it = missions.begin(); it != missions.end(); ++it)
	{
		std::string missionType = (*it)["type"].as<std::string>();
		if (mod->getAlienMission(missionType))
		{
			const RuleAlienMission &mRule = *mod->getAlienMission(missionType);
			AlienMission *mission = new AlienMission(mRule);
			mission->load(*it, *this);
			_activeMissions.push_back(mission);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load mission " << missionType;
		}
	}

	for (YAML::const_iterator i = doc["ufos"].begin(); i != doc["ufos"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (mod->getUfo(type))
		{
			Ufo *u = new Ufo(mod->getUfo(type));
			u->load(*i, *mod, *this);
			_ufos.push_back(u);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load UFO " << type;
		}
	}

	for (YAML::const_iterator i = doc["waypoints"].begin(); i != doc["waypoints"].end(); ++i)
	{
		Waypoint *w = new Waypoint();
		w->load(*i);
		_waypoints.push_back(w);
	}

	// Backwards compatibility
	for (YAML::const_iterator i = doc["terrorSites"].begin(); i != doc["terrorSites"].end(); ++i)
	{
		std::string type = "STR_ALIEN_TERROR";
		std::string deployment = "STR_TERROR_MISSION";
		if (mod->getAlienMission(type) && mod->getDeployment(deployment))
		{
			MissionSite *m = new MissionSite(mod->getAlienMission(type), mod->getDeployment(deployment));
			m->load(*i);
			_missionSites.push_back(m);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load mission " << type << " deployment " << deployment;
		}
	}

	for (YAML::const_iterator i = doc["missionSites"].begin(); i != doc["missionSites"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		std::string deployment = (*i)["deployment"].as<std::string>("STR_TERROR_MISSION");
		if (mod->getAlienMission(type) && mod->getDeployment(deployment))
		{
			MissionSite *m = new MissionSite(mod->getAlienMission(type), mod->getDeployment(deployment));
			m->load(*i);
			_missionSites.push_back(m);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load mission " << type << " deployment " << deployment;
		}
	}

	// Discovered Techs Should be loaded before Bases (e.g. for PSI evaluation)
	for (YAML::const_iterator it = doc["discovered"].begin(); it != doc["discovered"].end(); ++it)
	{
		std::string research = it->as<std::string>();
		if (mod->getResearch(research))
		{
			_discovered.push_back(mod->getResearch(research));
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load research " << research;
		}
	}

	for (YAML::const_iterator i = doc["bases"].begin(); i != doc["bases"].end(); ++i)
	{
		Base *b = new Base(mod);
		b->load(*i, this, false);
		_bases.push_back(b);
	}

	const YAML::Node &research = doc["poppedResearch"];
	for (YAML::const_iterator it = research.begin(); it != research.end(); ++it)
	{
		std::string id = it->as<std::string>();
		if (mod->getResearch(id))
		{
			_poppedResearch.push_back(mod->getResearch(id));
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load research " << id;
		}
	}
	_alienStrategy->load(doc["alienStrategy"]);

	for (YAML::const_iterator i = doc["deadSoldiers"].begin(); i != doc["deadSoldiers"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>(mod->getSoldiersList().front());
		if (mod->getSoldier(type))
		{
			Soldier *soldier = new Soldier(mod->getSoldier(type), 0);
			soldier->load(*i, mod, this);
			_deadSoldiers.push_back(soldier);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load soldier " << type;
		}
	}

	for (YAML::const_iterator i = doc["missionStatistics"].begin(); i != doc["missionStatistics"].end(); ++i)
	{
		MissionStatistics *ms = new MissionStatistics();
		ms->load(*i);
		_missionStatistics.push_back(ms);
	}

	if (const YAML::Node &battle = doc["battleGame"])
	{
		_battleGame = new SavedBattleGame();
		_battleGame->load(battle, mod, this);
	}
}

/**
 * Saves a saved game's contents to a YAML file.
 * @param filename YAML filename.
 */
void SavedGame::save(const std::string &filename) const
{
	std::string savPath = Options::getMasterUserFolder() + filename;
	std::string tmpPath = savPath + ".tmp";
	std::ofstream tmp(tmpPath.c_str());
	if (!tmp)
	{
		throw Exception("Failed to save " + filename);
	}

	YAML::Emitter out;

	// Saves the brief game info used in the saves list
	YAML::Node brief;
	brief["name"] = _name;
	brief["version"] = OPENXCOM_VERSION_SHORT;
	brief["engine"] = OPENXCOM_VERSION_ENGINE;
	std::string git_sha = OPENXCOM_VERSION_GIT;
	if (!git_sha.empty() && git_sha[0] ==  '.')
	{
		git_sha.erase(0,1);
	}
	brief["build"] = git_sha;
	brief["time"] = _time->save();
	if (_battleGame != 0)
	{
		brief["mission"] = _battleGame->getMissionType();
		brief["turn"] = _battleGame->getTurn();
	}

	// only save mods that work with the current master
	std::vector<const ModInfo*> activeMods = Options::getActiveMods();
	std::vector<std::string> modsList;
	for (std::vector<const ModInfo*>::const_iterator i = activeMods.begin(); i != activeMods.end(); ++i)
	{
		modsList.push_back((*i)->getId() + " ver: " + (*i)->getVersion());
	}
	brief["mods"] = modsList;
	if (_ironman)
		brief["ironman"] = _ironman;
	out << brief;
	// Saves the full game data to the save
	out << YAML::BeginDoc;
	YAML::Node node;
	node["difficulty"] = (int)_difficulty;
	node["end"] = (int)_end;
	node["monthsPassed"] = _monthsPassed;
	node["graphRegionToggles"] = _graphRegionToggles;
	node["graphCountryToggles"] = _graphCountryToggles;
	node["graphFinanceToggles"] = _graphFinanceToggles;
	node["rng"] = RNG::getSeed();
	node["funds"] = _funds;
	node["maintenance"] = _maintenance;
	node["researchScores"] = _researchScores;
	node["incomes"] = _incomes;
	node["expenditures"] = _expenditures;
	node["warned"] = _warned;
	node["globeLon"] = serializeDouble(_globeLon);
	node["globeLat"] = serializeDouble(_globeLat);
	node["globeZoom"] = _globeZoom;
	node["ids"] = _ids;
	for (std::vector<Country*>::const_iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		node["countries"].push_back((*i)->save());
	}
	for (std::vector<Region*>::const_iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		node["regions"].push_back((*i)->save());
	}
	for (std::vector<Base*>::const_iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		node["bases"].push_back((*i)->save());
	}
	for (std::vector<Waypoint*>::const_iterator i = _waypoints.begin(); i != _waypoints.end(); ++i)
	{
		node["waypoints"].push_back((*i)->save());
	}
	for (std::vector<MissionSite*>::const_iterator i = _missionSites.begin(); i != _missionSites.end(); ++i)
	{
		node["missionSites"].push_back((*i)->save());
	}
	// Alien bases must be saved before alien missions.
	for (std::vector<AlienBase*>::const_iterator i = _alienBases.begin(); i != _alienBases.end(); ++i)
	{
		node["alienBases"].push_back((*i)->save());
	}
	// Missions must be saved before UFOs, but after alien bases.
	for (std::vector<AlienMission *>::const_iterator i = _activeMissions.begin(); i != _activeMissions.end(); ++i)
	{
		node["alienMissions"].push_back((*i)->save());
	}
	// UFOs must be after missions
	for (std::vector<Ufo*>::const_iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		node["ufos"].push_back((*i)->save(getMonthsPassed() == -1));
	}
	for (std::vector<const RuleResearch *>::const_iterator i = _discovered.begin(); i != _discovered.end(); ++i)
	{
		node["discovered"].push_back((*i)->getName());
	}
	for (std::vector<const RuleResearch *>::const_iterator i = _poppedResearch.begin(); i != _poppedResearch.end(); ++i)
	{
		node["poppedResearch"].push_back((*i)->getName());
	}
	node["alienStrategy"] = _alienStrategy->save();
	for (std::vector<Soldier*>::const_iterator i = _deadSoldiers.begin(); i != _deadSoldiers.end(); ++i)
	{
		node["deadSoldiers"].push_back((*i)->save());
	}
	if (Options::soldierDiaries)
	{
		for (std::vector<MissionStatistics*>::const_iterator i = _missionStatistics.begin(); i != _missionStatistics.end(); ++i)
		{
			node["missionStatistics"].push_back((*i)->save());
		}
	}
	if (_battleGame != 0)
	{
		node["battleGame"] = _battleGame->save();
	}
	out << node;

	// Save to temp
	// If this goes wrong, the original save will be safe
	tmp << out.c_str();
	tmp.close();
	if (!tmp)
	{
		throw Exception("Failed to save " + filename);
	}

	// If temp went fine, save for real
	// If this goes wrong, they will have the temp
	std::ofstream sav(savPath.c_str());
	if (!sav)
	{
		throw Exception("Failed to save " + filename);
	}
	sav << out.c_str();
	sav.close();
	if (!sav)
	{
		throw Exception("Failed to save " + filename);
	}

	// Everything went fine, delete the temp
	// We don't care if this fails
	CrossPlatform::deleteFile(tmpPath);
}

/**
 * Returns the game's name shown in Save screens.
 * @return Save name.
 */
std::string SavedGame::getName() const
{
	return _name;
}

/**
 * Changes the game's name shown in Save screens.
 * @param name New name.
 */
void SavedGame::setName(const std::string &name)
{
	_name = name;
}

/**
 * Returns the game's difficulty level.
 * @return Difficulty level.
 */
GameDifficulty SavedGame::getDifficulty() const
{
	return _difficulty;
}

/**
 * Changes the game's difficulty to a new level.
 * @param difficulty New difficulty.
 */
void SavedGame::setDifficulty(GameDifficulty difficulty)
{
	_difficulty = difficulty;
}

/**
 * Returns the game's difficulty coefficient based
 * on the current level.
 * @return Difficulty coefficient.
 */
int SavedGame::getDifficultyCoefficient() const
{
	return Mod::DIFFICULTY_COEFFICIENT[std::min((int)_difficulty, 4)];
}

/**
 * Returns the game's current ending.
 * @return Ending state.
 */
GameEnding SavedGame::getEnding() const
{
	return _end;
}

/**
 * Changes the game's current ending.
 * @param end New ending.
 */
void SavedGame::setEnding(GameEnding end)
{
	_end = end;
}

/**
 * Returns if the game is set to ironman mode.
 * Ironman games cannot be manually saved.
 * @return Tony Stark
 */
bool SavedGame::isIronman() const
{
	return _ironman;
}

/**
 * Changes if the game is set to ironman mode.
 * Ironman games cannot be manually saved.
 * @param ironman Tony Stark
 */
void SavedGame::setIronman(bool ironman)
{
	_ironman = ironman;
}

/**
 * Returns the player's current funds.
 * @return Current funds.
 */
int64_t SavedGame::getFunds() const
{
	return _funds.back();
}

/**
 * Returns the player's funds for the last 12 months.
 * @return funds.
 */
std::vector<int64_t> &SavedGame::getFundsList()
{
	return _funds;
}

/**
 * Changes the player's funds to a new value.
 * @param funds New funds.
 */
void SavedGame::setFunds(int64_t funds)
{
	if (_funds.back() > funds)
	{
		_expenditures.back() += _funds.back() - funds;
	}
	else
	{
		_incomes.back() += funds - _funds.back();
	}
	_funds.back() = funds;
}

/**
 * Returns the current longitude of the Geoscape globe.
 * @return Longitude.
 */
double SavedGame::getGlobeLongitude() const
{
	return _globeLon;
}

/**
 * Changes the current longitude of the Geoscape globe.
 * @param lon Longitude.
 */
void SavedGame::setGlobeLongitude(double lon)
{
	_globeLon = lon;
}

/**
 * Returns the current latitude of the Geoscape globe.
 * @return Latitude.
 */
double SavedGame::getGlobeLatitude() const
{
	return _globeLat;
}

/**
 * Changes the current latitude of the Geoscape globe.
 * @param lat Latitude.
 */
void SavedGame::setGlobeLatitude(double lat)
{
	_globeLat = lat;
}

/**
 * Returns the current zoom level of the Geoscape globe.
 * @return Zoom level.
 */
int SavedGame::getGlobeZoom() const
{
	return _globeZoom;
}

/**
 * Changes the current zoom level of the Geoscape globe.
 * @param zoom Zoom level.
 */
void SavedGame::setGlobeZoom(int zoom)
{
	_globeZoom = zoom;
}

/**
 * Gives the player his monthly funds, taking in account
 * all maintenance and profit costs.
 */
void SavedGame::monthlyFunding()
{
	_funds.back() += getCountryFunding() - getBaseMaintenance();
	_funds.push_back(_funds.back());
	_maintenance.back() = getBaseMaintenance();
	_maintenance.push_back(0);
	_incomes.push_back(getCountryFunding());
	_expenditures.push_back(getBaseMaintenance());
	_researchScores.push_back(0);

	if (_incomes.size() > 12)
		_incomes.erase(_incomes.begin());
	if (_expenditures.size() > 12)
		_expenditures.erase(_expenditures.begin());
	if (_researchScores.size() > 12)
		_researchScores.erase(_researchScores.begin());
	if (_funds.size() > 12)
		_funds.erase(_funds.begin());
	if (_maintenance.size() > 12)
		_maintenance.erase(_maintenance.begin());
}

/**
 * Returns the current time of the game.
 * @return Pointer to the game time.
 */
GameTime *SavedGame::getTime() const
{
	return _time;
}

/**
 * Changes the current time of the game.
 * @param time Game time.
 */
void SavedGame::setTime(const GameTime& time)
{
	_time = new GameTime(time);
}

/**
 * Returns the latest ID for the specified object
 * and increases it.
 * @param name Object name.
 * @return Latest ID number.
 */
int SavedGame::getId(const std::string &name)
{
	std::map<std::string, int>::iterator i = _ids.find(name);
	if (i != _ids.end())
	{
		return i->second++;
	}
	else
	{
		_ids[name] = 1;
		return _ids[name]++;
	}
}

/**
* Resets the list of unique object IDs.
* @param ids New ID list.
*/
const std::map<std::string, int> &SavedGame::getAllIds() const
{
	return _ids;
}

/**
 * Resets the list of unique object IDs.
 * @param ids New ID list.
 */
void SavedGame::setAllIds(const std::map<std::string, int> &ids)
{
	_ids = ids;
}

/**
 * Returns the list of countries in the game world.
 * @return Pointer to country list.
 */
std::vector<Country*> *SavedGame::getCountries()
{
	return &_countries;
}

/**
 * Adds up the monthly funding of all the countries.
 * @return Total funding.
 */
int SavedGame::getCountryFunding() const
{
	int total = 0;
	for (std::vector<Country*>::const_iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		total += (*i)->getFunding().back();
	}
	return total;
}

/**
 * Returns the list of world regions.
 * @return Pointer to region list.
 */
std::vector<Region*> *SavedGame::getRegions()
{
	return &_regions;
}

/**
 * Returns the list of player bases.
 * @return Pointer to base list.
 */
std::vector<Base*> *SavedGame::getBases()
{
	return &_bases;
}

/**
 * Returns the last selected player base.
 * @return Pointer to base.
 */
Base *SavedGame::getSelectedBase()
{
	// in case a base was destroyed or something...
	if (_selectedBase < _bases.size())
	{
		return _bases.at(_selectedBase);
	}
	else
	{
		return _bases.front();
	}
}

/**
 * Sets the last selected player base.
 * @param base number of the base.
 */
void SavedGame::setSelectedBase(size_t base)
{
	_selectedBase = base;
}

/**
 * Returns an immutable list of player bases.
 * @return Pointer to base list.
 */
const std::vector<Base*> *SavedGame::getBases() const
{
	return &_bases;
}

/**
 * Adds up the monthly maintenance of all the bases.
 * @return Total maintenance.
 */
int SavedGame::getBaseMaintenance() const
{
	int total = 0;
	for (std::vector<Base*>::const_iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		total += (*i)->getMonthlyMaintenace();
	}
	return total;
}

/**
 * Returns the list of alien UFOs.
 * @return Pointer to UFO list.
 */
std::vector<Ufo*> *SavedGame::getUfos()
{
	return &_ufos;
}

/**
 * Returns the list of craft waypoints.
 * @return Pointer to waypoint list.
 */
std::vector<Waypoint*> *SavedGame::getWaypoints()
{
	return &_waypoints;
}

/**
 * Returns the list of mission sites.
 * @return Pointer to mission site list.
 */
std::vector<MissionSite*> *SavedGame::getMissionSites()
{
	return &_missionSites;
}

/**
 * Get pointer to the battleGame object.
 * @return Pointer to the battleGame object.
 */
SavedBattleGame *SavedGame::getSavedBattle()
{
	return _battleGame;
}

/**
 * Set battleGame object.
 * @param battleGame Pointer to the battleGame object.
 */
void SavedGame::setBattleGame(SavedBattleGame *battleGame)
{
	delete _battleGame;
	_battleGame = battleGame;
}

/**
 * Add a ResearchProject to the list of already discovered ResearchProject
 * @param research The newly found ResearchProject
 */
void SavedGame::addFinishedResearchSimple(const RuleResearch * research)
{
	_discovered.push_back(research);
}

/**
 * Add a ResearchProject to the list of already discovered ResearchProject
 * @param research The newly found ResearchProject
 * @param mod the game Mod
 * @param base the base, in which the project was finished
 * @param score should the score be awarded or not?
 */
void SavedGame::addFinishedResearch(const RuleResearch * research, const Mod * mod, Base * base, bool score)
{
	// Not really a queue in C++ terminology (we don't need or want pop_front())
	std::vector<const RuleResearch *> queue;
	queue.push_back(research);

	size_t currentQueueIndex = 0;
	while (queue.size() > currentQueueIndex)
	{
		const RuleResearch *currentQueueItem = queue.at(currentQueueIndex);

		// 1. Find out and remember if the currentQueueItem has any undiscovered "protected unlocks"
		bool hasUndiscoveredProtectedUnlocks = hasUndiscoveredProtectedUnlock(currentQueueItem, mod);

		// 2. If the currentQueueItem was *not* already discovered before, add it to discovered research
		bool checkRelatedZeroCostTopics = true;
		if (!isResearched(currentQueueItem->getName(), false))
		{
			_discovered.push_back(currentQueueItem);
			if (!hasUndiscoveredProtectedUnlocks && isResearched(currentQueueItem->getGetOneFree(), false))
			{
				// If the currentQueueItem can't tell you anything anymore, remove it from popped research
				// Note: this is for optimisation purposes only, functionally it is *not* required...
				// ... removing it prematurely leads to bugs, maybe we should not do it at all?
				removePoppedResearch(currentQueueItem);
			}
			if (score)
			{
				addResearchScore(currentQueueItem->getPoints());
			}
		}
		else
		{
			// If the currentQueueItem *was* already discovered before, check if it has any undiscovered "protected unlocks".
			// If not, all zero-cost topics have already been processed before (during the first discovery)
			// and we can basically terminate here (i.e. skip step 3.).
			if (!hasUndiscoveredProtectedUnlocks)
			{
				checkRelatedZeroCostTopics = false;
			}
		}

		// 3. If currentQueueItem is completed for the *first* time, or if it has any undiscovered "protected unlocks",
		// process all related zero-cost topics
		if (checkRelatedZeroCostTopics)
		{
			// 3a. Gather all available research projects
			std::vector<RuleResearch *> availableResearch;
			if (base)
			{
				// Note: even if two different but related projects are finished in two different bases at the same time,
				// the algorithm is robust enough to treat them *sequentially* (i.e. as if one was researched first and the other second),
				// thus calling this method for *one* base only is enough
				getAvailableResearchProjects(availableResearch, mod, base);
			}
			else
			{
				// Used in vanilla save converter only
				getAvailableResearchProjects(availableResearch, mod, 0);
			}

			// 3b. Iterate through all available projects and add zero-cost projects to the processing queue
			for (std::vector<RuleResearch*>::const_iterator itProjectToTest = availableResearch.begin(); itProjectToTest != availableResearch.end(); ++itProjectToTest)
			{
				// We are only interested in zero-cost projects!
				if ((*itProjectToTest)->getCost() == 0)
				{
					// We are only interested in *new* projects (i.e. not processed or scheduled for processing yet)
					bool isAlreadyInTheQueue = false;
					for (std::vector<const RuleResearch *>::const_iterator itQueue = queue.begin(); itQueue != queue.end(); ++itQueue)
					{
						if ((*itQueue)->getName() == (*itProjectToTest)->getName())
						{
							isAlreadyInTheQueue = true;
							break;
						}
					}

					if (!isAlreadyInTheQueue)
					{
						if ((*itProjectToTest)->getRequirements().empty())
						{
							// no additional checks for "unprotected" topics
							queue.push_back((*itProjectToTest));
						}
						else
						{
							// for "protected" topics, we need to check if the currentQueueItem can unlock it or not
							for (std::vector<std::string>::const_iterator itUnlocks = currentQueueItem->getUnlocked().begin(); itUnlocks != currentQueueItem->getUnlocked().end(); ++itUnlocks)
							{
								if ((*itProjectToTest)->getName() == (*itUnlocks))
								{
									queue.push_back((*itProjectToTest));
									break;
								}
							}
						}
					}
				}
			}
		}

		// 4. process remaining items in the queue
		++currentQueueIndex;
	}
}

/**
 *  Returns the list of already discovered ResearchProject
 * @return the list of already discovered ResearchProject
 */
const std::vector<const RuleResearch *> & SavedGame::getDiscoveredResearch() const
{
	return _discovered;
}

/**
 * Get the list of RuleResearch which can be researched in a Base.
 * @param projects the list of ResearchProject which are available.
 * @param mod the game Mod
 * @param base a pointer to a Base
 * @param considerDebugMode Should debug mode be considered or not.
 */
void SavedGame::getAvailableResearchProjects(std::vector<RuleResearch *> & projects, const Mod * mod, Base * base, bool considerDebugMode) const
{
	// This list is used for topics that can be researched even if *not all* dependencies have been discovered yet (e.g. STR_ALIEN_ORIGINS)
	// Note: all requirements of such topics *have to* be discovered though! This will be handled elsewhere.
	std::vector<const RuleResearch *> unlocked;
	for (std::vector<const RuleResearch *>::const_iterator it = _discovered.begin(); it != _discovered.end(); ++it)
	{
		for (std::vector<std::string>::const_iterator itUnlocked = (*it)->getUnlocked().begin(); itUnlocked != (*it)->getUnlocked().end(); ++itUnlocked)
		{
			unlocked.push_back(mod->getResearch(*itUnlocked, true));
		}
	}

	// Create a list of research topics available for research in the given base
	for (std::vector<std::string>::const_iterator iter = mod->getResearchList().begin(); iter != mod->getResearchList().end(); ++iter)
	{
		RuleResearch *research = mod->getResearch(*iter);

		if ((considerDebugMode && _debug) || std::find(unlocked.begin(), unlocked.end(), research) != unlocked.end())
		{
			// Empty, these research topics are on the "unlocked list", *don't* check the dependencies!
		}
		else
		{
			// These items are not on the "unlocked list", we must check if "dependencies" are satisfied!
			if (!isResearched(research->getDependencies(), considerDebugMode))
			{
				continue;
			}
		}

		// Check if "requires" are satisfied
		// IMPORTANT: research topics with "requires" will NEVER be directly visible to the player anyway
		//   - there is an additional filter in NewResearchListState::fillProjectList(), see comments there for more info
		//   - there is an additional filter in NewPossibleResearchState::NewPossibleResearchState()
		//   - we do this check for other functionality using this method, namely SavedGame::addFinishedResearch()
		//     - Note: when called from there, parameter considerDebugMode = false
		if (!isResearched(research->getRequirements(), considerDebugMode))
		{
			continue;
		}

		// Remove the already researched topics from the list *UNLESS* they can still give you something more
		if (isResearched(research->getName(), false))
		{
			if (!isResearched(research->getGetOneFree(), false))
			{
				// This research topic still has some more undiscovered "getOneFree" topics, keep it!
			}
			else if (hasUndiscoveredProtectedUnlock(research, mod))
			{
				// This research topic still has one or more undiscovered "protected unlocks", keep it!
			}
			else
			{
				// This topic can't give you anything else anymore, ignore it!
				continue;
			}
		}

		if (base)
		{
			// Check if this topic is already being researched in the given base
			const std::vector<ResearchProject *> & baseResearchProjects = base->getResearch();
			if (std::find_if(baseResearchProjects.begin(), baseResearchProjects.end(), findRuleResearch(research)) != baseResearchProjects.end())
			{
				continue;
			}

			// Check for needed item in the given base
			if (research->needItem() && base->getStorageItems()->getItem(research->getName()) == 0)
			{
				continue;
			}
		}
		else
		{
			// Used in vanilla save converter only
			if (research->needItem() && research->getCost() == 0)
			{
				continue;
			}
		}

		// Haleluja, all checks passed, add the research topic to the list
		projects.push_back(research);
	}
}

/**
 * Get the list of newly available research projects once a ResearchProject has been completed.
 * @param before the list of available RuleResearch before completing new research.
 * @param after the list of available RuleResearch after completing new research.
 * @param diff the list of newly available RuleResearch after completing new research (after - before).
 */
void SavedGame::getNewlyAvailableResearchProjects(std::vector<RuleResearch *> & before, std::vector<RuleResearch *> & after, std::vector<RuleResearch *> & diff) const
{
	// History lesson:
	// Completely rewritten the original recursive algorithm, because it was inefficient, unreadable and wrong
	// a/ inefficient: it could call SavedGame::getAvailableResearchProjects() way too many times
	// b/ unreadable: because of recursion
	// c/ wrong: could end in an endless loop! in two different ways! (not in vanilla, but in mods)

	// Note:
	// We could move the sorting of "before" vector right after its creation to optimize a little bit more.
	// But sorting a short list is negligible compared to other operations we had to do to get to this point.
	// So I decided to leave it here, so that it's 100% clear what's going on.
	std::sort(before.begin(), before.end(), CompareRuleResearch());
	std::sort(after.begin(), after.end(), CompareRuleResearch());
	std::set_difference(after.begin(), after.end(), before.begin(), before.end(), std::inserter(diff, diff.begin()), CompareRuleResearch());
}

/**
 * Get the list of RuleManufacture which can be manufacture in a Base.
 * @param productions the list of Productions which are available.
 * @param mod the Game Mod
 * @param base a pointer to a Base
 */
void SavedGame::getAvailableProductions (std::vector<RuleManufacture *> & productions, const Mod * mod, Base * base) const
{
	const std::vector<std::string> &items = mod->getManufactureList();
	const std::vector<Production *>& baseProductions (base->getProductions());

	for (std::vector<std::string>::const_iterator iter = items.begin();
		iter != items.end();
		++iter)
	{
		RuleManufacture *m = mod->getManufacture(*iter);
		if (!isResearched(m->getRequirements()))
		{
			continue;
		}
		if (std::find_if (baseProductions.begin(), baseProductions.end(), equalProduction(m)) != baseProductions.end())
		{
			continue;
		}
		productions.push_back(m);
	}
}

/**
 * Get the list of newly available manufacture projects once a ResearchProject has been completed. This function check for fake ResearchProject.
 * @param dependables the list of RuleManufacture which are now available.
 * @param research The RuleResearch which has just been discovered
 * @param mod the Game Mod
 * @param base a pointer to a Base
 */
void SavedGame::getDependableManufacture (std::vector<RuleManufacture *> & dependables, const RuleResearch *research, const Mod * mod, Base *) const
{
	const std::vector<std::string> &mans = mod->getManufactureList();
	for (std::vector<std::string>::const_iterator iter = mans.begin(); iter != mans.end(); ++iter)
	{
		RuleManufacture *m = mod->getManufacture(*iter);
		const std::vector<std::string> &reqs = m->getRequirements();
		if (isResearched(m->getRequirements()) && std::find(reqs.begin(), reqs.end(), research->getName()) != reqs.end())
		{
			dependables.push_back(m);
		}
	}
}

/**
 * Returns if a research still has undiscovered "protected unlocks".
 * @param r Research to check.
 * @param mod the Game Mod
 * @return Whether it has any undiscovered "protected unlocks" or not.
 */
bool SavedGame::hasUndiscoveredProtectedUnlock(const RuleResearch * r, const Mod * mod) const
{
	// Note: checking for not yet discovered unlocks protected by "requires" (which also implies cost = 0)
	for (std::vector<std::string>::const_iterator itUnlocked = r->getUnlocked().begin(); itUnlocked != r->getUnlocked().end(); ++itUnlocked)
	{
		RuleResearch *unlock = mod->getResearch(*itUnlocked, true);
		if (!unlock->getRequirements().empty())
		{
			if (!isResearched(unlock->getName(), false))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * Returns if a certain research topic has been completed.
 * @param research Research ID.
 * @param considerDebugMode Should debug mode be considered or not.
 * @return Whether it's researched or not.
 */
bool SavedGame::isResearched(const std::string &research, bool considerDebugMode) const
{
	//if (research.empty())
	//	return true;
	if (considerDebugMode && _debug)
		return true;
	for (std::vector<const RuleResearch *>::const_iterator i = _discovered.begin(); i != _discovered.end(); ++i)
	{
		if ((*i)->getName() == research)
			return true;
	}

	return false;
}

/**
 * Returns if a certain list of research topics has been completed.
 * @param research List of research IDs.
 * @param considerDebugMode Should debug mode be considered or not.
 * @return Whether it's researched or not.
 */
bool SavedGame::isResearched(const std::vector<std::string> &research, bool considerDebugMode) const
{
	if (research.empty())
		return true;
	if (considerDebugMode && _debug)
		return true;
	std::vector<std::string> matches = research;
	for (std::vector<const RuleResearch *>::const_iterator i = _discovered.begin(); i != _discovered.end(); ++i)
	{
		for (std::vector<std::string>::iterator j = matches.begin(); j != matches.end(); ++j)
		{
			if ((*i)->getName() == *j)
			{
				j = matches.erase(j);
				break;
			}
		}
		if (matches.empty())
			return true;
	}

	return false;
}

/**
 * Returns pointer to the Soldier given it's unique ID.
 * @param id A soldier's unique id.
 * @return Pointer to Soldier.
 */
Soldier *SavedGame::getSoldier(int id) const
{
	for (std::vector<Base*>::const_iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		for (std::vector<Soldier*>::const_iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
		{
			if ((*j)->getId() == id)
			{
				return (*j);
			}
		}
	}
	for (std::vector<Soldier*>::const_iterator j = _deadSoldiers.begin(); j != _deadSoldiers.end(); ++j)
	{
		if ((*j)->getId() == id)
		{
			return (*j);
		}
	}
	return 0;
}

/**
 * Handles the higher promotions (not the rookie-squaddie ones).
 * @param participants a list of soldiers that were actually present at the battle.
 * @return Whether or not some promotions happened - to show the promotions screen.
 */
bool SavedGame::handlePromotions(std::vector<Soldier*> &participants)
{
	int soldiersPromoted = 0;
	Soldier *highestRanked = 0;
	PromotionInfo soldierData;
	std::vector<Soldier*> soldiers;
	for (std::vector<Base*>::iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
		{
			soldiers.push_back(*j);
			processSoldier(*j, soldierData);
		}
		for (std::vector<Transfer*>::iterator j = (*i)->getTransfers()->begin(); j != (*i)->getTransfers()->end(); ++j)
		{
			if ((*j)->getType() == TRANSFER_SOLDIER)
			{
				soldiers.push_back((*j)->getSoldier());
				processSoldier((*j)->getSoldier(), soldierData);
			}
		}
	}

	int totalSoldiers = (int)(soldiers.size());

	if (soldierData.totalCommanders == 0)
	{
		if (totalSoldiers >= 30)
		{
			highestRanked = inspectSoldiers(soldiers, participants, RANK_COLONEL);
			if (highestRanked)
			{
				// only promote one colonel to commander
				highestRanked->promoteRank();
				soldiersPromoted++;
				soldierData.totalCommanders++;
				soldierData.totalColonels--;
			}
		}
	}

	if ((totalSoldiers / 23) > soldierData.totalColonels)
	{
		while ((totalSoldiers / 23) > soldierData.totalColonels)
		{
			highestRanked = inspectSoldiers(soldiers, participants, RANK_CAPTAIN);
			if (highestRanked)
			{
				highestRanked->promoteRank();
				soldiersPromoted++;
				soldierData.totalColonels++;
				soldierData.totalCaptains--;
			}
			else
			{
				break;
			}
		}
	}

	if ((totalSoldiers / 11) > soldierData.totalCaptains)
	{
		while ((totalSoldiers / 11) > soldierData.totalCaptains)
		{
			highestRanked = inspectSoldiers(soldiers, participants, RANK_SERGEANT);
			if (highestRanked)
			{
				highestRanked->promoteRank();
				soldiersPromoted++;
				soldierData.totalCaptains++;
				soldierData.totalSergeants--;
			}
			else
			{
				break;
			}
		}
	}

	if ((totalSoldiers / 5) > soldierData.totalSergeants)
	{
		while ((totalSoldiers / 5) > soldierData.totalSergeants)
		{
			highestRanked = inspectSoldiers(soldiers, participants, RANK_SQUADDIE);
			if (highestRanked)
			{
				highestRanked->promoteRank();
				soldiersPromoted++;
				soldierData.totalSergeants++;
			}
			else
			{
				break;
			}
		}
	}

	return (soldiersPromoted > 0);
}

/**
 * Processes a soldier, and adds their rank to the promotions data array.
 * @param soldier the soldier to process.
 * @param soldierData the data array to put their info into.
 */
void SavedGame::processSoldier(Soldier *soldier, PromotionInfo &soldierData)
{
	switch (soldier->getRank())
	{
	case RANK_COMMANDER:
		soldierData.totalCommanders++;
		break;
	case RANK_COLONEL:
		soldierData.totalColonels++;
		break;
	case RANK_CAPTAIN:
		soldierData.totalCaptains++;
		break;
	case RANK_SERGEANT:
		soldierData.totalSergeants++;
		break;
	default:
		break;
	}
}

/**
 * Checks how many soldiers of a rank exist and which one has the highest score.
 * @param soldiers full list of live soldiers.
 * @param participants list of participants on this mission.
 * @param rank Rank to inspect.
 * @return the highest ranked soldier
 */
Soldier *SavedGame::inspectSoldiers(std::vector<Soldier*> &soldiers, std::vector<Soldier*> &participants, int rank)
{
	int highestScore = 0;
	Soldier *highestRanked = 0;
	for (std::vector<Soldier*>::iterator i = soldiers.begin(); i != soldiers.end(); ++i)
	{
		if ((*i)->getRank() == rank)
		{
			int score = getSoldierScore(*i);
			if (score > highestScore && (!Options::fieldPromotions || std::find(participants.begin(), participants.end(), *i) != participants.end()))
			{
				highestScore = score;
				highestRanked = (*i);
			}
		}
	}
	return highestRanked;
}

/**
 * Evaluate the score of a soldier based on all of his stats, missions and kills.
 * @param soldier the soldier to get a score for.
 * @return this soldier's score.
 */
int SavedGame::getSoldierScore(Soldier *soldier)
{
	UnitStats *s = soldier->getCurrentStats();
	int v1 = 2 * s->health + 2 * s->stamina + 4 * s->reactions + 4 * s->bravery;
	int v2 = v1 + 3*( s->tu + 2*( s->firing ) );
	int v3 = v2 + s->melee + s->throwing + s->strength;
	if (s->psiSkill > 0) v3 += s->psiStrength + 2 * s->psiSkill;
	return v3 + 10 * ( soldier->getMissions() + soldier->getKills() );
}

/**
  * Returns the list of alien bases.
  * @return Pointer to alien base list.
  */
std::vector<AlienBase*> *SavedGame::getAlienBases()
{
	return &_alienBases;
}

/**
 * Toggles debug mode.
 */
void SavedGame::setDebugMode()
{
	_debug = !_debug;
}

/**
 * Gets the current debug mode.
 * @return Debug mode.
 */
bool SavedGame::getDebugMode() const
{
	return _debug;
}

/** @brief Match a mission based on region and type.
 * This function object will match alien missions based on region and type.
 */
class matchRegionAndType: public std::unary_function<AlienMission *, bool>
{
public:
	/// Store the region and type.
	matchRegionAndType(const std::string &region, MissionObjective objective) : _region(region), _objective(objective) { }
	/// Match against stored values.
	bool operator()(const AlienMission *mis) const
	{
		return mis->getRegion() == _region && mis->getRules().getObjective() == _objective;
	}
private:

	const std::string &_region;
	MissionObjective _objective;
};

/**
 * Find a mission type in the active alien missions.
 * @param region The region string ID.
 * @param objective The active mission objective.
 * @return A pointer to the mission, or 0 if no mission matched.
 */
AlienMission *SavedGame::findAlienMission(const std::string &region, MissionObjective objective) const
{
	std::vector<AlienMission*>::const_iterator ii = std::find_if(_activeMissions.begin(), _activeMissions.end(), matchRegionAndType(region, objective));
	if (ii == _activeMissions.end())
		return 0;
	return *ii;
}

/**
 * return the list of monthly maintenance costs
 * @return list of maintenances.
 */
std::vector<int64_t> &SavedGame::getMaintenances()
{
	return _maintenance;
}

/**
 * adds to this month's research score
 * @param score the amount to add.
 */
void SavedGame::addResearchScore(int score)
{
	_researchScores.back() += score;
}

/**
 * return the list of research scores
 * @return list of research scores.
 */
std::vector<int> &SavedGame::getResearchScores()
{
	return _researchScores;
}

/**
 * return the list of income scores
 * @return list of income scores.
 */
std::vector<int64_t> &SavedGame::getIncomes()
{
	return _incomes;
}

/**
 * return the list of expenditures scores
 * @return list of expenditures scores.
 */
std::vector<int64_t> &SavedGame::getExpenditures()
{
	return _expenditures;
}

/**
 * return if the player has been
 * warned about poor performance.
 * @return true or false.
 */
bool SavedGame::getWarned() const
{
	return _warned;
}

/**
 * sets the player's "warned" status.
 * @param warned set "warned" to this.
 */
void SavedGame::setWarned(bool warned)
{
	_warned = warned;
}

/** @brief Check if a point is contained in a region.
 * This function object checks if a point is contained inside a region.
 */
class ContainsPoint: public std::unary_function<const Region *, bool>
{
public:
	/// Remember the coordinates.
	ContainsPoint(double lon, double lat) : _lon(lon), _lat(lat) { /* Empty by design. */ }
	/// Check is the region contains the stored point.
	bool operator()(const Region *region) const { return region->getRules()->insideRegion(_lon, _lat); }
private:
	double _lon, _lat;
};

/**
 * Find the region containing this location.
 * @param lon The longtitude.
 * @param lat The latitude.
 * @return Pointer to the region, or 0.
 */
Region *SavedGame::locateRegion(double lon, double lat) const
{
	std::vector<Region *>::const_iterator found = std::find_if (_regions.begin(), _regions.end(), ContainsPoint(lon, lat));
	if (found != _regions.end())
	{
		return *found;
	}
	return 0;
}

/**
 * Find the region containing this target.
 * @param target The target to locate.
 * @return Pointer to the region, or 0.
 */
Region *SavedGame::locateRegion(const Target &target) const
{
	return locateRegion(target.getLongitude(), target.getLatitude());
}

/*
 * @return the month counter.
 */
int SavedGame::getMonthsPassed() const
{
	return _monthsPassed;
}

/*
 * @return the GraphRegionToggles.
 */
const std::string &SavedGame::getGraphRegionToggles() const
{
	return _graphRegionToggles;
}

/*
 * @return the GraphCountryToggles.
 */
const std::string &SavedGame::getGraphCountryToggles() const
{
	return _graphCountryToggles;
}

/*
 * @return the GraphFinanceToggles.
 */
const std::string &SavedGame::getGraphFinanceToggles() const
{
	return _graphFinanceToggles;
}

/**
 * Sets the GraphRegionToggles.
 * @param value The new value for GraphRegionToggles.
 */
void SavedGame::setGraphRegionToggles(const std::string &value)
{
	_graphRegionToggles = value;
}

/**
 * Sets the GraphCountryToggles.
 * @param value The new value for GraphCountryToggles.
 */
void SavedGame::setGraphCountryToggles(const std::string &value)
{
	_graphCountryToggles = value;
}

/**
 * Sets the GraphFinanceToggles.
 * @param value The new value for GraphFinanceToggles.
 */
void SavedGame::setGraphFinanceToggles(const std::string &value)
{
	_graphFinanceToggles = value;
}

/*
 * Increment the month counter.
 */
void SavedGame::addMonth()
{
	++_monthsPassed;
}

/*
 * marks a research topic as having already come up as "we can now research"
 * @param research is the project we want to add to the vector
 */
void SavedGame::addPoppedResearch(const RuleResearch* research)
{
	if (!wasResearchPopped(research))
		_poppedResearch.push_back(research);
}

/*
 * checks if an unresearched topic has previously been popped up.
 * @param research is the project we are checking for
 * @return whether or not it has been popped up.
 */
bool SavedGame::wasResearchPopped(const RuleResearch* research)
{
	return (std::find(_poppedResearch.begin(), _poppedResearch.end(), research) != _poppedResearch.end());
}

/*
 * checks for and removes a research project from the "has been popped up" array
 * @param research is the project we are checking for and removing, if necessary.
 */
void SavedGame::removePoppedResearch(const RuleResearch* research)
{
	std::vector<const RuleResearch*>::iterator r = std::find(_poppedResearch.begin(), _poppedResearch.end(), research);
	if (r != _poppedResearch.end())
	{
		_poppedResearch.erase(r);
	}
}

/**
 * Returns the list of dead soldiers.
 * @return Pointer to soldier list.
 */
std::vector<Soldier*> *SavedGame::getDeadSoldiers()
{
	return &_deadSoldiers;
}

/**
 * Sets the last selected armour.
 * @param value The new value for last selected armor - Armor type string.
 */

void SavedGame::setLastSelectedArmor(const std::string &value)
{
	_lastselectedArmor = value;
}

/**
 * Gets the last selected armour
 * @return last used armor type string
 */
std::string SavedGame::getLastSelectedArmor() const
{
	return _lastselectedArmor;
}

/**
 * Returns the list of mission statistics.
 * @return Pointer to statistics list.
 */
std::vector<MissionStatistics*> *SavedGame::getMissionStatistics()
{
	return &_missionStatistics;
}

/**
 * Registers a soldier's death in the memorial.
 * @param soldier Pointer to dead soldier.
 * @param cause Pointer to cause of death, NULL if missing in action.
 */
std::vector<Soldier*>::iterator SavedGame::killSoldier(Soldier *soldier, BattleUnitKills *cause)
{
	std::vector<Soldier*>::iterator j;
	for (std::vector<Base*>::const_iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		for (j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
		{
			if ((*j) == soldier)
			{
				soldier->die(new SoldierDeath(*_time, cause));
				_deadSoldiers.push_back(soldier);
				return (*i)->getSoldiers()->erase(j);
			}
		}
	}
	return j;
}

}
