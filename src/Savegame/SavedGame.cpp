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
#include "SavedGame.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "../Ruleset/Ruleset.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Interface/TextList.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include "SavedBattleGame.h"
#include "GameTime.h"
#include "Country.h"
#include "Base.h"
#include "Craft.h"
#include "Region.h"
#include "Ufo.h"
#include "Waypoint.h"
#include "../Ruleset/RuleResearch.h"
#include "ResearchProject.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "../Ruleset/RuleManufacture.h"
#include "Production.h"
#include "TerrorSite.h"

namespace OpenXcom
{
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
SavedGame::SavedGame() : _difficulty(DIFF_BEGINNER), _funds(0), _globeLon(0.0), _globeLat(0.0), _globeZoom(0), _battleGame(0), _debug(false)
{
	RNG::init();
	_time = new GameTime(6, 1, 1, 1999, 12, 0, 0);
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
	for (std::vector<TerrorSite*>::iterator i = _terrorSites.begin(); i != _terrorSites.end(); ++i)
	{
		delete *i;
	}
	delete _battleGame;
}

/**
 * Gets all the saves found in the user folder
 * and adds them to a text list.
 * @param list Text list.
 * @param lang Loaded language.
 */
void SavedGame::getList(TextList *list, Language *lang)
{
	std::vector<std::string> saves = CrossPlatform::getFolderContents(Options::getUserFolder(), "sav");

	for (std::vector<std::string>::iterator i = saves.begin(); i != saves.end(); ++i)
	{
		std::string file = (*i);
		std::string fullname = Options::getUserFolder() + file;
		std::ifstream fin(fullname.c_str());
		try
		{
			if (!fin)
			{
				throw Exception("Failed to load savegame");
			}
			YAML::Parser parser(fin);
			YAML::Node doc;

			parser.GetNextDocument(doc);
			GameTime time = GameTime(6, 1, 1, 1999, 12, 0, 0);
			time.load(doc["time"]);
			std::stringstream saveTime;
			std::wstringstream saveDay, saveMonth, saveYear;
			saveTime << time.getHour() << ":" << std::setfill('0') << std::setw(2) << time.getMinute();
			saveDay << time.getDay() << lang->getString(time.getDayString());
			saveMonth << lang->getString(time.getMonthString());
			saveYear << time.getYear();
			list->addRow(5, Language::utf8ToWstr(file.substr(0, file.length()-4)).c_str(), Language::utf8ToWstr(saveTime.str()).c_str(), saveDay.str().c_str(), saveMonth.str().c_str(), saveYear.str().c_str());
			fin.close();
		}
		catch (Exception &e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}
		catch (YAML::Exception &e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}
	}
}

/**
 * Loads a saved game's contents from a YAML file.
 * @note Assumes the saved game is blank.
 * @param filename YAML filename.
 * @param rule Ruleset for the saved game.
 */
void SavedGame::load(const std::string &filename, Ruleset *rule)
{
	std::string s = Options::getUserFolder() + filename + ".sav";
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception("Failed to load savegame");
	}
	YAML::Parser parser(fin);
	YAML::Node doc;

	// Get brief save info
	parser.GetNextDocument(doc);
	std::string v;
	doc["version"] >> v;
	if (v != Options::getVersion())
	{
		throw Exception("Version mismatch");
	}
	_time->load(doc["time"]);

	// Get full save data
	parser.GetNextDocument(doc);
	int a = 0;
	doc["difficulty"] >> a;
	_difficulty = (GameDifficulty)a;
	doc["funds"] >> _funds;
	doc["globeLon"] >> _globeLon;
	doc["globeLat"] >> _globeLat;
	doc["globeZoom"] >> _globeZoom;
	doc["ids"] >> _ids;

