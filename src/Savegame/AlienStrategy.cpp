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
#include <assert.h>
#include "AlienStrategy.h"
#include "WeightedOptions.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleRegion.h"

namespace OpenXcom
{

typedef std::map<std::string, WeightedOptions*> MissionsByRegion;

/**
 * Create an AlienStrategy with no values.
 * Running a game like this will most likely crash.
 */
AlienStrategy::AlienStrategy()
{
	// Empty by design!
}

/**
 * Free all resources used by this AlienStrategy.
 */
AlienStrategy::~AlienStrategy()
{
	// Free allocated memory.
	for (MissionsByRegion::iterator ii = _regionMissions.begin(); ii != _regionMissions.end(); ++ii)
	{
		delete ii->second;
	}
}

/**
 * Get starting values from the rules.
 * @param rules Pointer to the game ruleset.
 */
void AlienStrategy::init(const Ruleset *rules)
{
	std::vector<std::string> regions = rules->getRegionsList();
	for (std::vector<std::string>::const_iterator rr = regions.begin(); rr != regions.end(); ++rr)
	{
		RuleRegion *region = rules->getRegion(*rr);
		_regionChances.set(*rr, region->getWeight());
		WeightedOptions *missions = new WeightedOptions(region->getAvailableMissions());
		_regionMissions.insert(std::make_pair(*rr, missions));
	}
}

/**
 * Loads the data from a YAML file.
 * @param rules Pointer to the game ruleset.
 * @param node YAML node.
 */
void AlienStrategy::load(const Ruleset *, const YAML::Node &node)
{
	// Free allocated memory.
	for (MissionsByRegion::iterator ii = _regionMissions.begin(); ii != _regionMissions.end(); ++ii)
	{
		delete ii->second;
	}
	_regionMissions.clear();
	_regionChances.clear();
	_regionChances.load(node["regions"]);
	const YAML::Node &strat = node["possibleMissions"];
	for (YAML::const_iterator nn = strat.begin(); nn != strat.end(); ++nn)
	{
		std::string region = (*nn)["region"].as<std::string>();
		const YAML::Node &missions = (*nn)["missions"];
		std::auto_ptr<WeightedOptions> options(new WeightedOptions());
		options->load(missions);
		_regionMissions.insert(std::make_pair(region, options.release()));
	}
	_missionLocations = node["missionLocations"].as<std::map<std::string, std::vector<std::pair<std::string, int> > > >(_missionLocations);
	_missionRuns = node["missionsRun"].as<std::map<std::string, int> >(_missionRuns);
}

/**
 * Saves the alien data to a YAML file.
 * @return YAML node.
 */
YAML::Node AlienStrategy::save() const
{
	YAML::Node node;
	node["regions"] = _regionChances.save();
	for (MissionsByRegion::const_iterator ii = _regionMissions.begin(); ii != _regionMissions.end(); ++ii)
	{
		YAML::Node subnode;
		subnode["region"] = ii->first;
		subnode["missions"] = ii->second->save();
		node["possibleMissions"].push_back(subnode);
	}
	node["missionLocations"] = _missionLocations;
	node["missionsRun"] = _missionRuns;
	return node;
}

/**
 * Choose one of the regions for a mission.
 * @param rules Pointer to the ruleset.
 * @return The region id.
 */
const std::string AlienStrategy::chooseRandomRegion(const Ruleset *rules)
{
	std::string chosen = _regionChances.choose();
	if (chosen.empty())
	{
		// no more missions to choose from: refresh.
		// First, free allocated memory.
		for (MissionsByRegion::iterator ii = _regionMissions.begin(); ii != _regionMissions.end(); ++ii)
		{
			delete ii->second;
		}
		_regionMissions.clear();
		// re-initialize the list
		init(rules);
		// now let's try that again.
		chosen = _regionChances.choose();
	}
	assert ("" != chosen);
	return chosen;
}

/**
 * Choose one missions available for @a region.
 * @param region The region id.
 * @return The mission id.
 */
const std::string AlienStrategy::chooseRandomMission(const std::string &region) const
{
	MissionsByRegion::const_iterator found = _regionMissions.find(region);
	assert(found != _regionMissions.end());
	return found->second->choose();
}

/**
 * Remove @a mission from the list of possible missions for @a region.
 * @param region The region id.
 * @param mission The mission id.
 * @return If there are no more regions with missions available.
 */
bool AlienStrategy::removeMission(const std::string &region, const std::string &mission)
{
	MissionsByRegion::iterator found = _regionMissions.find(region);
	if (found != _regionMissions.end())
	{
		found->second->set(mission, 0);
		if (found->second->empty())
		{
			_regionMissions.erase(found);
			_regionChances.set(region, 0);
		}
	}
	return _regionMissions.empty();
}


/**
 * Checks the number of missions run labelled as "varName".
 * @return the number of missions run under the variable name.
 */
int AlienStrategy::getMissionsRun(const std::string &varName)
{
	if (_missionRuns.find(varName) != _missionRuns.end())
		return _missionRuns[varName];
	return 0;
}

/**
 * Increments the number of missions run labelled as "varName".
 * @param varName the variable name that we want to use to keep track of this.
 */
void AlienStrategy::addMissionRun(const std::string &varName)
{
	if (varName == "")
		return;
	_missionRuns[varName]++;
}

/**
 * Adds a mission location to our storage array.
 * @param varName the name on the variable under which to store this info.
 * @param regionName the name of the region we're using.
 * @param zoneNumber the number of the zone within that region we're using.
 * @param maximum the maximum size of the list we want to maintain.
 */
void AlienStrategy::addMissionLocation(const std::string varName, const std::string regionName, int zoneNumber, int maximum)
{
	if (maximum <= 0) return;
	_missionLocations[varName].push_back(std::make_pair(regionName, zoneNumber));
	if (_missionLocations[varName].size() > (size_t)(maximum))
	{
		_missionLocations.erase(_missionLocations.begin());
	}
}

/**
 * Checks that a given mission location (city or whatever) isn't stored in our list of previously attacked locations.
 * @param varName the name of the variable that is storing our data.
 * @param regionName the name of the region we're looking for.
 * @param zoneNumber the number in the region that we want to check.
 * @return if the region is valid (meaning it is not in our table).
 */
bool AlienStrategy::validMissionLocation(const std::string varName, const std::string regionName, int zoneNumber)
{
	if (_missionLocations.find(varName) != _missionLocations.end())
	{
		for (std::vector<std::pair<std::string, int> >::const_iterator i = _missionLocations[varName].begin();
			i != _missionLocations[varName].end();
			++i)
		{
			if ((*i).first == regionName && (*i).second == zoneNumber)
				return false;
		}
	}
	return true;
}
/**
 * Checks that a given region appears in our strategy table.
 * @param region the region we want to check for validity.
 * @return if the region appears in the table or not.
 */
bool AlienStrategy::validMissionRegion(const std::string region)
{
	std::map<std::string, WeightedOptions*>::iterator i = _regionMissions.find(region);
	return (i != _regionMissions.end());
}
}
