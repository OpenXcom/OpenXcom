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
#include "../Savegame/Craft.h"
#include "../Ruleset/SoldierNamePool.h"

namespace OpenXcom
{

/**
 * Initializes a new blank soldier.
 */
Soldier::Soldier() : _name(""), _tu(0), _stamina(0), _health(0), _bravery(0), _reactions(0), _firing(0), _throwing(0), _strength(0), _psiStrength(0), _psiSkill(0), _melee(0), _rank(RANK_ROOKIE), _craft(0), _gender(GENDER_MALE), _look(LOOK_BLONDE), _missions(0), _kills(0)
{

}

/**
 * Initializes a new soldier with random stats and a name
 * pulled from a set of SoldierNamePool's.
 * @param names List of name pools.
 */
Soldier::Soldier(std::vector<SoldierNamePool*> *names) : _psiSkill(0), _rank(RANK_ROOKIE), _craft(0), _missions(0), _kills(0)
{
	_tu = RNG::generate(50, 60);
	_stamina = RNG::generate(40, 70);
	_health = RNG::generate(25, 40);
	_bravery = RNG::generate(1, 6)*10;
	_reactions = RNG::generate(50, 60);
	_firing = RNG::generate(40, 70);
	_throwing = RNG::generate(50, 80);
	_strength = RNG::generate(20, 40);
	_psiStrength = RNG::generate(0, 100);
	_melee = RNG::generate(20, 40);

	int gender;
	_name = names->at(RNG::generate(0, names->size()-1))->genName(&gender);
	_gender = (SoldierGender)gender;
	_look = (SoldierLook)RNG::generate(0, 3);
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
	int a;
	node["name"] >> _name;
	node["tu"] >> _tu;
	node["stamina"] >> _stamina;
	node["health"] >> _health;
	node["bravery"] >> _bravery;
	node["reactions"] >> _reactions;
	node["firing"] >> _firing;
	node["throwing"] >> _throwing;
	node["strength"] >> _strength;
	node["psiStrength"] >> _psiStrength;
	node["psiSkill"] >> _psiSkill;
	node["melee"] >> _melee;
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
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "tu" << YAML::Value << _tu;
	out << YAML::Key << "stamina" << YAML::Value << _stamina;
	out << YAML::Key << "health" << YAML::Value << _health;
	out << YAML::Key << "bravery" << YAML::Value << _bravery;
	out << YAML::Key << "reactions" << YAML::Value << _reactions;
	out << YAML::Key << "firing" << YAML::Value << _firing;
	out << YAML::Key << "throwing" << YAML::Value << _throwing;
	out << YAML::Key << "strength" << YAML::Value << _strength;
	out << YAML::Key << "psiStrength" << YAML::Value << _psiStrength;
	out << YAML::Key << "psiSkill" << YAML::Value << _psiSkill;
	out << YAML::Key << "melee" << YAML::Value << _melee;
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
std::string Soldier::getName() const
{
	return _name;
}

/**
 * Changes the soldier's full name.
 * @param name Soldier name.
 */
void Soldier::setName(const std::string &name)
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
 * @return Sprite ID for rank.
 */
int Soldier::getRankSprite() const
{
	return 42 + _rank;
}

/**
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int Soldier::getTimeUnits() const
{
	return _tu;
}

/**
 * Returns the soldier's amount of stamina.
 * @return Stamina.
 */
int Soldier::getStamina() const
{
	return _stamina;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int Soldier::getHealth() const
{
	return _health;
}

/**
 * Returns the soldier's amount of bravery.
 * @return Bravery.
 */
int Soldier::getBravery() const
{
	return _bravery;
}

/**
 * Returns the soldier's amount of reactions.
 * @return Reactions.
 */
int Soldier::getReactions() const
{
	return _reactions;
}

/**
 * Returns the soldier's amount of firing accuracy.
 * @return Firing accuracy.
 */
int Soldier::getFiringAccuracy() const
{
	return _firing;
}

/**
 * Returns the soldier's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int Soldier::getThrowingAccuracy() const
{
	return _throwing;
}

/**
 * Returns the soldier's amount of strength.
 * @return Strength.
 */
int Soldier::getStrength() const
{
	return _strength;
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

}
