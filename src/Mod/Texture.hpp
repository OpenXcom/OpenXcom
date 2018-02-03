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
#include "Texture.h"

#include <yaml-cpp/yaml.h>

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
