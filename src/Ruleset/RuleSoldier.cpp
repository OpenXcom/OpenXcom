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
#include "RuleSoldier.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleunit for a certain
 * type of soldier.
 * @param type String defining the type.
 */
RuleSoldier::RuleSoldier(const std::string &type) : _type(type), _minStats(), _maxStats(), _statCaps(), _armor(""), _standHeight(0), _kneelHeight(0), _floatHeight(0)
{

}

/**
 *
 */
RuleSoldier::~RuleSoldier()
{

}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 */
void RuleSoldier::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_minStats = node["minStats"].as<UnitStats>(_minStats);
	_maxStats = node["maxStats"].as<UnitStats>(_maxStats);
	_statCaps = node["statCaps"].as<UnitStats>(_statCaps);
	_armor = node["armor"].as<std::string>(_armor);
	_standHeight = node["standHeight"].as<int>(_standHeight);
	_kneelHeight = node["kneelHeight"].as<int>(_kneelHeight);
	_floatHeight = node["floatHeight"].as<int>(_floatHeight);
	_extraWeight = node["extraWeight"].as<int>(_extraWeight);
}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return Unit name.
 */
std::string RuleSoldier::getType() const
{
	return _type;
}

/**
 * Gets the minimum stats for the random stats generator.
 * @return The minimum stats.
 */
UnitStats RuleSoldier::getMinStats() const
{
	return _minStats;
}

/**
 * Gets the maximum stats for the random stats generator.
 * @return The maximum stats.
 */
UnitStats RuleSoldier::getMaxStats() const
{
	return _maxStats;
}

/**
 * Gets the stat caps.
 * @return The stat caps.
 */
UnitStats RuleSoldier::getStatCaps() const
{
	return _statCaps;
}

/**
 * Gets the height of the soldier when it's standing.
 * @return The standing height.
 */
int RuleSoldier::getStandHeight() const
{
	return _standHeight;
}

/**
 * Gets the height of the soldier when it's kneeling.
 * @return The kneeling height.
 */
int RuleSoldier::getKneelHeight() const
{
	return _kneelHeight;
}

/**
 * Gets the elevation of the soldier when it's flying.
 * @return The floating height.
 */
int RuleSoldier::getFloatHeight() const
{
	return _floatHeight;
}

/**
 * Gets the armor name.
 * @return The armor name.
 */
std::string RuleSoldier::getArmor() const
{
	return _armor;
}

/**
 * Gets the extra weight when calculating the carried weight
 * @return The extra weight.
 */
int RuleSoldier::getExtraWeight() const
{
	return _extraWeight;
}

}
