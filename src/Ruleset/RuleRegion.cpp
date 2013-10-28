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
#define _USE_MATH_DEFINES
#include <assert.h>
#include "RuleRegion.h"
#include "City.h"
#include "../Engine/RNG.h"
#include <math.h>

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of region.
 * @param type String defining the type.
 */
RuleRegion::RuleRegion(const std::string &type): _type(type), _cost(0), _lonMin(), _lonMax(), _latMin(), _latMax(), _cities(), _regionWeight(0), _missionRegion("")
{
}

/**
 * Deletes the cities from memory.
 */
RuleRegion::~RuleRegion()
{
	for (std::vector<City*>::iterator i = _cities.begin(); i != _cities.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the region type from a YAML file.
 * @param node YAML node.
 */
void RuleRegion::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_cost = node["cost"].as<int>(_cost);
	std::vector< std::vector<double> > areas;
	areas = node["areas"].as< std::vector< std::vector<double> > >(areas);
	for (size_t i = 0; i != areas.size(); ++i)
	{
		_lonMin.push_back(areas[i][0] * M_PI / 180);
		_lonMax.push_back(areas[i][1] * M_PI / 180);
		_latMin.push_back(areas[i][2] * M_PI / 180);
		_latMax.push_back(areas[i][3] * M_PI / 180);
	}
	if (const YAML::Node &cities = node["cities"])
	{
		for (YAML::const_iterator i = cities.begin(); i != cities.end(); ++i)
		{
			City *rule = new City("", 0.0, 0.0);
			rule->load(*i);
			_cities.push_back(rule);
		}
	}
	if (const YAML::Node &weights = node["missionWeights"])
	{
		_missionWeights.load(weights);
	}
	_regionWeight = node["regionWeight"].as<unsigned>(_regionWeight);
	_missionZones = node["missionZones"].as< std::vector<MissionZone> >(_missionZones);
	_missionRegion = node["missionRegion"].as<std::string>(_missionRegion);
}

/**
 * Gets the language string that names
 * this region. Each region type
 * has a unique name.
 * @return The region type.
 */
std::string RuleRegion::getType() const
{
	return _type;
}

/**
 * Gets the cost of building a base inside this region.
 * @return The construction cost.
 */
int RuleRegion::getBaseCost() const
{
	return _cost;
}

/**
 * Checks if a point is inside this region.
 * @param lon Longitude in radians.
 * @param lat Latitude in radians.
 * @return True if it's inside, false if it's outside.
 */
bool RuleRegion::insideRegion(double lon, double lat) const
{
	for (unsigned int i = 0; i < _lonMin.size(); ++i)
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
 * Gets the list of cities contained in this region.
 * @return Pointer to a list.
 */
std::vector<City*> *RuleRegion::getCities()
{
	return &_cities;
}

/**
 * Gets the weight of this region for mission selection.
 * This is only used when creating a new game, since these weights change in the course of the game.
 * @return The initial weight of this region.
 */
unsigned RuleRegion::getWeight() const
{
	return _regionWeight;
}

/**
 * Gets a random point that is guaranteed to be inside the give zone.
 * If the region contains cities, they are the sites of zone 0 and the rest of the zones get one index higher.
 * @return A pair of longtitude and latitude.
 */
std::pair<double, double> RuleRegion::getRandomPoint(unsigned zone) const
{
	if (zone == 0 && !_cities.empty())
	{
		unsigned p = RNG::generate(0, _cities.size() - 1);
		return std::make_pair(_cities[p]->getLongitude(), _cities[p]->getLatitude());
	}
	if (zone != 0)
	{
		--zone;
	}
	if (zone < _missionZones.size())
	{
		unsigned a = RNG::generate(0, _missionZones[zone].areas.size() - 1);
		double lonMin = _missionZones[zone].areas[a].lonMin;
		double lonMax = _missionZones[zone].areas[a].lonMax;
		double latMin = _missionZones[zone].areas[a].latMin;
		double latMax = _missionZones[zone].areas[a].latMax;
		if (lonMin > lonMax)
		{
			lonMin = _missionZones[zone].areas[a].lonMax;
			lonMax = _missionZones[zone].areas[a].lonMin;
		}
		if (latMin > latMax)
		{
			latMin = _missionZones[zone].areas[a].latMax;
			latMax = _missionZones[zone].areas[a].latMin;
		}
		double lon = RNG::generate(lonMin, lonMax);
		double lat = RNG::generate(latMin, latMax);
		return std::make_pair(lon * M_PI / 180, lat * M_PI / 180);
	}
	assert(0 && "Invalid zone number");
	return std::make_pair(0.0, 0.0);
}

const std::vector<MissionZone> &RuleRegion::getMissionZones() const
{
	return _missionZones;
}
}
