/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "StatStringCondition.h"


namespace OpenXcom
{
/**
 * Creates a blank StatStringCondition.
 * @param conditionName Name of the condition.
 * @param minVal Minimum value.
 * @param maxVal Maximum value.
 */
StatStringCondition::StatStringCondition(const std::string &conditionName, int minVal, int maxVal) : _conditionName(conditionName), _minVal(minVal), _maxVal(maxVal)
{
}

/**
 * Cleans up the extra StatStringCondition.
 */
StatStringCondition::~StatStringCondition()
{
}

/**
 * Gets the condition string.
 * @return Name of the associated stat.
 */
std::string StatStringCondition::getConditionName() const
{
	return _conditionName;
}

/**
 * Gets the minimum value for the condition (default is 0).
 * @return The minimum value.
 */
int StatStringCondition::getMinVal() const
{
	return _minVal;
}

/**
 * Gets the maximum value for the condition (default is 255).
 * @return The maximum value.
 */
int StatStringCondition::getMaxVal() const
{
	return _maxVal;
}

/**
 * Checks if this condition is valid for the current stat.
 * @param stat The current soldier stat.
 * @param psi Can we show psi stats?
 * @return If the condition is met.
 */
bool StatStringCondition::isMet(int stat, bool psi) const
{
	if (_conditionName == "psiTraining")
	{
		return true;
	}
	bool conditionMet = (stat >= _minVal && stat <= _maxVal);
	if (_conditionName == "psiStrength" || _conditionName == "psiSkill")
	{
		conditionMet = conditionMet && psi;
	}
	return conditionMet;
}

}
