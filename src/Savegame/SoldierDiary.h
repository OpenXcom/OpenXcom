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
#include <yaml-cpp/yaml.h>
#include "BattleUnit.h"
#include "SavedGame.h"
#include "../Mod/Mod.h"

namespace OpenXcom
{

class Mod;
struct BattleUnitKills;

/**
 * Each entry will be its own commendation.
 */
class SoldierCommendations
{
private:
	std::string  _type, _noun;
	int  _decorationLevel;
	bool _isNew;
public:
	/// Creates a new commendation and loads its contents from YAML.
	SoldierCommendations(const YAML::Node& node);
	/// Creates a commendation of the specified type.
	SoldierCommendations(std::string commendationName, std::string noun = "noNoun");
	/// Cleans up the commendation.
	~SoldierCommendations();
	/// Loads the commendation information from YAML.
	void load(const YAML::Node& node);
	/// Saves the commendation information to YAML.
	YAML::Node save() const;
	/// Get commendation name.
	std::string getType() const;
	/// Get commendation noun.
	std::string getNoun() const;
	/// Get the commendation's decoration level's name.
	std::string getDecorationLevelName(int skipCounter);
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
	std::vector<SoldierCommendations*> _commendations;
	std::vector<BattleUnitKills*> _killList;
    std::vector<int> _missionIdList;
	std::map<std::string, int> _regionTotal, _countryTotal, _typeTotal, _UFOTotal;
	int _scoreTotal, _killTotal, _winTotal, _stunTotal, _daysWoundedTotal, _baseDefenseMissionTotal, _totalShotByFriendlyCounter, _totalShotFriendlyCounter, _loneSurvivorTotal,
		_terrorMissionTotal, _nightMissionTotal, _nightTerrorMissionTotal, _monthsService, _unconciousTotal, _shotAtCounterTotal, _hitCounterTotal, _ironManTotal,
		_importantMissionTotal, _longDistanceHitCounterTotal, _lowAccuracyHitCounterTotal, _shotsFiredCounterTotal, _shotsLandedCounterTotal, _shotAtCounter10in1Mission,
		_hitCounter5in1Mission, _reactionFireTotal, _timesWoundedTotal, _valiantCruxTotal, _KIA, _trapKillTotal, _alienBaseAssaultTotal, _allAliensKilledTotal, _allAliensStunnedTotal,
        _woundsHealedTotal, _allUFOs, _allMissionTypes, _statGainTotal, _revivedUnitTotal, _wholeMedikitTotal, _braveryGainTotal, _bestOfRank, _MIA, _martyrKillsTotal, _postMortemKills,
        _slaveKillsTotal, _panickTotal, _controlTotal, _lootValueTotal;
    bool _bestSoldier, _globeTrotter;
	void manageModularCommendations(std::map<std::string, int> &nextCommendationLevel, std::map<std::string, int> &modularCommendations, std::pair<std::string, int> statTotal, int criteria);
	void awardCommendation(std::string type, std::string noun = "noNoun");
public:
	/// Creates a new soldier diary and loads its contents from YAML.
	SoldierDiary(const YAML::Node& node);
	/// Construct a diary.
	SoldierDiary();
	/// Deconstruct a diary.
	~SoldierDiary();
	/// Load a diary.
	void load(const YAML::Node& node);
	/// Save a diary.
	YAML::Node save() const;
	/// Update the diary statistics.
	void updateDiary(BattleUnitStatistics *unitStatistics, MissionStatistics *missionStatistics, Mod *rules);
	/// Get the list of kills, mapped by rank.
	std::map<std::string, int> getAlienRankTotal();
	/// Get the list of kills, mapped by race.
	std::map<std::string, int> getAlienRaceTotal();
	/// Get the list of kills, mapped by weapon used.
	std::map<std::string, int> getWeaponTotal();
	/// Get the list of kills, mapped by weapon ammo used.
	std::map<std::string, int> getWeaponAmmoTotal();
	/// Get the list of missions, mapped by region.
	std::map<std::string, int> &getRegionTotal();
	/// Get the list of missions, mapped by country.
	std::map<std::string, int> &getCountryTotal();
	/// Get the list of missions, mapped by type.
	std::map<std::string, int> &getTypeTotal();
	/// Get the list of missions, mapped by UFO.
	std::map<std::string, int> &getUFOTotal();
	/// Get the total score.
	int getScoreTotal() const;
	/// Get the total number of kills.
	int getKillTotal() const;
	/// Get the total number of missions.
	int getMissionTotal() const;
	/// Get the total number of wins.
	int getWinTotal() const;
	/// Get the total number of stuns.
	int getStunTotal() const;
    /// Get the total number of psi panicks.
    int getPanickTotal() const;
    /// Get the total number of psi mind controls.
    int getControlTotal() const;
	/// Get the total number of days wounded.
	int getDaysWoundedTotal() const;
	/// Get the solder's commendations.
	std::vector<SoldierCommendations*> *getSoldierCommendations();
	/// Manage commendations, return true if a medal is awarded.
	bool manageCommendations(Mod *rules);
	/// Increment the soldier's service time.
	void addMonthlyService();
	/// Get the total months in service.
	int getMonthsService() const;
    /// Get the mission id list.
    std::vector<int> &getMissionIdList();
    /// Get the kill list.
    std::vector<BattleUnitKills*> &getKills();
    /// Award special commendation to the original 8 soldiers.
    void awardOriginalEightCommendation();
    /// Award post-humous best-of rank commendation.
    void awardBestOfRank(SoldierRank rank);
    /// Award post-humous best overall commendation.
    void awardBestOverall();
    /// Award post-humous kills commendation.
    void awardPostMortemKill(int kills);
	/// Get the total number of shots fired.
	int getShotsFiredTotal() const;
	/// Get the total number of shots landed on target.
	int getShotsLandedTotal() const;
    /// Get the soldier's accuracy.
    int getAccuracy() const;
};

}
