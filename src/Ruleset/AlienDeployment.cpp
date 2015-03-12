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
#include "AlienDeployment.h"
#include "../Engine/RNG.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::ItemSet>
	{
		static Node encode(const OpenXcom::ItemSet& rhs)
		{
			Node node;
			node = rhs.items;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::ItemSet& rhs)
		{
			if (!node.IsSequence())
				return false;

			rhs.items = node.as< std::vector<std::string> >(rhs.items);
			return true;
		}
	};
	template<>
	struct convert<OpenXcom::DeploymentData>
	{
		static Node encode(const OpenXcom::DeploymentData& rhs)
		{
			Node node;
			node["alienRank"] = rhs.alienRank;
			node["lowQty"] = rhs.lowQty;
			node["highQty"] = rhs.highQty;
			node["dQty"] = rhs.dQty;
			node["extraQty"] = rhs.extraQty;
			node["percentageOutsideUfo"] = rhs.percentageOutsideUfo;
			node["itemSets"] = rhs.itemSets;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::DeploymentData& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.alienRank = node["alienRank"].as<int>(rhs.alienRank);
			rhs.lowQty = node["lowQty"].as<int>(rhs.lowQty);
			rhs.highQty = node["highQty"].as<int>(rhs.highQty);
			rhs.dQty = node["dQty"].as<int>(rhs.dQty);
			rhs.extraQty = node["extraQty"].as<int>(0); // give this a default, as it's not 100% needed, unlike the others.
			rhs.percentageOutsideUfo = node["percentageOutsideUfo"].as<int>(rhs.percentageOutsideUfo);
			rhs.itemSets = node["itemSets"].as< std::vector<OpenXcom::ItemSet> >(rhs.itemSets);
			return true;
		}
	};
	template<>
	struct convert<OpenXcom::BriefingData>
	{
		static Node encode(const OpenXcom::BriefingData& rhs)
		{
			Node node;
			node["palette"] = rhs.palette;
			node["textOffset"] = rhs.textOffset;
			node["music"] = rhs.music;
			node["background"] = rhs.background;
			node["showCraft"] = rhs.showCraft;
			node["showTarget"] = rhs.showTarget;
			return node;
		}
		static bool decode(const Node& node, OpenXcom::BriefingData& rhs)
		{
			if (!node.IsMap())
				return false;
			rhs.palette = node["palette"].as<int>(rhs.palette);
			rhs.textOffset = node["textOffset"].as<int>(rhs.textOffset);
			rhs.music = node["music"].as<std::string>(rhs.music);
			rhs.background = node["background"].as<std::string>(rhs.background);
			rhs.showCraft = node["showCraft"].as<bool>(rhs.showCraft);
			rhs.showTarget = node["showTarget"].as<bool>(rhs.showTarget);
			return true;
		}
	};
}

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of deployment data.
 * @param type String defining the type.
 */
AlienDeployment::AlienDeployment(const std::string &type) : _type(type), _width(0), _length(0), _height(0), _civilians(0), _shade(-1), _noRetreat(false), _finalDestination(false), _finalMission(false), _markerIcon(-1), _durationMin(0), _durationMax(0)
{
}

/**
 *
 */
AlienDeployment::~AlienDeployment()
{
}

/**
 * Loads the Deployment from a YAML file.
 * @param node YAML node.
 */
