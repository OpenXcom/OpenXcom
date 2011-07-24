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
#include "RuleInventory.h"
#include "City.h"

namespace OpenXcom
{
// NOTES FOR ME: SLOT SIZE = 16x16, HAND SIZE=32x48

/**
 * Creates a blank ruleset for a certain
 * type of inventory section.
 * @param type String defining the type.
 */
RuleInventory::RuleInventory(std::string type): _type(type), _x(0), _y(0), _hand(false), _slotX(), _slotY()
{
}

RuleInventory::~RuleInventory()
{
}

/**
 * Returns the language string that names
 * this inventory section. Each section has a unique name.
 * @return Section name.
 */
std::string RuleInventory::getType() const
{
	return _type;
}

/**
 * Returns the X position of the inventory section on the screen.
 * @return Position in pixels.
 */
int RuleInventory::getX() const
{
	return _x;
}

/**
 * Changes the X position of the inventory section on the screen.
 * @param x Position in pixels.
 */
void RuleInventory::setX(int x)
{
	_x = x;
}

/**
 * Returns the Yposition of the inventory section on the screen.
 * @return Position in pixels.
 */
int RuleInventory::getY() const
{
	return _y;
}

/**
 * Changes the X position of the inventory section on the screen.
 * @param y Position in pixels.
 */
void RuleInventory::setY(int y)
{
	_y = y;
}

/**
 * Returns whether the inventory section is a hand.
 * A hand only contains one slot but can hold any item.
 * @return Is it a hand?!
 */
bool RuleInventory::getHand() const
{
	return _hand;
}

/**
 * Changes whether the inventory section is a hand.
 * A hand only contains one slot but can hold any item.
 * @rparam hand Is it a hand?!
 */
void RuleInventory::setHand(bool hand)
{
	_hand = hand;
}

/**
 * Adds a storage slot to the inventory section.
 * Each slot can only be occupied by one item (or part of one).
 * @param x X position in the grid.
 * @param y Y position in the grid.
 */
void RuleInventory::addSlot(int x, int y)
{
	_slotX.push_back(x);
	_slotY.push_back(y);
}

}
