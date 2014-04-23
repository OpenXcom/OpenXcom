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
#ifndef OPENXCOM_SOLDIERDIARY_H
#define OPENXCOM_SOLDIERDIARY_H

#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include "GameTime.h"
#include "../Ruleset/RuleCommendations.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

class GameTime;
class RuleCommendations;
class Ruleset;

enum UnitStatus {STATUS_STANDING, STATUS_WALKING, STATUS_FLYING, STATUS_TURNING, STATUS_AIMING, STATUS_COLLAPSING, STATUS_DEAD, STATUS_UNCONSCIOUS, STATUS_PANICKING, STATUS_BERSERK};
enum UnitFaction {FACTION_PLAYER, FACTION_HOSTILE, FACTION_NEUTRAL};
enum UnitSide {SIDE_FRONT, SIDE_LEFT, SIDE_RIGHT, SIDE_REAR, SIDE_UNDER};
enum UnitBodyPart {BODYPART_HEAD, BODYPART_TORSO, BODYPART_RIGHTARM, BODYPART_LEFTARM, BODYPART_RIGHTLEG, BODYPART_LEFTLEG};

/**
 * Each entry will have a list of kills, detailed by this class.
 */
class SoldierDiaryKills
{
private:
	std::string _alienRank, _alienRace, _weapon, _weaponAmmo;
	UnitFaction _alienFaction;
	UnitStatus _alienState;
    int _mission;
	int _turn;
	
public:
	/// Creates a new diary and loads its contents from YAML.
	SoldierDiaryKills(const YAML::Node& node);
	/// Creates a diary.
	SoldierDiaryKills(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, UnitStatus alienState, UnitFaction alienFaction, int turn);
	/// Cleans up the diary.
	~SoldierDiaryKills();
	/// Loads the diary information from YAML.
	void load(const YAML::Node& node);
	/// Saves the diary information to YAML.
	YAML::Node save() const;
	/// Get
	std::string getAlienRank() const;
	/// Get
	std::string getAlienRace() const;
	/// Get
	std::string getWeapon() const;
	/// Get
	std::string getWeaponAmmo() const;
	/// Get
	std::string getAlienState() const;
	/// Get
	std::string getAlienFaction() const;
    /// Get
    UnitStatus getAlienStateEnum() const;
    /// Get 
    UnitFaction getAlienFactionEnum() const;
	/// Get
	int getTurn() const;
	/// Add unique mission id to turn
	void makeTurnUnique(GameTime missionTime);

};

struct Statistics
{
	// Variables
	bool wasUnconcious;						// Tracks if the soldier fell unconcious
    std::vector<SoldierDiaryKills*> kills;	// Tracks kills
    int shotAtCounter;                      // Tracks how many times the unit was shot at
	int hitCounter;							// Tracks how many times the unit was hit
	int shotByFriendlyCounter;				// Tracks how many times the unit was hit by a friendly
	int shotFriendlyCounter;				// Tracks how many times the unit was hit a friendly
	bool loneSurvivor;						// Tracks if the soldier was the only survivor
	bool ironMan;							// Tracks if the soldier was the only soldier on the mission
	int longDistanceHitCounter;				// Tracks how many long distance shots were landed
	int lowAccuracyHitCounter;				// Tracks how many times the unit landed a low probability shot
	int shotsFiredCounter;					// Tracks how many times a unit has shot
	int shotsLandedCounter;					// Tracks how many times a unit has hit his target

