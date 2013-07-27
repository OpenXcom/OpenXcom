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

class WeightedOptions;

/** @brief Information about a mission wave.
 * Mission waves control the UFOs that will be generated during an alien mission.
 */
struct MissionWave
{
	/// The type of the spawned UFOs.
	std::string ufoType;
	/// The number of UFOs that will be generated.
	/** The UFOs are generated sequentially, one every @a spawnTimer minutes.
	 */
	unsigned ufoCount;
	/// The trajectory ID for this wave's UFOs.
	/** Trajectories control the way UFOs fly around the Geoscape.
	 */
	std::string trajectory;
	/// Number of minutes between UFOs in the wave.
	/** The actual value used is spawnTimer/4 or 3*spawnTimer/4.
	 */
	unsigned spawnTimer;
};


/// Output a MissionWave on YAML.
void operator<<(YAML::Emitter &emitter, const MissionWave &wave);
/// Load a MissionWave from YAML.
void operator>>(const YAML::Node &node, MissionWave &wave);

/**
 * Store fixed information about a mission type.
 * It stores the mission waves and the distribution of the races that can
 * undertake the mission based on game date
 */
class RuleAlienMission
{
public:
	/// Release all resources held by the mission.
	~RuleAlienMission();
	/// Get the mission's type.
        const std::string &getType() const { return _type; }
	/// Get a race based on the game time and the racial distribution.
        const std::string &generateRace(const unsigned monthsPassed) const;
	/// Loads alien mission data from YAML.
	void load(const YAML::Node &node);
	/// Saves the alien mission data to YAML.
	void save(YAML::Emitter &out) const;
	/// Get the number of waves.
	unsigned getWaveCount() const { return _waves.size(); }
	/// Gets the full wave information.
	const MissionWave &getWave(unsigned index) const { return _waves[index]; }
	/// Gets the score for this mission.
	int getPoints() const;
private:
	/// The mission's type ID.
	std::string _type;
	/// The race distribution over game time.
	std::vector<std::pair<unsigned, WeightedOptions*> > _raceDistribution;
	/// The mission's waves.
	std::vector<MissionWave> _waves;
	/// The mission's points
	int _points;

};

}
#endif
