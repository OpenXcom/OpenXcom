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
	std::string getDecorationLevelName(int skipCounter) const;
	/// Get the commendation's decoration description.
	std::string getDecorationDescription() const;
	/// Get the commendation's decoration level's int.
	int getDecorationLevelInt() const;
	/// Get the newness of the commendation.
	bool isNew() const;
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
	int _daysWoundedTotal, _totalShotByFriendlyCounter, _totalShotFriendlyCounter, _loneSurvivorTotal, _monthsService, _unconciousTotal, _shotAtCounterTotal,
		_hitCounterTotal, _ironManTotal, _longDistanceHitCounterTotal, _lowAccuracyHitCounterTotal, _shotsFiredCounterTotal, _shotsLandedCounterTotal,
		_shotAtCounter10in1Mission,	_hitCounter5in1Mission, _timesWoundedTotal, _KIA, _allAliensKilledTotal, _allAliensStunnedTotal,
		_woundsHealedTotal, _allUFOs, _allMissionTypes, _statGainTotal, _revivedUnitTotal, _wholeMedikitTotal, _braveryGainTotal, _bestOfRank, _MIA,
		_martyrKillsTotal, _postMortemKills, _slaveKillsTotal, _bestSoldier, _revivedSoldierTotal, _revivedHostileTotal, _revivedNeutralTotal;
	bool _globeTrotter;
	void manageModularCommendations(std::map<std::string, int> &nextCommendationLevel, std::map<std::string, int> &modularCommendations, std::pair<std::string, int> statTotal, int criteria);
	void awardCommendation(const std::string& type, const std::string& noun = "noNoun");
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
	void updateDiary(BattleUnitStatistics*, std::vector<MissionStatistics*>*, Mod*);
	/// Get the list of kills, mapped by rank.
	std::map<std::string, int> getAlienRankTotal();
	/// Get the list of kills, mapped by race.
	std::map<std::string, int> getAlienRaceTotal();
	/// Get the list of kills, mapped by weapon used.
	std::map<std::string, int> getWeaponTotal();
	/// Get the list of kills, mapped by weapon ammo used.
	std::map<std::string, int> getWeaponAmmoTotal();
	/// Get the list of missions, mapped by region.
	std::map<std::string, int> getRegionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the list of missions, mapped by country.
	std::map<std::string, int> getCountryTotal(std::vector<MissionStatistics*>*) const;
	/// Get the list of missions, mapped by type.
	std::map<std::string, int> getTypeTotal(std::vector<MissionStatistics*>*) const;
	/// Get the list of missions, mapped by UFO.
	std::map<std::string, int> getUFOTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of kills.
	int getKillTotal() const;
	/// Get the total number of missions.
	int getMissionTotal() const;
	/// Get the total number of wins.
	int getWinTotal(std::vector<MissionStatistics*>*) const;
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
	bool manageCommendations(Mod*, std::vector<MissionStatistics*>*);
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
	void awardBestOfRank(int score);
	/// Award post-humous best overall commendation.
	void awardBestOverall(int score);
	/// Award post-humous kills commendation.
	void awardPostMortemKill(int kills);
	/// Get the total number of shots fired.
	int getShotsFiredTotal() const;
	/// Get the total number of shots landed on target.
	int getShotsLandedTotal() const;
	/// Get the soldier's accuracy.
	int getAccuracy() const;
	/// Get the total number of trap kills.
	int getTrapKillTotal(Mod*) const;
	/// Get the total number of reaction fire kills.
	int getReactionFireKillTotal(Mod*) const;
	/// Get the total number of terror missions.
	int getTerrorMissionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of night missions.
	int getNightMissionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of night terror missions.
	int getNightTerrorMissionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of base defense missions.
	int getBaseDefenseMissionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of alien base assaults.
	int getAlienBaseAssaultTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total number of important missions.
	int getImportantMissionTotal(std::vector<MissionStatistics*>*) const;
	/// Get the total score.
	int getScoreTotal(std::vector<MissionStatistics*>*) const;
	/// Get the Valiant Crux total.
	int getValiantCruxTotal(std::vector<MissionStatistics*>*) const;
	/// Get the loot value total.
	int getLootValueTotal(std::vector<MissionStatistics*>*) const;
};

}
