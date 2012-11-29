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
#ifndef OPENXCOM_SAVEDGAME_H
#define OPENXCOM_SAVEDGAME_H

#include <map>
#include <vector>
#include <string>

namespace OpenXcom
{

class Ruleset;
class GameTime;
class Country;
class Base;
class Region;
class Ufo;
class Waypoint;
class SavedBattleGame;
class TextList;
class Language;
class RuleResearch;
class ResearchProject;
class Soldier;
class RuleManufacture;
class TerrorSite;
class AlienBase;

/**
 * Enumerator containing all the possible game difficulties.
 */
enum GameDifficulty { DIFF_BEGINNER, DIFF_EXPERIENCED, DIFF_VETERAN, DIFF_GENIUS, DIFF_SUPERHUMAN };

/**
 * The game data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like funds,
 * game time, current bases and contents, world activities, score, etc.
 */
class SavedGame
{
private:
	GameDifficulty _difficulty;
	GameTime *_time;
	std::vector<int> _funds, _maintenance, _researchScores;
	double _globeLon, _globeLat;
	int _globeZoom;
	std::map<std::string, int> _ids;
	std::vector<Country*> _countries;
	std::vector<Region*> _regions;
	std::vector<Base*> _bases;
	std::vector<Ufo*> _ufos;
	std::vector<Waypoint*> _waypoints;
	std::vector<TerrorSite*> _terrorSites;
	std::vector<AlienBase*> _alienBases;
	SavedBattleGame *_battleGame;
	std::vector<const RuleResearch *> _discovered;
	bool _debug, _warned;

	/// Check whether a ResearchProject can be researched
	bool isResearchAvailable (RuleResearch * r, const std::vector<const RuleResearch *> & unlocked, Ruleset * ruleset) const;
	void getDependableResearchBasic (std::vector<RuleResearch *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const;
public:
	/// Creates a new saved game.
	SavedGame();
	/// Cleans up the saved game.
	~SavedGame();
	/// Gets list of saves in the user directory.
	static void getList(TextList *list, Language *lang);
	/// Loads a saved game from YAML.
	void load(const std::string &filename, Ruleset *rule);
	/// Saves a saved game to YAML.
	void save(const std::string &filename) const;
	/// Gets game difficulty.
	GameDifficulty getDifficulty() const;
	/// Sets game difficulty.
	void setDifficulty(GameDifficulty difficulty);
	/// Gets the current funds.
	int getFunds() const;
	/// Gets the list of funds from previous months.
	std::vector<int> getFundsList() const;
	/// Sets new funds.
	void setFunds(int funds);
	/// Gets the current globe longitude.
	double getGlobeLongitude() const;
	/// Sets the new globe longitude.
	void setGlobeLongitude(double lon);
	/// Gets the current globe latitude.
	double getGlobeLatitude() const;
	/// Sets the new globe latitude.
	void setGlobeLatitude(double lat);
	/// Gets the current globe zoom.
	int getGlobeZoom() const;
	/// Sets the new globe zoom.
	void setGlobeZoom(int zoom);
	/// Handles monthly funding.
	void monthlyFunding();
	/// Gets the current game time.
	GameTime *getTime() const;
	/// Gets the current ID for an object.
	int getId(const std::string &name);
	/// Initializes te IDs list.
	void initIds(const std::map<std::string, int> &ids);
	/// Gets the list of countries.
	std::vector<Country*> *getCountries();
	/// Gets the total country funding.
	int getCountryFunding() const;
	/// Gets the list of regions.
	std::vector<Region*> *getRegions();
	/// Gets the list of bases.
	std::vector<Base*> *getBases();
	/// Gets the total base maintenance.
	int getBaseMaintenance() const;
	/// Gets the list of UFOs.
	std::vector<Ufo*> *getUfos();
	/// Gets the list of waypoints.
	std::vector<Waypoint*> *getWaypoints();
	/// Gets the list of terror sites.
	std::vector<TerrorSite*> *getTerrorSites();
	/// Gets the current battle game.
	SavedBattleGame *getBattleGame();
	/// Sets the current battle game.
	void setBattleGame(SavedBattleGame *battleGame);
	/// Add a finished ResearchProject
	void addFinishedResearch (const RuleResearch * r, Ruleset * ruleset = NULL);
	/// Get the list of already discovered research projects
	const std::vector<const RuleResearch *> & getDiscoveredResearch() const;
	/// Get the list of ResearchProject which can be researched in a Base
	void getAvailableResearchProjects (std::vector<RuleResearch *> & projects, Ruleset * ruleset, Base * base) const;
	/// Get the list of Productions which can be manufactured in a Base
	void getAvailableProductions (std::vector<RuleManufacture *> & productions, Ruleset * ruleset, Base * base) const;
	/// Get the list of newly available research projects once a research has been completed.
	void getDependableResearch (std::vector<RuleResearch *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const;
	/// Get the list of newly available manufacture projects once a research has been completed.
	void getDependableManufacture (std::vector<RuleManufacture *> & dependables, const RuleResearch *research, Ruleset * ruleset, Base * base) const;
	/// Gets if a research has been unlocked.
	bool isResearched(const std::string &research) const;
	/// Gets if a list of research has been unlocked.
	bool isResearched(const std::vector<std::string> &research) const;
	/// Gets the soldier matching this ID.
	Soldier *getSoldier(int id) const;
	/// Handles the higher promotions.
	bool handlePromotions();
	/// Checks how many soldiers of a rank exist and which one has the highest score.
	void inspectSoldiers(Soldier **highestRanked, size_t *total, int rank);
	///  Returns the list of alien bases.
	std::vector<AlienBase*> *getAlienBases();
	/// Sets debug mode.
	void setDebugMode();
	/// Gets debug mode.
	bool getDebugMode() const;
	/// return a list of maintenance costs
	std::vector<int> getMaintenances();
	/// sets the research score for the month
	void setResearchScore(int score);
	/// gets the list of research scores
	std::vector<int> getResearchScores();
	bool getWarned();
	void setWarned(bool warned);
};

}

#endif
