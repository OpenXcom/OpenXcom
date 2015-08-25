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
#include "RuleMissionScript.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * RuleMissionScript: the rules for the alien mission progression.
 * Each script element is independant, and the saved game will probe the list of these each month to determine what's going to happen.
 */
RuleMissionScript::RuleMissionScript(const std::string &type) : _type(type), _firstMonth(0), _lastMonth(-1), _label(0), _executionOdds(100),
															_targetBaseOdds(0), _minDifficulty(0), _maxRuns(-1), _avoidRepeats(0), _delay(0), 
															_useTable(true), _siteType(false)
{
}

/**
 * Destructor, cleans up the mess we left in ram.
 */
RuleMissionScript::~RuleMissionScript()
{
	for (std::vector<std::pair<size_t, WeightedOptions*> >::iterator i = _missionWeights.begin(); i != _missionWeights.end(); ++i)
	{
		delete i->second;
	}
	for (std::vector<std::pair<size_t, WeightedOptions*> >::iterator i = _raceWeights.begin(); i != _raceWeights.end(); ++i)
	{
		delete i->second;
	}
	for (std::vector<std::pair<size_t, WeightedOptions*> >::iterator i = _regionWeights.begin(); i != _regionWeights.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads a missionScript from a YML file.
 * @param node the node within the file we're reading.
 */
void RuleMissionScript::load(const YAML::Node& node)
{
	_varName = node["varName"].as<std::string>(_varName);
	_firstMonth = node["firstMonth"].as<int>(_firstMonth);
	_lastMonth = node["lastMonth"].as<int>(_lastMonth);
	_label = node["label"].as<unsigned int>(_label);
	_executionOdds = node["executionOdds"].as<int>(_executionOdds);
	_targetBaseOdds = node["targetBaseOdds"].as<int>(_targetBaseOdds);
	_minDifficulty = node["minDifficulty"].as<int>(_minDifficulty);
	_maxRuns = node["maxRuns"].as<int>(_maxRuns);
	_avoidRepeats = node["avoidRepeats"].as<int>(_avoidRepeats);
	_delay = node["startDelay"].as<int>(_delay);
	_conditionals = node["conditionals"].as<std::vector<int> >(_conditionals);
	if (const YAML::Node &weights = node["missionWeights"])
	{
		for (YAML::const_iterator nn = weights.begin(); nn != weights.end(); ++nn)
		{
			WeightedOptions *nw = new WeightedOptions();
			nw->load(nn->second);
			_missionWeights.push_back(std::make_pair(nn->first.as<size_t>(0), nw));
		}
	}
	if (const YAML::Node &weights = node["raceWeights"])
	{
		for (YAML::const_iterator nn = weights.begin(); nn != weights.end(); ++nn)
		{
			WeightedOptions *nw = new WeightedOptions();
			nw->load(nn->second);
			_raceWeights.push_back(std::make_pair(nn->first.as<size_t>(0), nw));
		}
	}
	if (const YAML::Node &weights = node["regionWeights"])
	{
		for (YAML::const_iterator nn = weights.begin(); nn != weights.end(); ++nn)
		{
			WeightedOptions *nw = new WeightedOptions();
			nw->load(nn->second);
			_regionWeights.push_back(std::make_pair(nn->first.as<size_t>(0), nw));
		}
	}
	_researchTriggers = node["researchTriggers"].as<std::map<std::string, bool> >(_researchTriggers);
	_useTable = node["useTable"].as<bool>(_useTable);
	if (_varName == "" && (_maxRuns > 0 || _avoidRepeats > 0))
	{
		throw Exception("Error in mission script: " + _type +": no varName provided for a script with maxRuns or repeatAvoidance.");
	}

}

/**
 * Gets the name of this command.
 * @return the name of the command.
 */
std::string RuleMissionScript::getType() const
{
	return _type;
}

/**
 * @return the first month this script should run.
 */
int RuleMissionScript::getFirstMonth() const
{
	return _firstMonth;
}

/**
 * @return the last month this script should run.
 */
int RuleMissionScript::getLastMonth() const
{
	return _lastMonth;
}

/**
 * @return the label this command uses for conditional tracking.
 */
int RuleMissionScript::getLabel() const
{
	return _label;
}

/**
 * @return the odds of this command's execution.
 */
int RuleMissionScript::getExecutionOdds() const
{
	return _executionOdds;
}

/**
 * @return the odds of this command targetting a base.
 */
int RuleMissionScript::getTargetBaseOdds() const
{
	return _targetBaseOdds;
}

/**
 * @return the minimum difficulty for this script to run.
 */
int RuleMissionScript::getMinDifficulty() const
{
	return _minDifficulty;
}

/**
 * @return the maximum runs for scripts tracking our varName.
 */
int RuleMissionScript::getMaxRuns() const
{
	return _maxRuns;
}

/**
 * @return the number of sites to avoid repeating missions against.
 */
int RuleMissionScript::getRepeatAvoidance() const
{
	return _avoidRepeats;
}

/**
 * @return the fixed delay on spawning the first wave (if any) to override whatever's written in the mission definition.
 */
int RuleMissionScript::getDelay() const
{
	return _delay;
}

/**
 * @return the list of conditions that govern execution of this command.
 */
const std::vector<int> &RuleMissionScript::getConditionals() const
{
	return _conditionals;
}

/**
 * @return if this command uses a weighted distribution to pick a race.
 */
bool RuleMissionScript::hasRaceWeights() const
{
	return !_raceWeights.empty();
}

/**
 * @return if this command uses a weighted distribution to pick a mission.
 */
bool RuleMissionScript::hasMissionWeights() const
{
	return !_missionWeights.empty();
}

/**
 * @return if this command uses a weighted distribution to pick a region.
 */
bool RuleMissionScript::hasRegionWeights() const
{
	return !_regionWeights.empty();
}

/**
 * @return a list of research topics that govern execution of this script.
 */
const std::map<std::string, bool> &RuleMissionScript::getResearchTriggers() const
{
	return _researchTriggers;
}

/**
 * @return if this command should remove the mission it generates from the strategy table.
 */
bool RuleMissionScript::getUseTable() const
{
	return _useTable;
}

/**
 * @return the name of the variable we want to use to track in the saved game.
 */
std::string RuleMissionScript::getVarName() const
{
	return _varName;
}

/**
 * @return a complete, unique list of all the mission types this command could possibly generate.
 */
std::set<std::string> RuleMissionScript::getAllMissionTypes() const
{
	std::set<std::string> types;
	for (std::vector<std::pair<size_t, WeightedOptions*> >::const_iterator i = _missionWeights.begin(); i != _missionWeights.end(); ++i)
	{
		std::vector<std::string> names = (*i).second->getNames();
		for (std::vector<std::string>::const_iterator j = names.begin(); j != names.end(); ++j)
		{
			types.insert(*j);
		}
	}
	return types;
}

/**
 * @param month the month for which we want info.
 * @return a list of the possible missions for the given month.
 */
std::vector<std::string> RuleMissionScript::getMissionTypes(const int month) const
{
	std::vector<std::string> missions;
	std::vector<std::pair<size_t, WeightedOptions*> >::const_reverse_iterator rw = _missionWeights.rbegin();
	while (month < (int)(rw->first))
	{
		++rw;
		if (rw == _missionWeights.rend())
		{
			--rw;
			break;
		}
	}
	std::vector<std::string> names = rw->second->getNames();
	for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		missions.push_back(*i);
	}
	return missions;
}

/**
 * @param month the month for which we want info.
 * @return the list of regions we have to pick from this month.
 */
std::vector<std::string> RuleMissionScript::getRegions(const int month) const
{
	std::vector<std::string> regions;
	std::vector<std::pair<size_t, WeightedOptions*> >::const_reverse_iterator rw = _regionWeights.rbegin();
	while (month < (int)(rw->first))
	{
		++rw;
		if (rw == _regionWeights.rend())
		{
			--rw;
			break;
		}
	}
	std::vector<std::string> names = rw->second->getNames();
	for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i)
	{
		regions.push_back(*i);
	}
	return regions;
}

/**
 * Chooses one of the available races, regions, or missions for this command.
 * @param monthsPassed The number of months that have passed in the game world.
 * @param type the type of thing we want to generate, region, mission or race.
 * @return The string id of the thing.
 */
std::string RuleMissionScript::generate(const size_t monthsPassed, const GenerationType type) const
{
	std::vector<std::pair<size_t, WeightedOptions*> >::const_reverse_iterator rw;
	if (type == GEN_RACE)
		rw = _raceWeights.rbegin();
	else if (type == GEN_REGION)
		rw = _regionWeights.rbegin();
	else
		rw = _missionWeights.rbegin();
	while (monthsPassed < rw->first)
		++rw;
	return rw->second->choose();
}

/**
 * @param siteType set this command to be a missionSite type or not.
 */
void RuleMissionScript::setSiteType(const bool siteType)
{
	_siteType = siteType;
}

/**
 * @return if this is a mission site type command or not.
 */
bool RuleMissionScript::getSiteType() const
{
	return _siteType;
}

}