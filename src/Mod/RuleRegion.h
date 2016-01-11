#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "../fmath.h"
#include <math.h>
#include "../Savegame/WeightedOptions.h"

namespace OpenXcom
{

/**
 * Defines a rectangle in polar coordinates.
 * It is used to define areas for a mission zone.
 */
struct MissionArea
{
	double lonMin, lonMax, latMin, latMax;
	int texture;
	std::string name;

    bool operator== (const MissionArea& ma) const
	{
		return AreSame(lonMax, ma.lonMax) && AreSame(lonMin, ma.lonMin) && AreSame(latMax, ma.latMax) && AreSame(latMin, ma.latMin);
	}

	bool isPoint() const
	{
		return AreSame(lonMin, lonMax) && AreSame(latMin, latMax);
	}
};

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

class City;
class Target;

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
	/// Weight of this region when selecting regions for alien missions.
	size_t _regionWeight;
	/// All the mission zones in this region.
	std::vector<MissionZone> _missionZones;
	/// Do missions in the region defined by this string instead.
	std::string _missionRegion;
public:
	/// Creates a blank region ruleset.
	RuleRegion(const std::string &type);
	/// Cleans up the region ruleset.
	~RuleRegion();
	/// Loads the region from YAML.
	void load(const YAML::Node& node);
	/// Gets the region's type.
	std::string getType() const;
	/// Gets the region's base cost.
	int getBaseCost() const;
	/// Checks if a point is inside the region.
	bool insideRegion(double lon, double lat) const;
	/// Gets the cities in this region.
	std::vector<City*> *getCities();
	/// Gets the weight of this region for mission selection.
	size_t getWeight() const;
	/// Gets the weighted list of missions for this region.
	const WeightedOptions &getAvailableMissions() const { return _missionWeights; }
	/// Gets the substitute mission region.
	const std::string &getMissionRegion() const { return _missionRegion; }
	/// Gets a random point inside a mission zone.
	std::pair<double, double> getRandomPoint(size_t zone) const;
	/// Gets the mission area for the corresponding target.
	MissionArea getMissionPoint(size_t zone, Target *target) const;
	/// Gets a random mission area.
	MissionArea getRandomMissionPoint(size_t zone) const;
	/// Gets the maximum longitude.
	const std::vector<double> &getLonMax() const { return _lonMax; }
	/// Gets the minimum longitude.
	const std::vector<double> &getLonMin() const { return _lonMin; }
	/// Gets the maximum latitude.
	const std::vector<double> &getLatMax() const { return _latMax; }
	/// Gets the minimum latitude.
	const std::vector<double> &getLatMin() const { return _latMin; }
	/// Gets a list of MissionZones.
	const std::vector<MissionZone> &getMissionZones() const;
};

}

namespace YAML
{
	template<>
	struct convert<OpenXcom::MissionArea>
	{
		static Node encode(const OpenXcom::MissionArea& rhs)
		{
			Node node;
			node.push_back(rhs.lonMin / M_PI * 180.0);
			node.push_back(rhs.lonMax / M_PI * 180.0);
			node.push_back(rhs.latMin / M_PI * 180.0);
			node.push_back(rhs.latMax / M_PI * 180.0);
			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionArea& rhs)
		{
			if (!node.IsSequence() || node.size() < 4)
				return false;

			rhs.lonMin = node[0].as<double>() * M_PI / 180.0;
			rhs.lonMax = node[1].as<double>() * M_PI / 180.0;
			rhs.latMin = node[2].as<double>() * M_PI / 180.0;
			rhs.latMax = node[3].as<double>() * M_PI / 180.0;
			if (rhs.latMin > rhs.latMax)
				std::swap(rhs.latMin, rhs.latMax);
			if (node.size() >= 5) rhs.texture = node[4].as<int>();
			if (node.size() >= 6) rhs.name = node[5].as<std::string>();
			return true;
		}
	};

	template<>
	struct convert<OpenXcom::MissionZone>
	{
		static Node encode(const OpenXcom::MissionZone& rhs)
		{
			Node node;
			node = rhs.areas;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionZone& rhs)
		{
			if (!node.IsSequence())
				return false;

			rhs.areas = node.as< std::vector<OpenXcom::MissionArea> >(rhs.areas);
			return true;
		}
	};
}
