/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "RuleRegion.h"
#include "City.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of region.
 * @param type String defining the type.
 */
RuleRegion::RuleRegion(const std::string &type): _type(type), _cost(0), _lonMin(), _lonMax(), _latMin(), _latMax(), _cities()
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
		else if (key == "lonMin")
		{
			i.second() >> _lonMin;
		}
		else if (key == "lonMax")
		{
			i.second() >> _lonMax;
		}
		else if (key == "latMin")
		{
			i.second() >> _latMin;
		}
		else if (key == "latMax")
		{
			i.second() >> _latMax;
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
	out << YAML::EndMap;
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
 * Changes the cost of building a base inside this region.
 * @param cost Construction cost.
 */
void RuleRegion::setBaseCost(int cost)
{
	_cost = cost;
}

/**
 * Adds a rectangular area that this region covers.
 * @param lonMin Minimum longitude.
 * @param lonMax Maximum longitude.
 * @param latMin Minimum latitude.
 * @param latMax Maximum latitude.
 */
void RuleRegion::addArea(double lonMin, double lonMax, double latMin, double latMax)
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
std::vector<City*> *const RuleRegion::getCities()
{
	return &_cities;
}

}
