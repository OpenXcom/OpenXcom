/*
 * Copyright 2010 OpenXcom Developers.
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
#include "Region.h"
#include "City.h"

namespace OpenXcom
{

/**
 * Initializes a region with a certain construction cost.
 * @param cost Base building cost.
 */
Region::Region(int cost): _cost(cost), _lonMin(), _lonMax(), _latMin(), _latMax()
{
}

/**
 * Deletes the cities from memory.
 */
Region::~Region()
{
	for (std::vector<City*>::iterator i = _cities.begin(); i != _cities.end(); i++)
	{
		delete *i;
	}
}

/**
 * Loads the region from a YAML file.
 * @param node YAML node.
 */
void Region::load(const YAML::Node &node)
{
	unsigned int size = 0;

	node["cost"] >> _cost;
	node["lonMin"] >> _lonMin;
	node["lonMax"] >> _lonMax;
	node["latMin"] >> _latMin;
	node["latMax"] >> _latMax;

	size = node["cities"].size();
	for (unsigned i = 0; i < size; i++)
	{
		City *c = new City("", 0, 0);
		c->load(node["cities"][i]);
		_cities.push_back(c);
	}
}

/**
 * Saves the region to a YAML file.
 * @param out YAML emitter.
 */
void Region::save(YAML::Emitter &out, std::string name) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << name;
	out << YAML::Key << "cost" << YAML::Value << _cost;
	out << YAML::Key << "lonMin" << YAML::Value << YAML::Flow << _lonMin;
	out << YAML::Key << "lonMax" << YAML::Value << YAML::Flow << _lonMax;
	out << YAML::Key << "latMin" << YAML::Value << YAML::Flow << _latMin;
	out << YAML::Key << "latMax" << YAML::Value << YAML::Flow << _latMax;
	out << YAML::Key << "cities" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<City*>::const_iterator i = _cities.begin(); i != _cities.end(); i++)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;
}

/**
 * Returns the cost of building a base inside this region.
 * @return Construction cost.
 */
int Region::getBaseCost() const
{
	return _cost;
}

/**
 * Adds a quad area of space that this region covers.
 * @param lonMin Minimum longitude.
 * @param lonMax Maximum longitude.
 * @param latMin Minimum latitude.
 * @param latMax Maximum latitude.
 */
void Region::addArea(double lonMin, double lonMax, double latMin, double latMax)
{
	_lonMin.push_back(lonMin);
	_lonMax.push_back(lonMax);
	_latMin.push_back(latMin);
	_latMax.push_back(latMax);
}

/**
 * Checks if a point is inside this region.
 * @param lon Longitude in radians.
 * @param lat Latitude in radians.
 * @return True if it's inside, False if it's outside.
 */
bool Region::insideRegion(double lon, double lat) const
{
	for (unsigned int i = 0; i < _lonMin.size(); i++)
	{
		bool inLon, inLat;

		if (_lonMin[i] <= _lonMax[i])
			inLon = (lon >= _lonMin[i] && lon < _lonMax[i]);
		else
			inLon = ((lon >= _lonMin[i] && lon < 6.283) || (lon >= 0 && lon < _lonMax[i]));
		
		inLat = (lat >= _latMin[i] && lat < _latMax[i]);

		if (inLon && inLat)
			return true;
	}
	return false;
}

/**
 * Returns the list of cities contained.
 * @return Pointer to list.
 */
std::vector<City*> *const Region::getCities()
{
	return &_cities;
}

}
