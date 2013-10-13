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
#include "Unit.h"

namespace OpenXcom
{

/**
 * Creates a certain type of unit.
 * @param type String defining the type.
 * @param race String defining the race.
 * @param rank String defining the rank.
 */
Unit::Unit(const std::string &type, std::string race, std::string rank) : _type(type), _race(race), _rank(rank), _stats(), _armor(""), _standHeight(0), _kneelHeight(0), _floatHeight(0),
																		_value(0), _deathSound(0), _aggroSound(-1), _moveSound(-1), _intelligence(0), _aggression(0), _specab(SPECAB_NONE),
																		_zombieUnit(""), _spawnUnit(""), _livingWeapon(false)
{
}

/**
 *
 */
Unit::~Unit()
{

}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void Unit::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_race = node["race"].as<std::string>(_race);
	_rank = node["rank"].as<std::string>(_rank);
	_stats = node["stats"].as<UnitStats>(_stats);
	_armor = node["armor"].as<std::string>(_armor);
	_standHeight = node["standHeight"].as<int>(_standHeight);
	_kneelHeight = node["kneelHeight"].as<int>(_kneelHeight);
	_floatHeight = node["floatHeight"].as<int>(_floatHeight);
	_value = node["value"].as<int>(_value);
	_deathSound = node["deathSound"].as<int>(_deathSound);
	_aggroSound = node["aggroSound"].as<int>(_aggroSound);
	_moveSound = node["moveSound"].as<int>(_moveSound);
	_intelligence = node["intelligence"].as<int>(_intelligence);
	_aggression = node["aggression"].as<int>(_aggression);
	_specab = (SpecialAbility)node["specab"].as<int>(_specab);
	_zombieUnit = node["zombieUnit"].as<std::string>(_zombieUnit);
	_spawnUnit = node["spawnUnit"].as<std::string>(_spawnUnit);
	_livingWeapon = node["livingWeapon"].as<bool>(_livingWeapon);
	_extraWeight = node["extraWeight"].as<int>(_extraWeight);
}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return The unit's name.
 */
std::string Unit::getType() const
{
	return _type;
}

/**
 * Returns the unit's stats data object.
 * @return The unit's stats.
 */
UnitStats *Unit::getStats()
{
	return &_stats;
}

/**
 * Returns the unit's height at standing.
 * @return The unit's height.
 */
int Unit::getStandHeight() const
{
	return _standHeight;
}

/**
 * Returns the unit's height at kneeling.
 * @return The unit's kneeling height.
 */
int Unit::getKneelHeight() const
{
	return _kneelHeight;
}

/**
 * Returns the unit's floating elevation.
 * @return The unit's floating height.
 */
int Unit::getFloatHeight() const
{
	return _floatHeight;
}

/**
 * Gets the unit's armor type.
 * @return The unit's armor type.
 */
std::string Unit::getArmor() const
{
	return _armor;
}

/**
 * Gets the alien's race.
 * @return The alien's race.
 */
std::string Unit::getRace() const
{
	return _race;
}

/**
 * Gets the unit's rank.
 * @return The unit's rank.
 */
std::string Unit::getRank() const
{
	return _rank;
}

/**
 * Gets the unit's value - for scoring.
 * @return The unit's value.
 */
int Unit::getValue() const
{
	return _value;
}

/**
 * Gets the unit's death sound.
 * @return The id of the unit's death sound.
 */
int Unit::getDeathSound() const
{
	return _deathSound;
}

/**
 * Gets the unit's move sound.
 * @return The id of the unit's move sound.
 */
int Unit::getMoveSound() const
{
	return _moveSound;
}

/**
 * Gets the intelligence. This is the number of turns the AI remembers your troop positions.
 * @return The unit's intelligence.
 */
int Unit::getIntelligence() const
{
	return _intelligence;
}

/**
 * Gets the aggression. Determines the chance of revenge and taking cover.
 * @return The unit's aggression.
 */
int Unit::getAggression() const
{
	return _aggression;
}

/**
 * Gets the unit's special ability.
 * @return The unit's specab.
 */
int Unit::getSpecialAbility() const
{
	return (int)_specab;
}

/**
 * Gets the unit that the victim is morphed into when attacked.
 * @return The unit's zombie unit.
 */
std::string Unit::getZombieUnit() const
{
	return _zombieUnit;
}

/**
 * Gets the unit that is spawned when this one dies.
 * @return The unit's spawn unit.
 */
std::string Unit::getSpawnUnit() const
{
	return _spawnUnit;
}

/**
 * Gets the unit's war cry.
 * @return The id of the unit's aggro sound.
 */
int Unit::getAggroSound() const
{
	return _aggroSound;
}

/**
 * Checks if this unit is a living weapon. (ie: chryssalid).
 * @return True if this unit is a living weapon.
 */
bool Unit::isLivingWeapon() const
{
	return _livingWeapon;
}

/**
 * Gets the extra weight when calculating the carried weight
 * @return The extra weight.
 */
int Unit::getExtraWeight() const
{
	return _extraWeight;
}
}
