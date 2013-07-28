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
#include "AlienDeployment.h"

namespace OpenXcom
{

void operator >> (const YAML::Node& node, ItemSet& s)
{
	node >> s.items;
}

YAML::Emitter& operator << (YAML::Emitter& out, const ItemSet& s)
{
	out << s.items;
	return out;
}

void operator >> (const YAML::Node& node, DeploymentData& s)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "alienRank")
		{
			i.second() >> s.alienRank;
		}
		else if (key == "lowQty")
		{
			i.second() >> s.lowQty;
		}
		else if (key == "highQty")
		{
			i.second() >> s.highQty;
		}
		else if (key == "dQty")
		{
			i.second() >> s.dQty;
		}
		else if (key == "percentageOutsideUfo")
		{
			i.second() >> s.percentageOutsideUfo;
		}
		else if (key == "itemSets")
		{
			i.second() >> s.itemSets;
		}
	}
}

YAML::Emitter& operator << (YAML::Emitter& out, const DeploymentData& s)
{
	out << YAML::BeginMap;
	out << YAML::Key << "alienRank" << YAML::Value << s.alienRank;
	out << YAML::Key << "lowQty" << YAML::Value << s.lowQty;
	out << YAML::Key << "highQty" << YAML::Value << s.highQty;
	out << YAML::Key << "dQty" << YAML::Value << s.dQty;
	out << YAML::Key << "percentageOutsideUfo" << YAML::Value << s.percentageOutsideUfo;
	out << YAML::Key << "itemSets" << YAML::Value << s.itemSets;
	out << YAML::EndMap;
	return out;
}

/**
 * Creates a blank ruleset for a certain
 * type of deployment data.
 * @param type String defining the type.
 */
AlienDeployment::AlienDeployment(const std::string &type) : _type(type), _data(), _width(0), _length(0), _height(0), _civilians(0), _terrain(""), _shade(-1), _nextStage("")
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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "data")
		{
			i.second() >> _data;
		}
		else if (key == "width")
		{
			i.second() >> _width;
		}
		else if (key == "length")
		{
			i.second() >> _length;
		}
		else if (key == "height")
		{
			i.second() >> _height;
		}
		else if (key == "civilians")
		{
			i.second() >> _civilians;
		}
		else if (key == "roadTypeOdds")
		{
			i.second() >> _roadTypeOdds;
		}
		else if (key == "terrain")
		{
			i.second() >> _terrain;
		}
		else if (key == "shade")
		{
			i.second() >> _shade;
		}
		else if (key == "nextStage")
		{
			i.second() >> _nextStage;
		}
	}
}

/**
 * Saves the Deployment to a YAML file.
 * @param out YAML emitter.
 */
void AlienDeployment::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "data" << YAML::Value << _data;
	out << YAML::Key << "width" << YAML::Value << _width;
	out << YAML::Key << "length" << YAML::Value << _length;
	out << YAML::Key << "height" << YAML::Value << _height;
	out << YAML::Key << "civilians" << YAML::Value << _civilians;	
	out << YAML::Key << "roadTypeOdds" << YAML::Value << _roadTypeOdds;
	out << YAML::Key << "terrain" << YAML::Value << _terrain;
	out << YAML::Key << "shade" << YAML::Value << _shade;
	out << YAML::Key << "nextStage" << YAML::Value << _nextStage;
	out << YAML::EndMap;
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

/// Gets a pointer to the data.
std::vector<DeploymentData>* AlienDeployment::getDeploymentData()
{
	return &_data;
}

/// Get dimensions.
void AlienDeployment::getDimensions(int *width, int *length, int *height)
{
	*width = _width;
	*length = _length;
	*height = _height;
}

int AlienDeployment::getCivilians() const
{
	return _civilians;
}

std::vector<int> AlienDeployment::getRoadTypeOdds() const
{
	return _roadTypeOdds;
}

/// Gets the terrain for battlescape generation.
std::string AlienDeployment::getTerrain() const
{
	return _terrain;
}
/// Gets the shade level for battlescape generation.
int AlienDeployment::getShade() const
{
	return _shade;
}
/// Gets the next stage of the mission.
std::string AlienDeployment::getNextStage() const
{
	return _nextStage;
}

}