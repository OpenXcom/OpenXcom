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
#ifndef OPENXCOM_TEXTURE_H
#define OPENXCOM_TEXTURE_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

struct TerrainCriteria
{
	std::string name;
	int weight;
	double lonMin, lonMax, latMin, latMax;
	TerrainCriteria() : weight(1), lonMin(0.0), lonMax(360.0), latMin(-90.0), latMax(90.0) {};
};

class Target;

/**
 * Represents the relations between a Geoscape texture
 * and the corresponding Battlescape mission attributes.
 */
class Texture
{
private:
	int _id;
	std::string _deployment;
	std::vector<TerrainCriteria> _terrain;
public:
	/// Creates a new texture with mission data.
	Texture(int id);
	/// Cleans up the texture.
	~Texture();
	/// Loads the texture from YAML.
	void load(const YAML::Node& node);
	/// Gets the list of terrain criteria.
	std::vector<TerrainCriteria> *getTerrain();
	/// Gets a random texture terrain for a given target.
	std::string getRandomTerrain(Target *target) const;
	/// Gets the alien deployment for this texture.
	std::string getDeployment() const;
};

}

namespace YAML
{
	template<>
	struct convert < OpenXcom::TerrainCriteria >
	{
		static Node encode(const OpenXcom::TerrainCriteria& rhs)
		{
			Node node;
			node["name"] = rhs.name;
			node["weight"] = rhs.weight;
			std::vector<double> area;
			area.push_back(rhs.lonMin);
			area.push_back(rhs.lonMax);
			area.push_back(rhs.latMin);
			area.push_back(rhs.latMax);
			node["area"] = area;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::TerrainCriteria& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.name = node["name"].as<std::string>(rhs.name);
			rhs.weight = node["weight"].as<int>(rhs.weight);
			if (node["area"])
			{
				std::vector<double> area = node["area"].as< std::vector<double> >();
				rhs.lonMin = area[0] * M_PI / 180.0;
				rhs.lonMax = area[1] * M_PI / 180.0;
				rhs.latMin = area[2] * M_PI / 180.0;
				rhs.latMax = area[3] * M_PI / 180.0;
			}
			return true;
		}
	};
}

#endif
