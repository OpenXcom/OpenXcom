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
#ifndef OPENXCOM_RULEALIEN_H
#define OPENXCOM_RULEALIEN_H

#include <string>
#include "../Savegame/Unit.h"

namespace OpenXcom
{

/**
 * Represents the creation data for a specific type of unit.
 * This info is copied to either Soldier for x-com soldiers or BattleUnit for aliens and civilians.
 * @sa Soldier BattleUnit
 */
class RuleAlien
{
private:
	std::string _type;
	std::string _race;
	std::string _rank;
	UnitStats _stats;
	std::string _armor;
	int _standHeight, _kneelHeight, _loftemps;
public:
	/// Creates a blank unit ruleset.
	RuleAlien(std::string type, std::string race, std::string rank);
	/// Cleans up the unit ruleset.
	~RuleAlien();
	/// Gets the unit's type.
	std::string getType() const;
	/// Sets the unit's stats.
	void setStats(struct UnitStats stats);
	/// Sets the unit default armor.
	void setArmor(std::string armor);
	/// Sets the voxel parameters.
	void setVoxelParameters(int standHeight, int kneelHeight, int loftemps);
	/// 
	UnitStats getStats();
	/// 
	int getStandHeight();
	/// 
	int getKneelHeight();
	/// 
	int gotLoftemps();
	///
	std::string getArmor() const;
	std::string getRace() const;
	std::string getRank() const;
};

}

#endif
