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
#ifndef OPENXCOM_RULECOMMENDATIONS_H
#define OPENXCOM_RULECOMMENDATIONS_H

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * For adding a set of extra sprite data to the game.
 */
class RuleCommendations
{
private:
	std::string _type, _description;
	std::map<std::string, std::vector<int> > _criteria;
	std::map<std::string, std::vector<int> > _bonus;
	int _listOrder;
public:
	/// Creates a blank commendation ruleset.
	RuleCommendations(const std::string &type);
	/// Cleans up the commendation ruleset.
	~RuleCommendations();
	/// Loads the unit data from YAML.
	void load(const YAML::Node& node, int listIndex);
	/// Get list order.
	int getListOrder() const;
	/// Get commendation name
	std::string getName() const;
	/// Get commendation description
	std::string getDescription() const;
	/// Get commendation award criteria
	std::map<std::string, std::vector<int> > getCriteria() const;
};

}

#endif
