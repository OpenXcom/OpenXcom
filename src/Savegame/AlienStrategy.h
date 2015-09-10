/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_ALIENSTRATEGY_H
#define OPENXCOM_ALIENSTRATEGY_H

#include <yaml-cpp/yaml.h>
#include "WeightedOptions.h"

namespace OpenXcom
{
class Mod;

/**
 * Stores the information about alien strategy.
 */
class AlienStrategy
{
public:
	/// Create an AlienStrategy with no data.
	AlienStrategy();
	/// Free resources used by the AlienStrategy.
	~AlienStrategy();
	/// Initialize values according to the rules.
	void init(const Mod *mod);
	/// Loads the data from YAML.
	void load(const YAML::Node& node);
	/// Saves the data to YAML.
	YAML::Node save() const;
	/// Choose a random region for a regular mission.
	std::string chooseRandomRegion(const Mod *mod);
	/// Choose a random mission for a region.
	std::string chooseRandomMission(const std::string &region) const;
	/// Remove a region and mission from the list of posibilities.
	bool removeMission(const std::string &region, const std::string &mission);
	/// Checks the number of missions run labelled as "varName".
	int getMissionsRun(const std::string &varName);
	/// Increments the number of missions run labelled as "varName".
	void addMissionRun(const std::string &varName);
	/// Adds a mission location to our storage array.
	void addMissionLocation(const std::string &varName, const std::string &regionName, int zoneNumber, int maximum);
	/// Checks that a given mission location (city or whatever) isn't stored in our list of previously attacked locations.
	bool validMissionLocation(const std::string &varName, const std::string &regionName, int zoneNumber);
	/// Checks that a given region appears in our strategy table.
	bool validMissionRegion(const std::string &regionName);
private:
	/// The chances of each region to be targeted for a mission.
	WeightedOptions _regionChances;
	/// The chances of each mission type for each region.
	std::map<std::string, WeightedOptions*> _regionMissions;

	// Disable copy and assignments.
	AlienStrategy(const AlienStrategy &);
	AlienStrategy &operator=(const AlienStrategy &);
	std::map<std::string, int> _missionRuns;
	std::map<std::string, std::vector<std::pair<std::string, int> > > _missionLocations;

};

}

#endif
