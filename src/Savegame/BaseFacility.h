/*
 * Copyright 2010 Daniel Albano
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
	/// Gets the facility's ruleset.
	RuleBaseFacility *getRules();
	/// Gets the facility's X position.
	int getX();
	/// Gets the facility's Y position.
	int getY();
	/// Gets the facility's construction time.
	int getBuildTime();
	/// Sets the facility's construction time.
	void setBuildTime(int time);
	/// Checks if a target is inside the facility's radar.
	bool insideRadarRange(Base *base, Target *target);
};

#endif
