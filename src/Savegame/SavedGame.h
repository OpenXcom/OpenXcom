#pragma once
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
#include <map>
#include <vector>
#include <string>
#include <time.h>
#include <stdint.h>
#include "GameTime.h"
#include "../Mod/RuleAlienMission.h"
#include "../Savegame/Craft.h"

namespace OpenXcom
{

class Mod;
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
class MissionSite;
class AlienBase;
class AlienStrategy;
class AlienMission;
class Target;
class Soldier;
class Craft;
struct MissionStatistics;
struct BattleUnitKills;

/**
 * Enumerator containing all the possible game difficulties.
 */
enum GameDifficulty { DIFF_BEGINNER = 0, DIFF_EXPERIENCED, DIFF_VETERAN, DIFF_GENIUS, DIFF_SUPERHUMAN };

/**
 * Enumerator for the various save types.
 */
enum SaveType { SAVE_DEFAULT, SAVE_QUICK, SAVE_AUTO_GEOSCAPE, SAVE_AUTO_BATTLESCAPE, SAVE_IRONMAN, SAVE_IRONMAN_END };

/**
 * Enumerator for the current game ending.
 */
enum GameEnding { END_NONE, END_WIN, END_LOSE };

/**
 * Container for savegame info displayed on listings.
 */
struct SaveInfo
{
	std::string fileName;
	std::wstring displayName;
	time_t timestamp;
	std::wstring isoDate, isoTime;
	std::wstring details;
	std::vector<std::string> mods;
	bool reserved;
};

struct PromotionInfo
{
	int totalCommanders;
	int totalColonels;
	int totalCaptains;
	int totalSergeants;
	PromotionInfo(): totalCommanders(0), totalColonels(0), totalCaptains(0), totalSergeants(0){}
};

/**
 *The game data that gets written to disk when the game is saved.
 *A saved game holds all the variable info in a game like funds,
 *game time, current bases and contents, world activities, score, etc.
 */
class SavedGame
{
private:
	std::wstring _name;
	GameDifficulty _difficulty;
	GameEnding _end;
	bool _ironman;
	GameTime *_time;
	std::vector<int> _researchScores;
	std::vector<int64_t> _funds, _maintenance, _incomes, _expenditures;
	double _globeLon, _globeLat;
	int _globeZoom;
	std::map<std::string, int> _ids;
	std::vector<Country*> _countries;
	std::vector<Region*> _regions;
	std::vector<Base*> _bases;
	std::vector<Ufo*> _ufos;
	std::vector<Waypoint*> _waypoints;
	std::vector<MissionSite*> _missionSites;
	std::vector<AlienBase*> _alienBases;
	AlienStrategy *_alienStrategy;
	SavedBattleGame *_battleGame;
	std::vector<const RuleResearch*> _discovered;
	std::vector<AlienMission*> _activeMissions;
	bool _debug, _warned;
	int _monthsPassed;
	std::string _graphRegionToggles;
	std::string _graphCountryToggles;
	std::string _graphFinanceToggles;
	std::vector<const RuleResearch*> _poppedResearch;
	std::vector<Soldier*> _deadSoldiers;
	size_t _selectedBase;
	std::string _lastselectedArmor; //contains the last selected armour
	std::vector<MissionStatistics*> _missionStatistics;

