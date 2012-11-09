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
#ifndef OPENXCOM_UNIT_H
#define OPENXCOM_UNIT_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

enum SpecialAbility { SPECAB_NONE = 0, SPECAB_EXPLODEONDEATH, SPECAB_BURNFLOOR };
/**
 * This struct holds some plain unit attribute data together.
 */
struct UnitStats
{
	int tu, stamina, health, bravery, reactions, firing, throwing, strength, psiStrength, psiSkill, melee;
};
void operator>> (const YAML::Node& node, UnitStats& stats);
YAML::Emitter& operator<< (YAML::Emitter& out, const UnitStats& stats);

/**
 * Represents the static data for a unit that is generated on the battlescape, this includes: HWPs, aliens and civilians.
 * @sa Soldier BattleUnit
 */
class Unit
{
private:
	std::string _type;
	std::string _race;
	std::string _rank;
	UnitStats _stats;
	std::string _armor;
	int _standHeight, _kneelHeight, _loftemps;
	int _value, _deathSound, _moveSound;
	int _intelligence, _aggression;
	SpecialAbility _specab;
	std::string _zombieUnit, _spawnUnit;
public:
	/// Creates a blank unit ruleset.
	Unit(const std::string &type, std::string race, std::string rank);
	/// Cleans up the unit ruleset.
	~Unit();
	/// Loads the unit data from YAML.
	void load(const YAML::Node& node);
	/// Saves the unit data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the unit's type.
	std::string getType() const;
	/// Get the unit's stats.
	UnitStats *getStats();
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
	/// Get the value - for score calculation.
	int getValue() const;
	/// Get the death sound id.
	int getDeathSound() const;
	/// Get the move sound id.
	int getMoveSound() const;
	/// Get the intelligence. This is the number of turns AI remembers your troops position.
	int getIntelligence() const;
	/// Get the aggression. Determines the chance of revenge and taking cover.
	int getAggression() const;
	/// Get the alien's special ability.
	int getSpecialAbility() const;
	/// Gets the unit's zombie unit.
	std::string getZombieUnit() const;
	/// Gets the unit's spawn unit.
	std::string getSpawnUnit() const;

};

}

#endif
