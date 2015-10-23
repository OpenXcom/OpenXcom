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
#include "RuleSoldier.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleunit for a certain
 * type of soldier.
 * @param type String defining the type.
 */
RuleSoldier::RuleSoldier(const std::string &type) : _type(type), _costBuy(0), _costSalary(0), _standHeight(0), _kneelHeight(0), _floatHeight(0), _femaleFrequency(50)
{
}

/**
 *
 */
RuleSoldier::~RuleSoldier()
{

}

/**
 * Loads the soldier from a YAML file.
 * @param node YAML node.
 */
void RuleSoldier::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	// Just in case
	if (_type == "XCOM")
		_type = "STR_SOLDIER";
	_requires = node["requires"].as< std::vector<std::string> >(_requires);
	_minStats.merge(node["minStats"].as<UnitStats>(_minStats));
	_maxStats.merge(node["maxStats"].as<UnitStats>(_maxStats));
	_statCaps.merge(node["statCaps"].as<UnitStats>(_statCaps));
	_armor = node["armor"].as<std::string>(_armor);
	_costBuy = node["costBuy"].as<int>(_costBuy);
	_costSalary = node["costSalary"].as<int>(_costSalary);
	_standHeight = node["standHeight"].as<int>(_standHeight);
	_kneelHeight = node["kneelHeight"].as<int>(_kneelHeight);
	_floatHeight = node["floatHeight"].as<int>(_floatHeight);
	_femaleFrequency = node["femaleFrequency"].as<int>(_femaleFrequency);
}

/**
 * Returns the language string that names
 * this soldier. Each soldier type has a unique name.
 * @return Soldier name.
 */
std::string RuleSoldier::getType() const
{
	return _type;
}

/**
 * Gets the list of research required to
 * acquire this soldier.
 * @return The list of research IDs.
*/
const std::vector<std::string> &RuleSoldier::getRequirements() const
{
	return _requires;
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
 * Gets the cost of hiring this soldier.
 * @return The cost.
 */
int RuleSoldier::getBuyCost() const
{
	return _costBuy;
}

/**
 * Gets the cost of salary for a month.
 * @return The cost.
 */
int RuleSoldier::getSalaryCost() const
{
	return _costSalary;
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
 * Gets the default armor name.
 * @return The armor name.
 */
std::string RuleSoldier::getArmor() const
{
	return _armor;
}

/**
 * Gets the female appearance ratio.
 * @return The percentage ratio.
 */
int RuleSoldier::getFemaleFrequency() const
{
	return _femaleFrequency;
}

}
