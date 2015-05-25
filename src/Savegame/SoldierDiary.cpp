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
#include "SoldierDiary.h"
#include "GameTime.h"
#include "../Ruleset/RuleCommendations.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Game.h"
namespace OpenXcom
{
/**
 * Initializes a new diary entry from YAML.
 * @param node YAML node.
 */
SoldierDiary::SoldierDiary(const YAML::Node &node)
{
	load(node);
}
/**
 * Initializes a new blank diary.
 */
SoldierDiary::SoldierDiary() : _killList(), _regionTotal(), _countryTotal(), _typeTotal(), _UFOTotal(), _missionIdList(), _commendations(), _scoreTotal(0),
	_killTotal(0), _winTotal(0), _stunTotal(0), _baseDefenseMissionTotal(0), _daysWoundedTotal(0), _terrorMissionTotal(0), _nightMissionTotal(0),
	_nightTerrorMissionTotal(0), _monthsService(0), _unconciousTotal(0), _shotAtCounterTotal(0), _hitCounterTotal(0), _loneSurvivorTotal(0),
	_totalShotByFriendlyCounter(0), _totalShotFriendlyCounter(0), _ironManTotal(0), _importantMissionTotal(0), _longDistanceHitCounterTotal(0),
    _lowAccuracyHitCounterTotal(0), _shotsFiredCounterTotal(0), _shotsLandedCounterTotal(0), _shotAtCounter10in1Mission(0), _hitCounter5in1Mission(0),
	_reactionFireTotal(0), _timesWoundedTotal(0), _valiantCruxTotal(0), _KIA(0), _trapKillTotal(0), _alienBaseAssaultTotal(0), _allAliensKilledTotal(0), _allAliensStunnedTotal(0),
    _woundsHealedTotal(0), _allUFOs(0), _allMissionTypes(0), _statGainTotal(0), _revivedUnitTotal(0), _wholeMedikitTotal(0), _braveryGainTotal(0), _bestOfRank(0),
    _bestSoldier(false), _MIA(0)
{
}
/**
 *
 */
SoldierDiary::~SoldierDiary()
{
	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin(); i != _commendations.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<BattleUnitKills*>::iterator i = _killList.begin(); i != _killList.end(); ++i)
	{
		delete *i;
	}
}
/**
 * Loads the diary from a YAML file.
 * @param node YAML node.
 */
