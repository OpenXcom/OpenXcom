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
#include <vector>

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
    std::string conditionNames[] = {"psiStrength", "psiSkill", "bravery", "strength", "firing", "reactions", "stamina", "tu", "health", "throwing"};
	_stringToBeAddedIfAllConditionsAreMet = node["string"].as<std::string>(_stringToBeAddedIfAllConditionsAreMet);
    for (unsigned int i = 0; i < sizeof(conditionNames)/sizeof(conditionNames[0]); i++)
	{
        if (node[conditionNames[i]])
		{
            _conditions.push_back(getCondition(conditionNames[i], node));
        }
    }
}

StatStringCondition *StatString::getCondition(const std::string &conditionName, const YAML::Node &node)
{
	// These are the defaults from xcomutil
	int minValue = 0, maxValue = 255;
	if (node[conditionName][0]) {
		minValue = node[conditionName][0].as<int>(minValue);
	}
	if (node[conditionName][1]) {
		maxValue = node[conditionName][1].as<int>(maxValue);
	}
	StatStringCondition *thisCondition = new StatStringCondition(conditionName, minValue, maxValue);
	return thisCondition;
}

const std::vector< StatStringCondition* > StatString::getConditions()
{
	return _conditions;
}

const std::string StatString::getString()
{
	return _stringToBeAddedIfAllConditionsAreMet;
}

const std::wstring StatString::calcStatString(UnitStats &currentStats, const std::vector<StatString *> &statStrings)
{
	unsigned int conditionsMet;
	int minVal, maxVal;
	std::string conditionName, string;
	std::wstring wstring;
	std::wstring statString = L"";
	bool continueCalc = true;
	std::map<std::string, int> currentStatsMap = getCurrentStats(currentStats);

	for (std::vector<StatString *>::const_iterator i1 = statStrings.begin(); i1 != statStrings.end() && continueCalc; ++i1)
	{
		string = (*i1)->getString();
		const std::vector<StatStringCondition* > conditions = (*i1)->getConditions();
		conditionsMet = 0;
		for (std::vector<StatStringCondition* >::const_iterator i2 = conditions.begin(); i2 != conditions.end() && continueCalc; ++i2)
		{
			conditionName = (*i2)->getConditionName();
			minVal = (*i2)->getMinVal();
			maxVal = (*i2)->getMaxVal();
			if (currentStatsMap.find(conditionName) != currentStatsMap.end())
			{
				if (currentStatsMap[conditionName] >= minVal && currentStatsMap[conditionName] <= maxVal) {
						conditionsMet++;
				}
				if (conditionsMet == conditions.size()) {
					wstring.assign(string.begin(), string.end());
					statString = statString + wstring;
					if (wstring.length() > 1) {
						continueCalc = false;
					}
				}
			}
		}
	}
	return statString;
}

/**
 * Get the ConditionNames vector.
 */
std::vector<std::string> StatString::getConditionNames()
{
	std::vector<std::string> conditionNames;
	conditionNames.push_back("psiStrength");
	conditionNames.push_back("psiSkill");
	conditionNames.push_back("bravery");
	conditionNames.push_back("strength");
	conditionNames.push_back("firing");
	conditionNames.push_back("reactions");
	conditionNames.push_back("stamina");
	conditionNames.push_back("tu");
	conditionNames.push_back("health");
	conditionNames.push_back("throwing");
	return conditionNames;
}

/**
 * Get a map of currentStats.
 */
std::map<std::string, int> StatString::getCurrentStats(UnitStats &currentStats)
{
	std::map<std::string, int> currentStatsMap;
	currentStatsMap["psiStrength"] = currentStats.psiStrength;
	currentStatsMap["psiSkill"] = currentStats.psiSkill;
	currentStatsMap["bravery"] = currentStats.bravery;
	currentStatsMap["strength"] = currentStats.strength;
	currentStatsMap["firing"] = currentStats.firing;
	currentStatsMap["reactions"] = currentStats.reactions;
	currentStatsMap["stamina"] = currentStats.stamina;
	currentStatsMap["tu"] = currentStats.tu;
	currentStatsMap["health"] = currentStats.health;
	currentStatsMap["throwing"] = currentStats.throwing;
	return currentStatsMap;
}


}