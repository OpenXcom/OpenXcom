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
#ifndef OPENXCOM_RULEGENUNIT_H
#define OPENXCOM_RULEGENUNIT_H

#include <string>
#include "../Savegame/Unit.h"

namespace OpenXcom
{

enum SpecialAbility { SPECAB_NONE = 0, SPECAB_EXPLODEONDEATH, SPECAB_MORPHONDEATH, SPECAB_BURNSFLOOR };

/**
 * Represents the static data for a unit that is generated on the battlescape, this includes: HWPs, aliens and civilians.
 * @sa GenUnit Soldier BattleUnit
 */
class RuleGenUnit
{
private:
	std::string _type;
	std::string _race;
	std::string _rank;
	UnitStats _stats;
	std::string _armor;
	int _standHeight, _kneelHeight, _loftemps;
	int _value, _deathSound;
	int _intelligence, _aggression;
	SpecialAbility _specab;
public:
	/// Creates a blank unit ruleset.
	RuleGenUnit(const std::string &type, std::string race, std::string rank);
	/// Cleans up the unit ruleset.
	~RuleGenUnit();
	/// Gets the unit's type.
	std::string getType() const;
	/// Sets the unit's stats.
	void setStats(struct UnitStats stats);
	/// Sets the unit default armor.
	void setArmor(const std::string &armor);
	/// Sets the voxel parameters.
	void setVoxelParameters(int standHeight, int kneelHeight, int loftemps);
	/// Sets various AI parameters.
	void setAIParameters(int intelligence, int aggression);
	/// Sets various AI parameters.
	void setSpecialAbility(SpecialAbility specab);
	/// Get the unit's stats.
	UnitStats getStats() const;
	/// Get the unit's height when standing.
	int getStandHeight() const;
	/// Get the unit's height when kneeling.
	int getKneelHeight() const;
	/// Get the 3D templates
	int getLoftemps() const;
	/// Get the armor type.
	std::string getArmor() const;
	/// Get the alien race type.
	std::string getRace() const;
	/// Get the alien rank.
	std::string getRank() const;
	/// Set the value - for score calculation.
	void setValue(int value);
	/// Get the value - for score calculation.
	int getValue() const;
	/// Set the death sound id.
	void setDeathSound(int id);
	/// Get the death sound id.
	int getDeathSound() const;
	/// Get the intelligence. This is the number of turns AI remembers your troops position.
	int getIntelligence() const;
	/// Get the aggression. Determines the chance of revenge and taking cover.
	int getAggression() const;
	/// Get the alien's special ability.
	int getSpecialAbility() const;
};

}

#endif
