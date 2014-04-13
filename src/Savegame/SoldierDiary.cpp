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
	_nightTerrorMissionTotal(0), _monthsService(0), _unconciousTotal(0)
{
}

/**
 * Deconstructor
 */
SoldierDiary::~SoldierDiary()
{
	for (std::vector<SoldierDiaryEntries*>::iterator i = _diaryEntries.begin(); i != _diaryEntries.end(); ++i)
	{
		delete *i;
	}
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
	if (const YAML::Node &diaryEntries = node["diaryEntries"])
	{
		for (YAML::const_iterator i = diaryEntries.begin(); i != diaryEntries.end(); ++i)
			_diaryEntries.push_back(new SoldierDiaryEntries(*i));
	}
	if (const YAML::Node &commendations = node["commendations"])
	{
		for (YAML::const_iterator i = commendations.begin(); i != commendations.end(); ++i)
			_commendations.push_back(new SoldierCommendations(*i));
	}
}

/**
 * Save function
 */
YAML::Node SoldierDiary::save() const
{
	YAML::Node node;
	for (std::vector<SoldierDiaryEntries*>::const_iterator i = _diaryEntries.begin(); i != _diaryEntries.end(); ++i)
			node["diaryEntries"].push_back((*i)->save());
	for (std::vector<SoldierCommendations*>::const_iterator i = _commendations.begin(); i != _commendations.end(); ++i)
			node["commendations"].push_back((*i)->save());
	return node;
}

/**
 * Get the diary entries
 */
std::vector<SoldierDiaryEntries*> SoldierDiary::getSoldierDiaryEntries()
{
	return _diaryEntries;
}

/**
 *  Add soldier diary entry.
 */
void SoldierDiary::addSoldierDiaryEntry(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, bool success, int rating, std::string score, std::string alienRace, int missionDaylight, int daysWounded, Statistics *missionStatistics)
{
	_diaryEntries.push_back(new SoldierDiaryEntries(missionTime, missionRegion, missionCountry, missionType, missionUFO, success, rating, score, alienRace, missionDaylight, daysWounded, missionStatistics));
    updateDiary();
}

/**
 * Updated soldier diary statistics
 */