void SoldierDiary::load(const YAML::Node& node)
{
	if (const YAML::Node &commendations = node["commendations"])
	{
		for (YAML::const_iterator i = commendations.begin(); i != commendations.end(); ++i)
			_commendations.push_back(new SoldierCommendations(*i));
	}
	if (const YAML::Node &killList = node["killList"])
	{
		for (YAML::const_iterator i = killList.begin(); i != killList.end(); ++i)
			_killList.push_back(new BattleUnitKills(*i));
	}
    _missionIdList = node["missionIdList"].as<std::vector<int> >(_missionIdList);
	_regionTotal = node["regionTotal"].as<std::map<std::string, int> >(_regionTotal);
	_countryTotal = node["countryTotal"].as<std::map<std::string, int> >(_countryTotal);
	_typeTotal = node["typeTotal"].as<std::map<std::string, int> >(_typeTotal);
	_UFOTotal = node["UFOTotal"].as<std::map<std::string, int> >(_UFOTotal);
	_scoreTotal = node["scoreTotal"].as<int>(_scoreTotal);
	_killTotal = node["killTotal"].as<int>(_killTotal);
	_winTotal = node["winTotal"].as<int>(_winTotal);
	_stunTotal = node["stunTotal"].as<int>(_stunTotal);
	_daysWoundedTotal = node["daysWoundedTotal"].as<int>(_daysWoundedTotal);
	_baseDefenseMissionTotal = node["baseDefenseMissionTotal"].as<int>(_baseDefenseMissionTotal);
	_totalShotByFriendlyCounter = node["totalShotByFriendlyCounter"].as<int>(_totalShotByFriendlyCounter);
	_totalShotFriendlyCounter = node["totalShotFriendlyCounter"].as<int>(_totalShotFriendlyCounter);
	_loneSurvivorTotal = node["loneSurvivorTotal"].as<int>(_loneSurvivorTotal);
	_terrorMissionTotal = node["terrorMissionTotal"].as<int>(_terrorMissionTotal);
	_nightMissionTotal = node["nightMissionTotal"].as<int>(_nightMissionTotal);
	_nightTerrorMissionTotal = node["nightTerrorMissionTotal"].as<int>(_nightTerrorMissionTotal);
	_monthsService = node["monthsService"].as<int>(_monthsService);
    _unconciousTotal = node["unconciousTotal"].as<int>(_unconciousTotal);
    _shotAtCounterTotal = node["shotAtCounterTotal"].as<int>(_shotAtCounterTotal);
    _hitCounterTotal = node["hitCounterTotal"].as<int>(_hitCounterTotal);
    _ironManTotal = node["ironManTotal"].as<int>(_ironManTotal);
    _importantMissionTotal = node["importantMissionTotal"].as<int>(_importantMissionTotal);
	_longDistanceHitCounterTotal = node["longDistanceHitCounterTotal"].as<int>(_longDistanceHitCounterTotal);
	_lowAccuracyHitCounterTotal = node["lowAccuracyHitCounterTotal"].as<int>(_lowAccuracyHitCounterTotal);
	_shotsFiredCounterTotal = node["shotsFiredCounterTotal"].as<int>(_shotsFiredCounterTotal);
	_shotsLandedCounterTotal = node["shotsLandedCounterTotal"].as<int>(_shotsLandedCounterTotal);
	_shotAtCounter10in1Mission = node["shotAtCounter10in1Mission"].as<int>(_shotAtCounter10in1Mission);
	_hitCounter5in1Mission = node["hitCounter5in1Mission"].as<int>(_hitCounter5in1Mission);
	_reactionFireTotal = node["reactionFireTotal"].as<int>(_reactionFireTotal);
    _timesWoundedTotal = node["timesWoundedTotal"].as<int>(_timesWoundedTotal);
	_valiantCruxTotal = node["valiantCruxTotal"].as<int>(_valiantCruxTotal);
	_trapKillTotal = node["trapKillTotal"].as<int>(_trapKillTotal);
	_alienBaseAssaultTotal = node["alienBaseAssaultTotal"].as<int>(_alienBaseAssaultTotal);
	_allAliensKilledTotal = node["allAliensKilledTotal"].as<int>(_allAliensKilledTotal);
    _allAliensStunnedTotal = node["_allAliensStunnedTotal"].as<int>(_allAliensStunnedTotal);
    _woundsHealedTotal = node["_woundsHealedTotal"].as<int>(_woundsHealedTotal);
	_allUFOs = node["_allUFOs"].as<int>(_allUFOs);
	_allMissionTypes = node["_allMissionTypes"].as<int>(_allMissionTypes);
	_statGainTotal = node["_statGainTotal"].as<int>(_statGainTotal);
    _revivedUnitTotal = node["_revivedUnitTotal"].as<int>(_revivedUnitTotal);
    _wholeMedikitTotal = node["_wholeMedikitTotal"].as<int>(_wholeMedikitTotal);
    _braveryGainTotal = node["_braveryGainTotal"].as<int>(_braveryGainTotal);
    _bestOfRank = node["_bestOfRank"].as<int>(_bestOfRank);
    _bestSoldier = node["_bestSoldier"].as<bool>(_bestSoldier);
}
/**
 * Saves the diary to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierDiary::save() const
{
	YAML::Node node;
	for (std::vector<SoldierCommendations*>::const_iterator i = _commendations.begin(); i != _commendations.end(); ++i)
			node["commendations"].push_back((*i)->save());
	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
			node["killList"].push_back((*i)->save());
    if (!_missionIdList.empty()) node["missionIdList"] = _missionIdList;
    if (!_regionTotal.empty()) node["regionTotal"] = _regionTotal;
    if (!_countryTotal.empty()) node["countryTotal"] = _countryTotal;
    if (!_typeTotal.empty()) node["typeTotal"] = _typeTotal;
    if (!_UFOTotal.empty()) node["UFOTotal"] = _UFOTotal;
    if (_scoreTotal) node["scoreTotal"] = _scoreTotal;
    if (_killTotal) node["killTotal"] = _killTotal;
    if (_winTotal) node["winTotal"] = _winTotal;
    if (_stunTotal) node["stunTotal"] = _stunTotal;
    if (_daysWoundedTotal) node["daysWoundedTotal"] = _daysWoundedTotal;
    if (_baseDefenseMissionTotal) node["baseDefenseMissionTotal"] = _baseDefenseMissionTotal;
    if (_totalShotByFriendlyCounter) node["totalShotByFriendlyCounter"] = _totalShotByFriendlyCounter;
	if (_totalShotFriendlyCounter) node["totalShotFriendlyCounter"] = _totalShotFriendlyCounter;
    if (_loneSurvivorTotal) node["loneSurvivorTotal"] = _loneSurvivorTotal;
    if (_terrorMissionTotal) node["terrorMissionTotal"] = _terrorMissionTotal;
    if (_nightMissionTotal) node["nightMissionTotal"] = _nightMissionTotal;
    if (_nightTerrorMissionTotal) node["nightTerrorMissionTotal"] = _nightTerrorMissionTotal;
    if (_monthsService) node["monthsService"] = _monthsService;
    if (_unconciousTotal) node["unconciousTotal"] = _unconciousTotal;
    if (_shotAtCounterTotal) node["shotAtCounterTotal"] = _shotAtCounterTotal;
    if (_hitCounterTotal) node["hitCounterTotal"] = _hitCounterTotal;
    if (_ironManTotal) node["ironManTotal"] = _ironManTotal;
    if (_importantMissionTotal) node["importantMissionTotal"] = _importantMissionTotal;
	if (_longDistanceHitCounterTotal) node["longDistanceHitCounterTotal"] = _longDistanceHitCounterTotal;
	if (_lowAccuracyHitCounterTotal) node["lowAccuracyHitCounterTotal"] = _lowAccuracyHitCounterTotal;
	if (_shotsFiredCounterTotal) node["shotsFiredCounterTotal"] = _shotsFiredCounterTotal;
	if (_shotsLandedCounterTotal) node["shotsLandedCounterTotal"] = _shotsLandedCounterTotal;
	if (_shotAtCounter10in1Mission) node["shotAtCounter10in1Mission"] = _shotAtCounter10in1Mission;
	if (_hitCounter5in1Mission) node["hitCounter5in1Mission"] = _hitCounter5in1Mission;
	if (_reactionFireTotal) node["reactionFireTotal"] = _reactionFireTotal;
    if (_timesWoundedTotal) node["timesWoundedTotal"] = _timesWoundedTotal;
	if (_valiantCruxTotal) node["valiantCruxTotal"] = _valiantCruxTotal;
	if (_trapKillTotal) node["trapKillTotal"] = _trapKillTotal;
	if (_alienBaseAssaultTotal) node["alienBaseAssaultTotal"] = _alienBaseAssaultTotal;
	if (_allAliensKilledTotal) node["allAliensKilledTotal"] = _allAliensKilledTotal;
    if (_allAliensStunnedTotal) node["_allAliensStunnedTotal"] = _allAliensStunnedTotal;
    if (_woundsHealedTotal) node["_woundsHealedTotal"] = _woundsHealedTotal;
	if (_allUFOs) node["_allUFOs"] = _allUFOs;
	if (_allMissionTypes) node["_allMissionTypes"] = _allMissionTypes;
	if (_statGainTotal) node["_statGainTotal"] =_statGainTotal;
    if (_revivedUnitTotal) node["_revivedUnitTotal"] = _revivedUnitTotal;
    if(_wholeMedikitTotal) node["_wholeMedikitTotal"] = _wholeMedikitTotal;
    if(_braveryGainTotal) node["_braveryGainTotal"] = _braveryGainTotal;
    if (_bestOfRank) node["_bestOfRank"] = _bestOfRank;
    if (_bestSoldier) node["_bestSoldier"] = _bestSoldier;
	return node;
}
/**
 * Update soldier diary statistics.
 * @param unitStatistics BattleUnitStatistics to get stats from.
 * @param missionStatistics MissionStatistics to get stats from.
 */
