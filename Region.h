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
#ifndef OPENXCOM__REGION_H
#define OPENXCOM__REGION_H

#include <vector>

class City;

using namespace std;

/**
 * Represents a region of the world.
 * Regions help localize X-Com and alien activity around
 * the globe and also affect base construction costs.
 */
class Region
{
private:
	int _cost;
	vector<double> _lonMin, _lonMax, _latMin, _latMax;
	vector<City*> _cities;
public:
	/// Creates a new region with a base cost.
	Region(int cost);
	/// Cleans up the region.
	~Region();
	/// Gets the region's base cost.
	int getBaseCost();
	/// Adds an area to the region.
	void addArea(double lonMin, double lonMax, double latMin, double latMax);
	/// Checks if a point is inside the region.
	bool insideRegion(double lon, double lat);
	/// Gets the cities in this region.
	vector<City*> *getCities();
};

#endif
