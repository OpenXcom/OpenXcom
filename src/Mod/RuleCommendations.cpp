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

#include "RuleCommendations.h"

namespace OpenXcom
{

/**
 * Creates a blank set of commendation data.
 */
RuleCommendations::RuleCommendations() : _criteria(), _killCriteria(), _description(""), _sprite()
{
}

/**
 * Cleans up the commendation.
 */
RuleCommendations::~RuleCommendations()
{
}

/**
 * Loads the commendations from YAML.
 * @param node YAML node.
 */
void RuleCommendations::load(const YAML::Node &node)
{
	_description = node["description"].as<std::string>(_description);
	_criteria = node["criteria"].as<std::map<std::string, std::vector<int> > >(_criteria);
	_sprite = node["sprite"].as<int>(_sprite);
	_killCriteria = node["killCriteria"].as<std::vector<std::vector<std::pair<int, std::vector<std::string> > > > >(_killCriteria);
}

/**
 * Get the commendation's description.
 * @return string Commendation description.
 */
std::string RuleCommendations::getDescription() const
{
	return _description;
}

/**
 * Get the commendation's award criteria.
 * @return map<string, int> Commendation criteria.
 */
std::map<std::string, std::vector<int> > *RuleCommendations::getCriteria()
{
	return &_criteria;
}

/**
 * Get the commendation's award kill criteria.
 * @return vecotr<string> Commendation kill criteria.
 */
std::vector<std::vector<std::pair<int, std::vector<std::string> > > > *RuleCommendations::getKillCriteria()
{
	return &_killCriteria;
}

/**
 * Get the commendation's sprite.
 * @return int Sprite number.
 */
int RuleCommendations::getSprite() const
{
	return _sprite;
}

}