void SoldierDiary::updateDiary(BattleUnitStatistics *unitStatistics, MissionStatistics *missionStatistics, Ruleset *rules)
{
	std::vector<BattleUnitKills*> unitKills = unitStatistics->kills;
	for (std::vector<BattleUnitKills*>::const_iterator kill = unitKills.begin() ; kill != unitKills.end() ; ++kill)
    {
		(*kill)->makeTurnUnique();
        _killList.push_back(*kill);
        if ((*kill)->getUnitFactionString() == "FACTION_HOSTILE")
        {
            if ((*kill)->getUnitStatusString() == "STATUS_DEAD")
            {
                _killTotal++;
            }
            else if ((*kill)->getUnitStatusString() == "STATUS_UNCONSCIOUS")
            {
                _stunTotal++;
            }		
            if ((*kill)->hostileTurn())
            {
                if (rules->getItem((*kill)->weapon)->getBattleType() == BT_GRENADE || rules->getItem((*kill)->weapon)->getBattleType() == BT_PROXIMITYGRENADE)
                {
                    _trapKillTotal++;
                }
                else
                {
                    _reactionFireTotal++;
                }
            }
        }
    }
    _regionTotal[missionStatistics->region.c_str()]++;
    _countryTotal[missionStatistics->country.c_str()]++;
    _typeTotal[missionStatistics->getMissionTypeLowerCase().c_str()]++;
    _UFOTotal[missionStatistics->ufo.c_str()]++;
    _scoreTotal += missionStatistics->score;
    if (missionStatistics->success)
    {
        _winTotal++;
        if (missionStatistics->type != "STR_SMALL_SCOUT" && missionStatistics->type != "STR_MEDIUM_SCOUT" && missionStatistics->type != "STR_LARGE_SCOUT" && missionStatistics->type != "STR_SUPPLY_SHIP")
            _importantMissionTotal++;
		if (missionStatistics->type == "STR_ALIEN_BASE_ASSAULT")
			_alienBaseAssaultTotal++;
        if (unitStatistics->loneSurvivor)
            _loneSurvivorTotal++;
        if (unitStatistics->ironMan)
            _ironManTotal++;
        if (unitStatistics->nikeCross)
            _allAliensKilledTotal++;
        if (unitStatistics->mercyCross)
            _allAliensStunnedTotal++;
    }
    _daysWoundedTotal += unitStatistics->daysWounded;
    if (unitStatistics->daysWounded)
        _timesWoundedTotal++;
    if (missionStatistics->type == "STR_BASE_DEFENSE")
        _baseDefenseMissionTotal++;
    else if (missionStatistics->type == "STR_TERROR_MISSION")
    {
        _terrorMissionTotal++;
        if (missionStatistics->daylight > 5)
            _nightTerrorMissionTotal++;
    }
    if (missionStatistics->daylight != 0)
        _nightMissionTotal++;
    if (unitStatistics->wasUnconcious)
        _unconciousTotal++;
	_shotAtCounterTotal += unitStatistics->shotAtCounter;
    _shotAtCounter10in1Mission += (unitStatistics->shotAtCounter)/10;
	_hitCounterTotal += unitStatistics->hitCounter;
    _hitCounter5in1Mission += (unitStatistics->hitCounter)/5;
	_totalShotByFriendlyCounter += unitStatistics->shotByFriendlyCounter;
	_totalShotFriendlyCounter += unitStatistics->shotFriendlyCounter;
	_longDistanceHitCounterTotal += unitStatistics->longDistanceHitCounter;
	_lowAccuracyHitCounterTotal += unitStatistics->lowAccuracyHitCounter;
	if (missionStatistics->valiantCrux)
		_valiantCruxTotal++;
	if (unitStatistics->KIA)
		_KIA++;
    if (unitStatistics->MIA)
        _MIA++;
	_woundsHealedTotal = unitStatistics->woundsHealed++;
	if (_UFOTotal.size() >= rules->getUfosList().size())
		_allUFOs = 1;
	if ((_UFOTotal.size() + _typeTotal.size()) == (rules->getUfosList().size() + rules->getDeploymentsList().size() - 2))
		_allMissionTypes = 1;

	// Stat change long hand calculation
	_statGainTotal = unitStatistics->delta.tu;
	_statGainTotal = unitStatistics->delta.stamina;
	_statGainTotal = unitStatistics->delta.health;
	_statGainTotal = unitStatistics->delta.bravery / 10; // Normalize
	_statGainTotal = unitStatistics->delta.reactions;
	_statGainTotal = unitStatistics->delta.firing;
	_statGainTotal = unitStatistics->delta.throwing;
	_statGainTotal = unitStatistics->delta.strength;
	_statGainTotal = unitStatistics->delta.psiStrength;
	_statGainTotal = unitStatistics->delta.melee;
	_statGainTotal = unitStatistics->delta.psiSkill;
    
    _braveryGainTotal = unitStatistics->delta.bravery;
    _revivedUnitTotal += unitStatistics->revivedSoldier;
    _wholeMedikitTotal += std::min( std::min(unitStatistics->woundsHealed, unitStatistics->appliedStimulant), unitStatistics->appliedPainKill);
    _missionIdList.push_back(missionStatistics->id);
}
/**
 * Get soldier commendations.
 * @return SoldierCommendations list of soldier's commendations.
 */
