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
#include "Waypoint.h"
#include <sstream>
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
	node["id"] >> _id;
}

/**
 * Saves the waypoint to a YAML file.
 * @param out YAML emitter.
 */
void Waypoint::save(YAML::Emitter &out) const
{
	Target::save(out);
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Saves the waypoint's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void Waypoint::saveId(YAML::Emitter &out) const
{
	Target::saveId(out);
	out << YAML::Key << "type" << YAML::Value << "STR_WAYPOINT";
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
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
	std::wstringstream name;
	name << lang->getString("STR_WAY_POINT_") << _id;
	return name.str();
}

}
