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
#ifndef OPENXCOM_UNIT_H
#define OPENXCOM_UNIT_H

#include <string>

namespace OpenXcom
{

/**
 * This struct holds some plain unit attribute data together.
 */
struct UnitStats
{
	int tu, stamina, health, bravery, reactions, firing, throwing, strength, psiStrength, psiSkill, melee;
};

class RuleArmor;
class Language;

/**
 * Represents a unit which is mostly just an abstract class to access common attributes/functions for both Soldiers and Generated Units.
 */
class Unit
{
private:
	RuleArmor *_armor;
public:
	/// Creates a new Unit.
	Unit();
	/// Creates a new unnit with specific rules and armor.
	Unit(RuleArmor *armor);
	/// Cleans up the Unit.
	~Unit();
	/// Gets the unit's armor.
	RuleArmor *getArmor() const;
	/// Gets the unit's name.
	virtual std::wstring getName(Language *lang) const = 0;
	/// Gets the unit's time units.
	virtual int getTimeUnits() const = 0;
	/// Gets the unit's stamina.
	virtual int getStamina() const = 0;
	/// Gets the unit's health.
	virtual int getHealth() const = 0;
	/// Gets the unit's bravery.
	virtual int getBravery() const = 0;
	/// Gets the unit's reactions.
	virtual int getReactions() const = 0;
	/// Gets the unit's firing accuracy.
	virtual int getFiringAccuracy() const = 0;
	/// Gets the unit's throwing accuracy.
	virtual int getThrowingAccuracy() const = 0;
	/// Gets the unit's strength.
	virtual int getStrength() const = 0;
	/// Get the unit's stand height.
	virtual int getStandHeight() const = 0;
	/// Get the unit's kneel height.
	virtual int getKneelHeight() const = 0;
	/// Get the unit's loft ID.
	virtual int getLoftemps() const = 0;
	/// Get the unit's value.
	virtual int getValue() const = 0;
	/// Get wether the unit is affected by fatal wounds.
	virtual bool isWoundable() const = 0;
	/// Get wether the unit is affected by fear.
	virtual bool isFearable() const = 0;
	/// Get the unit's intelligence.
	virtual int getIntelligence() const = 0;
	/// Get the unit's aggression.
	virtual int getAggression() const = 0;

};

}

#endif