std::vector<SoldierCommendations*> *SoldierDiary::getSoldierCommendations()
{
	return &_commendations;
}
/**
 * Manage the soldier's commendations.
 * Award new ones, if deserved.
 * @return bool Has a commendation been awarded?
 */
bool SoldierDiary::manageCommendations(Ruleset *rules)
{
	std::map<std::string, RuleCommendations *> commendationsList = rules->getCommendation();
	bool awardedCommendation = false;                   // This value is returned if at least one commendation was given.
    std::map<std::string, int> nextCommendationLevel;   // Noun, threshold.
    std::vector<std::string> modularCommendations;      // Commendation name.
	bool awardCommendationBool = false;                 // This value determines if a commendation will be given.
	// Loop over all possible commendations
	for (std::map<std::string, RuleCommendations *>::iterator i = commendationsList.begin(); i != commendationsList.end(); )
	{	
		awardCommendationBool = true;
        nextCommendationLevel.clear();
        nextCommendationLevel["noNoun"] = 0;
        modularCommendations.clear();
        // Loop over all the soldier's commendations, see if we already have the commendation.
        // If so, get the level and noun.
        for (std::vector<SoldierCommendations*>::const_iterator j = _commendations.begin(); j != _commendations.end(); ++j)
        {
            if ( (*i).first == (*j)->getType() )
            {
                nextCommendationLevel[(*j)->getNoun()] = (*j)->getDecorationLevelInt() + 1;
            }
        }
		// Go through each possible criteria. Assume the medal is awarded, set to false if not.
		// As soon as we find a medal criteria that we FAIL TO achieve, then we are not awarded a medal.
		for (std::map<std::string, std::vector<int> >::const_iterator j = (*i).second->getCriteria()->begin(); j != (*i).second->getCriteria()->end(); ++j)
		{
			// Skip this medal if we have reached its max award level.
			if (nextCommendationLevel["noNoun"] >= (*j).second.size())
			{
				awardCommendationBool = false;
				break;
			}
            // These criteria have no nouns, so only the nextCommendationLevel["noNoun"] will ever be used.
			else if( nextCommendationLevel.count("noNoun") == 1 &&
					((*j).first == "totalKills" && _killList.size() < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalMissions" && _missionIdList.size() < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWins" && _winTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalScore" && _scoreTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalStuns" && _stunTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalDaysWounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalBaseDefenseMissions" && _baseDefenseMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalTerrorMissions" && _terrorMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalNightMissions" && _nightMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalNightTerrorMissions" && _nightTerrorMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalMonthlyService" && _monthsService < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalFellUnconcious" && _unconciousTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
                    ((*j).first == "totalShotAt10Times" && _shotAtCounter10in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "totalHit5Times" && _hitCounter5in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalFriendlyFired" && (_totalShotByFriendlyCounter < (*j).second.at(nextCommendationLevel["noNoun"]) || _KIA || _MIA)) ||
					((*j).first == "total_lone_survivor" && _loneSurvivorTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalIronMan" && _ironManTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalImportantMissions" && _importantMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "totalLongDistanceHits" && _longDistanceHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "totalLowAccuracyHits" && _lowAccuracyHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalReactionFire" && _reactionFireTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
                    ((*j).first == "totalTimesWounded" && _timesWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
                    ((*j).first == "totalDaysWounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalValientCrux" && _valiantCruxTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "isDead" && _KIA < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalTrapKills" && _trapKillTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAlienBaseAssaults" && _alienBaseAssaultTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllAliensKilled" && _allAliensKilledTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
                    ((*j).first == "totalAllAliensStunned" && _allAliensStunnedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWoundsHealed" && _woundsHealedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllUFOs" && _allUFOs < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalAllMissionTypes" && _allMissionTypes < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalStatGain" && _statGainTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalRevives" && _revivedUnitTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalWholeMedikit" && _wholeMedikitTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "totalBraveryGain" && _braveryGainTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "bestOfRank" && _bestOfRank < (*j).second.at(nextCommendationLevel["noNoun"])) ||
                    ((*j).first == "bestSoldier" && (int)_bestSoldier < (*j).second.at(nextCommendationLevel["noNoun"])) || 
                    ((*j).first == "isMIA" && _MIA < (*j).second.at(nextCommendationLevel["noNoun"])) )
			{
				awardCommendationBool = false;
				break;
			}
			// Medals with the following criteria are unique because they need a noun.
            // And because they loop over a map<> (this allows for maximum moddability).
			else if ((*j).first == "totalKillsWithAWeapon" || (*j).first == "totalMissionsInARegion" || (*j).first == "totalKillsByRace" || (*j).first == "totalKillsByRank")
			{
				std::map<std::string, int> tempTotal;
				if ((*j).first == "totalKillsWithAWeapon")
					tempTotal = getWeaponTotal();
				else if ((*j).first == "totalMissionsInARegion")
					tempTotal = _regionTotal;
				else if ((*j).first == "totalKillsByRace")
					tempTotal = getAlienRaceTotal();
				else if ((*j).first == "totalKillsByRank")
					tempTotal = getAlienRankTotal();
				// Loop over the temporary map.
				// Match nouns and decoration levels.
				for(std::map<std::string, int>::const_iterator k = tempTotal.begin(); k != tempTotal.end(); ++k)
				{
					int criteria = -1;
                    std::string noun = (*k).first;
					// If there is no matching noun, get the first award criteria.
                    if (nextCommendationLevel.count(noun) == 0)
						criteria = (*j).second.front(); 
					// Otherwise, get the criteria that reflects the soldier's commendation level.
                    else if (nextCommendationLevel[noun] != (*j).second.size())
						criteria = (*j).second.at(nextCommendationLevel[noun]); 

                    // If a criteria was set AND the stat's count exceeds the criteria.
                    if (criteria != -1 && (*k).second >= criteria)
                    {
                        modularCommendations.push_back(noun);
                    }
				}
				// If it is still empty, we did not get a commendation.
				if (modularCommendations.empty())
				{
					awardCommendationBool = false;
					break;
				}
                else
                {
                    awardCommendationBool = true;
                }
			}
            else if ((*j).first == "killsWithCriteriaCareer" || (*j).first == "killsWithCriteriaMission" || (*j).first == "killsWithCriteriaTurn")
            {
                // Fetch the kill criteria list.
                std::vector<std::map<int, std::vector<std::string> > > *_killCriteriaList = (*i).second->getKillCriteria();
                
                // Loop over the OR vectors.
                for (std::vector<std::map<int, std::vector<std::string> > >::const_iterator orCriteria = _killCriteriaList->begin(); orCriteria != _killCriteriaList->end(); ++orCriteria)
                {
                    // Loop over the AND vectors.
                    for (std::map<int, std::vector<std::string> >::const_iterator andCriteria = orCriteria->begin(); andCriteria != orCriteria->end(); ++andCriteria)
                    {
                        int count = 0; // How many AND vectors (list of DETAILs) have been successful.
						if ((*j).first == "killsWithCriteriaTurn" || (*j).first == "killsWithCriteriaMission")
							count++; // Turns and missions start at 1 because of how thisTime and lastTime work.
                        int thisTime = -1; // Time being a turn or a mission.
                        int lastTime = -1;
                        bool goToNextTime = false;
                        // Loop over the KILLS.
                        for (std::vector<BattleUnitKills*>::const_iterator singleKill = _killList.begin(); singleKill != _killList.end(); ++singleKill)
                        {
                            if ((*j).first == "killsWithCriteriaMission")
                            {
                                thisTime = (*singleKill)->mission;
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->mission;
                                    ++singleKill;
                                }
                            }
                            else if ((*j).first == "killsWithCriteriaTurn")
                            {
                                thisTime = (*singleKill)->turn;
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->turn;
                                    ++singleKill;
                                }
                            }
                            // Skip kill-groups that we already got an award for.
                            // Skip kills that are inbetween turns.
                            if ( thisTime == lastTime && goToNextTime && (*j).first != "killsWithCriteriaCareer")
                            {
                                continue;
                            }
                            else if (thisTime != lastTime) 
                            {
                                count = 1; // Reset.
                                goToNextTime = false;
                                continue;
                            }
                            bool foundMatch = true;
							
                            // Loop over the DETAILs of the AND vector.
                            for (std::vector<std::string>::const_iterator detail = andCriteria->second.begin(); detail != andCriteria->second.end(); ++detail)
                            {
								std::string battleTypeArray[] = { "BT_NONE", "BT_FIREARM", "BT_AMMO", "BT_MELEE", "BT_GRENADE",
									"BT_PROXIMITYGRENADE", "BT_MEDIKIT", "BT_SCANNER", "BT_MINDPROBE", "BT_PSIAMP", "BT_FLARE", "BT_CORPSE", "BT_END" };
								std::vector<std::string> battleTypeVector(battleTypeArray, battleTypeArray + 13);
								int battleType = 0;
								for (; battleType != battleTypeVector.size(); ++battleType)
								{
									if ((*detail) == battleTypeVector[battleType])
									{
										break;
									}
								}

								std::string damageTypeArray[] = { "DT_NONE", "DT_AP", "DT_IN", "DT_HE", "DT_LASER", "DT_PLASMA", 
									"DT_STUN", "DT_MELEE", "DT_ACID", "DT_SMOKE", "DT_END"};
								std::vector<std::string> damageTypeVector(damageTypeArray, damageTypeArray + 11);
								int damageType = 0;
								for (; damageType != damageTypeVector.size(); ++damageType)
								{
									if ((*detail) == damageTypeVector[damageType])
									{
										break;
									}
								}

                                // See if we find no matches with any criteria. If so, break and try the next kill.
								if ( (*singleKill)->weapon == "STR_WEAPON_UNKNOWN" || (*singleKill)->weaponAmmo == "STR_WEAPON_UNKNOWN" ||
									((*singleKill)->rank != (*detail) && (*singleKill)->race != (*detail) &&
									 (*singleKill)->weapon != (*detail) && (*singleKill)->weaponAmmo != (*detail) &&
									 (*singleKill)->getUnitStatusString() != (*detail) && (*singleKill)->getUnitFactionString() != (*detail)  &&
									 rules->getItem((*singleKill)->weaponAmmo)->getDamageType() != damageType && 
									 rules->getItem((*singleKill)->weapon)->getBattleType() != battleType &&
                                     (*singleKill)->getUnitSideString() != (*detail) && (*singleKill)->getUnitBodyPartString() != (*detail)) )
                                {
                                    foundMatch = false;
                                    break;
                                }
                            }
                            if (foundMatch) 
                            {
                                count++;
                                if ( count == (*andCriteria).first) 
                                    goToNextTime = true; // Criteria met, move to next mission/turn.
                            }
                        }
                        int multiCriteria = (*andCriteria).first;
                        // If one of the AND criteria fail, stop looking.
                        if (multiCriteria == 0 || count / multiCriteria < (*j).second.at(nextCommendationLevel["noNoun"]))
                        {
                            awardCommendationBool = false;
                            break;
                        }
                        else
                        {
                            awardCommendationBool = true;
                        }
                    }
                    if (awardCommendationBool) 
                        break; // Stop looking because we are getting one regardless.
                }
            }
        }
		bool awardedModularCommendation = false;
		if (awardCommendationBool)
		{
            // If we do not have modular medals, but are awarded a different medal,
            // its noun will be "noNoun".
            if (modularCommendations.empty())
            {
                modularCommendations.push_back("noNoun");
            }
			else
			{
				awardedModularCommendation = true;
			}
            for (std::vector<std::string>::const_iterator j = modularCommendations.begin(); j != modularCommendations.end(); ++j)
            {
				bool newCommendation = true;
				for (std::vector<SoldierCommendations*>::iterator k = _commendations.begin() ; k != _commendations.end() ; ++k)
				{
					if ( (*k)->getType() == (*i).first && (*k)->getNoun() == (*j))
					{
						(*k)->addDecoration();
						newCommendation = false;
						break;
					}
				}
				if (newCommendation)
				{
					_commendations.push_back(new SoldierCommendations((*i).first, (*j)));
				}
            }
			awardedCommendation = true;
		}
		else
		{
			++i;
		}
	}
	return awardedCommendation;
}
/**
 * Manage modular commendations (private)
 * @param nextCommendationLevel map<string, int> 
 * @param modularCommendations map<string, int>
 * @param statTotal pair<string, int>
 * @param criteria int
 */
void SoldierDiary::manageModularCommendations(std::map<std::string, int> &nextCommendationLevel, std::map<std::string, int> &modularCommendations, std::pair<std::string, int> statTotal, int criteria)
{
	// If criteria is 0, we don't have this noun OR if we meet the criteria, remember the noun for award purposes
	if ((modularCommendations.count(statTotal.first) == 0 && statTotal.second >= criteria) || (modularCommendations.count(statTotal.first) != 0 && nextCommendationLevel.at(statTotal.first) >= criteria))
	{
		modularCommendations[statTotal.first]++;
	}
}
/**
 * Award commendations to the soldier.
 * @param type string
 * @param noun string
 */
void SoldierDiary::awardCommendation(std::string type, std::string noun)
{
	bool newCommendation = true;
	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin() ; i != _commendations.end() ; ++i)
	{
		if ( (*i)->getType() == type && (*i)->getNoun() == noun)
		{
			(*i)->addDecoration();
			newCommendation = false;
			break;
		}
	}
	if (newCommendation)
	{
		_commendations.push_back(new SoldierCommendations(type, noun));
	}
}
/**
 * Get vector of mission ids.
 * @return Vector of mission ids.
 */
std::vector<int> &SoldierDiary::getMissionIdList()
{ 
    return _missionIdList;
}
/**
 * Get vector of kills.
 * @return vector of BattleUnitKills
 */
std::vector<BattleUnitKills*> &SoldierDiary::getKills()
{
    return _killList;
}
/**
 * Get list of kills sorted by rank
 * @return 
 */
std::map<std::string, int> SoldierDiary::getAlienRankTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->rank.c_str()]++;
	}
	return list;
}
/**
 *
 */
