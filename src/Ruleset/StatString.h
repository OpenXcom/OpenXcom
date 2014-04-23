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

/*
  statStrings:
  - string: "x"
    psiStrength: [~, 30]
  - string: "P"
    psiStrength: [80, ~]
  - string: "p"
    psiStrength: [60, 79]
  - string: "K"
    psiSkill: [60, ~]
  - string: "k"
    psiSkill: [30, 59]
  - string: "b"
    bravery: [60, ~]
  - string: "c"
    bravery: [~, 10]
  - string: "w"
    strength: [~, 25]
  - string: "Snpr"
    firing: [60, ~]
    reactions: [60, ~]
  - string: "M"
    firing: [70, ~]
  - string: "m"
    firing: [60, 69]
  - string: "Sct"
    reactions: [50, ~]
    stamina: [60, ~]
  - string: "R"
    reactions: [60, ~]
  - string: "r"
    reactions: [50, 59]
*/
class StatString
{
private:
	std::string _string;
	std::vector<double> _minVal, _maxVal;
public:
	/// Creates a blank StatString ruleset.
	StatString();
	/// Cleans up the StatString ruleset.
	virtual ~StatString();
	/// Loads the StatString from YAML.
	void load(const YAML::Node& node);
};
}
#endif