	/// Functions
	// Friendly fire check
	bool hasFriendlyFired()
	{
		for (std::vector<SoldierDiaryKills*>::const_iterator i = kills.begin(); i != kills.end(); ++i)
		{
			if ((*i)->getAlienFactionEnum() == FACTION_PLAYER)
				return true;
		}
		return false;
	}
	// Load function
	void load(const YAML::Node& node)
	{
		wasUnconcious = node["wasUnconcious"].as<bool>(wasUnconcious);
		if (const YAML::Node &YAMLkills = node["kills"])
		{
			for (YAML::const_iterator i = YAMLkills.begin(); i != YAMLkills.end(); ++i)
				kills.push_back(new SoldierDiaryKills(*i));
		}
        shotAtCounter = node["shotAtCounter"].as<int>(shotAtCounter);
		hitCounter = node["hitCounter"].as<int>(hitCounter);
		shotByFriendlyCounter = node["shotByFriendlyCounter"].as<int>(shotByFriendlyCounter);
		shotFriendlyCounter = node["shotFriendlyCounter"].as<int>(shotFriendlyCounter);
		loneSurvivor = node["loneSurvivor"].as<bool>(loneSurvivor);
		ironMan = node["ironMan"].as<bool>(ironMan);
		longDistanceHitCounter = node["longDistanceHitCounter"].as<int>(longDistanceHitCounter);
		lowAccuracyHitCounter = node["lowAccuracyHitCounter"].as<int>(lowAccuracyHitCounter);
		shotsFiredCounter = node["shotsFiredCounter"].as<int>(shotsFiredCounter);
		shotsLandedCounter = node["shotsLandedCounter"].as<int>(shotsLandedCounter);
	}
	// Save function
	YAML::Node save() const
	{
		YAML::Node node;
		node["wasUnconcious"] = wasUnconcious;
		if (!kills.empty())
		{
			for (std::vector<SoldierDiaryKills*>::const_iterator i = kills.begin() ; i != kills.end() ; ++i)
				node["kills"].push_back((*i)->save());
		}
        node["shotAtCounter"] = shotAtCounter;
		node["hitCounter"] = hitCounter;
		node["shotByFriendlyCounter"] = shotByFriendlyCounter;
		node["shotFriendlyCounter"] = shotFriendlyCounter;
		node["loneSurvivor"] = loneSurvivor;
		node["ironMan"] = ironMan;
		node["longDistanceHitCounter"] = longDistanceHitCounter;
		node["lowAccuracyHitCounter"] = lowAccuracyHitCounter;
		node["shotsFiredCounter"] = shotsFiredCounter;
		node["shotsLandedCounter"] = shotsLandedCounter;
		return node;
	}
	Statistics(const YAML::Node& node) { load(node); }	// Constructor from YAML (needed?)
	Statistics() :	wasUnconcious(false), kills(), shotAtCounter(0), hitCounter(0), shotByFriendlyCounter(0), shotFriendlyCounter(0), loneSurvivor(false),
					ironMan(false), longDistanceHitCounter(0), lowAccuracyHitCounter(0) { }	// Default constructor
	~Statistics() {for (std::vector<SoldierDiaryKills*>::iterator i = kills.begin(); i != kills.end(); ++i) delete*i;} // Deconstructor
};

/**
 * The entry is like the title. Details from each mission.
 */
class SoldierDiaryEntries
{
private:
	GameTime _missionTime;
	std::string _missionRegion, _missionCountry, _missionType, _missionUFO;	
	bool _success;
	std::string _rating;
	int _score;
	std::string _alienRace;
	int _missionDaylight;
	int _daysWounded;
	Statistics *_missionStatistics;
public:
	/// Creates a new diary and loads its contents from YAML.
	SoldierDiaryEntries(const YAML::Node& node);
	/// Creates a diary.
	SoldierDiaryEntries(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded, Statistics *missionStatistics);
	/// Cleans up the diary.
	~SoldierDiaryEntries();
	/// Loads the diary information from YAML.
	void load(const YAML::Node& node);
	/// Saves the diary information to YAML.
	YAML::Node save() const;
	/// Get
	const GameTime *getMissionTime() const;
	/// Get
	std::string getMissionRegion() const;
	/// Get
	std::string getMissionCountry() const;
	/// Get
	std::string getMissionType();
	/// Get
	std::string getMissionUFO() const;
	/// Get
	std::vector<SoldierDiaryKills*> getMissionKills() const;
	/// Get
	std::vector<SoldierDiaryKills*> *getMissionKillsReference();
	/// Get
	bool getMissionSuccess() const;
	/// Get
	std::string getMissionRating() const;
	/// Get
	int getMissionScore() const;
	/// Get
	int getMissionKillTotal() const;
	/// Get
	std::string getMissionRace() const;
	/// Get
	int getMissionDaylight() const;
	/// Get
	int getMissionStunTotal() const;
	/// Get
	int getDaysWounded() const;
	/// Get
	Statistics *getMissionStatistics() const;
};

/**
 * Each entry will be its own commendation.
 */
