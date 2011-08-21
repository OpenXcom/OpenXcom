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
#include "RuleRegion.h"
#include "City.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of region.
 * @param type String defining the type.
 */
RuleRegion::RuleRegion(std::string type): _type(type), _cost(0), _lonMin(), _lonMax(), _latMin(), _latMax(), _cities()
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
