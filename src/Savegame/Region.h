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
#ifndef OPENXCOM_REGION_H
#define OPENXCOM_REGION_H

#include <vector>

namespace OpenXcom
{

class City;

/**
 * Represents a region of the world.
 * Regions help localize X-Com and alien activity around
 * the globe and also affect base construction costs.
 */
class Region
{
private:
	int _cost;
	std::vector<double> _lonMin, _lonMax, _latMin, _latMax;
	std::vector<City*> _cities;
public:
	/// Creates a new region with a base cost.
	Region(int cost);
	/// Cleans up the region.
	~Region();
	/// Gets the region's base cost.
	int getBaseCost() const;
	/// Adds an area to the region.
	void addArea(double lonMin, double lonMax, double latMin, double latMax);
	/// Checks if a point is inside the region.
	bool insideRegion(double lon, double lat) const;
	/// Gets the cities in this region.
	std::vector<City*> *const getCities();
};

}

#endif
