/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEALIENRACE_H
#define OPENXCOM_RULEALIENRACE_H

#include <string>
#include <vector>
#include "yaml.h"

enum AlienRank{AR_COMMANDER, AR_LEADER, AR_ENGINEER, AR_MEDIC, AR_NAVIGATOR, AR_SOLDIER, AR_TERRORIST};

namespace OpenXcom
{

/**
 * Represents a specific race "family", or a "main race" if you wish.
 * Here is defined which ranks it contains and also which accompanying terror units.
 */
class RuleAlienRace
{
private:
	std::string _id;
	std::vector<std::string> _members;
public:
	/// Creates a blank alien race ruleset.
	RuleAlienRace(const std::string &id);
	/// Cleans up the alien race ruleset.
	~RuleAlienRace();
	/// Loads alien race data from YAML.
	void load(const YAML::Node& node);
	/// Saves the alien race data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the alien race's id.
	std::string getId() const;
	/// Adds a member to the alien race family.
	void addMember(const std::string &member);
	/// Gets a certain member of this alien race family.
	std::string getMember(int id) const;
};

}

#endif
