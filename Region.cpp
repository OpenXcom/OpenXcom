/*
 * Copyright 2010 Daniel Albano
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
	for (vector<City*>::iterator i = _cities.begin(); i != _cities.end(); i++)
	{
		delete *i;
	}
}

/**
 * Returns the cost of building a base inside this region.
 * @return Construction cost.
 */
int Region::getBaseCost()
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
bool Region::insideRegion(double lon, double lat)
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
vector<City*> *Region::getCities()
{
	return &_cities;
}
