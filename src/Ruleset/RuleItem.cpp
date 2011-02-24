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
#include "RuleItem.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of item.
 * @param type String defining the type.
 */
RuleItem::RuleItem(std::string type) : _type(type), _size(0.0), _cost(0), _time(24), _equip(true), _twoHanded(false)
{
}

/**
 *
 */
RuleItem::~RuleItem()
{
}

/**
 * Returns the language string that names
 * this item. Each item type has a unique name.
 * @return Item name.
 */
std::string RuleItem::getType() const
{
	return _type;
}

/**
 * Returns the amount of space this item
 * takes up in a storage facility.
 * @return Storage size.
 */
float RuleItem::getSize() const
{
	return _size;
}

/**
 * Changes the amount of space this item
 * takes up in a storage facility.
 * @param size Storage size.
 */
void RuleItem::setSize(float size)
{
	_size = size;
}

/**
 * Returns the amount of money this item
 * costs in purchase/sale.
 * @return Cost.
 */
int RuleItem::getCost() const
{
	return _cost;
}

/**
 * Changes the amount of money this item
 * costs in purchase/sale.
 * @param cost Cost.
 */
void RuleItem::setCost(int cost)
{
	_cost = cost;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleItem::getTransferTime() const
{
	return _time;
}

/**
 * Changes the amount of time this item
 * takes to arrive at a base.
 * @param time Time in hours.
 */
void RuleItem::setTransferTime(int time)
{
	_time = time;
}

/**
 * Returns whether this item can be equipped
 * by soldiers and loaded on transport craft.
 * @return Is it equippable?
 */
bool RuleItem::getEquippable() const
{
	return _equip;
}

/**
 * Changes whether this item can be equipped
 * by soldiers and loaded on transport craft.
 * @param equip Is it equippable?
 */
void RuleItem::setEquippable(bool equip)
{
	_equip = equip;
}

/**
 * Returns the reference in BIGOBS.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getBigSprite() const
{
	return _bigSprite;
}

/**
 * Changes the reference in BIGOBS.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setBigSprite(int sprite)
{
	_bigSprite = sprite;
}

/**
 * Returns the reference in FLOOROB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getFloorSprite() const
{
	return _floorSprite;
}

/**
 * Changes the reference in FLOOROB.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setFloorSprite(int sprite)
{
	_floorSprite = sprite;
}

/**
 * Returns the reference in HANDOB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getHandSprite() const
{
	return _handSprite;
}

/**
 * Changes the reference in HANDOB.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setHandSprite(int sprite)
{
	_handSprite = sprite;
}

/**
 * Returns whether this item is held with two hands.
 * @return Is it two-handed?
 */
bool RuleItem::getTwoHanded() const
{
	return _twoHanded;
}

/**
 * Changes whether this item is held with two hands.
 * @param equip Is it two-handed?
 */
void RuleItem::setTwoHanded(bool flag)
{
	_twoHanded = flag;
}
}
