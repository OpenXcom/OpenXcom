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
#ifndef OPENXCOM_RULEREGION_H
#define OPENXCOM_RULEREGION_H

#include <string>
#include <vector>
#include "yaml.h"

namespace OpenXcom
{

class City;

/**
 * Represents a specific region of the world.
 * Contains constant info about a region like area
 * covered and base construction costs.
 */
class RuleRegion
{
private:
	std::string _type;
	int _cost;
	std::vector<double> _lonMin, _lonMax, _latMin, _latMax;
	std::vector<City*> _cities;
public:
	/// Creates a blank region ruleset.
	RuleRegion(const std::string &type);
	/// Cleans up the region ruleset.
	~RuleRegion();
	/// Loads the region from YAML.
	void load(const YAML::Node& node);
	/// Saves the region to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the region's type.
	std::string getType() const;
	/// Gets the region's base cost.
	int getBaseCost() const;
	/// Sets the region's base cost.
	void setBaseCost(int cost);
	/// Adds an area to the region.
	void addArea(double lonMin, double lonMax, double latMin, double latMax);
	/// Checks if a point is inside the region.
	bool insideRegion(double lon, double lat) const;
	/// Gets the cities in this region.
	std::vector<City*> *getCities();
};

}

#endif
