/*
 * Copyright 2010 OpenXcom Developers.
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
#include "../Savegame/Craft.h"
#include "../Ruleset/SoldierNamePool.h"
#include "../Ruleset/RuleSoldier.h"

namespace OpenXcom
{

/**
 * Initializes a new blank soldier.
 * @param rules Soldier ruleset.
 * @param armor Soldier armor.
 */
Soldier::Soldier(RuleSoldier *rules, RuleArmor *armor) : Unit(armor), _name(L""), _rules(rules), _rank(RANK_ROOKIE), _craft(0), _gender(GENDER_MALE), _look(LOOK_BLONDE), _missions(0), _kills(0), _recentlyPromoted(false)
{
	_initialStats.bravery = 0;
	_initialStats.firing = 0;
	_initialStats.health = 0;
	_initialStats.melee = 0;
	_initialStats.psiSkill = 0;
	_initialStats.psiStrength = 0;
	_initialStats.reactions = 0;
	_initialStats.stamina = 0;
	_initialStats.strength = 0;
	_initialStats.throwing = 0;
	_initialStats.tu = 0;

	_currentStats = _initialStats;
}

/**
 * Initializes a new soldier with random stats and a name
 * pulled from a set of SoldierNamePool's.
 * @param rules Soldier ruleset.
 * @param armor Soldier armor.
 * @param names List of name pools.
 */
Soldier::Soldier(RuleSoldier *rules, RuleArmor *armor, const std::vector<SoldierNamePool*> *names, int *id) : Unit(armor), _id(*id), _rules(rules), _rank(RANK_ROOKIE), _craft(0), _missions(0), _kills(0)
{
	UnitStats minStats = rules->getMinStats();
	UnitStats maxStats = rules->getMaxStats();

	_initialStats.tu = RNG::generate(minStats.tu, maxStats.tu);
	_initialStats.stamina = RNG::generate(minStats.stamina, maxStats.stamina);
	_initialStats.health = RNG::generate(minStats.health, maxStats.health);
	_initialStats.bravery = RNG::generate(minStats.bravery, maxStats.bravery);
	_initialStats.reactions = RNG::generate(minStats.reactions, maxStats.reactions);
	_initialStats.firing = RNG::generate(minStats.firing, maxStats.firing);
	_initialStats.throwing = RNG::generate(minStats.throwing, maxStats.throwing);
	_initialStats.strength = RNG::generate(minStats.strength, maxStats.strength);
	_initialStats.psiStrength = RNG::generate(minStats.psiStrength, maxStats.psiStrength);
	_initialStats.melee = RNG::generate(minStats.melee, maxStats.melee);
	_initialStats.psiSkill = 0;

	_currentStats = _initialStats;

	int gender;
	_name = names->at(RNG::generate(0, names->size()-1))->genName(&gender);
	_gender = (SoldierGender)gender;
	_look = (SoldierLook)RNG::generate(0, 3);

	(*id)++; // increase id for next soldier
}

/**
 *
 */
Soldier::~Soldier()
{
}

/**
 * Loads the soldier from a YAML file.
 * @param node YAML node.
 */
void Soldier::load(const YAML::Node &node)
{
	int a = 0;
	node["id"] >> _id;
	std::string name;
	node["name"] >> name;
	_name = Language::utf8ToWstr(name);
	node["tu"] >> _initialStats.tu;
	node["stamina"] >> _initialStats.stamina;
	node["health"] >> _initialStats.health;
	node["bravery"] >> _initialStats.bravery;
	node["reactions"] >> _initialStats.reactions;
	node["firing"] >> _initialStats.firing;
	node["throwing"] >> _initialStats.throwing;
	node["strength"] >> _initialStats.strength;
	node["psiStrength"] >> _initialStats.psiStrength;
	node["psiSkill"] >> _initialStats.psiSkill;
	node["melee"] >> _initialStats.melee;
	_currentStats = _initialStats;
	node["rank"] >> a;
	_rank = (SoldierRank)a;
	node["gender"] >> a;
	_gender = (SoldierGender)a;
	node["look"] >> a;
	_look = (SoldierLook)a;
	node["missions"] >> _missions;
	node["kills"] >> _kills;
}

/**
 * Saves the soldier to a YAML file.
 * @param out YAML emitter.
 */
