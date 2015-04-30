/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "Soldier.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Savegame/Craft.h"
#include "../Savegame/EquipmentLayoutItem.h"
#include "../Savegame/SoldierDeath.h"
#include "../Savegame/SoldierDiary.h"
#include "../Ruleset/SoldierNamePool.h"
#include "../Ruleset/RuleSoldier.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/StatString.h"
#include "../Engine/Options.h"
#include "SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes a new soldier, either blank or randomly generated.
 * @param rules Soldier ruleset.
 * @param armor Soldier armor.
 * @param names List of name pools for soldier generation.
 * @param id Pointer to unique soldier id for soldier generation.
 */
Soldier::Soldier(RuleSoldier *rules, Armor *armor, const std::vector<SoldierNamePool*> *names, int id) : _id(id), _improvement(0), _psiStrImprovement(0), _rules(rules), _rank(RANK_ROOKIE), _craft(0), _gender(GENDER_MALE), _look(LOOK_BLONDE), _missions(0), _kills(0), _recovery(0), _recentlyPromoted(false), _psiTraining(false), _armor(armor), _death(0), _diary()
{
	_diary = new SoldierDiary();

	if (names != 0)
	{
		UnitStats minStats = rules->getMinStats();
		UnitStats maxStats = rules->getMaxStats();

		_initialStats.tu = RNG::generate(minStats.tu, maxStats.tu);
		_initialStats.stamina = RNG::generate(minStats.stamina, maxStats.stamina);
		_initialStats.health = RNG::generate(minStats.health, maxStats.health);
		_initialStats.bravery = RNG::generate(minStats.bravery/10, maxStats.bravery/10)*10;
		_initialStats.reactions = RNG::generate(minStats.reactions, maxStats.reactions);
		_initialStats.firing = RNG::generate(minStats.firing, maxStats.firing);
		_initialStats.throwing = RNG::generate(minStats.throwing, maxStats.throwing);
		_initialStats.strength = RNG::generate(minStats.strength, maxStats.strength);
		_initialStats.psiStrength = RNG::generate(minStats.psiStrength, maxStats.psiStrength);
		_initialStats.melee = RNG::generate(minStats.melee, maxStats.melee);
		_initialStats.psiSkill = minStats.psiSkill;

		_currentStats = _initialStats;	

		if (!names->empty())
		{
			size_t nationality = RNG::generate(0, names->size()-1);
			_name = names->at(nationality)->genName(&_gender, rules->getFemaleFrequency());
			_look = (SoldierLook)names->at(nationality)->genLook(4); // Once we add the ability to mod in extra looks, this will need to reference the ruleset for the maximum amount of looks.
		}
		else
		{
			_name = L"";
			_gender = (RNG::percent(rules->getFemaleFrequency()) ? GENDER_FEMALE : GENDER_MALE);
			_look = (SoldierLook)RNG::generate(0,3);
		}
	}
}

/**
 *
 */
Soldier::~Soldier()
{
	for (std::vector<EquipmentLayoutItem*>::iterator i = _equipmentLayout.begin(); i != _equipmentLayout.end(); ++i)
	{
		delete *i;
	}
	delete _death;
	delete _diary;
}

/**
 * Loads the soldier from a YAML file.
 * @param node YAML node.
 * @param rule Game ruleset.
 * @param save Pointer to savegame.
 */
void Soldier::load(const YAML::Node& node, const Ruleset *rule, SavedGame *save)
{
	_id = node["id"].as<int>(_id);
	_name = Language::utf8ToWstr(node["name"].as<std::string>());
	_initialStats = node["initialStats"].as<UnitStats>(_initialStats);
	_currentStats = node["currentStats"].as<UnitStats>(_currentStats);
	_rank = (SoldierRank)node["rank"].as<int>();
	_gender = (SoldierGender)node["gender"].as<int>();
	_look = (SoldierLook)node["look"].as<int>();
	_missions = node["missions"].as<int>(_missions);
	_kills = node["kills"].as<int>(_kills);
	_recovery = node["recovery"].as<int>(_recovery);
	Armor *armor = rule->getArmor(node["armor"].as<std::string>());
	if (armor == 0)
	{
		armor = rule->getArmor("STR_NONE_UC");
	}
	_armor = armor;
	_psiTraining = node["psiTraining"].as<bool>(_psiTraining);
	_improvement = node["improvement"].as<int>(_improvement);
	_psiStrImprovement = node["psiStrImprovement"].as<int>(_psiStrImprovement);
	if (const YAML::Node &layout = node["equipmentLayout"])
	{
		for (YAML::const_iterator i = layout.begin(); i != layout.end(); ++i)
		{
			EquipmentLayoutItem *layoutItem = new EquipmentLayoutItem(*i);
			if (rule->getInventory(layoutItem->getSlot()))
			{
				_equipmentLayout.push_back(layoutItem);
			}
			else
			{
				delete layoutItem;
			}
		}
	}
	if (node["death"])
	{
		_death = new SoldierDeath();
		_death->load(node["death"]);
	}
	if (node["diary"])
	{
		_diary = new SoldierDiary();
		_diary->load(node["diary"]);
	}
	calcStatString(rule->getStatStrings(), (Options::psiStrengthEval && save->isResearched(rule->getPsiRequirements())));
}

