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
#ifndef OPENXCOM_STATSTRING_H
#define OPENXCOM_STATSTRING_H

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * For adding statStrings to the game.
 * See http://ufopaedia.org/index.php?title=Statstrings
 */
struct limits 
{
	int minVal; 
	int maxVal;
};

class StatString
{
private:
	std::string _string;	
	std::map<std::string, limits> _conditions;
	StatString::limits getCondition(const std::string &conditionName, const YAML::Node &node);
public:
	/// Creates a blank StatString ruleset.
	StatString();
	/// Cleans up the StatString ruleset.
	virtual ~StatString();
	/// Loads the StatString from YAML.
	void load(const YAML::Node& node);
 	/// Get the conditions for this StatString.
	std::map<std::string, limits> getConditions();
};

}
#endif