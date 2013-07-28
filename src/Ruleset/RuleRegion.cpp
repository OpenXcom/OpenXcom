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
 * Defines a rectangle in polar coordinates.
 * It is used to define areas for a mission zone.
 */
struct MissionArea
{
	double lonMin, lonMax, latMin, latMax;
};

/// Write a MissionArea to YAML.
YAML::Emitter &operator<<(YAML::Emitter &out, const MissionArea &ma)
{
	out << YAML::Flow << YAML::BeginSeq;
	out << ma.lonMin << ma.lonMax << ma.latMin << ma.latMax;
	out << YAML::EndSeq;
	return out;
}

/// Read a MissionArea from YAML.
const YAML::Node &operator>>(const YAML::Node &nd, MissionArea &ma)
{
	nd[0] >> ma.lonMin;
	nd[1] >> ma.lonMax;
	nd[2] >> ma.latMin;
	nd[3] >> ma.latMax;
	return nd;
}

/**
 * A zone (set of areas) on the globe.
 */
struct MissionZone
{
	std::vector<MissionArea> areas;

	void swap(MissionZone &other)
	{
		areas.swap(other.areas);
	}
};

/// Write a MissionZone to YAML.
YAML::Emitter &operator<<(YAML::Emitter &out, const MissionZone &mz)
{
	out << mz.areas;
	return out;
}

/// Read a MissionZone from YAML.
const YAML::Node &operator>>(const YAML::Node &nd, MissionZone &mz)
{
	nd >> mz.areas;
	return nd;
}

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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "cost")
		{
			i.second() >> _cost;
		}
		else if (key == "areas")
		{
			for (size_t j = 0; j != i.second().size(); ++j)
			{
				std::vector<double> k;
				i.second()[j] >> k;
				_lonMin.push_back(k[0] * M_PI / 180);
				_lonMax.push_back(k[1] * M_PI / 180);
				_latMin.push_back(k[2] * M_PI / 180);
				_latMax.push_back(k[3] * M_PI / 180);
			}
		}
		else if (key == "cities")
		{
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				City *rule = new City("", 0.0, 0.0);
				rule->load(*j);
				_cities.push_back(rule);
			}
		}
		else if (key == "regionWeight")
		{
			i.second() >> _regionWeight;
		}
		else if (key == "missionWeights")
		{
			_missionWeights.load(i.second());
		}
		else if (key == "missionZones")
		{
			i.second() >> _missionZones;
		}
		else if (key == "missionRegion")
		{
			i.second() >> _missionRegion;
		}
	}
}

/**
 * Saves the region type to a YAML file.
 * @param out YAML emitter.
 */
void RuleRegion::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "cost" << YAML::Value << _cost;
	out << YAML::Key << "lonMin" << YAML::Value << _lonMin;
	out << YAML::Key << "lonMax" << YAML::Value << _lonMax;
	out << YAML::Key << "latMin" << YAML::Value << _latMin;
	out << YAML::Key << "latMax" << YAML::Value << _latMax;
	out << YAML::Key << "cities" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<City*>::const_iterator i = _cities.begin(); i != _cities.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "regionWeight" << YAML::Value << _regionWeight;
	out << YAML::Key << "missionWeights" << YAML::Value;
	_missionWeights.save(out);
	out << YAML::EndMap;
	out << YAML::Key << "missionZones" << YAML::Value << _missionZones;
}

/**
 * Returns the language string that names
 * this region. Each region type
 * has a unique name.
 * @return Region name.
 */
std::string RuleRegion::getType() const
{
	return _type;
}

/**
 * Returns the cost of building a base inside this region.
 * @return Construction cost.
 */
int RuleRegion::getBaseCost() const
{
	return _cost;
}

/**
 * Checks if a point is inside this region.
 * @param lon Longitude in radians.
 * @param lat Latitude in radians.
 * @return True if it's inside, False if it's outside.
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
 * Returns the list of cities contained.
 * @return Pointer to list.
 */
std::vector<City*> *RuleRegion::getCities()
{
	return &_cities;
}

/**
 * Returns the weight of this region for mission selection.
 * This is only used when creating a new game, since these weights change in the cource of the game.
 * @return The initial weight of this region.
 */
unsigned RuleRegion::getWeight() const
{
	return _regionWeight;
}

/**
 * Returns a random point that is guaranteed to be inside the give zone.
 * If the region contains cities, they are the sites of zone 0 and the rest of the zones get one index higher.
 * @return A pair of longtitude and latitude.
 */
std::pair<double, double> RuleRegion::getRandomPoint(unsigned zone) const
{
	if (zone == 0 && _cities.size() > 0)
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

}
