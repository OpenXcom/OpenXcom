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

enum AlienState {KILLED, STUNNED, UNCONCIOUS};

class GameTime;
class RuleCommendations;
class Ruleset;

/**
 * Each entry will have a list of kills, detailed by this class.
 */
class SoldierDiaryKills
{
private:
	std::string _alienRank, _alienRace;
	std::string _weapon, _weaponAmmo;
	AlienState _alienState;
	
public:
	/// Creates a new diary and loads its contents from YAML.
	SoldierDiaryKills(const YAML::Node& node);
	/// Creates a diary.
	SoldierDiaryKills(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, AlienState alienState);
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
	AlienState getAlienState() const;

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

	std::vector<SoldierDiaryKills*> _missionKills;
public:
	/// Creates a new diary and loads its contents from YAML.
	SoldierDiaryEntries(const YAML::Node& node);
	/// Creates a diary.
	SoldierDiaryEntries(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded);
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
	bool getMissionSuccess() const;
	/// Get
	std::string getMissionRating() const;
	/// Get
	int getMissionScore() const;
	/// Add kill
	void addMissionKill(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, AlienState alienState);
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
};

/**
 * Each entry will be its own commendation.
 */
class SoldierCommendations
{
private:
	RuleCommendations *_rules;
	std::string  _commendationName, _commendationDescription;
	int  _decorationLevel;
	bool _isNew;
public:
	/// Creates a new commendation and loads its contents from YAML.
	SoldierCommendations(const YAML::Node& node);
	/// Creates a commendation of the specified type.
	SoldierCommendations(std::string commendationName, std::string commendationDescription, int decorationLevel, bool isNew);
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
	/// Get the commendation's decoration level's name.
	std::string getDecorationLevelName();
	/// Get the commendation's decoration description.
	std::string getDecorationDescription();
	/// Get the commendation's decoration level's int.
	int getDecorationLevelInt();
	/// Get the newness of the commendation.
	bool isNew();
	/// Set the commendation newness to false.
	void makeOld();
	/// Increment decoration level.
	// Sets _isNew to true.
	void addDecoration();
};

class SoldierDiary
{
private:
	std::vector<SoldierDiaryEntries*> _diaryEntries;
	std::vector<SoldierCommendations*> _commendations;
	RuleCommendations *_rules;
	std::map<std::string, int> _alienRankTotal, _alienRaceTotal, _weaponTotal, _weaponAmmoTotal, _regionTotal, _countryTotal, _typeTotal, _UFOTotal;
	int _scoreTotal, _killTotal, _missionTotal, _winTotal, _stunTotal, _daysWoundedTotal, _baseDefenseMissionTotal,
		_terrorMissionTotal, _nightMissionTotal, _nightTerrorMissionTotal, _monthsService;
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
	void addSoldierDiaryEntry(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded);
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
	void awardCommendation(std::string commendationName, std::string commendationDescription);
	/// Increment soldier's service time.
	void addMonthlyService();
};

}

#endif
