/*
 * Copyright 2010 Daniel Albano
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

/**
 * Creates a blank ruleset for a certain type of item.
 * @param type String defining the type.
 */
RuleItem::RuleItem(LangString type) : _type(type), _size(0.0), _value(0), _time(24), _equip(true)
{
}

/**
 *
 */
RuleItem::~RuleItem()
{
}

/**
 * Returns the language std::string that names
 * this item. Each item type has a unique name.
 * @return Item name.
 */
LangString RuleItem::getType()
{
	return _type;
}

/**
 * Returns the amount of space this item
 * takes up in a storage facility.
 * @return Storage size.
 */
double RuleItem::getSize()
{
	return _size;
}

/**
 * Changes the amount of space this item
 * takes up in a storage facility.
 * @param size Storage size.
 */
void RuleItem::setSize(double size)
{
	_size = size;
}

/**
 * Returns the amount of money you get for
 * selling this item.
 * @return Sale value.
 */
int RuleItem::getSaleValue()
{
	return _value;
}

/**
 * Changes the amount of money you get for
 * selling this item.
 * @param value Sale value.
 */
void RuleItem::setSaleValue(int value)
{
	_value = value;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleItem::getTransferTime()
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
bool RuleItem::getEquippable()
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
