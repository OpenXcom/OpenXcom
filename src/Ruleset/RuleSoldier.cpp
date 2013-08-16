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

UnitStats RuleSoldier::getMinStats() const
{
	return _minStats;
}
///
UnitStats RuleSoldier::getMaxStats() const
{
	return _maxStats;
}
///
UnitStats RuleSoldier::getStatCaps() const
{
	return _statCaps;
}
///
int RuleSoldier::getStandHeight() const
{
	return _standHeight;
}
///
int RuleSoldier::getKneelHeight() const
{
	return _kneelHeight;
}
///
int RuleSoldier::getFloatHeight() const
{
	return _floatHeight;
}

std::string RuleSoldier::getArmor() const
{
	return _armor;
}

}