class SoldierCommendations
{
private:
	RuleCommendations *_rules;
	std::string  _commendationName, _commendationDescription, _noun;
	int  _decorationLevel, _sprite;
	bool _isNew;
public:
	/// Creates a new commendation and loads its contents from YAML.
	SoldierCommendations(const YAML::Node& node);
	/// Creates a commendation of the specified type.
	SoldierCommendations(std::string commendationName, std::string commendationDescription, std::string noun, int decorationLevel, bool isNew, int sprite);
	/// Cleans up the commendation.
	~SoldierCommendations();
	/// Loads the commendation information from YAML.
	void load(const YAML::Node& node);
	/// Saves the commendation information to YAML.
	YAML::Node save() const;
	/// Get commendation name.
	std::string getCommendationName() const;
	/// Get commendation description.
	std::string getCommendationDescription() const;
	/// Get commendation noun.
	std::string getNoun() const;
	/// Get the commendation's decoration level's name.
	std::string getDecorationLevelName();
	/// Get the commendation's decoration description.
	std::string getDecorationDescription();
	/// Get the commendation's decoration sprite.
	int getDecorationSprite() const;
	/// Get the commendation's decoration level's int.
	int getDecorationLevelInt();
	/// Get the newness of the commendation.
	bool isNew();
	/// Set the commendation newness to false.
	void makeOld();
	/// Increment decoration level.
	// Sets _isNew to true.
	void addDecoration();
	/// Get sprite
	int getSprite() const;
};

class SoldierDiary
{
private:
	std::vector<SoldierDiaryEntries*> _diaryEntries;
	std::vector<SoldierCommendations*> _commendations;
	RuleCommendations *_rules;
	std::vector<SoldierDiaryKills*> _killList;
	std::map<std::string, int> _alienRankTotal, _alienRaceTotal, _weaponTotal, _weaponAmmoTotal, _regionTotal, _countryTotal, _typeTotal, _UFOTotal;
	int _scoreTotal, _killTotal, _missionTotal, _winTotal, _stunTotal, _daysWoundedTotal, _baseDefenseMissionTotal, _totalShotByFriendlyCounter, _totalShotFriendlyCounter, _loneSurvivorTotal,
		_terrorMissionTotal, _nightMissionTotal, _nightTerrorMissionTotal, _monthsService, _unconciousTotal, _shotAtCounterTotal, _hitCounterTotal, _ironManTotal,
		_importantMissionTotal, _longDistanceHitCounterTotal, _lowAccuracyHitCounterTotal, _shotsFiredCounterTotal, _shotsLandedCounterTotal;

	void manageModularCommendations(std::map<std::string, int> nextCommendationLevel, std::map<std::string, int> modularCommendations, std::pair<std::string, int> statTotal, int criteria);
public:
	/// Creates a new soldier-equipment layout item and loads its contents from YAML.
	SoldierDiary(const YAML::Node& node);
	/// Construct a diary.
	SoldierDiary();
	/// Deconstruct a diary.
	~SoldierDiary();
	/// Load a diary.
	void load(const YAML::Node& node);
	/// Save a diary.
	YAML::Node save() const;
	/// Get the diary entries
	std::vector<SoldierDiaryEntries*> getSoldierDiaryEntries();
	/// Add an entry to the diary
	void addSoldierDiaryEntry(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded, Statistics *missionStatistics);
	/// Update the diary statistics
	void updateDiary();
	/// Get
	std::map<std::string, int> getAlienRankTotal() const;
	/// Get
	std::map<std::string, int> getAlienRaceTotal() const;
	/// Get
	std::map<std::string, int> getWeaponTotal() const;
	/// Get
	std::map<std::string, int> getWeaponAmmoTotal() const;
	/// Get
	std::map<std::string, int> getRegionTotal() const;
	/// Get
	std::map<std::string, int> getCountryTotal() const;
	/// Get
	std::map<std::string, int> getTypeTotal() const;
	/// Get
	std::map<std::string, int> getUFOTotal() const;
	/// Get
	int getScoreTotal() const;
	/// Get
	int getKillTotal() const;
	/// Get
	int getMissionTotal() const;
	/// Get
	int getWinTotal() const;
	/// Get
	int getStunTotal() const;
	/// Get
	int getDaysWoundedTotal() const;
	/// Get total base defense missions.
	int getBaseDefenseMissionTotal() const;
	/// Get total terror missions.
	int getTerrorMissionTotal() const;
	/// Get total night missions.
	int getNightMissionTotal() const;
	/// Get total night terror missions.
	int getNightTerrorMissionTotal() const;
	/// Get commendations
	std::vector<SoldierCommendations*> *getSoldierCommendations();
	/// Manage commendations, return true if a medal is awarded.
	bool manageCommendations(Ruleset *rules);
	/// Award commendations
	void awardCommendation(std::string commendationName, std::string commendationDescription, std::string noun = "", int sprite = 100);
	/// Increment soldier's service time.
	void addMonthlyService();
};

}

#endif
