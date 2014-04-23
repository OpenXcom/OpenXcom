/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#define _USE_MATH_DEFINES
#include "StatString.h"

namespace OpenXcom
{

/**
 * Creates a blank StatString.
 */
StatString::StatString()
{
}

/**
 * Cleans up the extra StatString.
 */
StatString::~StatString()
{
}

/**
 * Loads the StatString from a YAML file.
 * @param node YAML node.
 */
void StatString::load(const YAML::Node &node)
{
	// NOTE: conditionNames must be terminated with an empty string
	std::string conditionNames[] = {"psiStrength", "psiSkill", "bravery", "strength", "firing", "reactions", "stamina", ""};
	_string = node["string"].as<std::string>(_string);
	int i = 0;
	while (conditionNames[i] != "")
	{
		if (node[conditionNames[i]])
		{
			_conditions[conditionNames[i]] = getCondition(conditionNames[i], node);
		}
		i++;
	}
}

StatStringLimits *StatString::getCondition(const std::string &conditionName, const YAML::Node &node)
{
	int minValue = -999999999, maxValue = 999999999;
	if (node[conditionName][0]) {
		minValue = node[conditionName][0].as<int>(minValue);
	}
	if (node[conditionName][1]) {
		maxValue = node[conditionName][1].as<int>(maxValue);
	}
	StatStringLimits *thisLimit = new StatStringLimits(minValue, maxValue);
	return thisLimit;
}

const std::map<std::string, StatStringLimits* > StatString::getConditions()
{
	return _conditions;
}

}