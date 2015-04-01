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
#include "Texture.h"
#include "../Savegame/Target.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Initializes a globe texture.
 * @param id Texture identifier.
 */
Texture::Texture(int id) : _id(id)
{
}

/**
 *
 */
Texture::~Texture()
{
}

/**
 * Loads the texture type from a YAML file.
 * @param node YAML node.
 */
void Texture::load(const YAML::Node &node)
{
	_id = node["id"].as<int>(_id);
	_deployment = node["deployment"].as<std::string>(_deployment);
	_terrain = node["terrain"].as< std::vector<TerrainCriteria> >(_terrain);
}

/**
 * Returns the list of terrain criteria associated
 * with this texture.
 * @return List of terrain.
 */
std::vector<TerrainCriteria> *Texture::getTerrain()
{
	return &_terrain;
}

/**
 * Calculates a random terrain for a mission target based
 * on the texture's available terrain criteria.
 * @target Pointer to the mission target.
 */
std::string Texture::getRandomTerrain(Target *target) const
{
	int totalWeight = 0;
	std::map<int, std::string> possibilities;
	for (std::vector<TerrainCriteria>::const_iterator i = _terrain.begin(); i != _terrain.end(); ++i)
	{
		if (i->weight > 0 &&
			target->getLongitude() >= i->lonMin && target->getLongitude() < i->lonMax &&
			target->getLatitude() >= i->latMin && target->getLatitude() < i->latMax)
		{
			totalWeight += i->weight;
			possibilities[totalWeight] = i->name;
		}
	}
	int pick = RNG::generate(1, totalWeight);
	for (std::map<int, std::string>::const_iterator i = possibilities.begin(); i != possibilities.end(); ++i)
	{
		if (pick <= i->first)
		{
			return i->second;
		}
	}
	return "";
}

std::string Texture::getDeployment() const
{
	return _deployment;
}

}
