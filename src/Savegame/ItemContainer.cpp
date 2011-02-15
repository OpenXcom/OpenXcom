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
#include "ItemContainer.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes an item container with no contents.
 */
ItemContainer::ItemContainer() : _qty(), _rules()
{
}

/**
 *
 */
ItemContainer::~ItemContainer()
{
}

/**
 * Loads the item container from a YAML file.
 * @param node YAML node.
 */
void ItemContainer::load(const YAML::Node &node)
{
	node >> _qty;
}

/**
 * Saves the item container to a YAML file.
 * @param out YAML emitter.
 */
void ItemContainer::save(YAML::Emitter &out) const
{
	out << _qty;
}

/**
 * Adds an item amount to the container.
 * @param iitem Item ruleset.
 * @param qty Item quantity.
 */
void ItemContainer::addItem(RuleItem *item, int qty)
{
	if (_qty.find(item->getType()) == _qty.end())
	{
		_qty[item->getType()] = 0;
		_rules[item->getType()] = item;
	}
	_qty[item->getType()] += qty;
}

/**
 * Removes an item amount from the container.
 * @param id Item ID.
 * @param qty Item quantity.
 */
void ItemContainer::removeItem(const std::string &id, int qty)
{
	if (_qty.find(id) == _qty.end())
	{
		return;
	}
	if (qty < _qty[id])
	{
		_qty[id] -= qty;
	}
	else
	{
		_qty.erase(id);
	}
}

/**
 * Returns the quantity of an item in the container.
 * @param id Item ID.
 * @return Item quantity.
 */
int ItemContainer::getItem(const std::string &id) const
{
	std::map<std::string, int>::const_iterator it = _qty.find(id);

	if (it == _qty.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

/**
 * Returns the ruleset of an item in the container.
 * @param id Item ID.
 * @return Item rules.
 */
RuleItem *ItemContainer::getRule(const std::string &id) const
{
	std::map<std::string, RuleItem*>::const_iterator it = _rules.find(id);

	if (it == _rules.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

/**
 * Returns the total quantity of the items in the container.
 * @return Total item quantity.
 */
int ItemContainer::getTotalQuantity() const
{
	int total = 0;
	for (std::map<std::string, int>::const_iterator i = _qty.begin(); i != _qty.end(); i++)
	{
		total += i->second;
	}
	return total;
}

/**
 * Returns the total size of the items in the container.
 * @return Total item size.
 */
double ItemContainer::getTotalSize() const
{
	double total = 0;
	std::map<std::string, int>::const_iterator i;
	std::map<std::string, RuleItem*>::const_iterator j;
	for (i = _qty.begin(), j = _rules.begin(); i != _qty.end(); i++)
	{
		total += j->second->getSize() * i->second;
	}
	return total;
}

/**
 * Returns all the items currently contained within.
 * @return List of contents.
 */
std::map<std::string, int> *const ItemContainer::getContents()
{
	return &_qty;
}

}
