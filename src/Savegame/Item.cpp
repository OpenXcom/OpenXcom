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
#include "Item.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes a item of the specified type.
 * @param rules Pointer to ruleset.
 * @param qty Initial quantity.
 */
Item::Item(RuleItem *rules, int qty) : _rules(rules), _qty(qty)
{
}

/**
 *
 */
Item::~Item()
{
}

/**
 * Loads the item from a YAML file.
 * @param node YAML node.
 */
void Item::load(const YAML::Node &node)
{
	node["qty"] >> _qty;
}

/**
 * Saves the item to a YAML file.
 * @param out YAML emitter.
 */
void Item::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "qty" << YAML::Value << _qty;
	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the item's type.
 * @return Pointer to ruleset.
 */
RuleItem *const Item::getRules() const
{
	return _rules;
}

/**
 * Returns the quantity stored of this item.
 * @return Item quantity.
 */
int Item::getQuantity() const
{
	return _qty;
}

/**
 * Changes the quantity stored of this item.
 * @param qty Item quantity.
 */
void Item::setQuantity(int qty)
{
	_qty = qty;
}

/**
 * Returns the storage size of all the items
 * totalled up.
 * @return Item size.
 */
double Item::getTotalSize() const
{
	return _qty * _rules->getSize();
}

}
