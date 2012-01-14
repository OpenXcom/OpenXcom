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
#include "GenUnit.h"
#include <sstream>
#include "../Ruleset/RuleGenUnit.h"
#include "../Engine/Language.h"


namespace OpenXcom
{

/**
 * Initializes a new blank generated unit.
 */
GenUnit::GenUnit() : Unit(), _rules(0)
{
}

/**
 * Initializes a new generated unit with stats pulled from a ruleset.
 * @param rules generated unit ruleset.
 * @param armor generated unit armor.
 * @param lang Current language.
 */
GenUnit::GenUnit(RuleGenUnit *rules, RuleArmor *armor) : Unit(armor), _rules(rules)
{
}

/**
 *
 */
GenUnit::~GenUnit()
{
}

/**
 * Returns the generated unit's full name. HWPs and civilians don't have a rank.
 * @return name.
 */
std::wstring GenUnit::getName(Language *lang) const
{
	std::wstringstream name;
	if (lang)
	{
		name << lang->getString(_rules->getRace());
		name << lang->getString(_rules->getRank());
	}
	return name.str();
}

/**
 * Returns the generated unit's amount of base time units.
 * @return Time units.
 */
int GenUnit::getTimeUnits() const
{
	return _rules->getStats().tu;
}

/**
 * Returns the generated unit's amount of base stamina.
 * @return Stamina.
 */
int GenUnit::getStamina() const
{
	return _rules->getStats().stamina;
}

/**
 * Returns the generated unit's amount of base health.
 * @return Health.
 */
int GenUnit::getHealth() const
{
	return _rules->getStats().health;
}

/**
 * Returns the generated unit's amount of bravery.
 * @return Bravery.
 */
int GenUnit::getBravery() const
{
	return _rules->getStats().bravery;
}

/**
 * Returns the generated unit's amount of reactions.
 * @return Reactions.
 */
int GenUnit::getReactions() const
{
	return _rules->getStats().reactions;
}

/**
 * Returns the generated unit's amount of firing accuracy.
 * @return Firing accuracy.
 */
int GenUnit::getFiringAccuracy() const
{
	return _rules->getStats().firing;
}

/**
 * Returns the generated unit's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int GenUnit::getThrowingAccuracy() const
{
	return _rules->getStats().throwing;
}

/**
 * Returns the generated unit's amount of strength.
 * @return Strength.
 */
int GenUnit::getStrength() const
{
	return _rules->getStats().strength;
}

/**
 * Returns the generated unit's rules.
 * @return RuleGenUnit
 */
RuleGenUnit *GenUnit::getRules() const
{
	return _rules;
}

/**
 * Returns the generated unit's stand height.
 * @return stand height
 */
int GenUnit::getStandHeight() const
{
	return _rules->getStandHeight();
}

/**
 * Returns the generated unit's kneel height.
 * @return kneel height
 */
int GenUnit::getKneelHeight() const
{
	return _rules->getKneelHeight();
}
/**
 * Returns the generated unit's loftemps ID.
 * @return loftemps ID
 */
int GenUnit::getLoftemps() const
{
	return _rules->getLoftemps();
}

/**
 * Returns the generated unit's value.
 * @return value
 */
int GenUnit::getValue() const
{
	return _rules->getValue();
}

/**
 * Returns wether the unit is affected by fatal wounds.
 * Aliens can't stop bleeding of fatal wounds, so it's only fair they don't receive fatail wounds.
 * @return bool
 */
bool GenUnit::isWoundable() const
{
	return false;
}

/**
 * Returns wether the unit is affected by fear..
 * @return bool
 */
bool GenUnit::isFearable() const
{
	return false;
}


/**
 * Returns the unit's intelligence.
 * @return int
 */
int GenUnit::getIntelligence() const
{
	return _rules->getIntelligence();
}

/**
 * Returns the unit's aggression.
 * @return int
 */
int GenUnit::getAggression() const
{
	return _rules->getAggression();
}

}
