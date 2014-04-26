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
#include "StatStringCondition.h"


namespace OpenXcom
{
/**
 * Creates a blank StatStringCondition.
 */
StatStringCondition::StatStringCondition(const std::string conditionName, int minVal, int maxVal)
{
	_conditionName = conditionName;
	_minVal = minVal;
	_maxVal = maxVal;
}

/**
 * Cleans up the extra StatStringCondition.
 */
StatStringCondition::~StatStringCondition()
{
}

/// Get ConditionName.
const std::string StatStringCondition::getConditionName()
{
	return _conditionName;
}

/// Get MinVal.
int StatStringCondition::getMinVal()
{
	return _minVal;
}

/// Get MaxVal.
int StatStringCondition::getMaxVal()
{
	return _maxVal;
}

}