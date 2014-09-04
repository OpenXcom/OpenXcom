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
#include "AlienDeployment.h"
#include "../Engine/RNG.h"
#include "MapBlock.h"

namespace YAML
{
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
			rhs.percentageOutsideUfo = node["percentageOutsideUfo"].as<int>(rhs.percentageOutsideUfo);
			rhs.itemSets = node["itemSets"].as< std::vector<OpenXcom::ItemSet> >(rhs.itemSets);
			return true;
		}
	};

	template<>
	struct convert<OpenXcom::TerrainFeature>
	{
		static Node encode(const OpenXcom::TerrainFeature& rhs)
		{
			Node node;
			node["type"] = (int)rhs.type;
			node["width"] = rhs.width;
			node["length"] = rhs.length;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::TerrainFeature& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.type = (OpenXcom::MapBlockType)node["type"].as<int>(rhs.type);
			rhs.width = node["width"].as<int>(rhs.width);
			rhs.length = node["length"].as<int>(rhs.length);
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
AlienDeployment::AlienDeployment(const std::string &type) : _type(type), _data(), _width(0), _length(0), _height(0), _showCraft(true), _terrainFeature(), _terrainDefault(), _civilians(0), _shade(-1), _nextStage("")
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
	_showCraft = node["showCraft"].as<bool>(_showCraft);
	_terrainFeature = node["terrainFeature"].as< std::vector<TerrainFeature> >(_terrainFeature);	
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
 * Gets a pointer to the terrain feature list
 * @return Pointer to the data.
 */
std::vector<TerrainFeature>* AlienDeployment::getTerrainFeature()
{
	return &_terrainFeature;
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
 * Gets the flag on whether to show the XCom craft
 * @return The flag
 */
bool AlienDeployment::getShowCraft() const
{
	return _showCraft;
}

}
