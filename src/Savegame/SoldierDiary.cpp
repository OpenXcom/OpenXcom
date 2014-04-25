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
 * Constructor
 */
SoldierDiary::SoldierDiary() : _killList(), _alienRankTotal(), _alienRaceTotal(), _weaponTotal(), _weaponAmmoTotal(), 
    _regionTotal(), _countryTotal(), _typeTotal(), _UFOTotal(), _scoreTotal(0), _killTotal(0), _missionTotal(0),
    _winTotal(0), _stunTotal(0), _daysWoundedTotal(0), _baseDefenseMissionTotal(0), _terrorMissionTotal(0), _nightMissionTotal(0),
	_nightTerrorMissionTotal(0), _monthsService(0), _unconciousTotal(0), _shotAtCounterTotal(0), _hitCounterTotal(0), _loneSurvivorTotal(0),
	_totalShotByFriendlyCounter(0), _totalShotFriendlyCounter(0), _ironManTotal(0), _importantMissionTotal(0), _longDistanceHitCounterTotal(0),
    _lowAccuracyHitCounterTotal(0), _shotsFiredCounterTotal(0), _shotsLandedCounterTotal(0)
{

}

/**
 * Deconstructor
 */
SoldierDiary::~SoldierDiary()
{
	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin(); i != _commendations.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Load function
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
	_alienRankTotal = node["alienRankTotal"].as<std::map<std::string, int> >(_alienRankTotal);
	_alienRaceTotal = node["alienRaceTotal"].as<std::map<std::string, int> >(_alienRaceTotal);
	_weaponTotal = node["weaponTotal"].as<std::map<std::string, int> >(_weaponTotal);
	_weaponAmmoTotal = node["weaponAmmoTotal"].as<std::map<std::string, int> >(_weaponAmmoTotal);
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
}

/**
 * Save function
 */
YAML::Node SoldierDiary::save() const
{
	YAML::Node node;
	for (std::vector<SoldierCommendations*>::const_iterator i = _commendations.begin(); i != _commendations.end(); ++i)
			node["commendations"].push_back((*i)->save());
	for (std::vector<BattleUnitKills*>::const_iterator i = _killList.begin(); i != _killList.end(); ++i)
			node["killList"].push_back((*i)->save());
	node["alienRankTotal"] = _alienRankTotal;
    node["alienRaceTotal"] = _alienRaceTotal;
    node["weaponTotal"] = _weaponTotal;
    node["weaponAmmoTotal"] = _weaponAmmoTotal;
    node["regionTotal"] = _regionTotal;
    node["countryTotal"] = _countryTotal;
    node["typeTotal"] = _typeTotal;
    node["UFOTotal"] = _UFOTotal;
    node["scoreTotal"] = _scoreTotal;
    node["killTotal"] = _killTotal;
    node["missionTotal"] = _missionTotal;
    node["winTotal"] = _winTotal;
    node["stunTotal"] = _stunTotal;
    node["daysWoundedTotal"] = _daysWoundedTotal;
    node["baseDefenseMissionTotal"] = _baseDefenseMissionTotal;
    node["totalShotByFriendlyCounter"] = _totalShotByFriendlyCounter;
	node["totalShotFriendlyCounter"] = _totalShotFriendlyCounter;
    node["loneSurvivorTotal"] = _loneSurvivorTotal;
    node["terrorMissionTotal"] = _terrorMissionTotal;
    node["nightMissionTotal"] = _nightMissionTotal;
    node["nightTerrorMissionTotal"] = _nightTerrorMissionTotal;
    node["monthsService"] = _monthsService;
    node["unconciousTotal"] = _unconciousTotal;
    node["shotAtCounterTotal"] = _shotAtCounterTotal;
    node["hitCounterTotal"] = _hitCounterTotal;
    node["ironManTotal"] = _ironManTotal;
    node["importantMissionTotal"] = _importantMissionTotal;
	node["longDistanceHitCounterTotal"] = _longDistanceHitCounterTotal;
	node["lowAccuracyHitCounterTotal"] = _lowAccuracyHitCounterTotal;
	node["shotsFiredCounterTotal"] = _shotsFiredCounterTotal;
	node["shotsLandedCounterTotal"] = _shotsLandedCounterTotal;
	return node;
}

