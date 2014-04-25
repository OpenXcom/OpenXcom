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
#include "BattleUnit.h"
#include "SavedGame.h"

namespace OpenXcom
{

class GameTime;
class RuleCommendations;
class Ruleset;

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
	std::vector<SoldierCommendations*> _commendations;
	RuleCommendations *_rules;
	std::vector<BattleUnitKills*> _killList;
    std::vector<int> _missionIdList;
    std::vector<std::pair<int ,int> > _daysWounded;
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
	/// Update the diary statistics
	void updateDiary(std::vector<BattleUnitKills*> unitKills, BattleUnitStatistics *unitStatistics, MissionStatistics *missionStatistics);
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
    /// Get mission Id list
    std::vector<int> getMissionIdList() const { return _missionIdList; }
};

}

#endif