	for (YAML::Iterator i = doc["countries"].begin(); i != doc["countries"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		Country *c = new Country(rule->getCountry(type), false);
		c->load(*i);
		_countries.push_back(c);
	}

	for (YAML::Iterator i = doc["regions"].begin(); i != doc["regions"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		Region *r = new Region(rule->getRegion(type));
		r->load(*i);
		_regions.push_back(r);
	}

	for (YAML::Iterator i = doc["ufos"].begin(); i != doc["ufos"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		Ufo *u = new Ufo(rule->getUfo(type));
		u->load(*i);
		_ufos.push_back(u);
	}

	for (YAML::Iterator i = doc["waypoints"].begin(); i != doc["waypoints"].end(); ++i)
	{
		Waypoint *w = new Waypoint();
		w->load(*i);
		_waypoints.push_back(w);
	}

	for (YAML::Iterator i = doc["terrorSites"].begin(); i != doc["terrorSites"].end(); ++i)
	{
		TerrorSite *t = new TerrorSite();
		t->load(*i);
		_terrorSites.push_back(t);
	}

	for (YAML::Iterator i = doc["bases"].begin(); i != doc["bases"].end(); ++i)
	{
		Base *b = new Base(rule);
		b->load(*i, this);
		_bases.push_back(b);
	}

	for(YAML::Iterator it=doc["discovered"].begin();it!=doc["discovered"].end();++it)
	{
		std::string research;
		*it >> research;
		_discovered.push_back(rule->getResearch(research));
	}

	if (const YAML::Node *pName = doc.FindValue("battleGame"))
	{
		_battleGame = new SavedBattleGame();
		_battleGame->load(*pName, rule, this);
	}

	fin.close();
}

/**
 * Saves a saved game's contents to a YAML file.
 * @param filename YAML filename.
 */
void SavedGame::save(const std::string &filename) const
{
	std::string s = Options::getUserFolder() + filename + ".sav";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		throw Exception("Failed to save savegame");
	}

	YAML::Emitter out;

	// Saves the brief game info used in the saves list
	out << YAML::BeginMap;
	out << YAML::Key << "version" << YAML::Value << Options::getVersion();
	out << YAML::Key << "time" << YAML::Value;
	_time->save(out);
	out << YAML::EndMap;

	// Saves the full game data to the save
	out << YAML::BeginDoc;
	out << YAML::BeginMap;
	out << YAML::Key << "difficulty" << YAML::Value << _difficulty;
	out << YAML::Key << "funds" << YAML::Value << _funds;
	out << YAML::Key << "globeLon" << YAML::Value << _globeLon;
	out << YAML::Key << "globeLat" << YAML::Value << _globeLat;
	out << YAML::Key << "globeZoom" << YAML::Value << _globeZoom;
	out << YAML::Key << "ids" << YAML::Value << _ids;
	out << YAML::Key << "countries" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Country*>::const_iterator i = _countries.begin(); i != _countries.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "regions" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Region*>::const_iterator i = _regions.begin(); i != _regions.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "bases" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Base*>::const_iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "ufos" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Ufo*>::const_iterator i = _ufos.begin(); i != _ufos.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "waypoints" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Waypoint*>::const_iterator i = _waypoints.begin(); i != _waypoints.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "terrorSites" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<TerrorSite*>::const_iterator i = _terrorSites.begin(); i != _terrorSites.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "discovered" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<const RuleResearch *>::const_iterator i = _discovered.begin(); i != _discovered.end(); ++i)
	{
		out << (*i)->getName ();
	}
	out << YAML::EndSeq;
	if (_battleGame != 0)
	{
		out << YAML::Key << "battleGame" << YAML::Value;
		_battleGame->save(out);
	}
	out << YAML::EndMap;
	sav << out.c_str();
	sav.close();
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
 * Returns the player's current funds.
 * @return Current funds.
 */
int SavedGame::getFunds() const
{
	return _funds;
}

/**
 * Changes the player's funds to a new value.
 * @param funds New funds.
 */
void SavedGame::setFunds(int funds)
{
	_funds = funds;
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
	_funds += getCountryFunding() - getBaseMaintenance();
}

/**
 * Returns the current time of the game.
 * @return Pointer to the game time.
 */
GameTime *const SavedGame::getTime() const
{
	return _time;
}

/**
 * Returns the latest ID for the specified object
 * and increases it.
 * @param name Object name.
 * @return Latest ID number.
 */
int SavedGame::getId(const std::string &name)
{
	int id = _ids[name];
	_ids[name]++;
	return id;
}

/**
 * Initializes the list of object IDs.
 * @param ids ID number list.
 */
void SavedGame::initIds(const std::map<std::string, int> &ids)
{
	_ids = ids;
}

/**
 * Returns the list of countries in the game world.
 * @return Pointer to country list.
 */
std::vector<Country*> *const SavedGame::getCountries()
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
		total += (*i)->getFunding();
	}
	return total;
}