/**
 * Saves the soldier to a YAML file.
 * @return YAML node.
 */
YAML::Node Soldier::save() const
{
	YAML::Node node;
	node["id"] = _id;
	node["name"] = Language::wstrToUtf8(_name);
	node["initialStats"] = _initialStats;
	node["currentStats"] = _currentStats;
	node["rank"] = (int)_rank;
	if (_craft != 0)
	{
		node["craft"] = _craft->saveId();
	}
	node["gender"] = (int)_gender;
	node["look"] = (int)_look;
	node["missions"] = _missions;
	node["kills"] = _kills;
	if (_recovery > 0)
		node["recovery"] = _recovery;
	node["armor"] = _armor->getType();
	if (_psiTraining)
		node["psiTraining"] = _psiTraining;
	node["improvement"] = _improvement;
	node["psiStrImprovement"] = _psiStrImprovement;
	if (!_equipmentLayout.empty())
	{
		for (std::vector<EquipmentLayoutItem*>::const_iterator i = _equipmentLayout.begin(); i != _equipmentLayout.end(); ++i)
			node["equipmentLayout"].push_back((*i)->save());
	}
	if (_death != 0)
	{
		node["death"] = _death->save();
	}
	if (!_diary->getMissionIdList().empty() || !_diary->getSoldierCommendations()->empty())
	{
	node["diary"] = _diary->save();
	}

	return node;
}

/**
 * Returns the soldier's full name (and, optionally, statString).
 * @param statstring Add stat string?
 * @param maxLength Restrict length to a certain value.
 * @return Soldier name.
 */
std::wstring Soldier::getName(bool statstring, unsigned int maxLength) const
{
	if (statstring && !_statString.empty())
	{
		if (_name.length() + _statString.length() > maxLength)
		{
			return _name.substr(0, maxLength - _statString.length()) + L"/" + _statString;
		}
		else
		{
			return _name + L"/" + _statString;
		}
	}
	else
	{
		return _name;
	}
}

/**
 * Changes the soldier's full name.
 * @param name Soldier name.
 */
void Soldier::setName(const std::wstring &name)
{
	_name = name;
}

/**
 * Returns the craft the soldier is assigned to.
 * @return Pointer to craft.
 */
Craft *Soldier::getCraft() const
{
	return _craft;
}

/**
 * Assigns the soldier to a new craft.
 * @param craft Pointer to craft.
 */
void Soldier::setCraft(Craft *craft)
{
	_craft = craft;
}

/**
 * Returns the soldier's craft string, which
 * is either the soldier's wounded status,
 * the assigned craft name, or none.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Soldier::getCraftString(Language *lang) const
{
	std::wstring s;
	if (_recovery > 0)
	{
		s = lang->getString("STR_WOUNDED");
	}
	else if (_craft == 0)
	{
		s = lang->getString("STR_NONE_UC");
	}
	else
	{
		s = _craft->getName(lang);
	}
	return s;
}

/**
 * Returns a localizable-string representation of
 * the soldier's military rank.
 * @return String ID for rank.
 */
std::string Soldier::getRankString() const
{
	switch (_rank)
	{
	case RANK_ROOKIE:
		return "STR_ROOKIE";
	case RANK_SQUADDIE:
		return "STR_SQUADDIE";
	case RANK_SERGEANT:
		return "STR_SERGEANT";
	case RANK_CAPTAIN:
		return "STR_CAPTAIN";
	case RANK_COLONEL:
		return "STR_COLONEL";
	case RANK_COMMANDER:
		return "STR_COMMANDER";
	default:
		return "";
	}
}

