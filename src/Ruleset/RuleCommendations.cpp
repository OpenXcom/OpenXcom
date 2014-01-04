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

#include "RuleCommendations.h"

namespace OpenXcom
{

/**
 * Creates a blank set of extra sprite data.
 */
RuleCommendations::RuleCommendations(const std::string &type) : _type(type), _description(""), _criteria(), _bonus(), _listOrder(0)
{
}

/**
 * Cleans up the extra sprite set.
 */
RuleCommendations::~RuleCommendations()
{
}

/**
 * Loads the commendations from YAML.
 * @param node YAML node.
 */
void RuleCommendations::load(YAML::Node &node, int listOrder)
{
	_type = node["type"].as<std::string>(_type);
	_description = node["description"].as<std::string>(_description);
	_criteria = node["critera"].as<std::vector<std::pair<std::string, int>*> >(_criteria);
	_bonus = node["bonus"].as<std::vector<std::pair<std::string, int>*> >(_bonus);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Gets the list order
 * @return The list order.
 */
int RuleCommendations::getListOrder() const
{
	 return _listOrder;
}

/**
 * Get commendation name
 * @return string Commendation name
 */
std::string RuleCommendations::getName() const
{
	return _type;
}

/**
 * Get commendation description
 * @return string Commendation description
 */
std::string RuleCommendations::getDescription() const
{
	return _description;
}

/**
 * Get commendation award criteria
 * @return map<string, int> Commendation criteria
 */
std::vector<std::pair<std::string, int>*> *RuleCommendations::getCriteria()
{
	return _criteria;
}

/**
 * Get commendation award bonuses
 * @return map<string, int> Commendation bonuses
 */
std::vector<std::pair<std::string, int>*> *RuleCommendations::getBonus()
{
	return _bonus;
}

}
