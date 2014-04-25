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
	std::string conditionNames[] = {"psiStrength", "psiSkill", "bravery", "strength", "firing", "reactions", "stamina", "tu", "health", "throwing", ""};
	_string = node["string"].as<std::string>(_string);
	int i = 0;
	while (conditionNames[i] != "")
	{
		if (node[conditionNames[i]])
		{
			_conditions.push_back(getCondition(conditionNames[i], node));
		}
		i++;
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
	return _string;
}

const std::wstring StatString::calcStatString(UnitStats &currentStats, const std::vector<StatString *> &statStrings)
{
	unsigned int i1, i2, conditionsMet;
	int minVal, maxVal;
	std::string conditionName, string;
	std::wstring wstring;
	std::wstring statString = L"";
	bool continueCalc = true;
	for (i1=0; i1 < statStrings.size() && continueCalc; i1++)
	{
		string = statStrings[i1]->getString();
		// now, iterate through the conditions found in the StatString
		const std::vector<StatStringCondition* > conditions = statStrings[i1]->getConditions();
		conditionsMet = 0;
		for (i2=0; i2 < conditions.size() && continueCalc; i2++)
		{
			conditionName = conditions[i2]->getConditionName();
			minVal = conditions[i2]->getMinVal();
			maxVal = conditions[i2]->getMaxVal();
			if (conditionName == "psiStrength")
			{
				if (currentStats.psiStrength >= minVal && currentStats.psiStrength <= maxVal) {
					conditionsMet++;
				}
			}
			else if (conditionName == "psiSkill")
			{
				if (currentStats.psiSkill >= minVal && currentStats.psiSkill <= maxVal) {
					conditionsMet++;
				}			
			}
			else if (conditionName == "bravery")
			{
				if (currentStats.bravery >= minVal && currentStats.bravery <= maxVal) {
					conditionsMet++;
				}			
			}
			else if (conditionName == "strength")
			{
				if (currentStats.strength >= minVal && currentStats.strength <= maxVal) {
					conditionsMet++;
				}			
			}
			else if (conditionName == "firing")
			{
				if (currentStats.firing >= minVal && currentStats.firing <= maxVal) {
					conditionsMet++;
				}	
			}
			else if (conditionName == "reactions")
			{
				if (currentStats.reactions >= minVal && currentStats.reactions <= maxVal) {
					conditionsMet++;
				}	
			}
			else if (conditionName == "stamina")
			{
				if (currentStats.stamina >= minVal && currentStats.stamina <= maxVal) {
					conditionsMet++;
				}	
			}
			else if (conditionName == "tu")
			{
				if (currentStats.tu >= minVal && currentStats.tu <= maxVal) {
					conditionsMet++;
				}	
			}
			else if (conditionName == "health")
			{
				if (currentStats.health >= minVal && currentStats.health <= maxVal) {
					conditionsMet++;
				}	
			}
			else if (conditionName == "throwing")
			{
				if (currentStats.throwing >= minVal && currentStats.throwing <= maxVal) {
					conditionsMet++;
				}	
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
	return statString;
}

}