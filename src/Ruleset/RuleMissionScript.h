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

#ifndef OPENXCOM_RULEMISSIONSCRIPT_H
#define OPENXCOM_RULEMISSIONSCRIPT_H

#include <string>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>
#include "../Savegame/WeightedOptions.h"

namespace OpenXcom
{
enum GenerationType { GEN_REGION, GEN_MISSION, GEN_RACE };
class WeightedOptions;
class RuleMissionScript
{
private:
	std::string _type, _varName;
	int _firstMonth, _lastMonth, _label, _executionOdds, _targetBaseOdds, _minDifficulty, _maxRuns, _avoidRepeats, _delay;
	std::vector<int> _conditionals;
	std::vector<std::pair<size_t, WeightedOptions*> > _regionWeights, _missionWeights, _raceWeights;
	std::map<std::string, bool> _researchTriggers;
	bool _useTable, _siteType;
public:
	/// Creates a new mission script.
	RuleMissionScript(const std::string type);
	/// Deletes a mission script.
	~RuleMissionScript();
	/// Loads a mission script from yaml.
	void load(const YAML::Node& node);
	/// Gets the name of the script command.
	const std::string getType() const;
	/// Gets the name of the variable to use for keeping track of... things.
	const std::string getVarName() const;
	/// Gets a complete and unique list of all the mission types contained within this command.
	const std::set<std::string> getAllMissionTypes() const;
	/// Gets a list of the regions in this command's region weights for a given month.
	const std::vector<std::string> getRegions(const int month) const;
	/// Gets a list of the mission types in this command's mission weights for a given month.
	const std::vector<std::string> getMissionTypes(const int month) const;
	/// Gets the first month this command will run.
	const int getFirstMonth() const;
	/// Gets the last month this command will run.
	const int getLastMonth() const;
	/// Gets the label of this command, used for conditionals.
	const int getLabel() const;
	/// Gets the odds of this command executing.
	const int getExecutionOdds() const;
	/// Gets the odds of this mission targetting an xcom base.
	const int getTargetBaseOdds() const;
	/// Gets the minimum difficulty for this command to run
	const int getMinDifficulty() const;
	/// Gets the maximum number of times to run a command with this varName
	const int getMaxRuns() const;
	/// Gets how many previous mission sites to keep track of (to avoid using them again)
	const int getRepeatAvoidance() const;
	/// Gets the number of minutes to delay spawning of the first wave of this mission, overrides the spawn delay defined in the mission waves.
	const int getDelay() const;
	/// Gets the list of conditions this command requires in order to run.
	const std::vector<int> &getConditionals() const;
	/// Does this command have raceWeights?
	const bool hasRaceWeights() const;
	/// Does this command have mission weights?
	const bool hasMissionWeights() const;
	/// Does this command have region weights?
	const bool hasRegionWeights() const;
	/// Gets the research triggers that may apply to this command.
	const std::map<std::string, bool> &getResearchTriggers() const;
	/// Delete this mission from the table? stops it coming up again in random selection, but NOT if a missionScript calls it by name.
	const bool getUseTable() const;
	/// Sets this script to a terror mission type command or not.
	void setSiteType(const bool siteType);
	/// Checks if this is a terror-type mission or not.
	const bool getSiteType() const;
	/// Generates either a region, a mission, or a race based on the month.
	const std::string generate(const size_t monthsPassed, const GenerationType type) const;
	
};

}
#endif
