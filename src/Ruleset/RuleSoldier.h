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
#ifndef OPENXCOM_RULESOLDIER_H
#define OPENXCOM_RULESOLDIER_H

#include <string>
#include "../Ruleset/Unit.h"

namespace OpenXcom
{

/**
 * Represents the creation data for a specific type of unit.
 * This info is copied to either Soldier for x-com soldiers or BattleUnit for aliens and civilians.
 * @sa Soldier BattleUnit
 */
class RuleSoldier
{
private:
	std::string _type;
	UnitStats _minStats, _maxStats;
	std::string _armor;
	int _standHeight, _kneelHeight;
	std::vector<int> _loftempsSet;
public:
	/// Creates a blank unit ruleset.
	RuleSoldier(const std::string &type);
	/// Cleans up the unit ruleset.
	~RuleSoldier();
	/// Loads the unit data from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the unit's type.
	std::string getType() const;
	/// Get the minimum stats for the random stats generator.
	UnitStats getMinStats() const;
	/// Get the maximum stats for the random stats generator.
	UnitStats getMaxStats() const;
	/// Get the height of the soldier when it's standing.
	int getStandHeight() const;
	/// Get the height of the soldier when it's kneeling.
	int getKneelHeight() const;
	/// Get the line of fire template (3D voxels)
	int getLoftemps(int entry = 0) const;
	std::vector<int> getLoftempsSet() const;
	/// Get the armor name.
	std::string getArmor() const;

};

}

#endif
