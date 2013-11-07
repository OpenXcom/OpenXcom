/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "RuleManufacture.h"

namespace OpenXcom
{
/**
 * Creates a new Manufacture.
 * @param name The unique manufacture name.
 */
RuleManufacture::RuleManufacture(const std::string &name) : _name(name), _producedItem(name), _space(0), _time(0), _cost(0), _listOrder(0), _produceQty(1)
{
}

/**
 * Loads the manufacture project from a YAML file.
 * @param node YAML node.
 * @param listOrder The list weight for this manufacture.
 */
void RuleManufacture::load(const YAML::Node &node, int listOrder)
{
	if (_name == _producedItem) _producedItem = _name = node["name"].as<std::string>(_name);
	else _name = node["name"].as<std::string>(_name);
	if (node["producedItem"]) _producedItem = node["producedItem"].as<std::string>(_producedItem);
	_category = node["category"].as<std::string>(_category);
	_requires = node["requires"].as< std::vector<std::string> >(_requires);
	_space = node["space"].as<int>(_space);
	_time = node["time"].as<int>(_time);
	_cost = node["cost"].as<int>(_cost);
	if (node["produceQty"]) _produceQty = node["produceQty"].as<int>(_produceQty);
	_requiredItems = node["requiredItems"].as< std::map<std::string, int> >(_requiredItems);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Gets the unique name of the manufacture.
 * @return The name.
 */
std::string RuleManufacture::getName () const
{
	return _name;
}

/**
 * Get the name of the produced item (it is the same as getName in most cases, and by default)
 * @return the name
*/
std::string RuleManufacture::getProducedItem() const
{
  return _producedItem;
}

/**
 * Gets the category shown in the manufacture list.
 * @return The category.
 */
std::string RuleManufacture::getCategory () const
{
	return _category;
}

/**
 * Gets the list of research required to
 * manufacture this object.
 * @return A list of research IDs.
 */
const std::vector<std::string> &RuleManufacture::getRequirements() const
{
	return _requires;
}

/**
 * Gets the required workspace to start production.
 * @return The required workspace.
*/
int RuleManufacture::getRequiredSpace () const
{
	return _space;
}

/**
 * Gets the time needed to manufacture one object.
 * @return The time needed to manufacture one object (in man/hour).
*/
int RuleManufacture::getManufactureTime () const
{
	return _time;
}


/**
 * Gets the cost of manufacturing one object.
 * @return The cost of manufacturing one object.
*/
int RuleManufacture::getManufactureCost () const
{
	return _cost;
}

/**
 * Get the number of objects to manufacture instead of one (it is 1 in most cases, and by default)
 * @return the number
*/
int RuleManufacture::getProduceQty() const
{
  return _produceQty;
}
/**
 * Gets the list of items required to manufacture one object.
 * @return The list of items required to manufacture one object.
*/
const std::map<std::string, int> & RuleManufacture::getRequiredItems() const
{
	return _requiredItems;
}

/**
 * Gets the list weight for this manufacture item.
 * @return The list weight for this manufacture item.
 */
int RuleManufacture::getListOrder() const
{
	return _listOrder;
}
}
