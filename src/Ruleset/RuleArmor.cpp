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
#include "RuleArmor.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of armor.
 * @param type String defining the type.
 */
RuleArmor::RuleArmor(std::string type, std::string spriteSheet) : _type(type), _spriteSheet(spriteSheet)
{

}

/**
 *
 */
RuleArmor::~RuleArmor()
{

}

/**
 * Returns the language string that names
 * this armor. Each armor has a unique name. Coveralls, Power Suit,...
 * @return Craft name.
 */
std::string RuleArmor::getType() const
{
	return _type;
}

/// Gets the unit's sprite sheet.
std::string RuleArmor::getSpriteSheet() const
{
	return _spriteSheet;
}

/// Sets the different armor levels.
void RuleArmor::setArmor(int front, int side, int rear, int under)
{
	_frontArmor = front;
	_sideArmor = side;
	_rearArmor = rear;
	_underArmor = under;
}

/// Get the front armor level.
int RuleArmor::getFrontArmor() const
{
	return _frontArmor;
}
/// Get the side armor level.
int RuleArmor::getSideArmor() const
{
	return _sideArmor;
}
/// get the rear armor level.
int RuleArmor::getRearArmor() const
{
	return _rearArmor;
}
/// get the under armor level.
int RuleArmor::getUnderArmor() const
{
	return _underArmor;
}
/// Set the corpse item.
void RuleArmor::setCorpseItem(std::string corpseItem)
{
	_corpseItem = corpseItem;
}
/// Get the corpse item.
std::string RuleArmor::getCorpseItem() const
{
	return _corpseItem;
}


}
