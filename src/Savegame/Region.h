#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleRegion;

/**
 * Represents a region of the world.
 * Contains variable info about a region like
 * X-Com and alien activity in it.
 */
class Region
{
private:
	RuleRegion *_rules;
	std::vector<int> _activityXcom, _activityAlien;
public:
	/// Creates a new region of the specified type.
	Region(RuleRegion *rules);
	/// Cleans up the region.
	~Region();
	/// Loads the region from YAML.
	void load(const YAML::Node& node);
	/// Saves the region to YAML.
	YAML::Node save() const;
	/// Gets the region's ruleset.
	RuleRegion *getRules() const;
	/// add xcom activity in this region
	void addActivityXcom(int activity);
	/// add alien activity in this region
	void addActivityAlien(int activity);
	/// get xcom activity to this region
	std::vector<int> &getActivityXcom();
	/// get xcom activity to this region
	std::vector<int> &getActivityAlien();
	/// start new month of activity
	void newMonth();
};

}
