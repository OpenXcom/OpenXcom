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
#ifndef OPENXCOM_STATSTRINGCONDITION_H
#define OPENXCOM_STATSTRINGCONDITION_H

#include <string>

namespace OpenXcom
{

class StatStringCondition 
{
private:
	std::string _conditionName;
	int _minVal;
	int _maxVal;
public:
	/// Creates a blank StatStringCondition.
	StatStringCondition(const std::string &conditionName, int minVal, int maxVal);
	/// Cleans up the StatStringCondition.
	virtual ~StatStringCondition();
	/// Get MinVal.
	std::string getConditionName();
	/// Get MinVal.
	int getMinVal();
	/// Get MaxVal.
	int getMaxVal();
};

}
#endif