/**
 * Returns the list of world regions.
 * @return Pointer to region list.
 */
std::vector<Region*> *const SavedGame::getRegions()
{
	return &_regions;
}

/**
 * Returns the list of player bases.
 * @return Pointer to base list.
 */
std::vector<Base*> *const SavedGame::getBases()
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
std::vector<Ufo*> *const SavedGame::getUfos()
{
	return &_ufos;
}

/**
 * Returns the list of craft waypoints.
 * @return Pointer to waypoint list.
 */
std::vector<Waypoint*> *const SavedGame::getWaypoints()
{
	return &_waypoints;
}

/**
 * Returns the list of terror sites.
 * @return Pointer to terror site list.
 */
std::vector<TerrorSite*> *const SavedGame::getTerrorSites()
{
	return &_terrorSites;
}

/**
 * Get pointer to the battleGame object.
 * @return Pointer to the battleGame object.
 */
SavedBattleGame *const SavedGame::getBattleGame()
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
 * @param r The newly found ResearchProject
*/
void SavedGame::addFinishedResearch (const RuleResearch * r, Ruleset * ruleset)
{
	_discovered.push_back(r);
	if(ruleset)
	{
		std::vector<RuleResearch*> availableResearch;
		for(std::vector<Base*>::const_iterator it = _bases.begin (); it != _bases.end (); ++it)
		{
			getDependableResearchBasic(availableResearch, r, ruleset, *it);
		}
		for(std::vector<RuleResearch*>::iterator it = availableResearch.begin (); it != availableResearch.end (); ++it)
		{
			if((*it)->getCost() == 0)
			{
				addFinishedResearch(*it, ruleset);
			}
		}
	}
}

/**
   Returns the list of already discovered ResearchProject
 * @return the list of already discovered ResearchProject
*/
const std::vector<const RuleResearch *> & SavedGame::getDiscoveredResearch() const
{
	return _discovered;
}

/**
   Get the list of RuleResearch which can be researched in a Base.
   * @param projects the list of ResearchProject which are available.
   * @param ruleset the Game Ruleset
   * @param base a pointer to a Base
*/
void SavedGame::getAvailableResearchProjects (std::vector<RuleResearch *> & projects, Ruleset * ruleset, Base * base) const
{
	const std::vector<const RuleResearch *> & discovered(getDiscoveredResearch());
	std::vector<std::string> researchProjects = ruleset->getResearchList();
	const std::vector<ResearchProject *> & baseResearchProjects = base->getResearch();
	std::vector<const RuleResearch *> unlocked;
	for(std::vector<const RuleResearch *>::const_iterator it = discovered.begin (); it != discovered.end (); ++it)
	{
		for(std::vector<std::string>::const_iterator itUnlocked = (*it)->getUnlocked ().begin (); itUnlocked != (*it)->getUnlocked ().end (); ++itUnlocked)
		{
			unlocked.push_back(ruleset->getResearch(*itUnlocked));
		}
	}
	for(std::vector<std::string>::const_iterator iter = researchProjects.begin (); iter != researchProjects.end (); ++iter)
	{
		RuleResearch *research = ruleset->getResearch(*iter);
		if (!isResearchAvailable(research, unlocked, ruleset))
		{
			continue;
		}
		std::vector<const RuleResearch *>::const_iterator itDiscovered = std::find(discovered.begin (), discovered.end (), research);
		if (itDiscovered != discovered.end ())
		{
			continue;
		}
		if (std::find_if (baseResearchProjects.begin(), baseResearchProjects.end (), findRuleResearch(research)) != baseResearchProjects.end ())
		{
			continue;
		}
		if (research->needItem() && base->getItems()->getItem(research->getName ()) == 0)
		{
			continue;
		}
		projects.push_back (research);
	}
}

/**
   Get the list of RuleManufacture which can be manufacture in a Base.
   * @param productions the list of Productions which are available.
   * @param ruleset the Game Ruleset
   * @param base a pointer to a Base
*/
void SavedGame::getAvailableProductions (std::vector<RuleManufacture *> & productions, Ruleset * ruleset, Base * base) const
{
	const std::vector<std::string> items (ruleset->getManufactureList ());
	const std::vector<Production *> baseProductions (base->getProductions ());

	for(std::vector<std::string>::const_iterator iter = items.begin ();
		iter != items.end ();
		++iter)
	{
		RuleManufacture *m = ruleset->getManufacture(*iter);
		if(!isResearched(m->getRequirements()))
		{
		 	continue;
		}
		if(std::find_if(baseProductions.begin (), baseProductions.end (), equalProduction(m)) != baseProductions.end ())
		{
			continue;
		}
		productions.push_back(m);
	}
}

