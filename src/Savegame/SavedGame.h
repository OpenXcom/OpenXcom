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
#ifndef OPENXCOM_SAVEDGAME_H
#define OPENXCOM_SAVEDGAME_H

#include <map>
#include <vector>
#include <string>
#include <time.h>
#include "../Ruleset/RuleScriptedEvent.h"
#include <stdint.h>

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
class Craft;
class TerrorSite;
class AlienBase;
class AlienStrategy;
class AlienMission;
class ScriptedEvent;
class ScriptedEventLocation;
class RuleScriptedEvent;
class Target;
class Soldier;
class Globe;

/**
 * Enumerator containing all the possible game difficulties.
 */
enum GameDifficulty { DIFF_BEGINNER = 0, DIFF_EXPERIENCED, DIFF_VETERAN, DIFF_GENIUS, DIFF_SUPERHUMAN };

/**
 * Enumerator for the various save types.
 */
enum SaveType { SAVE_DEFAULT, SAVE_QUICK, SAVE_AUTO_GEOSCAPE, SAVE_AUTO_BATTLESCAPE, SAVE_IRONMAN, SAVE_IRONMAN_END };

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
	std::vector<std::string> rulesets;
	bool reserved;
};

/**
 * The game data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like funds,
 * game time, current bases and contents, world activities, score, etc.
 */
class SavedGame
{
private:
	std::wstring _name;
	GameDifficulty _difficulty;
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
	std::vector<TerrorSite*> _terrorSites;
	std::vector<AlienBase*> _alienBases;
	AlienStrategy *_alienStrategy;
	SavedBattleGame *_battleGame;
	std::vector<const RuleResearch *> _discovered;
	std::vector<AlienMission*> _activeMissions;
	std::vector<ScriptedEvent*> _scriptedEvents;
	std::vector<ScriptedEventLocation*> _scriptedEventLocations;
	bool _debug, _warned;
	int _monthsPassed;
	std::string _graphRegionToggles;
	std::string _graphCountryToggles;
	std::string _graphFinanceToggles;
	std::vector<const RuleResearch *> _poppedResearch;
	std::vector<Soldier*> _deadSoldiers;
	size_t _selectedBase;
	std::string _lastselectedArmor; //contains the last selected armour

	void getDependableResearchBasic (std::vector<RuleResearch *> & dependables, const RuleResearch *research, const Ruleset * ruleset, Base * base) const;
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
	void load(const std::string &filename, Ruleset *rule);
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
	void setTime(GameTime time);
	/// Gets the current ID for an object.
	int getId(const std::string &name);
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
	/// Gets the list of terror sites.
	std::vector<TerrorSite*> *getTerrorSites();
	/// Scripted event Locations
	std::vector<ScriptedEventLocation*> *getScriptedEventLocations() { return &_scriptedEventLocations; }	
	/// Gets the current battle game.
	SavedBattleGame *getSavedBattle();
	/// Sets the current battle game.
	void setBattleGame(SavedBattleGame *battleGame);
	/// Add a finished ResearchProject
	void addFinishedResearch (const RuleResearch * r, const Ruleset * ruleset = NULL);
	/// Get the list of already discovered research projects
	const std::vector<const RuleResearch *> & getDiscoveredResearch() const;
	/// Get the list of ResearchProject which can be researched in a Base
	void getAvailableResearchProjects (std::vector<RuleResearch *> & projects, const Ruleset * ruleset, Base * base) const;
	/// Get the list of Productions which can be manufactured in a Base
	void getAvailableProductions (std::vector<RuleManufacture *> & productions, const Ruleset * ruleset, Base * base) const;
	/// Get the list of newly available research projects once a research has been completed.
	void getDependableResearch (std::vector<RuleResearch *> & dependables, const RuleResearch *research, const Ruleset * ruleset, Base * base) const;
	/// Get the list of newly available manufacture projects once a research has been completed.
	void getDependableManufacture (std::vector<RuleManufacture *> & dependables, const RuleResearch *research, const Ruleset * ruleset, Base * base) const;
	/// Check whether a ResearchProject can be researched
	bool isResearchAvailable (RuleResearch * r, const std::vector<const RuleResearch *> & unlocked, const Ruleset * ruleset) const;
	/// Gets if a research has been unlocked.
	bool isResearched(const std::string &research) const;
	/// Gets if a list of research has been unlocked.
	bool isResearched(const std::vector<std::string> &research) const;
	/// Gets the soldier matching this ID.
	Soldier *getSoldier(int id) const;
	/// Handles the higher promotions.
	bool handlePromotions(std::vector<Soldier*> &participants);
	/// Checks how many soldiers of a rank exist and which one has the highest score.
	void inspectSoldiers(Soldier **highestRanked, size_t *total, int rank);
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
	/// Gets a mission matching region and type.
	AlienMission *getAlienMission(const std::string &region, const std::string &type) const;
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
	/// Scripted Events
	std::vector<ScriptedEvent*> &getScriptedEvents() { return _scriptedEvents; }	
	/// Get a particular Scripted event
	ScriptedEvent *getScriptedEvent(const std::string &type);
	//Sets the the last selected armour
	void setLastSelectedArmor(const std::string &value);
	//Gets the the last selected armour
	std::string getLastSelectedArmor();
	/// Check through rules and spawn new events if required
	void checkEventsToSpawn(const Ruleset * ruleset, SCRIPTED_EVENT_TYPE eventType, Globe *globe);
	/// Create a new scripted event
	ScriptedEvent *spawnScriptedEvent(const Ruleset * ruleset, RuleScriptedEvent *rules, Globe *globe);
	/// Flag an event as completed
	void completeScriptedEvent(const std::string &type);
	/// Destination for scripted event button (e.g. Cydonia)
	ScriptedEvent *findScriptedEventDestination(const Ruleset * ruleset, Craft *craft);

};

}

#endif
