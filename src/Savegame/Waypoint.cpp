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
#include "Waypoint.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a waypoint.
 */
Waypoint::Waypoint() : Target(), _id(0)
{
}

/**
 *
 */
Waypoint::~Waypoint()
{
}

/**
 * Loads the waypoint from a YAML file.
 * @param node YAML node.
 */
void Waypoint::load(const YAML::Node &node)
{
	Target::load(node);
	_id = node["id"].as<int>(_id);
}

/**
 * Saves the waypoint to a YAML file.
 * @return YAML node.
 */
YAML::Node Waypoint::save() const
{
	YAML::Node node = Target::save();
	node["id"] = _id;
	return node;
}

/**
 * Saves the waypoint's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node Waypoint::saveId() const
{
	YAML::Node node = Target::saveId();
	node["type"] = "STR_WAYPOINT";
	node["id"] = _id;
	return node;
}

/**
 * Returns the waypoint's unique ID.
 * @return Unique ID.
 */
int Waypoint::getId() const
{
	return _id;
}

/**
 * Changes the waypoint's unique ID.
 * @param id Unique ID.
 */
void Waypoint::setId(int id)
{
	_id = id;
}

/**
 * Returns the waypoint's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Waypoint::getName(Language *lang) const
{
	return lang->getString("STR_WAY_POINT_").arg(_id);
}

/**
 * Returns the globe marker for the waypoint.
 * @return Marker sprite, -1 if none.
 */
int Waypoint::getMarker() const
{
	return 6;
}

}
