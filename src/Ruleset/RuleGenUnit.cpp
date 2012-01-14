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
#include "RuleGenUnit.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleunit for a certain type of unit.
 * @param type String defining the type.
 * @param race String defining the race.
 * @param rank String defining the rank.
 */
RuleGenUnit::RuleGenUnit(const std::string &type, std::string race, std::string rank) : _type(type), _race(race), _rank(rank), _armor(""), _standHeight(0), _kneelHeight(0), _loftemps(0), _value(0), _deathSound(0), _intelligence(0), _aggression(0)
{

}

/**
 *
 */
RuleGenUnit::~RuleGenUnit()
{

}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return Unit name.
 */
std::string RuleGenUnit::getType() const
{
	return _type;
}

/**
 * Sets the unit's stats data object.
 * @param Stats.
 */
void RuleGenUnit::setStats(struct UnitStats stats)
{
	_stats = stats;
}

/**
 * Sets the unit's armor type.
 * @param string.
 */
void RuleGenUnit::setArmor(const std::string &armor)
{
	_armor = armor;
}

/**
 * Set the unit's height at standing, kneeling and 3D template.
 * @param string.
 */
void RuleGenUnit::setVoxelParameters(int standHeight, int kneelHeight, int loftemps)
{
	_standHeight = standHeight;
	_kneelHeight = kneelHeight;
	_loftemps = loftemps;
}

/**
 * Set the unit's intelligence and aggression.
 * @param string.
 */
void RuleGenUnit::setAIParameters(int intelligence, int aggression)
{
	_intelligence = intelligence;
	_aggression = aggression;
}

/**
 * Returns the unit's stats data object.
 * @return Stats.
 */
UnitStats RuleGenUnit::getStats() const
{
	return _stats;
}

/**
 * Returns the unit's height at standing.
 * @return height.
 */
int RuleGenUnit::getStandHeight() const
{
	return _standHeight;
}

/**
 * Returns the unit's height at kneeling.
 * @return height.
 */
int RuleGenUnit::getKneelHeight() const
{
	return _kneelHeight;
}

/**
 * Returns the unit's 3D template ID.
 * @return template ID.
 */
int RuleGenUnit::getLoftemps() const
{
	return _loftemps;
}

/**
 * Gets the unit's armor type.
 * @return string.
 */
std::string RuleGenUnit::getArmor() const
{
	return _armor;
}

/**
 * Gets the alien's race.
 * @return string.
 */
std::string RuleGenUnit::getRace() const
{
	return _race;
}

/**
 * Gets the alien's rank.
 * @return string.
 */
std::string RuleGenUnit::getRank() const
{
	return _rank;
}

/**
 * Set the unit's value - for scoring.
 * @param value.
 */
void RuleGenUnit::setValue(int value)
{
	_value = value;
}

/**
 * Get the unit's value - for scoring.
 * @return value.
 */
int RuleGenUnit::getValue() const
{
	return _value;
}

/**
 * Set the unit's death sound.
 * @param id.
 */
void RuleGenUnit::setDeathSound(int id)
{
	_deathSound = id;
}

/**
 * Get the unit's death sound.
 * @return id.
 */
int RuleGenUnit::getDeathSound() const
{
	return _deathSound;
}

/**
 * Get the intelligence. This is the number of turns AI remembers your troops position.
 * @return intelligence.
 */
int RuleGenUnit::getIntelligence() const
{
	return _intelligence;
}

/**
 * Get the aggression. Determines the chance of revenge and taking cover.
 * @return intelligence.
 */
int RuleGenUnit::getAggression() const
{
	return _aggression;
}

}
