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
#include "Alien.h"
#include "../Ruleset/RuleAlien.h"

namespace OpenXcom
{

/**
 * Initializes a new blank soldier.
 */
Alien::Alien()
{
}

/**
 * Initializes a new soldier with random stats and a name
 * pulled from a set of SoldierNamePool's.
 * @param names List of name pools.
 */
Alien::Alien(RuleAlien *rules, RuleArmor *armor, Language *lang) : Unit(armor), _rules(rules), _lang(lang)
{
}

/**
 *
 */
Alien::~Alien()
{
}

/**
 * Returns the Alien's full name.
 * @return Soldier name.
 */
std::wstring Alien::getName() const
{
	return L"Alien";
}

/**
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int Alien::getTimeUnits() const
{
	return _rules->getStats().tu;
}

/**
 * Returns the soldier's amount of stamina.
 * @return Stamina.
 */
int Alien::getStamina() const
{
	return _rules->getStats().stamina;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int Alien::getHealth() const
{
	return _rules->getStats().health;
}

/**
 * Returns the soldier's amount of bravery.
 * @return Bravery.
 */
int Alien::getBravery() const
{
	return _rules->getStats().bravery;
}

/**
 * Returns the soldier's amount of reactions.
 * @return Reactions.
 */
int Alien::getReactions() const
{
	return _rules->getStats().reactions;
}

/**
 * Returns the soldier's amount of firing accuracy.
 * @return Firing accuracy.
 */
int Alien::getFiringAccuracy() const
{
	return _rules->getStats().firing;
}

/**
 * Returns the soldier's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int Alien::getThrowingAccuracy() const
{
	return _rules->getStats().throwing;
}

/**
 * Returns the soldier's amount of strength.
 * @return Strength.
 */
int Alien::getStrength() const
{
	return _rules->getStats().strength;
}

/**
 * Returns the soldier's rules.
 * @return rulealien
 */
RuleAlien *Alien::getRules() const
{
	return _rules;
}

/**
 * Returns the soldier's stand height.
 * @return stand height
 */ 
int Alien::getStandHeight() const
{
	return _rules->getStandHeight();
}

/**
 * Returns the soldier's kneel height.
 * @return kneel height
 */
int Alien::getKneelHeight() const
{
	return _rules->getKneelHeight();
}
/**
 * Returns the soldier's loftemps ID.
 * @return loftemps ID
 */
int Alien::gotLoftemps() const
{
	return _rules->gotLoftemps();
}

}