/**
 * Returns a graphic representation of
 * the soldier's military rank.
 * @note THE MEANING OF LIFE
 * @return Sprite ID for rank.
 */
int Soldier::getRankSprite() const
{
	return 42 + _rank;
}


/**
 * Returns the soldier's military rank.
 * @return Rank enum.
 */
SoldierRank Soldier::getRank() const
{
	return _rank;
}

/**
 * Increase the soldier's military rank.
 */
void Soldier::promoteRank()
{
	_rank = (SoldierRank)((int)_rank + 1);
	if (_rank > RANK_SQUADDIE)
	{
		// only promotions above SQUADDIE are worth to be mentioned
		_recentlyPromoted = true;
	}
}

/**
 * Returns the soldier's amount of missions.
 * @return Missions.
 */
int Soldier::getMissions() const
{
	return _missions;
}

/**
 * Returns the soldier's amount of kills.
 * @return Kills.
 */
int Soldier::getKills() const
{
	return _kills;
}

/**
 * Returns the soldier's gender.
 * @return Gender.
 */
SoldierGender Soldier::getGender() const
{
	return _gender;
}

/**
 * Returns the soldier's look.
 * @return Look.
 */
SoldierLook Soldier::getLook() const
{
	return _look;
}

/**
 * Returns the soldier's rules.
 * @return rulesoldier
 */
RuleSoldier *Soldier::getRules() const
{
	return _rules;
}

/**
 * Returns the soldier's unique ID. Each soldier
 * can be identified by its ID. (not it's name)
 * @return Unique ID.
 */
int Soldier::getId() const
{
	return _id;
}

/**
 * Add a mission to the counter.
 */
void Soldier::addMissionCount()
{
	_missions++;
}

/**
 * Add a kill to the counter.
 */
void Soldier::addKillCount(int count)
{
	_kills += count;
}

/**
 * Get pointer to initial stats.
 */
UnitStats *Soldier::getInitStats()
{
	return &_initialStats;
}

/**
 * Get pointer to current stats.
 */
UnitStats *Soldier::getCurrentStats()
{
	return &_currentStats;
}

/**
 * Returns the unit's promotion status and resets it.
 * @return True if recently promoted, False otherwise.
 */
bool Soldier::isPromoted()
{
	bool promoted = _recentlyPromoted;
	_recentlyPromoted = false;
	return promoted;
}

/**
 * Returns the unit's current armor.
 * @return Pointer to armor data.
 */
Armor *Soldier::getArmor() const
{
	return _armor;
}

/**
 * Changes the unit's current armor.
 * @param armor Pointer to armor data.
 */
void Soldier::setArmor(Armor *armor)
{
	_armor = armor;
}

/**
 * Returns the amount of time until the soldier is healed.
 * @return Number of days.
 */
int Soldier::getWoundRecovery() const
{
	return _recovery;
}

/**
 * Changes the amount of time until the soldier is healed.
 * @param recovery Number of days.
 */
void Soldier::setWoundRecovery(int recovery)
{
	_recovery = recovery;

	// dismiss from craft
	if (_recovery > 0)
	{
		_craft = 0;
	}
}

/**
 * Heals soldier wounds.
 */
void Soldier::heal()
{
	_recovery--;
}

/**
 * Returns the list of EquipmentLayoutItems of a soldier.
 * @return Pointer to the EquipmentLayoutItem list.
 */
std::vector<EquipmentLayoutItem*> *Soldier::getEquipmentLayout()
{
	return &_equipmentLayout;
}

/**
 * Trains a soldier's Psychic abilities after 1 month.
 */
