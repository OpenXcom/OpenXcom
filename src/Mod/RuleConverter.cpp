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
#include "RuleConverter.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for converter data.
 */
RuleConverter::RuleConverter()
{
}

/**
 *
 */
RuleConverter::~RuleConverter()
{
}

/**
 * Loads the converter data from a YAML file.
 * @param node YAML node.
 */
void RuleConverter::load(const YAML::Node &node)
{
	_offsets = node["offsets"].as< std::map<std::string, int> >(_offsets);
	_markers = node["markers"].as< std::vector<std::string> >(_markers);
	_countries = node["countries"].as< std::vector<std::string> >(_countries);
	_regions = node["regions"].as< std::vector<std::string> >(_regions);
	_facilities = node["facilities"].as< std::vector<std::string> >(_facilities);
	_items = node["items"].as< std::vector<std::string> >(_items);
	_crews = node["crews"].as< std::vector<std::string> >(_crews);
	_crafts = node["crafts"].as< std::vector<std::string> >(_crafts);
	_ufos = node["ufos"].as< std::vector<std::string> >(_ufos);
	_craftWeapons = node["craftWeapons"].as< std::vector<std::string> >(_craftWeapons);
	_missions = node["missions"].as< std::vector<std::string> >(_missions);
	_armor = node["armor"].as< std::vector<std::string> >(_armor);
	_alienRaces = node["alienRaces"].as< std::vector<std::string> >(_alienRaces);
	_alienRanks = node["alienRanks"].as< std::vector<std::string> >(_alienRanks);
	_research = node["research"].as< std::vector<std::string> >(_research);
	_manufacture = node["manufacture"].as< std::vector<std::string> >(_manufacture);
	_ufopaedia = node["ufopaedia"].as< std::vector<std::string> >(_ufopaedia);
}

}
