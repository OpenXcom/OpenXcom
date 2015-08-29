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

#include "RuleInterface.h"
#include <climits>

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of interface, containing an index of elements that make it up.
 * @param type String defining the type.
 */
RuleInterface::RuleInterface(const std::string & type) : _type(type)
{
}

RuleInterface::~RuleInterface()
{
}

/**
 * Loads the elements from a YAML file.
 * @param node YAML node.
 */
void RuleInterface::load(const YAML::Node& node)
{
	_palette = node["palette"].as<std::string>(_palette);
	_parent = node["parent"].as<std::string>(_parent);
	_music = node["music"].as<std::string>(_music);
	for (YAML::const_iterator i = node["elements"].begin(); i != node["elements"].end(); ++i)
	{
		Element element;
		if ((*i)["size"])
		{
			std::pair<int, int> pos = (*i)["size"].as<std::pair<int, int> >();
			element.w = pos.first;
			element.h = pos.second;
		}
		else
		{
			element.w = element.h = INT_MAX;
		}
		if ((*i)["pos"])
		{
			std::pair<int, int> pos = (*i)["pos"].as<std::pair<int, int> >();
			element.x = pos.first;
			element.y = pos.second;
		}
		else
		{
			element.x = element.y = INT_MAX;
		}
		element.color = (*i)["color"].as<int>(INT_MAX);
		element.color2 = (*i)["color2"].as<int>(INT_MAX);
		element.border = (*i)["border"].as<int>(INT_MAX);
		element.TFTDMode = (*i)["TFTDMode"].as<bool>(false);

		std::string id = (*i)["id"].as<std::string>("");
		_elements[id] = element;
	}
}

/**
 * Retrieves info on an element
 * @param id String defining the element.
 */
Element *RuleInterface::getElement(const std::string &id)
{
	std::map<std::string, Element>::iterator i = _elements.find(id);
	if (_elements.end() != i) return &i->second; else return 0;
}

const std::string &RuleInterface::getPalette() const
{
	return _palette;
}

const std::string &RuleInterface::getParent() const
{
	return _parent;
}

const std::string &RuleInterface::getMusic() const
{
	return _music;
}

}