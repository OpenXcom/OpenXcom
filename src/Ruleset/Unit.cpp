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
}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return Unit name.
 */
std::string Unit::getType() const
{
	return _type;
}

/**
 * Returns the unit's stats data object.
 * @return Stats.
 */
UnitStats *Unit::getStats()
{
	return &_stats;
}

/**
 * Returns the unit's height at standing.
 * @return height.
 */
int Unit::getStandHeight() const
{
	return _standHeight;
}

/**
 * Returns the unit's height at kneeling.
 * @return height.
 */
int Unit::getKneelHeight() const
{
	return _kneelHeight;
}

/**
 * Returns the unit's floating eleavtion.
 * @return height.
 */
int Unit::getFloatHeight() const
{
	return _floatHeight;
}

/**
 * Gets the unit's armor type.
 * @return string.
 */
std::string Unit::getArmor() const
{
	return _armor;
}

/**
 * Gets the alien's race.
 * @return string.
 */
std::string Unit::getRace() const
{
	return _race;
}

/**
 * Gets the alien's rank.
 * @return string.
 */
std::string Unit::getRank() const
{
	return _rank;
}

/**
 * Get the unit's value - for scoring.
 * @return value.
 */
int Unit::getValue() const
{
	return _value;
}

/**
 * Get the unit's death sound.
 * @return id.
 */
int Unit::getDeathSound() const
{
	return _deathSound;
}

/**
 * Get the unit's move sound.
 * @return id.
 */
int Unit::getMoveSound() const
{
	return _moveSound;
}


/**
 * Get the intelligence. This is the number of turns AI remembers your troops position.
 * @return intelligence.
 */
int Unit::getIntelligence() const
{
	return _intelligence;
}

/**
 * Get the aggression. Determines the chance of revenge and taking cover.
 * @return aggression.
 */
int Unit::getAggression() const
{
	return _aggression;
}

/**
 * Get the unit's special ability.
 * @return specab.
 */
int Unit::getSpecialAbility() const
{
	return (int)_specab;
}

/**
 * Get the unit that the victim is morphed into when attacked.
 * @return unit.
 */
std::string Unit::getZombieUnit() const
{
	return _zombieUnit;
}

/**
 * Get the unit that is spawned when this one dies.
 * @return unit.
 */
std::string Unit::getSpawnUnit() const
{
	return _spawnUnit;
}

/**
 * Get the unit's war cry.
 * @return sound number.
 */
int Unit::getAggroSound() const
{
	return _aggroSound;
}

/**
 * is this unit a living weapon? (ie: chryssalid).
 * @return if it is or not.
 */
bool Unit::isLivingWeapon() const
{
	return _livingWeapon;
}

}