std::map<std::string, int> SoldierDiary::getAlienRaceTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->race.c_str()]++;
	}
	return list;
}
/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->weapon.c_str()]++;
	}
	return list;
}
/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponAmmoTotal()
{
	std::map<std::string, int> list;
	for(std::vector<BattleUnitKills*>::const_iterator kill = _killList.begin(); kill != _killList.end(); ++kill)
	{
		list[(*kill)->weaponAmmo.c_str()]++;
	}
	return list;
}
/**
 *
 */
std::map<std::string, int> &SoldierDiary::getRegionTotal()
{
	return _regionTotal;
}
/**
 *
 */
std::map<std::string, int> &SoldierDiary::getCountryTotal()
{
	return _countryTotal;
}
/**
 *
 */
std::map<std::string, int> &SoldierDiary::getTypeTotal()
{
	return _typeTotal;
}
/**
 *
 */
std::map<std::string, int> &SoldierDiary::getUFOTotal()
{
	return _UFOTotal;
}
/**
 *
 */
int SoldierDiary::getScoreTotal() const
{
	return _scoreTotal;
}
/**
 *
 */
int SoldierDiary::getKillTotal() const
{
	return _killTotal;
}
/**
 *
 */
int SoldierDiary::getMissionTotal() const
{
	return _missionIdList.size();
}
/**
 *
 */