/**
 * Updated soldier diary statistics
 */
void SoldierDiary::updateDiary(std::vector<BattleUnitKills*> unitKills, BattleUnitStatistics *unitStatistics, MissionStatistics *missionStatistics)
{
	for (std::vector<BattleUnitKills*>::const_iterator kill = unitKills.begin() ; kill != unitKills.end() ; ++kill)
    {
		_killList.push_back(*kill);
		(*kill)->makeTurnUnique();
		if ((*kill)->getUnitFactionString() != "FACTION_HOSTILE") 
            continue;
        _alienRankTotal[(*kill)->rank.c_str()]++;
        _alienRaceTotal[(*kill)->race.c_str()]++;
        _weaponTotal[(*kill)->weapon.c_str()]++;
        _weaponAmmoTotal[(*kill)->weaponAmmo.c_str()]++;
        if ((*kill)->getUnitStatusString() == "STATUS_DEAD")
            _killTotal++;
        else if ((*kill)->getUnitStatusString() == "STATUS_UNCONSCIOUS")
            _stunTotal++;
    }
    _regionTotal[missionStatistics->region.c_str()]++;
    _countryTotal[missionStatistics->country.c_str()]++;
    _typeTotal[missionStatistics->type.c_str()]++;
    _UFOTotal[missionStatistics->ufo.c_str()]++;
    _scoreTotal += missionStatistics->score;
    if (missionStatistics->success)
        _winTotal++;
    _daysWoundedTotal += unitStatistics->daysWounded;
    if (missionStatistics->type == "STR_BASE_DEFENSE")
        _baseDefenseMissionTotal++;
    else if (missionStatistics->type == "STR_TERROR_MISSION")
    {
        _terrorMissionTotal++;
        if (missionStatistics->daylight != 0)
            _nightTerrorMissionTotal++;
    }
    if (missionStatistics->daylight != 0)
        _nightMissionTotal++;
    if (unitStatistics->wasUnconcious)
        _unconciousTotal++;
    if (missionStatistics->success && missionStatistics->type != "STR_SMALL_SCOUT" && missionStatistics->type != "STR_BASE_DEFENSE" && missionStatistics->type != "STR_MEDIUM_SCOUT")
		_importantMissionTotal++;
    _shotAtCounterTotal += (unitStatistics->shotAtCounter); // Commendation is for getting shot 10 times in one mission, BUT THIS DOES NOT SEPERATE MISSIONS
    _hitCounterTotal += (unitStatistics->hitCounter); // Commendation si for getting hit 5 times in one mission
	_totalShotByFriendlyCounter += unitStatistics->shotByFriendlyCounter;
	_totalShotFriendlyCounter += unitStatistics->shotFriendlyCounter;
	if (unitStatistics->loneSurvivor && missionStatistics->success)
		_loneSurvivorTotal++;
	if (unitStatistics->ironMan && missionStatistics->success)
		_ironManTotal++;
	_longDistanceHitCounterTotal += unitStatistics->longDistanceHitCounter;
	_lowAccuracyHitCounterTotal += unitStatistics->lowAccuracyHitCounter;

    _daysWounded.push_back(std::make_pair(missionStatistics->id, unitStatistics->daysWounded)); // Catalogging purposes
    _missionIdList.push_back(missionStatistics->id);
    _missionTotal = _missionIdList.size(); /// CAN GET RID OF MISSION TOTAL
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getAlienRankTotal() const
{
	return _alienRankTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getAlienRaceTotal() const
{
	return _alienRaceTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponTotal() const
{
	return _weaponTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getWeaponAmmoTotal() const
{
	return _weaponAmmoTotal;
}


/**
 *
 */
std::map<std::string, int> SoldierDiary::getRegionTotal() const
{
	return _regionTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getCountryTotal() const
{
	return _countryTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getTypeTotal() const
{
	return _typeTotal;
}

/**
 *
 */
std::map<std::string, int> SoldierDiary::getUFOTotal() const
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
 * Get total base defense missions
 * @return int Total base defense missions
 */
int SoldierDiary::getBaseDefenseMissionTotal() const
{
	return _baseDefenseMissionTotal;
}

/**
 * Get total terror missions
 * @return int Total terror missions
 */
int SoldierDiary::getTerrorMissionTotal() const
{
	return _terrorMissionTotal;
}

/**
 * Get total night missions
 * @return int Total night missions
 */
int SoldierDiary::getNightMissionTotal() const
{
	return _nightMissionTotal;
}

/**
 * Get total night terror missions
 * @return int Total night terror missions
 */
int SoldierDiary::getNightTerrorMissionTotal() const
{
	return _nightTerrorMissionTotal;
}
/**
 * Get soldier commendations.
 * @return SoldierCommendations soldier commendations.
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
	std::vector<std::pair<std::string, RuleCommendations *> > _commendationsList = rules->getCommendation();
    bool _awardCommendation;
	bool _awardedCommendation = false;
    std::map<std::string, int> _nextCommendationLevel;
    std::map<std::string, int> _modularCommendations;

	// Loop over all possible commendations
	for (std::vector<std::pair<std::string, RuleCommendations *> >::iterator i = _commendationsList.begin(); i != _commendationsList.end(); ++i)
	{	
		bool _awardCommendation = true;
        _nextCommendationLevel.clear();
        _modularCommendations.clear();

		// See if we already have the commendation
		// If so, get the level and noun
		for (std::vector<SoldierCommendations*>::const_iterator j = _commendations.begin(); j != _commendations.end(); ++j)
		{
			if ( (*i).first == (*j)->getCommendationName() )
			{
                // A map is used for modular medals
                // A commendation that has no noun is always given the noun ""
				_nextCommendationLevel[(*j)->getNoun()] = (*j)->getDecorationLevelInt() + 1;
			}
		}
        // If we don't have this commendation, add one element to the vector
        if (_nextCommendationLevel.empty())
            _nextCommendationLevel[""] = 0;

		// Go through each possible criteria. Assume the medal is awarded, set to false if not.
		// As soon as we find a medal criteria that we do NOT achieve, then we are not awarded a medal
		for (std::map<std::string, std::vector<int> >::const_iterator j = (*i).second->getCriteria()->begin(); j != (*i).second->getCriteria()->end(); ++j)
		{
			// Skip this medal if we have reached its max award level
			if (_nextCommendationLevel[""] >= (*j).second.size())
			{
				_awardCommendation = false;
				break;
			}
            // These criteria have no nouns, so only the _nextCommendationLevel[""] will ever be used
			else if(((*j).first == "total_kills" && getKillTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_missions" && getMissionTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_wins" && getWinTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_score" && getScoreTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_stuns" && getStunTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_days_wounded" && getDaysWoundedTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_base_defense_missions" && getBaseDefenseMissionTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_terror_missions" && getTerrorMissionTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_night_missions" && getNightMissionTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_night_terror_missions" && getNightTerrorMissionTotal() < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_monthly_service" && _monthsService < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_fell_unconcious" && _unconciousTotal < (*j).second.at(_nextCommendationLevel[""])) || 
                    ((*j).first == "total_shot_at_10_times" && _shotAtCounterTotal < (*j).second.at(_nextCommendationLevel[""])) || 
					((*j).first == "total_hit_5_times" && _hitCounterTotal < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_friendly_fired" && _totalShotByFriendlyCounter < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_lone_survivor" && _loneSurvivorTotal < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_iron_man" && _ironManTotal < (*j).second.at(_nextCommendationLevel[""])) ||
					((*j).first == "total_important_missions" && _importantMissionTotal < (*j).second.at(_nextCommendationLevel[""])) || 
					((*j).first == "total_long_distance_hits" && _longDistanceHitCounterTotal < (*j).second.at(_nextCommendationLevel[""])) || 
					((*j).first == "total_low_accuracy_hits" && _lowAccuracyHitCounterTotal < (*j).second.at(_nextCommendationLevel[""])) )
			{
				_awardCommendation = false;
				break;
			}
			// Medals with the following criteria are unique because they need a noun
            // And because they loop over a map<> (this allows for maximum moddability)
			else if ((*j).first == "total_kills_with_a_weapon")
			{
                // Loop over the map of kills
                // match nouns and decoration levels
                for(std::map<std::string, int>::const_iterator k = _weaponTotal.begin(); k != _weaponTotal.end(); ++k)
                {
					int _criteria = _nextCommendationLevel[""] == 0 ? 0 : (*j).second.at(_nextCommendationLevel.at((*k).first));
					manageModularCommendations(_nextCommendationLevel, _modularCommendations, (*k), _criteria);
                }
				// If it is still empty, we did not get a commendation
				if (_modularCommendations.empty())
				{
					_awardCommendation = false;
					break;
				}
            }
			else if ((*j).first == "total_missions_in_a_region")
			{
				for(std::map<std::string, int>::const_iterator k = _regionTotal.begin(); k != _regionTotal.end(); ++k)
				{
					int _criteria = _nextCommendationLevel[""] == 0 ? 0 : (*j).second.at(_nextCommendationLevel.at((*k).first));
					manageModularCommendations(_nextCommendationLevel, _modularCommendations, (*k), _criteria);
				}
				if (_modularCommendations.empty())
				{
					_awardCommendation = false;
					break;
				}
			}
			else if ((*j).first == "total_kills_by_race")
			{
				for(std::map<std::string, int>::const_iterator k = _alienRaceTotal.begin(); k != _alienRaceTotal.end(); ++k)
				{
					int _criteria = _nextCommendationLevel[""] == 0 ? 0 : (*j).second.at(_nextCommendationLevel.at((*k).first));
					manageModularCommendations(_nextCommendationLevel, _modularCommendations, (*k), _criteria);
				}
				if (_modularCommendations.empty())
				{
					_awardCommendation = false;
					break;
				}
			}
			else if ((*j).first == "total_kills_by_rank")
			{
				for(std::map<std::string, int>::const_iterator k = _alienRankTotal.begin(); k != _alienRankTotal.end(); ++k)
				{
					int _criteria = _nextCommendationLevel[""] == 0 ? 0 : (*j).second.at(_nextCommendationLevel.at((*k).first));
					manageModularCommendations(_nextCommendationLevel, _modularCommendations, (*k), _criteria);
				}
				if (_modularCommendations.empty())
				{
					_awardCommendation = false;
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
                        int count = 1; // How many AND vectors (list of DETAILs) have been successful
                        int thisTime = -1; // Time being a turn or a mission
                        int lastTime = -1;
                        bool goToNextTime = false;
                        // Loop over the KILLS
                        for (std::vector<BattleUnitKills*>::const_iterator singleKill = _killList.begin(); singleKill != _killList.end(); ++singleKill)
                        {
                            if ((*j).first == "kills_with_criteria_mission")
                            {
                                thisTime = (*singleKill)->mission
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->mission
                                    ++singleKill;
                                }
                            }
                            else if ((*j).first == "kills_with_criteria_turn")
                            {
                                thisTime = (*singleKill)->turn
                                if (singleKill != _killList.begin())
                                {
                                    --singleKill;
                                    lastTime = (*singleKill)->turn
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
                                     (*singleKill)->status != (*detail) && (*singleKill)->faction != (*detail) )
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
                        if (multiCriteria == 0 || count / multiCriteria < (*j).second.at(_nextCommendationLevel[""]))
                        {
                            _awardCommendation = false;
                            break;
                        }
                    }

                    }
                    if (_awardCommendation) break; // Stop looking because we are getting one regardless
            }
        }
		if (_awardCommendation)
		{
            // If we do not have modular medals, but are awarded a different medal,
            // its noun will be ""
            if (_modularCommendations.empty())
            {
                _modularCommendations[""] = 0;
            }
            for (std::map<std::string, int>::const_iterator j = _modularCommendations.begin(); j != _modularCommendations.end(); ++j)
            {
                awardCommendation((*i).first, (*i).second->getDescription(), (*j).first, (*i).second->getSprite());
				_awardedCommendation = true;
            }
		}
	}

	return _awardedCommendation;
}

/**
 * Manage modular commendations (private)
 */
void SoldierDiary::manageModularCommendations(std::map<std::string, int> nextCommendationLevel, std::map<std::string, int> modularCommendations, std::pair<std::string, int> statTotal, int criteria)
{
	// If criteria is 0, we don't have this noun
	if (criteria == 0 && statTotal.second >= criteria)
	{
		modularCommendations[statTotal.first]++;
	}
	// If we meet the criteria, remember the noun for award purposes
    else if (criteria != 0 && nextCommendationLevel.at(statTotal.first) >= criteria)
    {
        modularCommendations[statTotal.first]++;
    }
}

/**
 * Award commendations to the soldier.
 * @param string Commendation Name.
 */
void SoldierDiary::awardCommendation(std::string _commendationName, std::string _commendationDescription, std::string _noun, int sprite)
{
	bool _newCommendation = true;

	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin() ; i != _commendations.end() ; ++i)
	{
		if ( (*i)->getCommendationName() == _commendationName && (*i)->getNoun() == _noun)
		{
			(*i)->addDecoration();
			_newCommendation = false;
			break;
		}
	}
	if (_newCommendation)
	{
		_commendations.push_back(new SoldierCommendations(_commendationName, _commendationDescription, _noun, 0, true, sprite));
	}
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
SoldierCommendations::SoldierCommendations(std::string commendationName, std::string commendationDescription, std::string noun, int decorationLevel, bool isNew, int sprite) : _commendationName(commendationName), _commendationDescription(commendationDescription), _noun(noun), _decorationLevel(decorationLevel), _isNew(isNew), _sprite(sprite)
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
	_commendationName = node["commendationName"].as<std::string>(_commendationName);
	_commendationDescription = node["commendationDescription"].as<std::string>(_commendationDescription);
	_noun = node["noun"].as<std::string>(_noun);
	_decorationLevel = node["decorationLevel"].as<int>(_decorationLevel);
	_isNew = node["isNew"].as<bool>(_isNew);
	_sprite = node["sprite"].as<int>(_sprite);
}

/**
 * Saves the commendation to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierCommendations::save() const
{
	YAML::Node node;
	node["commendationName"] = _commendationName;
	node["commendationDescription"] = _commendationDescription;
	node["noun"] = _noun;
	node["decorationLevel"] = _decorationLevel;
	node["isNew"] = _isNew;
	node["sprite"] = _sprite;
	return node;
}

/**
 * Get the soldier's commendation's name.
 * @return string Commendation name.
 */
std::string SoldierCommendations::getCommendationName() const
{
	return _commendationName;
}

/**
 * Get the soldier's commendation's description.
 * @return string Commendation description.
 */
std::string SoldierCommendations::getCommendationDescription() const
{
	return _commendationDescription;
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
 * Get the soldier commendation decoration sprite number.
 * @return int sprite number.
 */
int SoldierCommendations::getDecorationSprite() const
{
	// Assuming the ruleset still lists the sprites starting at 200
	return _decorationLevel;
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

/**
 * Get the sprite int
 * @return sprite int
 */
int SoldierCommendations::getSprite() const
{
	return _sprite;
}

}