void AlienDeployment::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_data = node["data"].as< std::vector<DeploymentData> >(_data);
	_width = node["width"].as<int>(_width);
	_length = node["length"].as<int>(_length);
	_height = node["height"].as<int>(_height);
	_civilians = node["civilians"].as<int>(_civilians);
	_terrains = node["terrains"].as<std::vector<std::string> >(_terrains);
	_shade = node["shade"].as<int>(_shade);
	_nextStage = node["nextStage"].as<std::string>(_nextStage);
	_race = node["race"].as<std::string>(_race);
	_noRetreat = node["noRetreat"].as<bool>(_noRetreat);
	_finalDestination = node["finalDestination"].as<bool>(_finalDestination);
	_finalMission = node["finalMission"].as<bool>(_finalMission);
	_script = node["script"].as<std::string>(_script);
	_alert = node["alert"].as<std::string>(_alert);
	_briefingData = node["briefing"].as<BriefingData>(_briefingData);
	_markerName = node["markerName"].as<std::string>(_markerName);
	_markerIcon = node["markerIcon"].as<int>(_markerIcon);
	if (node["duration"])
	{
		_durationMin = node["duration"][0].as<int>(_durationMin);
		_durationMax = node["duration"][1].as<int>(_durationMax);
	}
}

/**
 * Returns the language string that names
 * this deployment. Each deployment type has a unique name.
 * @return Deployment name.
 */
std::string AlienDeployment::getType() const
{
	return _type;
}

/**
 * Gets a pointer to the data.
 * @return Pointer to the data.
 */
std::vector<DeploymentData>* AlienDeployment::getDeploymentData()
{
	return &_data;
}

/**
 * Gets dimensions.
 * @param width Width.
 * @param length Length.
 * @param height Height.
 */
void AlienDeployment::getDimensions(int *width, int *length, int *height)
{
	*width = _width;
	*length = _length;
	*height = _height;
}

/**
 * Gets the number of civilians.
 * @return The number of civilians.
 */
int AlienDeployment::getCivilians() const
{
	return _civilians;
}

/**
 * Gets the terrain for battlescape generation.
 * @return The terrain.
 */
std::vector<std::string> AlienDeployment::getTerrains() const
{
	return _terrains;
}

/**
 * Gets the shade level for battlescape generation.
 * @return The shade level.
 */
int AlienDeployment::getShade() const
{
	return _shade;
}

/**
 * Gets the next stage of the mission.
 * @return The next stage of the mission.
 */
std::string AlienDeployment::getNextStage() const
{
	return _nextStage;
}

/**
 * Gets the race to use on the next stage of the mission.
 * @return The race for the next stage of the mission.
 */
std::string AlienDeployment::getRace() const
{
	return _race;
}

/**
 * Gets the script to use to generate a mission of this type.
 * @return The script to use to generate a mission of this type.
 */
std::string AlienDeployment::getScript() const
{
	return _script;
}

/**
 * Gets if aborting this mission will fail the game.
 * @return if aborting this mission will fail the game.
 */
bool AlienDeployment::isNoRetreat() const
{
	return _noRetreat;
}

/**
 * Gets if winning this mission completes the game.
 * @return if winning this mission completes the game.
 */
bool AlienDeployment::isFinalDestination() const
{
	return _finalDestination;
}

/**
 * Gets if winning this mission completes the game.
 * @return if winning this mission completes the game.
 */
bool AlienDeployment::isFinalMission() const
{
	return _finalMission;
}

/**
 * Gets the alert message displayed when this mission spawns.
 * @return String ID for the message.
 */
std::string AlienDeployment::getAlertMessage() const
{
	return _alert;
}

/**
 * Gets the briefing data for this mission type.
 * @return data for the briefing window to use.
 */
BriefingData AlienDeployment::getBriefingData() const
{
	return _briefingData;
}

/**
 * Returns the globe marker name for this mission.
 * @return String ID for marker name.
 */
std::string AlienDeployment::getMarkerName() const
{
	return _markerName;
}

/**
 * Returns the globe marker icon for this mission.
 * @return Marker sprite, -1 if none.
 */
int AlienDeployment::getMarkerIcon() const
{
	return _markerIcon;
}

/**
 * Returns the minimum duration for this mission type.
 * @return Duration in hours.
 */
int AlienDeployment::getDurationMin() const
{
	return _durationMin;
}

/**
 * Returns the maximum duration for this mission type.
 * @return Duration in hours.
 */
int AlienDeployment::getDurationMax() const
{
	return _durationMax;
}

}
