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
#include "RuleScriptedEvent.h"
#include "RuleMissionSequence.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::MissionData>
	{
		static Node encode(const OpenXcom::MissionData& rhs)
		{
			Node node;
			node["deployment"] = rhs.deployment_id;
			node["race"] = rhs.race_id;
			node["briefingScreen"] = rhs.briefingScreen;

			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionData& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.deployment_id = node["deployment"].as<std::string>(rhs.deployment_id);
			rhs.race_id = node["race"].as<std::string>(rhs.race_id);
			rhs.briefingScreen = node["briefingScreen"].as<OpenXcom::MissionScreenData>(rhs.briefingScreen);

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::MissionScreenData>
	{
		static Node encode(const OpenXcom::MissionScreenData& rhs)
		{
			Node node;
			node["background"] = rhs.background_id;
			node["title"] = rhs.title_id;
			node["music"] = rhs.music_id;
			node["palette"] = rhs.palette;
			node["mainText"] = rhs.mainText_id;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionScreenData& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.background_id = node["background"].as<std::string>(rhs.background_id);
			rhs.title_id = node["title"].as<std::string>(rhs.title_id);
			rhs.palette = node["palette"].as<int>(rhs.palette);
			rhs.music_id = node["music"].as<std::string>(rhs.music_id);
			rhs.mainText_id = node["mainText"].as<std::string>(rhs.mainText_id);
			return true;
		}
	};

}

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of mission screen
 * @param type String defining the type.
 */
RuleMissionSequence::RuleMissionSequence(const std::string &type) : _type(type)
{
}

/**
 *
 */
RuleMissionSequence::~RuleMissionSequence()
{
}

/**
 * Loads the Scripted Event from a YAML file.
 * @param node YAML node.
 * @param ruleset Ruleset for the Scripted Event.
 */
void RuleMissionSequence::load(const YAML::Node &node, Ruleset *ruleset)
{
	_type = node["type"].as<std::string>(_type);

	_missionSequence = node["missionSequence"].as<std::vector<MissionData>>(_missionSequence);
}


/**
 * Gets the language string that names
 * this Scripted Event. Each Scripted Event has a unique name.
 * @return The Scripted Event name.
 */
const std::string &RuleMissionSequence::getType() const
{
	return _type;
}

/**
 * Returns the number of missions in this event
 * @return number of missions
 */
const int RuleMissionSequence::getNumMissions() const
{
	return _missionSequence.size();
}

/**
 * Returns the mission at the index provided
 * @param i number of mission
 * @return mission data
 */

const MissionData &RuleMissionSequence::getMission(int i) const
{
	return _missionSequence.at(i);
}

}

