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
#ifndef OPENXCOM_ALIEN_H
#define OPENXCOM_ALIEN_H

#include <string>
#include "Unit.h"

namespace OpenXcom
{
	class RuleAlien;
	class Language;
/**
 * Represents an alien's base class. It doesn't hold actual attributes as it just uses RuleUnit for that.
 */
class Alien : public Unit
{
private:
	RuleAlien *_rules;
	Language *_lang;
public:
	/// Creates a new Alien.
	Alien();
	/// Creates a new Alien with specific rules and armor.
	Alien(RuleAlien *rules, RuleArmor *armor, Language *lang);
	/// Cleans up the Alien.
	~Alien();
	/// Gets the Alien's name.
	std::wstring getName() const;
	/// Gets the Alien's time units.
	int getTimeUnits() const;
	/// Gets the Alien's stamina.
	int getStamina() const;
	/// Gets the Alien's health.
	int getHealth() const;
	/// Gets the Alien's bravery.
	int getBravery() const;
	/// Gets the Alien's reactions.
	int getReactions() const;
	/// Gets the Alien's firing accuracy.
	int getFiringAccuracy() const;
	/// Gets the Alien's throwing accuracy.
	int getThrowingAccuracy() const;
	/// Gets the Alien's strength.
	int getStrength() const;
	RuleAlien *getRules() const;
	/// 
	int getStandHeight() const;
	/// 
	int getKneelHeight() const;
	/// 
	int gotLoftemps() const;
};

}

#endif
