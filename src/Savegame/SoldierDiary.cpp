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
	_killTotal(0), _missionTotal(0), _winTotal(0), _stunTotal(0), _baseDefenseMissionTotal(0), _daysWoundedTotal(0), _terrorMissionTotal(0), _nightMissionTotal(0),
	_nightTerrorMissionTotal(0), _monthsService(0), _unconciousTotal(0), _shotAtCounterTotal(0), _hitCounterTotal(0), _loneSurvivorTotal(0),
	_totalShotByFriendlyCounter(0), _totalShotFriendlyCounter(0), _ironManTotal(0), _importantMissionTotal(0), _longDistanceHitCounterTotal(0),
    _lowAccuracyHitCounterTotal(0), _shotsFiredCounterTotal(0), _shotsLandedCounterTotal(0), _shotAtCounter10in1Mission(0), _hitCounter5in1Mission(0),
	_reactionFireTotal(0), _timesWoundedTotal(0), _valiantCruxTotal(0), _KIA(0)
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
	_missionTotal = node["missionTotal"].as<int>(_missionTotal);
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
    if (_missionTotal) node["missionTotal"] = _missionTotal;
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
	return node;
}
/**
 * Update soldier diary statistics.
 * @param unitStatistics BattleUnitStatistics to get stats from.
 * @param missionStatistics MissionStatistics to get stats from.
 */
