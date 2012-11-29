/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_ALIEN_MISSION_H
#define OPENXCOM_ALIEN_MISSION_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleAlienMission;
class Ruleset;
class SavedGame;
class Ufo;
class Globe;

/**
 * Represents an ongoing alien mission.
 * Contains variable info about the mission, like spawn counter, target region
 * and current wave.
 * @sa RuleAlienMission
 */
class AlienMission
{
private:
	const RuleAlienMission &_rule;
	std::string _region, _race;
	unsigned _nextWave;
	unsigned _nextUfoCounter;
	unsigned _spawnCountdown;
	unsigned _liveUfos;
public:
	/// Creates a mission of the specified type.
	AlienMission(const RuleAlienMission &rule);
	/// Cleans up the mission info.
	~AlienMission();
	/// Loads the mission from YAML.
	void load(const YAML::Node& node);
	/// Saves the mission to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the mission's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the mission's type.
	const std::string &getType() const;
	/// Gets the mission's region.
	const std::string &getRegion() const { return _region; }
	/// Sets the mission's region.
	void setRegion(const std::string &region) { _region = region; }
	/// Gets the mission's race.
	const std::string &getRace() const { return _race; }
	/// Sets the mission's race.
	void setRace(const std::string &race) { _race = race; }
	/// Is this mission over?
	bool isOver() const;
	/// Handle UFO spawning for the mission.
	void think(const Ruleset &rules, SavedGame &game, const Globe &globe);
	/// Initialize with values from rules.
	void start(unsigned initialCount = 0);
	/// Increase number of live UFOs.
	void increaseLiveUfos() { ++_liveUfos; }
	/// Decrease number of live UFOs.
	void decreaseLiveUfos() { --_liveUfos; }
	/// Handle UFO reaching a waypoint.
	void ufoReachedWaypoint(Ufo &ufo, const Ruleset &rules, SavedGame &game, const Globe &globe);
	/// Handle UFO lifting from the ground.
	void ufoLifting(Ufo &ufo, const Ruleset &rules, SavedGame &game, const Globe &globe);
	/// Handle UFO shot down.
	void ufoShotDown(Ufo &ufo, const Ruleset &rules, SavedGame &game);
	/// Gets the minutes until next wave spawns.
	unsigned getWaveCountdown() const { return _spawnCountdown; }
	/// Sets the minutes until next wave spawns.
	void setWaveCountdown(unsigned minutes);
};

}

#endif
