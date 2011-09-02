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
#include "Unit.h"

namespace OpenXcom
{

/**
 * Initializes a new blank unit.
 */
Unit::Unit() : _armor(0)
{

}

/**
 * Initializes a new unit with armor.
 * @param armor
 */
Unit::Unit(RuleArmor *armor) : _armor(armor)
{

}

/**
 *
 */
Unit::~Unit()
{

}

/**
 * Returns the unit's armor.
 * @return armor.
 */
RuleArmor * Unit::getArmor() const
{
	return _armor;
}


}