void SoldierDiary::updateDiary(BattleUnitStatistics *unitStatistics, MissionStatistics *missionStatistics)
{
	std::vector<BattleUnitKills*> unitKills = unitStatistics->kills;
	for (std::vector<BattleUnitKills*>::const_iterator kill = unitKills.begin() ; kill != unitKills.end() ; ++kill)
    {
		(*kill)->makeTurnUnique();
        if ((*kill)->getUnitStatusString() == "STATUS_DEAD")
            _killTotal++;
        else if ((*kill)->getUnitStatusString() == "STATUS_UNCONSCIOUS")
            _stunTotal++;
		_killList.push_back(*kill);
		if ((*kill)->hostileTurn())
			_reactionFireTotal++;
    }
    _regionTotal[missionStatistics->region.c_str()]++;
    _countryTotal[missionStatistics->country.c_str()]++;
    _typeTotal[missionStatistics->getMissionTypeLowerCase().c_str()]++;
    _UFOTotal[missionStatistics->ufo.c_str()]++;
    _scoreTotal += missionStatistics->score;
    if (missionStatistics->success)
        _winTotal++;
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
    if (missionStatistics->success && missionStatistics->type != "STR_SMALL_SCOUT" && missionStatistics->type != "STR_MEDIUM_SCOUT" && missionStatistics->type != "STR_LARGE_SCOUT" && missionStatistics->type != "STR_SUPPLY_SHIP")
		_importantMissionTotal++;
	_shotAtCounterTotal += unitStatistics->shotAtCounter;
    _shotAtCounter10in1Mission += (unitStatistics->shotAtCounter)/10;
	_hitCounterTotal += unitStatistics->hitCounter;
    _hitCounter5in1Mission += (unitStatistics->hitCounter)/5;
	_totalShotByFriendlyCounter += unitStatistics->shotByFriendlyCounter;
	_totalShotFriendlyCounter += unitStatistics->shotFriendlyCounter;
	if (unitStatistics->loneSurvivor && missionStatistics->success)
		_loneSurvivorTotal++;
	if (unitStatistics->ironMan && missionStatistics->success)
		_ironManTotal++;
	_longDistanceHitCounterTotal += unitStatistics->longDistanceHitCounter;
	_lowAccuracyHitCounterTotal += unitStatistics->lowAccuracyHitCounter;
	if (missionStatistics->valiantCrux)
		_valiantCruxTotal++;
	if (unitStatistics->KIA)
		_KIA++;
    _missionIdList.push_back(missionStatistics->id);
    _missionTotal = _missionIdList.size(); /// CAN GET RID OF MISSION TOTAL
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
	bool awardedCommendation = false;
    std::map<std::string, int> nextCommendationLevel;
    std::map<std::string, int> modularCommendations;
	bool awardCommendationBool = false;
	// Loop over all possible commendations
	for (std::map<std::string, RuleCommendations *>::iterator i = commendationsList.begin(); i != commendationsList.end(); ++i)
	{	
		if (awardCommendationBool)
		{
			--i; // If we awarded the previous commendation, set the iterator back one step to see if we can get it again!
		}
		awardCommendationBool = true;
        nextCommendationLevel.clear();
        modularCommendations.clear();
		// See if we already have the commendation
		// If so, get the level and noun
        for (std::vector<SoldierCommendations*>::const_iterator j = _commendations.begin(); j != _commendations.end(); ++j)
        {
            if ( (*i).first == (*j)->getType() )
            {
                // A map is used for modular medals
                // A commendation that has no noun is always given the noun "noNoun"
                nextCommendationLevel[(*j)->getNoun()] = (*j)->getDecorationLevelInt() + 1;
                break;
            }
        }
        // If we don't have this commendation, add one element to the vector
        if (nextCommendationLevel.empty())
            nextCommendationLevel["noNoun"] = 0;
		// Go through each possible criteria. Assume the medal is awarded, set to false if not.
		// As soon as we find a medal criteria that we do NOT achieve, then we are not awarded a medal
		for (std::map<std::string, std::vector<int> >::const_iterator j = (*i).second->getCriteria()->begin(); j != (*i).second->getCriteria()->end(); ++j)
		{
			// Skip this medal if we have reached its max award level
			if (nextCommendationLevel["noNoun"] >= (*j).second.size())
			{
				awardCommendationBool = false;
				break;
			}
            // These criteria have no nouns, so only the nextCommendationLevel["noNoun"] will ever be used
			else if( nextCommendationLevel.count("noNoun") == 1 &&
					((*j).first == "total_kills" && _killList.size() < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_missions" && _missionIdList.size() < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_wins" && _winTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_score" && _scoreTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_stuns" && _stunTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_days_wounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_base_defense_missions" && _baseDefenseMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_terror_missions" && _terrorMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_night_missions" && _nightMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_night_terror_missions" && _nightTerrorMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_monthly_service" && _monthsService < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_fell_unconcious" && _unconciousTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
                    ((*j).first == "total_shot_at_10_times" && _shotAtCounter10in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "total_hit_5_times" && _hitCounter5in1Mission < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_friendly_fired" && _totalShotByFriendlyCounter < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_lone_survivor" && _loneSurvivorTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_iron_man" && _ironManTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_important_missions" && _importantMissionTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "total_long_distance_hits" && _longDistanceHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "total_low_accuracy_hits" && _lowAccuracyHitCounterTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_reaction_fire" && _reactionFireTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
                    ((*j).first == "total_times_wounded" && _timesWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
                    ((*j).first == "total_days_wounded" && _daysWoundedTotal < (*j).second.at(nextCommendationLevel["noNoun"])) ||
					((*j).first == "total_valient_crux" && _valiantCruxTotal < (*j).second.at(nextCommendationLevel["noNoun"])) || 
					((*j).first == "is_dead" && _KIA < (*j).second.at(nextCommendationLevel["noNoun"])) )					
			{
				awardCommendationBool = false;
				break;
			}
			// Medals with the following criteria are unique because they need a noun
            // And because they loop over a map<> (this allows for maximum moddability)
			else if ((*j).first == "total_kills_with_a_weapon" || (*j).first == "total_missions_in_a_region" || (*j).first == "total_kills_by_race" || (*j).first == "total_kills_by_rank")
			{
				std::map<std::string, int> tempTotal;
				if ((*j).first == "total_kills_with_a_weapon")
					tempTotal = getWeaponTotal();
				else if ((*j).first == "total_missions_in_a_region")
					tempTotal = _regionTotal;
				else if ((*j).first == "total_kills_by_race")
					tempTotal = getAlienRaceTotal();
				else if ((*j).first == "total_kills_by_rank")
					tempTotal = getAlienRankTotal();
				// Loop over the map of kills
				// match nouns and decoration levels
				for(std::map<std::string, int>::const_iterator k = tempTotal.begin(); k != tempTotal.end(); ++k)
				{
					int criteria = -1;
					if (nextCommendationLevel.count("noNoun") != 0)
						criteria = (*j).second.front();
					else if (nextCommendationLevel.count((*k).first) != 0  && (*j).second.at(nextCommendationLevel.at((*k).first)))
						criteria = (*j).second.at(nextCommendationLevel.at((*k).first));
					if (criteria != -1)
						manageModularCommendations(nextCommendationLevel, modularCommendations, (*k), criteria );
				}
				// If it is still empty, we did not get a commendation
				if (modularCommendations.empty())
				{
					awardCommendationBool = false;
					break;
				}
			}
            else if ((*j).first == "kills_with_criteria_career" || (*j).first == "kills_with_criteria_mission" || (*j).first == "kills_with_criteria_turn")
            {
                // Looks to see how many kills the soldier has received over the course of his career
                std::vector<std::map<int, std::vector<std::string> > > *_killCriteriaList = (*i).second->getKillCriteria();
                
                // Loop over the OR vectors
                for (std::vector<std::map<int, std::vector<std::string> > >::const_iterator orCriteria = _killCriteriaList->begin(); orCriteria != _killCriteriaList->end(); ++orCriteria)
                {
                    // Loop over the AND vectors
                    for (std::map<int, std::vector<std::string> >::const_iterator andCriteria = orCriteria->begin(); andCriteria != orCriteria->end(); ++andCriteria)
                    {
                        int count = 0; // How many AND vectors (list of DETAILs) have been successful
						if ((*j).first == "kills_with_criteria_turn" || (*j).first == "kills_with_criteria_mission")
							count++; // Turns and missions start at 1 because of how thisTime and lastTime work.
                        int thisTime = -1; // Time being a turn or a mission
                        int lastTime = -1;
                        bool goToNextTime = false;
                        // Loop over the KILLS
                        for (std::vector<BattleUnitKills*>::const_iterator singleKill = _killList.begin(); singleKill != _killList.end(); ++singleKill)
                        {
                            if ((*j).first == "kills_with_criteria_mission")
                            {
                                thisTime = (*singleKill)->mission;
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->mission;
                                    ++singleKill;
                                }
                            }
                            else if ((*j).first == "kills_with_criteria_turn")
                            {
                                thisTime = (*singleKill)->turn;
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->turn;
                                    ++singleKill;
                                }
                            }
                            // Skip kill-groups that we already got an award for
                            // Skip kills that are inbetween turns
                            if ( thisTime == lastTime && goToNextTime && (*j).first != "kills_with_criteria_career")
                            {
                                continue;
                            }
                            else if (thisTime != lastTime) 
                            {
                                count = 1; // Reset
                                goToNextTime = false;
                                continue;
                            }
                            bool foundMatch = true;
                            // Loop over the DETAILs of the AND vector
                            for (std::vector<std::string>::const_iterator detail = andCriteria->second.begin(); detail != andCriteria->second.end(); ++detail)
                            {
                                // See if we find no matches with any criteria. If so, break and try the next kill.
                                if ( (*singleKill)->rank != (*detail) && (*singleKill)->race != (*detail) &&
                                     (*singleKill)->weapon != (*detail) && (*singleKill)->weaponAmmo != (*detail) &&
                                     (*singleKill)->getUnitStatusString() != (*detail) && (*singleKill)->getUnitFactionString() != (*detail) )
                                {
                                    foundMatch = false;
                                    break;
                                }
                            }
                            if (foundMatch) 
                            {
                                count++;
                                if ( count == (*andCriteria).first) goToNextTime = true; // Criteria met, move to next mission/turn
                            }
                        }
                        int multiCriteria = (*andCriteria).first;
                        // If one of the AND criteria fail, stop looking
                        if (multiCriteria == 0 || count / multiCriteria < (*j).second.at(nextCommendationLevel["noNoun"]))
                        {
                            awardCommendationBool = false;
                            break;
                        }
                    }
                    }
                    if (awardCommendationBool) break; // Stop looking because we are getting one regardless
            }
        }
		if (awardCommendationBool)
		{
            // If we do not have modular medals, but are awarded a different medal,
            // its noun will be "noNoun"
            if (modularCommendations.empty())
            {
                modularCommendations["noNoun"] = 0;
            }
            for (std::map<std::string, int>::const_iterator j = modularCommendations.begin(); j != modularCommendations.end(); ++j)
            {
                awardCommendation((*i).first, (*j).first);
            }
			awardedCommendation = true;
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
	return _missionTotal;
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
std::string SoldierCommendations::getDecorationLevelName()
{
	std::stringstream ss;
	ss << "STR_AWARD_" << _decorationLevel;
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