	static SaveInfo getSaveInfo(const std::string &file, Language *lang);
public:
	static const std::string AUTOSAVE_GEOSCAPE, AUTOSAVE_BATTLESCAPE, QUICKSAVE;
	/// Creates a new saved game.
	SavedGame();
	/// Cleans up the saved game.
	~SavedGame();
	/// Gets list of saves in the user directory.
	static std::vector<SaveInfo> getList(Language *lang, bool autoquick);
	/// Loads a saved game from YAML.
	void load(const std::string &filename, Mod *mod);
	/// Saves a saved game to YAML.
	void save(const std::string &filename) const;
	/// Gets the game name.
	std::wstring getName() const;
	/// Sets the game name.
	void setName(const std::wstring &name);
	/// Gets the game difficulty.
	GameDifficulty getDifficulty() const;
	/// Sets the game difficulty.
	void setDifficulty(GameDifficulty difficulty);
	/// Gets the game difficulty coefficient.
	int getDifficultyCoefficient() const;
	/// Gets the game ending.
	GameEnding getEnding() const;
	/// Sets the game ending.
	void setEnding(GameEnding end);
	/// Gets if the game is in ironman mode.
	bool isIronman() const;
	/// Sets if the game is in ironman mode.
	void setIronman(bool ironman);
	/// Gets the current funds.
	int64_t getFunds() const;
	/// Gets the list of funds from previous months.
	std::vector<int64_t> &getFundsList();
	/// Sets new funds.
	void setFunds(int64_t funds);
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
	/// Sets the current game time.
	void setTime(const GameTime& time);
	/// Gets the current ID for an object.
	int getId(const std::string &name);
	/// Resets the list of object IDs.
	const std::map<std::string, int> &getAllIds() const;
	/// Resets the list of object IDs.
	void setAllIds(const std::map<std::string, int> &ids);
	/// Gets the list of countries.
	std::vector<Country*> *getCountries();
	/// Gets the total country funding.
	int getCountryFunding() const;
	/// Gets the list of regions.
	std::vector<Region*> *getRegions();
	/// Gets the list of bases.
	std::vector<Base*> *getBases();
	/// Gets the list of bases.
	const std::vector<Base*> *getBases() const;
	/// Gets the total base maintenance.
	int getBaseMaintenance() const;
	/// Gets the list of UFOs.
	std::vector<Ufo*> *getUfos();
	/// Gets the list of waypoints.
	std::vector<Waypoint*> *getWaypoints();
	/// Gets the list of mission sites.
	std::vector<MissionSite*> *getMissionSites();
	/// Gets the current battle game.
	SavedBattleGame *getSavedBattle();
	/// Sets the current battle game.
	void setBattleGame(SavedBattleGame *battleGame);
	/// Add a finished ResearchProject
	void addFinishedResearchSimple(const RuleResearch *research);
	/// Add a finished ResearchProject
	void addFinishedResearch(const RuleResearch *research, const Mod *mod, Base *base, bool score = true);
	/// Get the list of already discovered research projects
	const std::vector<const RuleResearch*> & getDiscoveredResearch() const;
	/// Get the list of ResearchProject which can be researched in a Base
	void getAvailableResearchProjects(std::vector<RuleResearch*> & projects, const Mod *mod, Base *base, bool considerDebugMode = false) const;
	/// Get the list of newly available research projects once a research has been completed.
	void getNewlyAvailableResearchProjects(std::vector<RuleResearch*> & before, std::vector<RuleResearch*> & after, std::vector<RuleResearch*> & diff) const;
	/// Get the list of Productions which can be manufactured in a Base
	void getAvailableProductions(std::vector<RuleManufacture*> & productions, const Mod *mod, Base *base) const;
	/// Get the list of newly available manufacture projects once a research has been completed.
	void getDependableManufacture(std::vector<RuleManufacture*> & dependables, const RuleResearch *research, const Mod *mod, Base *base) const;
	/// Gets if a research still has undiscovered "protected unlocks".
	bool hasUndiscoveredProtectedUnlock(const RuleResearch * r, const Mod * mod) const;
	/// Gets if a certain research has been completed.
	bool isResearched(const std::string &research, bool considerDebugMode = true) const;
	/// Gets if a certain list of research topics has been completed.
	bool isResearched(const std::vector<std::string> &research, bool considerDebugMode = true) const;
	/// Gets the soldier matching this ID.
	Soldier *getSoldier(int id) const;
	/// Handles the higher promotions.
	bool handlePromotions(std::vector<Soldier*> &participants);
	/// Processes a soldier's promotion.
	void processSoldier(Soldier *soldier, PromotionInfo &soldierData);
	/// Checks how many soldiers of a rank exist and which one has the highest score.
	Soldier *inspectSoldiers(std::vector<Soldier*> &soldiers, std::vector<Soldier*> &participants, int rank);
	///  Returns the list of alien bases.
	std::vector<AlienBase*> *getAlienBases();
	/// Sets debug mode.
	void setDebugMode();
	/// Gets debug mode.
	bool getDebugMode() const;
	/// return a list of maintenance costs
	std::vector<int64_t> &getMaintenances();
	/// sets the research score for the month
	void addResearchScore(int score);
	/// gets the list of research scores
	std::vector<int> &getResearchScores();
	/// gets the list of incomes.
	std::vector<int64_t> &getIncomes();
	/// gets the list of expenditures.
	std::vector<int64_t> &getExpenditures();
	/// gets whether or not the player has been warned
	bool getWarned() const;
	/// sets whether or not the player has been warned
	void setWarned(bool warned);
	/// Full access to the alien strategy data.
	AlienStrategy &getAlienStrategy() { return *_alienStrategy; }
	/// Read-only access to the alien strategy data.
	const AlienStrategy &getAlienStrategy() const { return *_alienStrategy; }
	/// Full access to the current alien missions.
	std::vector<AlienMission*> &getAlienMissions() { return _activeMissions; }
	/// Read-only access to the current alien missions.
	const std::vector<AlienMission*> &getAlienMissions() const { return _activeMissions; }
	/// Finds a mission by region and objective.
	AlienMission *findAlienMission(const std::string &region, MissionObjective objective) const;
	/// Locate a region containing a position.
	Region *locateRegion(double lon, double lat) const;
	/// Locate a region containing a Target.
	Region *locateRegion(const Target &target) const;
	/// Return the month counter.
	int getMonthsPassed() const;
	/// Return the GraphRegionToggles.
	const std::string &getGraphRegionToggles() const;
	/// Return the GraphCountryToggles.
	const std::string &getGraphCountryToggles() const;
	/// Return the GraphFinanceToggles.
	const std::string &getGraphFinanceToggles() const;
	/// Sets the GraphRegionToggles.
	void setGraphRegionToggles(const std::string &value);
	/// Sets the GraphCountryToggles.
	void setGraphCountryToggles(const std::string &value);
	/// Sets the GraphFinanceToggles.
	void setGraphFinanceToggles(const std::string &value);
	/// Increment the month counter.
	void addMonth();
	/// add a research to the "popped up" array
	void addPoppedResearch(const RuleResearch* research);
	/// check if a research is on the "popped up" array
	bool wasResearchPopped(const RuleResearch* research);
	/// remove a research from the "popped up" array
	void removePoppedResearch(const RuleResearch* research);
	/// Gets the list of dead soldiers.
	std::vector<Soldier*> *getDeadSoldiers();
	/// Gets the last selected player base.
	Base *getSelectedBase();
	/// Set the last selected player base.
	void setSelectedBase(size_t base);
	/// Evaluate the score of a soldier based on all of his stats, missions and kills.
	int getSoldierScore(Soldier *soldier);
	/// Sets the last selected armour
	void setLastSelectedArmor(const std::string &value);
	/// Gets the last selected armour
	std::string getLastSelectedArmor() const;
	/// Returns the craft corresponding to the specified unique id.
	Craft *findCraftByUniqueId(const CraftId& craftId) const;
	/// Gets the list of missions statistics
	std::vector<MissionStatistics*> *getMissionStatistics();
	/// Handles a soldier's death.
	std::vector<Soldier*>::iterator killSoldier(Soldier *soldier, BattleUnitKills *cause = 0);
};

}