int SoldierDiary::getWinTotal() const
{
	return _winTotal;
}
/**
 *
 */
int SoldierDiary::getStunTotal() const
{
	return _stunTotal;
}
/*
 *
 */
int SoldierDiary::getDaysWoundedTotal() const
{
	return _daysWoundedTotal;
}
/**
 * Increment soldier's service time one month
 */
void SoldierDiary::addMonthlyService()
{
	_monthsService++;
}
/**
 *  Award special commendation to the original 8 soldiers.
 */
void SoldierDiary::awardOriginalEightCommendation()
{
    _commendations.push_back(new SoldierCommendations("STR_MEDAL_ORIGINAL8_NAME", "NoNoun"));
}
/**
 *  Award post-humous best-of commendation.
 */
void SoldierDiary::awardBestOfRank(SoldierRank rank)
{
    _bestOfRank = (int)rank + 1;
}
/**
 *  Award post-humous best-of commendation.
 */
void SoldierDiary::awardBestOverall()
{
    _bestSoldier = true;
}

/**
 * Initializes a new commendation entry from YAML.
 * @param node YAML node.
 */
SoldierCommendations::SoldierCommendations(const YAML::Node &node)
{
	load(node);
}
/**
 * Initializes a soldier commendation.
 */
SoldierCommendations::SoldierCommendations(std::string commendationName, std::string noun) : _type(commendationName), _noun(noun), _decorationLevel(0), _isNew(true)
{
}
/**
 *
 */
