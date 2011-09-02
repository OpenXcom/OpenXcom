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
#include <sstream>
#include "../Ruleset/RuleAlien.h"
#include "../Engine/Language.h"


namespace OpenXcom
{

/**
 * Initializes a new blank alien.
 */
Alien::Alien() : Unit(), _rules(0), _lang(0)
{
}

/**
 * Initializes a new alien with stats pulled from a ruleset.
 * @param rules Alien ruleset.
 * @param armor Alien armor.
 * @param lang Current language.
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
	std::wstringstream name;
	name << _lang->getString(_rules->getRace());
	name << _lang->getString(_rules->getRank());
	return name.str();
}

/**
 * Returns the alien's amount of time units.
 * @return Time units.
 */
int Alien::getTimeUnits() const
{
	return _rules->getStats().tu;
}

/**
 * Returns the alien's amount of stamina.
 * @return Stamina.
 */
int Alien::getStamina() const
{
	return _rules->getStats().stamina;
}

/**
 * Returns the alien's amount of health.
 * @return Health.
 */
int Alien::getHealth() const
{
	return _rules->getStats().health;
}

/**
 * Returns the alien's amount of bravery.
 * @return Bravery.
 */
int Alien::getBravery() const
{
	return _rules->getStats().bravery;
}

/**
 * Returns the alien's amount of reactions.
 * @return Reactions.
 */
int Alien::getReactions() const
{
	return _rules->getStats().reactions;
}

/**
 * Returns the alien's amount of firing accuracy.
 * @return Firing accuracy.
 */
int Alien::getFiringAccuracy() const
{
	return _rules->getStats().firing;
}

/**
 * Returns the alien's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int Alien::getThrowingAccuracy() const
{
	return _rules->getStats().throwing;
}

/**
 * Returns the alien's amount of strength.
 * @return Strength.
 */
int Alien::getStrength() const
{
	return _rules->getStats().strength;
}

/**
 * Returns the alien's rules.
 * @return rulealien
 */
RuleAlien *Alien::getRules() const
{
	return _rules;
}

/**
 * Returns the alien's stand height.
 * @return stand height
 */ 
int Alien::getStandHeight() const
{
	return _rules->getStandHeight();
}

/**
 * Returns the alien's kneel height.
 * @return kneel height
 */
int Alien::getKneelHeight() const
{
	return _rules->getKneelHeight();
}
/**
 * Returns the alien's loftemps ID.
 * @return loftemps ID
 */
int Alien::gotLoftemps() const
{
	return _rules->gotLoftemps();
}

}