void Soldier::trainPsi()
{
	int psiSkillCap = _rules->getStatCaps().psiSkill;
	int psiStrengthCap = _rules->getStatCaps().psiStrength;

	_improvement = _psiStrImprovement = 0;
	// -10 days - tolerance threshold for switch from anytimePsiTraining option.
	// If soldier has psiskill -10..-1, he was trained 20..59 days. 81.7% probability, he was trained more that 30 days.
	if (_currentStats.psiSkill < -10 + _rules->getMinStats().psiSkill)
		_currentStats.psiSkill = _rules->getMinStats().psiSkill;
	else if (_currentStats.psiSkill <= _rules->getMaxStats().psiSkill)
	{
		int max = _rules->getMaxStats().psiSkill + _rules->getMaxStats().psiSkill / 2;
		_improvement = RNG::generate(_rules->getMaxStats().psiSkill, max);
	}
	else
	{
		if (_currentStats.psiSkill <= (psiSkillCap / 2)) _improvement = RNG::generate(5, 12);
		else if (_currentStats.psiSkill < psiSkillCap) _improvement = RNG::generate(1, 3);

		if (Options::allowPsiStrengthImprovement)
		{
			if (_currentStats.psiStrength <= (psiStrengthCap / 2)) _psiStrImprovement = RNG::generate(5, 12);
			else if (_currentStats.psiStrength < psiStrengthCap) _psiStrImprovement = RNG::generate(1, 3);
		}
	}
	_currentStats.psiSkill += _improvement;
	_currentStats.psiStrength += _psiStrImprovement;
	if (_currentStats.psiSkill > psiSkillCap) _currentStats.psiSkill = psiSkillCap;
	if (_currentStats.psiStrength > psiStrengthCap) _currentStats.psiStrength = psiStrengthCap;
}

/**
 * Trains a soldier's Psychic abilities after 1 day.
 * (anytimePsiTraining option)
 */
void Soldier::trainPsi1Day()
{
	if (!_psiTraining)
	{
		_improvement = 0;
		return;
	}

	if (_currentStats.psiSkill > 0) // yes, 0. _rules->getMinStats().psiSkill was wrong.
	{
		if (8 * 100 >= _currentStats.psiSkill * RNG::generate(1, 100) && _currentStats.psiSkill < _rules->getStatCaps().psiSkill)
		{
			++_improvement;
			++_currentStats.psiSkill;
		}

		if (Options::allowPsiStrengthImprovement)
		{
			if (8 * 100 >= _currentStats.psiStrength * RNG::generate(1, 100) && _currentStats.psiStrength < _rules->getStatCaps().psiStrength)
			{
				++_psiStrImprovement;
				++_currentStats.psiStrength;
			}
		}
	}
	else if (_currentStats.psiSkill < _rules->getMinStats().psiSkill)
	{
		if (++_currentStats.psiSkill == _rules->getMinStats().psiSkill)	// initial training is over
		{
			_improvement = _rules->getMaxStats().psiSkill + RNG::generate(0, _rules->getMaxStats().psiSkill / 2);
			_currentStats.psiSkill = _improvement;
		}
	}
	else // minStats.psiSkill <= 0 && _currentStats.psiSkill == minStats.psiSkill
		_currentStats.psiSkill -= RNG::generate(30, 60);	// set initial training from 30 to 60 days
}

/**
 * returns whether or not the unit is in psi training
 * @return true/false
 */
bool Soldier::isInPsiTraining()
{
	return _psiTraining;
}

/**
 * toggles whether or not the unit is in psi training
 */
void Soldier::setPsiTraining()
{
	_psiTraining = !_psiTraining;
}

/**
 * returns this soldier's psionic skill improvement score for this month.
 * @return score
 */
int Soldier::getImprovement()
{
	return _improvement;
}

/**
 * returns this soldier's psionic strength improvement score for this month.
 */
int Soldier::getPsiStrImprovement()
{
	return _psiStrImprovement;
}

/**
 * Returns the soldier's death details.
 * @return Pointer to death data. NULL if no death has occured.
 */
SoldierDeath *Soldier::getDeath() const
{
	return _death;
}

/**
 * Kills the soldier in the Geoscape.
 * @param death Pointer to death data.
 */
void Soldier::die(SoldierDeath *death)
{
	delete _death;
	_death = death;

	// Clean up associations
	_craft = 0;
	_psiTraining = false;
	_recentlyPromoted = false;
	_recovery = 0;
	for (std::vector<EquipmentLayoutItem*>::iterator i = _equipmentLayout.begin(); i != _equipmentLayout.end(); ++i)
	{
		delete *i;
	}
	_equipmentLayout.clear();
}

/**
 * Returns the soldier's diary.
 * @return Diary.
 */
SoldierDiary *Soldier::getDiary()
{
	return _diary;
}

/**
 * Calculates the soldier's statString
 * Calculates the soldier's statString.
 * @param statStrings List of statString rules.
 * @param psiStrengthEval Are psi stats available?
 */
void Soldier::calcStatString(const std::vector<StatString *> &statStrings, bool psiStrengthEval)
{
	_statString = StatString::calcStatString(_currentStats, statStrings, psiStrengthEval);
}

}