/**
   Check whether a ResearchProject can be researched.
   * @param r the RuleResearch to test.
   * @param unlocked the list of currently unlocked RuleResearch
   * @return true if the RuleResearch can be researched
*/
bool SavedGame::isResearchAvailable (RuleResearch * r, const std::vector<const RuleResearch *> & unlocked, Ruleset * ruleset) const
{
	std::vector<std::string> deps = r->getDependencies();
	const std::vector<const RuleResearch *> & discovered(getDiscoveredResearch());
	if(std::find(unlocked.begin (), unlocked.end (),
			 r) != unlocked.end ())
	{
		return true;
	}
	for(std::vector<std::string>::const_iterator iter = deps.begin (); iter != deps.end (); ++ iter)
	{
		RuleResearch *research = ruleset->getResearch(*iter);
		std::vector<const RuleResearch *>::const_iterator itDiscovered = std::find(discovered.begin (), discovered.end (), research);
		if (itDiscovered == discovered.end ())
		{
			return false;
		}
	}

	return true;
}

/**
   Get the list of newly available research projects once a ResearchProject has been completed. This function check for fake ResearchProject.
   * @param dependables the list of RuleResearch which are now available.
   * @param research The RuleResearch which has just been discovered
   * @param ruleset the Game Ruleset
   * @param base a pointer to a Base
*/
void SavedGame::getDependableResearch (std::vector<RuleResearch *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const
{
	getDependableResearchBasic(dependables, research, ruleset, base);
	for(std::vector<const RuleResearch *>::const_iterator iter = _discovered.begin (); iter != _discovered.end (); ++iter)
	{
		if((*iter)->getCost() == 0)
		{
			if (std::find((*iter)->getDependencies().begin (), (*iter)->getDependencies().end (), research->getName()) != (*iter)->getDependencies().end ())
			{
				getDependableResearchBasic(dependables, *iter, ruleset, base);
			}
		}
	}
}

/**
   Get the list of newly available research projects once a ResearchProject has been completed. This function doesn't check for fake ResearchProject.
   * @param dependables the list of RuleResearch which are now available.
   * @param research The RuleResearch which has just been discovered
   * @param ruleset the Game Ruleset
   * @param base a pointer to a Base
*/
void SavedGame::getDependableResearchBasic (std::vector<RuleResearch *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const
{
	std::vector<RuleResearch *> possibleProjects;
	getAvailableResearchProjects(possibleProjects, ruleset, base);
	for(std::vector<RuleResearch *>::iterator iter = possibleProjects.begin (); iter != possibleProjects.end (); ++iter)
	{
		if (std::find((*iter)->getDependencies().begin (), (*iter)->getDependencies().end (), research->getName()) != (*iter)->getDependencies().end ()
			||
			std::find((*iter)->getUnlocked().begin (), (*iter)->getUnlocked().end (), research->getName()) != (*iter)->getUnlocked().end ()
			)
		{
				dependables.push_back(*iter);
			if ((*iter)->getCost() == 0)
			{
				getDependableResearchBasic(dependables, *iter, ruleset, base);
			}
			else
			{
			}
		}
	}
}

/**
   Get the list of newly available manufacture projects once a ResearchProject has been completed. This function check for fake ResearchProject.
   * @param dependables the list of RuleManufacture which are now available.
   * @param research The RuleResearch which has just been discovered
   * @param ruleset the Game Ruleset
   * @param base a pointer to a Base
*/
void SavedGame::getDependableManufacture (std::vector<RuleManufacture *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const
{
	std::vector<std::string> mans = ruleset->getManufactureList();
	for(std::vector<std::string>::const_iterator iter = mans.begin (); iter != mans.end (); ++iter)
	{
		RuleManufacture *m = ruleset->getManufacture(*iter);
		std::vector<std::string> reqs = m->getRequirements();
		if(isResearched(m->getRequirements()) && std::find(reqs.begin(), reqs.end(), research->getName()) != reqs.end())
		{
			dependables.push_back(m);
		}
	}
}

/**
 * Returns if a certain research has been completed.
 * @param research Research ID.
 * @return Whether it's researched or not.
 */
bool SavedGame::isResearched(const std::string &research) const
{
	if (research.empty() || _debug)
		return true;
	for (std::vector<const RuleResearch *>::const_iterator i = _discovered.begin(); i != _discovered.end(); ++i)
	{
		if ((*i)->getName() == research)
			return true;
	}

	return false;
}

/**
 * Returns if a certain list of research has been completed.
 * @param research List of research IDs.
 * @return Whether it's researched or not.
 */
bool SavedGame::isResearched(const std::vector<std::string> &research) const
{
	if (research.empty() || _debug)
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
Soldier *const SavedGame::getSoldier(int id) const
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
	return 0;
}

/**
 * Handles the higher promotions (not the rookie-squaddie ones).
 * @return Whether or not some promotions happened - to show the promotions screen.
 */
bool SavedGame::handlePromotions()
{
	size_t soldiersPromoted = 0, soldiersTotal = 0;

	for (std::vector<Base*>::iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		soldiersTotal += (*i)->getSoldiers()->size();
	}
	Soldier *highestRanked = 0;

	// now determine the number of positions we have of each rank,
	// and the soldier with the heighest promotion score of the rank below it

	size_t filledPositions = 0, filledPositions2 = 0;
	inspectSoldiers(&highestRanked, &filledPositions, RANK_COMMANDER);
	inspectSoldiers(&highestRanked, &filledPositions2, RANK_COLONEL);
	if (filledPositions < 1 && filledPositions2 > 0)
	{
		// only promote one colonel to commander
		highestRanked->promoteRank();
		soldiersPromoted++;
	}
	inspectSoldiers(&highestRanked, &filledPositions, RANK_COLONEL);
	inspectSoldiers(&highestRanked, &filledPositions2, RANK_CAPTAIN);
	if (filledPositions < (soldiersTotal / 23) && filledPositions2 > 0)
	{
		highestRanked->promoteRank();
		soldiersPromoted++;
	}
	inspectSoldiers(&highestRanked, &filledPositions, RANK_CAPTAIN);
	inspectSoldiers(&highestRanked, &filledPositions2, RANK_SERGEANT);
	if (filledPositions < (soldiersTotal / 11) && filledPositions2 > 0)
	{
		highestRanked->promoteRank();
		soldiersPromoted++;
	}
	inspectSoldiers(&highestRanked, &filledPositions, RANK_SERGEANT);
	inspectSoldiers(&highestRanked, &filledPositions2, RANK_SQUADDIE);
	if (filledPositions < (soldiersTotal / 5) && filledPositions2 > 0)
	{
		highestRanked->promoteRank();
		soldiersPromoted++;
	}

	return (soldiersPromoted > 0);
}

/**
 * Checks how many soldiers of a rank exist and which one has the highest score.
 * @param Pointer to an int to store the total in.
 * @param Rank to inspect.
 * @return The highest scoring soldier of that rank.
 */
void SavedGame::inspectSoldiers(Soldier **highestRanked, size_t *total, int rank)
{
	int highestScore = 0;
	*total = 0;

	for (std::vector<Base*>::iterator i = _bases.begin(); i != _bases.end(); ++i)
	{
		for (std::vector<Soldier*>::iterator j = (*i)->getSoldiers()->begin(); j != (*i)->getSoldiers()->end(); ++j)
		{
			if ((*j)->getRank() == (SoldierRank)rank)
			{
				(*total)++;
				UnitStats *s = (*j)->getCurrentStats();
				int v1 = 2 * s->health + 2 * s->stamina + 4 * s->reactions + 4 * s->bravery;
				int v2 = v1 + 3*( s->tu + 2*( s->firing ) );
				int v3 = v2 + s->melee + s->throwing + s->strength;
				//if (PsiSkill>0) c3 += PsiStrength + 2* PsiSkill
				int score = v3 + 10 * ( (*j)->getMissions() + (*j)->getKills() );
				if (score > highestScore)
				{
					highestScore = score;
					*highestRanked = (*j);
				}
			}
		}
	}
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

}
