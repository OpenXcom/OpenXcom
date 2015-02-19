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
#ifndef OPENXCOM_MISSIONSITE_H
#define OPENXCOM_MISSIONSITE_H

#include "Target.h"
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleAlienMission;

/**
 * Represents an alien mission site on the world.
 */
class MissionSite : public Target
{
private:
	const RuleAlienMission *_rules;
	int _id;
	size_t _secondsRemaining;
	std::string _race;
	bool _inBattlescape;
public:
	/// Creates a mission site.
	MissionSite(const RuleAlienMission *rules);
	/// Cleans up the mission site.
	~MissionSite();
	/// Loads the mission site from YAML.
	void load(const YAML::Node& node);
	/// Saves the mission site to YAML.
	YAML::Node save() const;
	/// Saves the mission site's ID to YAML.
	YAML::Node saveId() const;
	/// Gets the mission site's ruleset.
	const RuleAlienMission *getRules() const;
	/// Gets the mission site's ID.
	int getId() const;
	/// Sets the mission site's ID.
	void setId(int id);
	/// Gets the mission site's name.
	std::wstring getName(Language *lang) const;
	/// Gets the mission site's marker.
	int getMarker() const;
	/// Gets the seconds until this mission site expires.
	size_t getSecondsRemaining() const;
	/// Sets the seconds until this mission site expires.
	void setSecondsRemaining(size_t seconds);
	/// Gets the mission site's alien race.
	std::string getAlienRace() const;
	/// Sets the mission site's alien race.
	void setAlienRace(const std::string &race);
	/// Sets the mission site's battlescape status.
	void setInBattlescape(bool inbattle);
	/// Gets if the mission site is in battlescape.
	bool isInBattlescape() const;
};

}

#endif