void SoldierDiary::updateDiary()
{
	SoldierDiaryEntries *latestEntry = _diaryEntries.back();
	for (std::vector<SoldierDiaryKills*>::const_iterator j = latestEntry->getMissionStatistics()->kills.begin() ; j != latestEntry->getMissionStatistics()->kills.end() ; ++j)
    {
		_killList.push_back(*j);
		(*j)->makeTurnUnique(*latestEntry->getMissionTime());
		if ((*j)->getAlienFaction() != "FACTION_HOSTILE") continue;
        _alienRankTotal[(*j)->getAlienRank().c_str()]++;
        _alienRaceTotal[(*j)->getAlienRace().c_str()]++;
        _weaponTotal[(*j)->getWeapon().c_str()]++;
        _weaponAmmoTotal[(*j)->getWeaponAmmo().c_str()]++;		
    }
    _regionTotal[latestEntry->getMissionRegion().c_str()]++;
    _countryTotal[latestEntry->getMissionCountry().c_str()]++;
    _typeTotal[latestEntry->getMissionType().c_str()]++;
    _UFOTotal[latestEntry->getMissionUFO().c_str()]++;
    _scoreTotal += latestEntry->getMissionScore();
    _killTotal += latestEntry->getMissionKillTotal();
    _missionTotal = _diaryEntries.size();
    if (latestEntry->getMissionSuccess())
    {
        _winTotal++;
    }
    _stunTotal += latestEntry->getMissionStunTotal();
    _daysWoundedTotal += latestEntry->getDaysWounded();
    if (latestEntry->getMissionType() == "STR_BASE_DEFENSE")
    {
        _baseDefenseMissionTotal++;
    }
    else if (latestEntry->getMissionType() == "STR_TERROR_MISSION")
    {
        _terrorMissionTotal++;
        if (latestEntry->getMissionDaylight() != 0)
        {
            _nightTerrorMissionTotal++;
        }
    }
    if (latestEntry->getMissionDaylight() != 0)
    {
        _nightMissionTotal++;
    }
    if (latestEntry->getMissionStatistics()->wasUnconcious)
    {
        _unconciousTotal++;
    }
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
					((*j).first == "total_fell_unconcious" && _unconciousTotal < (*j).second.at(_nextCommendationLevel[""])) )
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
                /**
                Career: Look at all
                Mission: Look only at kills with same mission ID
                Turn: Look only at kills with same turn ID
                **/
                // Looks to see how many kills the soldier has received over the course of his career
                std::vector<std::map<int, std::vector<std::string> > > *_killCriteriaList = (*i).second->getKillCriteria();
                
                // Loop over the OR vectors
                for (std::vector<std::map<int, std::vector<std::string> > >::const_iterator orCriteria = _killCriteriaList->begin(); orCriteria != _killCriteriaList->end(); ++orCriteria)
                {
                    // Loop over the AND vectors
                    for (std::map<int, std::vector<std::string> >::const_iterator andCriteria = orCriteria->begin(); andCriteria != orCriteria->end(); ++andCriteria)
                    {
                        if ((*j).first != "kills_with_criteria_mission")
                        {
                            int count = 0; // Each AND vector has to match the award criteria
                            // Loop over the KILLS
                            for (std::vector<SoldierDiaryKills*>::const_iterator singleKill = _killList.begin(); singleKill != _killList.end(); ++singleKill)
                            {
                                bool foundMatch = true;
                                // Loop over the DETAILs of the AND vector
                                for (std::vector<std::string>::const_iterator detail = (*andCriteria).second->begin(); detail != (*andCriteria).second->end(); ++detail)
                                {
                                    // See if we find no matches with any criteria. If so, break and try the next kill.
                                    if ( (*singleKill)->getAlienRank() != (*detail) && (*singleKill)->getAlienRace() != (*detail) &&
                                         (*singleKill)->getWeapon() != (*detail) && (*singleKill)->getWeaponAmmo() != (*detail) &&
                                         (*singleKill)->getAlienState() != (*detail) && (*singleKill)->getAlienFaction() != (*detail) )
                                    {
                                        foundMatch = false;
                                        break;
                                    }
                                }
                                if (foundMatch) count++;
                            }
                            int multiCriteria = (*andCriteria).first;
                            // If one of the AND criteria fail, stop looking
                            if (multiCriteria == 0 || count / multiCriteria < (*j).second.at(_nextCommendationLevel[""]))
                            {
                                _awardCommendation = false;
                                break;
                            }
                        }
                        else
                        {
                            int count = 0;
                            // Loop over MISSIONS
                            for (std::vector<SoldierDiaryEntries*>::const_iterator singleMission = _diaryEntries->begin(); singleMission != _diaryEntries->end(); ++singleMission)
                            {
                                // Loop over KILLS
                                // These kills are confined to the mission; kills won't be borrowed from other missions for commendation purposes
                                for (std::vector<SoldierDiaryKills*>::const_iterator singleKill = (*singleMission)->getMissionKills()->begin(); singleKill != (*singleMission)->getMissionKills()->end(); ++singleKill)
                                {
                                    bool foundMatch = true;
                                    // Loop over the DETAILs of the AND vector
                                    for (std::vector<std::string>::const_iterator detail = (*andCriteria).second->begin(); detail != (*andCriteria).second->end(); ++detail)
                                    {
                                        // See if we find no matches with any criteria. If so, break and try the next kill.
                                        if ( (*singleKill)->getAlienRank() != (*detail) && (*singleKill)->getAlienRace() != (*detail) &&
                                             (*singleKill)->getWeapon() != (*detail) && (*singleKill)->getWeaponAmmo() != (*detail) &&
                                             (*singleKill)->getAlienState() != (*detail) && (*singleKill)->getAlienFaction() != (*detail) )
                                        {
                                            foundMatch = false;
                                            break;
                                        }
                                    }
                                    if (foundMatch) count++;
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
                    }
                    if (_awardCommendation) break; // Stop looking because we are getting one regardless
                }
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
 * Initializes a new diary entry from YAML.
 * @param node YAML node.
 */
SoldierDiaryEntries::SoldierDiaryEntries(const YAML::Node &node) : _missionTime(0,0,0,0,0,0,0)
{
	load(node);
}

/**
 * Initializes a soldier diary.
 * @param missionTime Mission's Time.
 * @param missionRegion Mission's region.
 * @param missionCountry Mission's country.
 * @param missionType Mission's type.
 * @param missionUFO Mission's UFO.
 */
SoldierDiaryEntries::SoldierDiaryEntries(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded, Statistics *missionStatistics) : _missionTime(missionTime), _missionRegion(missionRegion), _missionCountry(missionCountry), _missionType(missionType), _missionUFO(missionUFO), _success(success), _score(score), _rating(rating), _alienRace(alienRace), _missionDaylight(missionDaylight), _daysWounded(daysWounded), _missionStatistics()
{
	_missionStatistics = new Statistics();
	_missionStatistics->wasUnconcious = missionStatistics->wasUnconcious;
	for (std::vector<SoldierDiaryKills*>::const_iterator i = missionStatistics->kills.begin() ; i != missionStatistics->kills.end(); ++i)
	{
		_missionStatistics->kills.push_back(new SoldierDiaryKills((*i)->getAlienRank(), (*i)->getAlienRace(), (*i)->getWeapon(), (*i)->getWeaponAmmo(), (*i)->getAlienStateEnum(), (*i)->getAlienFactionEnum(), (*i)->getTurn()));
	}
}

/**
 *
 */
SoldierDiaryEntries::~SoldierDiaryEntries()
{
	delete _missionStatistics;
}

/**
 * Loads the soldier-equipment layout item from a YAML file.
 * @param node YAML node.
 */
void SoldierDiaryEntries::load(const YAML::Node &node)
{
	_missionTime.load(node["missionTime"]);
	_missionRegion = node["missionRegion"].as<std::string>(_missionRegion);
	_missionCountry = node["missionCountry"].as<std::string>(_missionCountry);
	_missionType = node["missionType"].as<std::string>(_missionType);
	_missionUFO = node["missionUFO"].as<std::string>(_missionUFO);
	_success = node["success"].as<bool>(_success);
	_score = node["score"].as<int>(_score);
	_rating = node["rating"].as<std::string>(_rating);
	_alienRace = node["alienRace"].as<std::string>(_alienRace);
	_missionDaylight = node["missionDaylight"].as<int>(_missionDaylight);
	_daysWounded = node["daysWounded"].as<int>(_daysWounded);
	if (node["missionStatistics"])
	{
		_missionStatistics = new Statistics();
		_missionStatistics->load(node["missionStatistics"]);
	}
}

/**
 * Saves the soldier-equipment layout item to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierDiaryEntries::save() const
{
	YAML::Node node;
	node["missionTime"] = _missionTime.save();
	node["missionRegion"] = _missionRegion;
	node["missionCountry"] = _missionCountry;
	node["missionType"] = _missionType;
	node["missionUFO"] = _missionUFO;
	node["success"] = _success;
	node["score"] = _score;
	node["rating"] = _rating;
	node["alienRace"] = _alienRace;
	node["missionDaylight"] = _missionDaylight;
	node["daysWounded"] = _daysWounded;
	node["missionStatistics"] = _missionStatistics->save();
	return node;
}

/**
 * Get mission time
 * @return Mission Time.
 */
const GameTime *SoldierDiaryEntries::getMissionTime() const
{
	return &_missionTime;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionRegion() const
{
	return _missionRegion;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionCountry() const
{
	return _missionCountry;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionType()
{
	if		(_missionType == "STR_UFO_CRASH_RECOVERY")	_missionType = "STR_UFO_CRASH_RECOVERY_LC";
	else if (_missionType == "STR_UFO_GROUND_ASSAULT")	_missionType = "STR_UFO_GROUND_ASSAULT_LC";
	else if (_missionType == "STR_BASE_DEFENSE")		_missionType = "STR_BASE_DEFENSE_LC";
	else if (_missionType == "STR_ALIEN_BASE_ASSAULT")	_missionType = "STR_ALIEN_BASE_ASSAULT_LC";
	else if (_missionType == "STR_TERROR_MISSION")		_missionType = "STR_TERROR_MISSION_LC";

	return _missionType;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionUFO() const
{
	return _missionUFO;
}

/**
 *
 */
std::vector<SoldierDiaryKills*> SoldierDiaryEntries::getMissionKills() const
{
	return _missionStatistics->kills;
}

/**
 *
 */
bool SoldierDiaryEntries::getMissionSuccess() const
{
	return _success;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionRating() const
{
	return _rating;
}

/**
 *
 */
int SoldierDiaryEntries::getMissionScore() const
{
	return _score;
}

/**
 *
 */
int SoldierDiaryEntries::getMissionKillTotal() const
{
	int _total = 0;
	for (std::vector<SoldierDiaryKills*>::const_iterator i = _missionStatistics->kills.begin() ; i != _missionStatistics->kills.end() ; ++i)
	{
		if ((*i)->getAlienState() == "STATUS_DEAD")
		{
			_total++;
		}
	}
	return _total;
}

/**
 *
 */
std::string SoldierDiaryEntries::getMissionRace() const
{
	return _alienRace;
}

/**
 *
 */
int SoldierDiaryEntries::getMissionDaylight() const
{
	return _missionDaylight;
}

/**
 *
 */
int SoldierDiaryEntries::getMissionStunTotal() const
{
	int _total = 0;
	for (std::vector<SoldierDiaryKills*>::const_iterator i = _missionStatistics->kills.begin() ; i != _missionStatistics->kills.end() ; ++i)
	{
		if ((*i)->getAlienState() == "STATUS_UNCONSCIOUS")
		{
			_total++;
		}
	}
	return _total;
}

/**
 *
 */
int SoldierDiaryEntries::getDaysWounded() const
{
	return _daysWounded;
}

/**
 *
 */
Statistics *SoldierDiaryEntries::getMissionStatistics() const
{
	return _missionStatistics;
}

/**
 * Initializes a new diary entry from YAML.
 * @param node YAML node.
 */
SoldierDiaryKills::SoldierDiaryKills(const YAML::Node &node)
{
	load(node);
}

/**
 * Initializes a soldier diary.
 */
SoldierDiaryKills::SoldierDiaryKills(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, UnitStatus alienState, UnitFaction alienFaction, int turn) : _alienRank(alienRank), _alienRace(alienRace), _weapon(weapon), _weaponAmmo(weaponAmmo), _alienState(alienState), _alienFaction(alienFaction), _turn(turn)
{
}

/**
 *
 */
SoldierDiaryKills::~SoldierDiaryKills()
{
}

/**
 * Loads the soldier-equipment layout item from a YAML file.
 * @param node YAML node.
 */
void SoldierDiaryKills::load(const YAML::Node &node)
{
	_alienRank = node["alienRank"].as<std::string>(_alienRank);
	_alienRace = node["alienRace"].as<std::string>(_alienRace);
	_weapon = node["weapon"].as<std::string>(_weapon);
	_weaponAmmo = node["weaponAmmo"].as<std::string>(_weaponAmmo);
	_alienState = (UnitStatus)node["alienState"].as<int>();
	_alienFaction = (UnitFaction)node["alienFaction"].as<int>();
	_turn = node["turn"].as<int>(_turn);

}

/**
 * Saves the soldier-equipment layout item to a YAML file.
 * @return YAML node.
 */
YAML::Node SoldierDiaryKills::save() const
{
	YAML::Node node;
	node["alienRank"] = _alienRank;
	node["alienRace"] = _alienRace;
	node["weapon"] = _weapon;
	node["weaponAmmo"] = _weaponAmmo;
	node["alienState"] = (int)_alienState;
	node["alienFaction"] = (int)_alienFaction;
	node["turn"] = (int)_turn;
	return node;
}

/**
 *
 */
std::string SoldierDiaryKills::getAlienRank() const
{
	return _alienRank;
}

/**
 *
 */
std::string SoldierDiaryKills::getAlienRace() const
{
	return _alienRace;
}

/**
 *
 */
std::string SoldierDiaryKills::getWeapon() const
{
	return _weapon;
}

/**
 *
 */
std::string SoldierDiaryKills::getWeaponAmmo() const
{
	return _weaponAmmo;
}

/**
 *
 */
std::string SoldierDiaryKills::getAlienState() const
{
	switch (_alienState)
	{
	case STATUS_DEAD:
		return "STATUS_DEAD";
	case STATUS_UNCONSCIOUS:
		return "STATUS_UNCONSCIOUS";
	default:
		return "";
	}
}

/**
 *
 */
std::string SoldierDiaryKills::getAlienFaction() const
{
	switch (_alienFaction)
	{
	case FACTION_PLAYER:
		return "FACTION_PLAYER";
	case FACTION_HOSTILE:
		return "FACTION_HOSTILE";
	case FACTION_NEUTRAL:
		return "FACTION_NEUTRAL";
	default:
		return "";
	}
}

/**
 *
 */
UnitStatus SoldierDiaryKills::getAlienStateEnum() const
{
    return _alienState;
}

/**
 *
 */
UnitFaction SoldierDiaryKills::getAlienFactionEnum() const
{
    return _alienFaction;
}

/**
 *
 */
int SoldierDiaryKills::getTurn() const
{
	return _turn;
}

/**
 *
 */
void SoldierDiaryKills::makeTurnUnique(GameTime missionTime)
{
	_turn += (missionTime.getYear() + missionTime.getMonth() + missionTime.getDay() + missionTime.getHour() + missionTime.getMinute())*12; // Divisible by 1, 2, 3 and greater than 10
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
