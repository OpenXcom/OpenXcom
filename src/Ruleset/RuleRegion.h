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
#ifndef OPENXCOM_RULEREGION_H
#define OPENXCOM_RULEREGION_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "../Savegame/WeightedOptions.h"

namespace OpenXcom
{

class City;
struct MissionZone;

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
	/// Weighted list of the different mission types for this region.
	WeightedOptions _missionWeights;
	unsigned _regionWeight; ///< Weight of this region when selecting regions for alien missions.
	std::vector<MissionZone> _missionZones; ///< All the mission zones in this region.
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
	/// Checks if a point is inside the region.
	bool insideRegion(double lon, double lat) const;
	/// Gets the cities in this region.
	std::vector<City*> *getCities();
	/// Gets the weight of this region for mission selection.
	unsigned getWeight() const;
	/// Gets the weighted list of missions for this region.
	const WeightedOptions &getAvailableMissions() const { return _missionWeights; }
	/// Gets a random point inside a mission site.
	std::pair<double, double> getRandomPoint(unsigned site) const;
	const std::vector<double> &getLonMax() const { return _lonMax; }
	const std::vector<double> &getLonMin() const { return _lonMin; }
	const std::vector<double> &getLatMax() const { return _latMax; }
	const std::vector<double> &getLatMin() const { return _latMin; }
};

}

#endif
