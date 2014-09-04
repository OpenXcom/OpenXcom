/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEMISSIONSEQUENCE_H
#define OPENXCOM_RULEMISSIONSEQUENCE_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "RuleScriptedEvent.h"

namespace OpenXcom
{

class Ruleset;

struct MissionData
{
	std::string deployment_id;
	std::string race_id;
	int shade;
	MissionScreenData briefingScreen;
};

/**
 * Represents a specific type of UFO.
 * Contains constant info about a UFO like
 * speed, weapons, scores, etc.
 * @sa Ufo
 */
class RuleMissionSequence
{
private:
	std::string _type;
	std::vector<MissionData> _missionSequence;

public:
	/// Creates a blank Scripted Event ruleset.
	RuleMissionSequence(const std::string &type);
	/// Cleans up the Scripted Event ruleset.
	~RuleMissionSequence();
	/// Loads Scripted Event data from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset);

	/// Gets the ID for the name text
	const std::string &getType() const;
	/// Get the battlescape mission payloads
	const int getNumMissions() const;
	const MissionData &getMission(int i) const;
};

}

#endif