SoldierCommendations::~SoldierCommendations()
{
}
/**
 * Loads the commendation from a YAML file.
 * @param node YAML node.
 */
void SoldierCommendations::load(const YAML::Node &node)
{
	_type = node["commendationName"].as<std::string>(_type);
	_noun = node["noun"].as<std::string>("noNoun");
	_decorationLevel = node["decorationLevel"].as<int>(_decorationLevel);
	_isNew = node["isNew"].as<bool>(false);
}
/**
 * Saves the commendation to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierCommendations::save() const
{
	YAML::Node node;
	node["commendationName"] = _type;
	if (_noun != "noNoun") node["noun"] = _noun;
	node["decorationLevel"] = _decorationLevel;
	return node;
}
/**
 * Get the soldier's commendation's name.
 * @return string Commendation name.
 */
std::string SoldierCommendations::getType() const
{
	return _type;
}
/**
 * Get the soldier's commendation's noun.
 * @return string Commendation noun
 */
std::string SoldierCommendations::getNoun() const
{
	return _noun;
}
/**
 * Get the soldier commendation level's name.
 * @return string Commendation level.
 */
std::string SoldierCommendations::getDecorationLevelName(int skipCounter)
{
	std::stringstream ss;
	ss << "STR_AWARD_" << _decorationLevel - skipCounter;
	return ss.str();
}
/**
 * Get the soldier commendation level's description.
 * @return string Commendation level description.
 */
std::string SoldierCommendations::getDecorationDescription()
{
	std::stringstream ss;
	ss << "STR_AWARD_DECOR_" << _decorationLevel;
	return ss.str();
}
/**
 * Get the soldier commendation level's int.
 * @return int Commendation level.
 */
int SoldierCommendations::getDecorationLevelInt()
{
	return _decorationLevel;
}
/**
 * Get newness of commendation.
 * @return bool Is the commendation new?
 */
bool SoldierCommendations::isNew()
{
	return _isNew;
}
/**
 * Set the newness of the commendation to old.
 */
void SoldierCommendations::makeOld()
{
	_isNew = false;
}
/**
 * Add a level of decoration to the commendation.
 * Sets isNew to true.
 */
void SoldierCommendations::addDecoration()
{
	_decorationLevel++;
	_isNew = true;
}
}
