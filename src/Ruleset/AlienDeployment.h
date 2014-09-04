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
#ifndef OPENXCOM_ALIENDEPLOYMENT_H
#define OPENXCOM_ALIENDEPLOYMENT_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "MapBlock.h"

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;
class MapBlock;

struct ItemSet
{
	std::vector<std::string> items;
};

struct DeploymentData
{
	int alienRank;
	int lowQty, highQty, dQty;
	int percentageOutsideUfo;
	std::vector<ItemSet> itemSets;
};

struct TerrainDefault
{
	MapBlockType type;
	int width, length, weight;
};

struct TerrainFeature
{
	MapBlockType type;
	int width, length;
};

/**
 * Represents a specific type of Alien Deployment.
 * Contains constant info about a Alien Deployment like
 * the number of aliens for each alien type and what items they carry
 * (itemset depends on alien technology advancement level 0, 1 or 2).
 * - deployment type can be a craft's name, but also alien base or cydonia.
 * - alienRank is used to check which nodeRanks can be used to deploy this unit
 *   + to match to a specific unit (=race/rank combination) that should be deployed.
 * @sa Node
 */
class AlienDeployment
{
private:
	std::string _type;
	std::vector<DeploymentData> _data;
	int _width, _length, _height, _civilians;
	std::vector<std::string> _terrains;
	bool _showCraft;
	std::vector<TerrainFeature> _terrainFeature;
	std::vector<TerrainDefault> _terrainDefault;
	int _shade;
	std::string _nextStage;
public:
	/// Creates a blank Alien Deployment ruleset.
	AlienDeployment(const std::string &type);
	/// Cleans up the Alien Deployment ruleset.
	~AlienDeployment();
	/// Loads Alien Deployment data from YAML.
	void load(const YAML::Node& node);
	/// Gets the Alien Deployment's type.
	std::string getType() const;
	/// Gets a pointer to the data.
	std::vector<DeploymentData>* getDeploymentData();
	/// Gets whether or not to show craft on the terrain
	bool getShowCraft() const;
	/// Gets a pointer to the terrain defaults.
	std::vector<TerrainDefault>* getTerrainDefault();
	/// Gets a pointer to the terrain features.
	std::vector<TerrainFeature>* getTerrainFeature();
	/// Gets dimensions.
	void getDimensions(int *width, int *length, int *height);
	/// Gets civilians.
	int getCivilians() const;
	/// Gets the terrain for battlescape generation.
	std::vector<std::string> getTerrains() const;
	/// Gets the shade level for battlescape generation.
	int getShade() const;
	/// Gets the next stage of the mission.
	std::string getNextStage() const;
};

}

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
}

#endif
