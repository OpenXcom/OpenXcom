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



namespace OpenXcom
{

enum AlienState {KILLED, STUNNED, UNCONCIOUS};

class GameTime;

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
 * The entry is like the title. Simle details like
 * mission region/type, total aliens, etc.
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
	// int _alienTotal;
	// int _daysWounded;
	// bool _wasMindControlled, _panicked, _fellUnconcious;

	std::vector<SoldierDiaryKills*> _missionKills;
public:
	/// Creates a new diary and loads its contents from YAML.
	SoldierDiaryEntries(const YAML::Node& node);
	/// Creates a diary.
	SoldierDiaryEntries(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int score, std::string rating, std::string alienRace, int missionDaylight);
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
};

class SoldierDiary
{
private:
	std::vector<SoldierDiaryEntries*> _diaryEntries;
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
	void addSoldierDiaryEntry(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int score, std::string rating, std::string alienRace, int missionDaylight);
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
};



}

#endif
