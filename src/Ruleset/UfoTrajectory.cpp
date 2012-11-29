/*
 * Copyright 2012 OpenXcom Developers.
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
#include "UfoTrajectory.h"

namespace {
const char *altitudeString[] = {
	"STR_GROUND",
	"STR_VERY_LOW",
	"STR_LOW_UC",
	"STR_HIGH_UC",
	"STR_VERY_HIGH"
};
}

namespace OpenXcom
{

/**
 * Send @a wp over @a emitter.
 * @param emitter The YAML emitter.
 * @param wp The waypoint.
 * @return A reference to @a emitter, to allow chaining.
 */
YAML::Emitter &operator<<(YAML::Emitter &emitter, const TrajectoryWaypoint &wp)
{
	emitter << YAML::Flow << YAML::BeginSeq;
	emitter << wp.zone << wp.altitude << wp.speed;
	emitter << YAML::EndSeq;
	return emitter;
}

/**
 * Read @a wp from @a node.
 * @param node The YAML node.
 * @param wp The waypoint.
 * @return If we succeeded in reading.
 */
bool operator>>(const YAML::Node &node, TrajectoryWaypoint &wp)
{
	if (node.Type() != YAML::NodeType::Sequence || node.size() != 3)
	{
		return false;
	}
	YAML::Iterator ii = node.begin();
	(*ii) >> wp.zone;
	(*++ii) >> wp.altitude;
	(*++ii) >> wp.speed;
	return true;
}

/**
 * Overwrites trajectory data with the data stored in @a node.
 * Only the fields contained in the node will be overwritten.
 * @param node The node containing the new values.
 */
void UfoTrajectory::load(const YAML::Node &node)
{
	node["id"] >> _id;
	if (const YAML::Node *np = node.FindValue("groundTimer"))
	{
		 *np >> _groundTimer;
	}
	if (const YAML::Node *np = node.FindValue("waypoints"))
	{
		*np >> _waypoints;
	}
}

/// Saves the trajectory data to YAML.
void UfoTrajectory::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "groundTimer" << YAML::Value << _groundTimer;
	out << YAML::Key << "waypoints" << YAML::Value << _waypoints;
	out << YAML::EndMap;
}

/// Gets the altitude at a waypoint.
std::string UfoTrajectory::getAltitude(unsigned wp) const
{
	return altitudeString[_waypoints[wp].altitude];
}

}
