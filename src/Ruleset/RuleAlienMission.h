/*
 * Copyright 2012 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEALIENMISSION_H
#define OPENXCOM_RULEALIENMISSION_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Ruleset;
class WeightedOptions;
class Region;
class GameTime;

struct MissionWave
{
	std::string ufoType;
	unsigned ufoCount;
	std::string trajectory;
	unsigned spawnTimer;
};


void operator<<(YAML::Emitter &emitter, const MissionWave &wave);
void operator>>(const YAML::Node &node, MissionWave &wave);

class RuleAlienMission
{
public:
	/// Release all resources held by the mission.
	~RuleAlienMission();
	/// Get the mission's type.
        const std::string &getType() const { return _type; }
	/// Get a race based on the game time and the racial distribution.
        const std::string &generateRace(const GameTime &gtime) const;
	/// Loads alien mission data from YAML.
	void load(const YAML::Node &node);
	/// Saves the alien mission data to YAML.
	void save(YAML::Emitter &out) const;
private:
	std::string _type;
	std::vector<std::pair<unsigned, WeightedOptions*> > _raceDistribution;
	std::vector<MissionWave> _waves;

};

}
#endif
