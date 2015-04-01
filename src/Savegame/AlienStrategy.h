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
class Ruleset;

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
	void init(const Ruleset *rules);
	/// Loads the data from YAML.
	void load(const Ruleset *rules, const YAML::Node& node);
	/// Saves the data to YAML.
	YAML::Node save() const;
	/// Choose a random region for a regular mission.
	const std::string chooseRandomRegion(const Ruleset *rules);
	/// Choose a random mission for a region.
	const std::string chooseRandomMission(const std::string &region) const;
	/// Remove a region and mission from the list of posibilities.
	bool removeMission(const std::string &region, const std::string &mission);
private:
	/// The chances of each region to be targeted for a mission.
	WeightedOptions _regionChances;
	/// The chances of each mission type for each region.
	std::map<std::string, WeightedOptions*> _regionMissions;

	// Disable copy and assignments.
	AlienStrategy(const AlienStrategy &);
	AlienStrategy &operator=(const AlienStrategy &);
};

}

#endif
