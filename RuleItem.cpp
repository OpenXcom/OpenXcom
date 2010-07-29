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
RuleItem::RuleItem(LangString type) : _type(type), _size(0.0), _value(0), _transferTime(24), _equipment(true)
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