/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "RuleManufactureInfo.h"

namespace OpenXcom
{
/**
 * Create a new ManufactureInfo
 * @param name The unique manufacture name
*/
RuleManufactureInfo::RuleManufactureInfo(const std::string &name) : _name(name)
{
}

/**
 * Get the unique name of the manufacture
 * @return the name
*/
std::string RuleManufactureInfo::getName () const
{
	return _name;
}

/**
 * Change the category shown in the manufacture list
 * @param category The new category
*/
void RuleManufactureInfo::setCategory (const std::string &category)
{
	_category = category;
}

/**
 * Get the category shown in the manufacture list
 * @return the category
*/
std::string RuleManufactureInfo::getCategory () const
{
	return _category;
}

/**
 * Change required workshop space
 * @param workspace The new required workspace
*/
void RuleManufactureInfo::setRequiredSpace (int workspace)
{
	_space = workspace;
}

/**
 * Get the required workspace
 * @return the required workspace to start production
*/
int RuleManufactureInfo::getRequiredSpace () const
{
	return _space;
}

/**
 * Change the time needed to manufacture one item
 * @param time the time needed to manufacture one item(in man/hour)
*/
void RuleManufactureInfo::setManufactureTime (int time)
{
	_time = time;
}

/**
 * Get the time needed to manufacture one item
 * @return The time needed to manufacture one item(in man/hour)
*/
int RuleManufactureInfo::getManufactureTime () const
{
	return _time;
}

/**
 * Change the cost of manufacturing one item
 * @param cost The cost of one item
*/
void RuleManufactureInfo::setManufactureCost (int cost)
{
	_cost = cost;
}

/**
 * Get the cost of one item
 * @return the cost of manufacturing one item
*/
int RuleManufactureInfo::getManufactureCost () const
{
	return _cost;
}

/**
 * Get the list of needed items to manufacture one item
 * @return the list of needed items to manufacture one item
*/
const std::map<std::string, int> & RuleManufactureInfo::getNeededItems() const
{
	return _neededItems;
}

/**
 * Add a new item to the list of needed items
 * @param type The item type
 * @param quantity Item quantity's needed to manufacture one item
*/
void RuleManufactureInfo::addNeededItem (std::string type, int quantity)
{
	_neededItems[type] = quantity;
}
}