void Soldier::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "name" << YAML::Value << Language::wstrToUtf8(_name);
	out << YAML::Key << "tu" << YAML::Value << _initialStats.tu;
	out << YAML::Key << "stamina" << YAML::Value << _initialStats.stamina;
	out << YAML::Key << "health" << YAML::Value << _initialStats.health;
	out << YAML::Key << "bravery" << YAML::Value << _initialStats.bravery;
	out << YAML::Key << "reactions" << YAML::Value << _initialStats.reactions;
	out << YAML::Key << "firing" << YAML::Value << _initialStats.firing;
	out << YAML::Key << "throwing" << YAML::Value << _initialStats.throwing;
	out << YAML::Key << "strength" << YAML::Value << _initialStats.strength;
	out << YAML::Key << "psiStrength" << YAML::Value << _initialStats.psiStrength;
	out << YAML::Key << "psiSkill" << YAML::Value << _initialStats.psiSkill;
	out << YAML::Key << "melee" << YAML::Value << _initialStats.melee;
	out << YAML::Key << "rank" << YAML::Value << _rank;
	if (_craft != 0)
	{
		out << YAML::Key << "craft" << YAML::Value;
		_craft->saveId(out);
	}
	out << YAML::Key << "gender" << YAML::Value << _gender;
	out << YAML::Key << "look" << YAML::Value << _look;
	out << YAML::Key << "missions" << YAML::Value << _missions;
	out << YAML::Key << "kills" << YAML::Value << _kills;
	out << YAML::EndMap;
}

/**
 * Returns the soldier's full name.
 * @return Soldier name.
 */
std::wstring Soldier::getName(Language *lang) const
{
	return _name;
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
Craft *const Soldier::getCraft() const
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
		break;
	case RANK_SQUADDIE:
		return "STR_SQUADDIE";
		break;
	case RANK_SERGEANT:
		return "STR_SERGEANT";
		break;
	case RANK_CAPTAIN:
		return "STR_CAPTAIN";
		break;
	case RANK_COLONEL:
		return "STR_COLONEL";
		break;
	case RANK_COMMANDER:
		return "STR_COMMANDER";
		break;
	}
	return "";
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
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int Soldier::getTimeUnits() const
{
	return _currentStats.tu;
}

/**
 * Returns the soldier's amount of stamina.
 * @return Stamina.
 */
int Soldier::getStamina() const
{
	return _currentStats.stamina;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int Soldier::getHealth() const
{
	return _currentStats.health;
}

/**
 * Returns the soldier's amount of bravery.
 * @return Bravery.
 */
int Soldier::getBravery() const
{
	return _currentStats.bravery;
}

/**
 * Returns the soldier's amount of reactions.
 * @return Reactions.
 */
int Soldier::getReactions() const
{
	return _currentStats.reactions;
}

/**
 * Returns the soldier's amount of firing accuracy.
 * @return Firing accuracy.
 */
int Soldier::getFiringAccuracy() const
{
	return _currentStats.firing;
}

/**
 * Returns the soldier's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int Soldier::getThrowingAccuracy() const
{
	return _currentStats.throwing;
}

/**
 * Returns the soldier's amount of melee accuracy.
 * @return Melee accuracy.
 */
int Soldier::getMeleeAccuracy() const
{
	return _currentStats.melee;
}

/**
 * Returns the soldier's amount of strength.
 * @return Strength.
 */
int Soldier::getStrength() const
{
	return _currentStats.strength;
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
 * Returns the soldier's stand height.
 * @return stand height
 */
int Soldier::getStandHeight() const
{
	return _rules->getStandHeight();
}

/**
 * Returns the soldier's kneel height.
 * @return kneel height
 */
int Soldier::getKneelHeight() const
{
	return _rules->getKneelHeight();
}
/**
 * Returns the soldier's loftemps ID.
 * @return loftemps ID
 */
int Soldier::getLoftemps() const
{
	return _rules->gotLoftemps();
}

/**
 * Returns the soldier's value.
 * Determines the victory point loss if he dies on a mission and
 * may also affect the likelihood and severity of allied morale checks.
 * @return value
 */
int Soldier::getValue() const
{
	int rankbonus = 0;

	switch (_rank)
	{
	case RANK_SERGEANT:  rankbonus =  1; break;
	case RANK_CAPTAIN:   rankbonus =  3; break;
	case RANK_COLONEL:   rankbonus =  6; break;
	case RANK_COMMANDER: rankbonus = 10; break;
	}

	return 20 + _missions + rankbonus;
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
void Soldier::addKillCount()
{
	_kills++;
}

/**
 * Get pointer to current stats.
 */
UnitStats *Soldier::getCurrentStats()
{
	return &_currentStats;
}

/**
 * Returns whether the unit is affected by fatal wounds.
 * @return bool
 */
bool Soldier::isWoundable() const
{
	return true;
}

/**
 * Returns whether the unit is affected by fear..
 * @return bool
 */
bool Soldier::isFearable() const
{
	return true;
}

/**
 * Returns the unit's intelligence.
 * @return int
 */
int Soldier::getIntelligence() const
{
	return 5;
}

/**
 * Returns the unit's aggression.
 * @return int
 */
int Soldier::getAggression() const
{
	return 1;
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
 * Returns the soldier's special ability. Which is none.
 * @return none.
 */
int Soldier::getSpecialAbility() const
{
	return 0;
}


}
