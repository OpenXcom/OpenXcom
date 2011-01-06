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
#ifndef OPENXCOM_BASEFACILITY_H
#define OPENXCOM_BASEFACILITY_H

#include "yaml.h"

namespace OpenXcom
{

class RuleBaseFacility;
class Base;
class Target;

/**
 * Represents a base facility placed in a base.
 * Contains variable info about a facility like
 * position and build time.
 * @sa RuleBaseFacility
 */
class BaseFacility
{
private:
	RuleBaseFacility *_rules;
	int _x, _y, _buildTime;
public:
	/// Creates a base facility of the specified type.
	BaseFacility(RuleBaseFacility *rules, int x, int y);
	/// Cleans up the base facility.
	~BaseFacility();
	/// Loads the base facility from YAML.
	void load(const YAML::Node& node);
	/// Saves the base facility to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the facility's ruleset.
	RuleBaseFacility *const getRules() const;
	/// Gets the facility's X position.
	int getX() const;
	/// Gets the facility's Y position.
	int getY() const;
	/// Gets the facility's construction time.
	int getBuildTime() const;
	/// Sets the facility's construction time.
	void setBuildTime(int time);
	/// Builds up the facility.
	void build();
	/// Checks if a target is inside the facility's radar.
	bool insideRadarRange(Base *base, Target *target) const;
};

}

#endif
