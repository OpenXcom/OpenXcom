/*
 * Copyright 2011 OpenXcom Developers.
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
 * @param workspace Required workshop space
 * @param time Required time(in man/hour) to manufacture one item
 * @param cost Cost to manufacture one item
*/
RuleManufactureInfo::RuleManufactureInfo(int workspace, int time, int cost) : _requiredWorkspace(workspace), _manufactureTime(time), _manufactureCost(cost)
{
}

/**
 * Change required workshop space
 * param workspace The new required workspace
*/
void RuleManufactureInfo::setRequiredSpace (int workspace)
{
	_requiredWorkspace = workspace;
}

/**
 * Get the required workspace
 * @return the required workspace to start production
*/
int RuleManufactureInfo::getRequiredSpace () const
{
	return _requiredWorkspace;
}

/**
 * Change the time needed to manufacture one item
 * @param time the time needed to manufacture one item(in man/hour)
*/
void RuleManufactureInfo::setManufactureTime (int time)
{
	_manufactureTime = time;
}

/**
 * Get the time needed to manufacture one item
 * @return The time needed to manufacture one item(in man/hour)
*/
int RuleManufactureInfo::getManufactureTime () const
{
	return _manufactureTime;
}

/**
 * Change the cost of manufacturing one item
 * @param cost The cost of one item
*/
void RuleManufactureInfo::setManufactureCost (int cost)
{
	_manufactureCost = cost;
}

/**
 * Get the cost of one item
 * @return the cost of manufacturing one item
*/
int RuleManufactureInfo::getManufactureCost () const
{
	return _manufactureCost;
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
