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
SoldierDiary::SoldierDiary() : _alienRankTotal(), _alienRaceTotal(), _weaponTotal(), _weaponAmmoTotal(),
    _regionTotal(), _countryTotal(), _typeTotal(), _UFOTotal(), _scoreTotal(0), _killTotal(0), _missionTotal(0),
    _winTotal(0), _stunTotal(0), _daysWoundedTotal(0), _baseDefenseMissionTotal(0), _terrorMissionTotal(0), _nightMissionTotal(0),
	_nightTerrorMissionTotal(0), _monthsService(0)
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
void SoldierDiary::addSoldierDiaryEntry(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int rating, std::string score, std::string alienRace, int missionDaylight, int daysWounded)
{
	_diaryEntries.push_back(new SoldierDiaryEntries(missionTime, missionRegion, missionCountry, missionType, missionUFO, missionKills, success, rating, score, alienRace, missionDaylight, daysWounded));
}

/**
 * Updated soldier diary statistics
 */
void SoldierDiary::updateDiary()
{
	for (std::vector<SoldierDiaryEntries*>::const_iterator i = _diaryEntries.begin() ; i != _diaryEntries.end() ; ++i)
	{
		for (std::vector<SoldierDiaryKills*>::const_iterator j = (*i)->getMissionKills().begin() ; j != (*i)->getMissionKills().end() ; ++j)
		{
			_alienRankTotal[(*j)->getAlienRank().c_str()]++;
			_alienRaceTotal[(*j)->getAlienRace().c_str()]++;
			_weaponTotal[(*j)->getWeapon().c_str()]++;
			_weaponAmmoTotal[(*j)->getWeaponAmmo().c_str()]++;
		}
        _regionTotal[(*i)->getMissionRegion().c_str()]++;
        _countryTotal[(*i)->getMissionCountry().c_str()]++;
        _typeTotal[(*i)->getMissionType().c_str()]++;
        _UFOTotal[(*i)->getMissionUFO().c_str()]++;
        _scoreTotal += (*i)->getMissionScore();
        _killTotal += (*i)->getMissionKillTotal();
        _missionTotal = _diaryEntries.size();
        if ((*i)->getMissionSuccess())
        {
            _winTotal++;
        }
        _stunTotal += (*i)->getMissionStunTotal();
        _daysWoundedTotal += (*i)->getDaysWounded();
		if ((*i)->getMissionType() == "STR_BASE_DEFENSE")
		{
			_baseDefenseMissionTotal++;
		}
		else if ((*i)->getMissionType() == "STR_TERROR_MISSION")
		{
			_terrorMissionTotal++;
            if ((*i)->getMissionDaylight() != 0)
            {
                _nightTerrorMissionTotal++;
            }
		}
		if ((*i)->getMissionDaylight() != 0)
		{
			_nightMissionTotal++;
		}
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
	bool awardedCommendation;
	int _nextCommendationLevel;

	// Loop over all commendations
	for (std::vector<std::pair<std::string, RuleCommendations *> >::const_iterator i = _commendationsList.begin(); i != _commendationsList.end(); ++i)
	{	
		_nextCommendationLevel = 0;
		awardedCommendation = true;

		// See if we already have the commendation, and if so what level it is
		for (std::vector<SoldierCommendations*>::const_iterator j = _commendations.begin(); j != _commendations.end(); ++j)
		{
			// Do we already have the commendation?
			if ( (*i).first == (*j)->getCommendationName() )
			{
				_nextCommendationLevel = (*j)->getDecorationLevelInt() + 1;
				break;
			}
		}

		// Go through each possible criteria
		for (std::map<std::string, std::vector<int> >::const_iterator j = (*i).second->getCriteria()->begin(); j != (*i).second->getCriteria()->end(); ++j)
		{
			// if I am dealing with ___ and I DON'T make the requirements... then no
			if ( ((*j).first == "total_kills" && getKillTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_missions" && getMissionTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_wins" && getWinTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_score" && getScoreTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_stuns" && getStunTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_days_wounded" && getDaysWoundedTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_base_defense_missions" && getBaseDefenseMissionTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_terror_missions" && getTerrorMissionTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_night_missions" && getNightMissionTotal() < (*j).second.at(_nextCommendationLevel)) ||
					((*j).first == "total_night_terror_missions" && getNightTerrorMissionTotal() < (*j).second.at(_nextCommendationLevel)) )
			{
				awardedCommendation = false;
				break;
			}
		}

		if (awardedCommendation)
		{
			awardCommendation((*i).first, (*i).second->getDescription());
		}
	}

	return awardedCommendation;
}

/**
 * Award commendations to the soldier.
 * @param string Commendation Name.
 */
void SoldierDiary::awardCommendation(std::string _commendationName, std::string _commendationDescription)
{
	bool _newCommendation = true;

	for (std::vector<SoldierCommendations*>::iterator i = _commendations.begin() ; i != _commendations.end() ; ++i)
	{
		if ( (*i)->getCommendationName() == _commendationName)
		{
			(*i)->addDecoration();
			_newCommendation = false;
			break;
		}
	}
	if (_newCommendation)
	{
		_commendations.push_back(new SoldierCommendations(_commendationName, _commendationDescription, 0, true));
	}
}

/**
 * Increment soldier's service time one month
 */
int SoldierDiary::addMonthlyService() const
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
SoldierDiaryEntries::SoldierDiaryEntries(GameTime missionTime, std::string missionRegion, std::string missionCountry, std::string missionType, std::string missionUFO, std::vector<SoldierDiaryKills*> missionKills, bool success, int score, std::string rating, std::string alienRace, int missionDaylight, int daysWounded) : _missionTime(missionTime), _missionRegion(missionRegion), _missionCountry(missionCountry), _missionType(missionType), _missionUFO(missionUFO), _missionKills(missionKills), _success(success), _score(score), _rating(rating), _alienRace(alienRace), _missionDaylight(missionDaylight), _daysWounded(daysWounded)
{
}

/**
 *
 */
SoldierDiaryEntries::~SoldierDiaryEntries()
{
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
	if (const YAML::Node &missionKills = node["missionKills"])
	{
		for (YAML::const_iterator i = missionKills.begin() ; i != missionKills.end() ; ++i)
			_missionKills.push_back(new SoldierDiaryKills(*i));
	}
	_success = node["success"].as<bool>(_success);
	_score = node["score"].as<int>(_score);
	_rating = node["rating"].as<std::string>(_rating);
	_alienRace = node["alienRace"].as<std::string>(_alienRace);
	_missionDaylight = node["missionDaylight"].as<int>(_missionDaylight);
	_daysWounded = node["daysWounded"].as<int>(_daysWounded);
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
	if (!_missionKills.empty())
	{
		for (std::vector<SoldierDiaryKills*>::const_iterator i = _missionKills.begin() ; i != _missionKills.end() ; ++i)
		{
			node["missionKills"].push_back((*i)->save());
		}
	}
	node["success"] = _success;
	node["score"] = _score;
	node["rating"] = _rating;
	node["alienRace"] = _alienRace;
	node["missionDaylight"] = _missionDaylight;
	node["daysWounded"] = _daysWounded;
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
	return _missionKills;
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
void SoldierDiaryEntries::addMissionKill(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, AlienState alienState)
{
	_missionKills.push_back(new SoldierDiaryKills(alienRank, alienRace, weapon, weaponAmmo, alienState));
}

/**
 *
 */
int SoldierDiaryEntries::getMissionKillTotal() const
{
	int _total = 0;
	for (std::vector<SoldierDiaryKills*>::const_iterator i = _missionKills.begin() ; i != _missionKills.end() ; ++i)
	{
		if ((*i)->getAlienState() == KILLED)
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
	for (std::vector<SoldierDiaryKills*>::const_iterator i = _missionKills.begin() ; i != _missionKills.end() ; ++i)
	{
		if ((*i)->getAlienState() == STUNNED)
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
SoldierDiaryKills::SoldierDiaryKills(std::string alienRank, std::string alienRace, std::string weapon, std::string weaponAmmo, AlienState alienState) : _alienRank(alienRank), _alienRace(alienRace), _weapon(weapon), _weaponAmmo(weaponAmmo), _alienState(alienState)
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
	_alienState = (AlienState)node["alienState"].as<int>();

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
AlienState SoldierDiaryKills::getAlienState() const
{
	return _alienState;
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
SoldierCommendations::SoldierCommendations(std::string commendationName, std::string commendationDescription, int decorationLevel, bool isNew) : _commendationName(commendationName), _commendationDescription(commendationDescription), _decorationLevel(decorationLevel), _isNew(isNew)
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
	_decorationLevel = node["decorationLevel"].as<int>(_decorationLevel);
	_isNew = node["isNew"].as<bool>(_isNew);
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
	node["decorationLevel"] = _decorationLevel;
	node["isNew"] = _isNew;
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
 * Get the soldier commendation level's name.
 * @return string Commendation level.
 */
std::string SoldierCommendations::getDecorationLevelName()
{
	switch (_decorationLevel)
	{
    case 0:		return "STR_1ST_AWARD";
    case 1:		return "STR_2ND_AWARD";
    case 2:		return "STR_3RD_AWARD";
    case 3:		return "STR_4TH_AWARD";
    case 4:		return "STR_5TH_AWARD";
    case 5:		return "STR_6TH_AWARD";
    case 6:		return "STR_7TH_AWARD";
    case 7:		return "STR_8TH_AWARD";
    case 8:		return "STR_9TH_AWARD";
    case 9:    return "STR_10TH_AWARD";

    default:	return "AWARD LEVEL ERROR";
	}
}

/**
 * Get the soldier commendation level's description.
 * @return string Commendation level description.
 */
std::string SoldierCommendations::getDecorationDescription()
{
	switch (_decorationLevel)
	{
    case 0:		return "STR_1ST_AWARD_DECOR";
    case 1:		return "STR_2ND_AWARD_DECOR";
    case 2:		return "STR_3RD_AWARD_DECOR";
    case 3:		return "STR_4TH_AWARD_DECOR";
    case 4:		return "STR_5TH_AWARD_DECOR";
    case 5:		return "STR_6TH_AWARD_DECOR";
    case 6:		return "STR_7TH_AWARD_DECOR";
    case 7:		return "STR_8TH_AWARD_DECOR";
    case 8:		return "STR_9TH_AWARD_DECOR";
    case 9:		return "STR_10TH_AWARD_DECOR";

    default:	return "AWARD DESCRIPTION ERROR";
	}
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
