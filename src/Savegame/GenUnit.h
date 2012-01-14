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
#ifndef OPENXCOM_GENUNIT_H
#define OPENXCOM_GENUNIT_H

#include <string>
#include "Unit.h"

namespace OpenXcom
{
	class RuleGenUnit;
	class Language;
/**
 * Represents a unit that is generated on the battlescape, this includes: HWPs, aliens and civilians.
 */
class GenUnit : public Unit
{
private:
	RuleGenUnit *_rules;
public:
	/// Creates a new generated unit.
	GenUnit();
	/// Creates a new generated unit with specific rules and armor.
	GenUnit(RuleGenUnit *rules, RuleArmor *armor);
	/// Cleans up the generated unit.
	~GenUnit();
	/// Gets the generated unit's name.
	std::wstring getName(Language *lang) const;
	/// Gets the generated unit's time units.
	int getTimeUnits() const;
	/// Gets the generated unit's stamina.
	int getStamina() const;
	/// Gets the generated unit's health.
	int getHealth() const;
	/// Gets the generated unit's bravery.
	int getBravery() const;
	/// Gets the generated unit's reactions.
	int getReactions() const;
	/// Gets the generated unit's firing accuracy.
	int getFiringAccuracy() const;
	/// Gets the generated unit's throwing accuracy.
	int getThrowingAccuracy() const;
	/// Gets the generated unit's strength.
	int getStrength() const;
	/// Gets the generated unit's rules.
	RuleGenUnit *getRules() const;
	/// Gets the generated unit's stand height.
	int getStandHeight() const;
	/// Gets the generated unit's kneel height.
	int getKneelHeight() const;
	/// Gets the generated unit's loft ID.
	int getLoftemps() const;
	/// Gets the generated unit's value.
	int getValue() const;
	/// Get wether the unit is affected by fatal wounds.
	bool isWoundable() const;
	/// Get wether the unit is affected by fear.
	bool isFearable() const;
	/// Get the unit's intelligence.
	int getIntelligence() const;
	/// Get the unit's aggression.
	int getAggression() const;

};

}

#endif
