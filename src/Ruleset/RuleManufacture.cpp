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
#include "RuleManufacture.h"

namespace OpenXcom
{
/**
 * Create a new Manufacture
 * @param name The unique manufacture name
*/
RuleManufacture::RuleManufacture(const std::string &name) : _name(name), _producedItem(name), _space(0), _time(0), _cost(0), _produceQty(1)
{
}

/**
 * Create a new Manufacture
 * @param name The unique manufacture name
 * @param category The category type
 * @param requires The required research
 * @param space The required workshop space
 * @param time The required time to produce one unit
 * @param cost The required money
 * @param requiredItems The required materials
*/
RuleManufacture::RuleManufacture(const std::string &name, const std::string &producedItem, const std::string &category, const std::vector<std::string> &requires, int space, int time, int cost, int produceQty, const std::map<std::string, int> &requiredItems) :
	_name(name), _producedItem(producedItem), _category(category), _requires(requires), _space(space), _time(time), _cost(cost), _produceQty(produceQty), _requiredItems(requiredItems)
{
}

/**
 * Loads the manufacture project from a YAML file.
 * @param node YAML node.
 */
void RuleManufacture::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "category")
		{
			i.second() >> _category;
		}
		else if (key == "requires")
		{
			i.second() >> _requires;
		}
		else if (key == "space")
		{
			i.second() >> _space;
		}
		else if (key == "time")
		{
			i.second() >> _time;
		}
		else if (key == "cost")
		{
			i.second() >> _cost;
		}
		else if (key == "requiredItems")
		{
			i.second() >> _requiredItems;
		}
	}
}

/**
 * Saves the manufacture project to a YAML file.
 * @param out YAML emitter.
 */
void RuleManufacture::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "category" << YAML::Value << _category;
	out << YAML::Key << "requires" << YAML::Value << _requires;
	out << YAML::Key << "space" << YAML::Value << _space;
	out << YAML::Key << "time" << YAML::Value << _time;
	out << YAML::Key << "cost" << YAML::Value << _cost;
	out << YAML::Key << "requiredItems" << YAML::Value << _requiredItems;
	out << YAML::EndMap;
}

/**
 * Get the unique name of the manufacture
 * @return the name
*/
std::string RuleManufacture::getName () const
{
	return _name;
}

/**
 * Get the name of the produced item (it is the same as getName in most cases)
 * @return the name
*/
std::string RuleManufacture::getProducedItem() const
{
	return _producedItem;
}

/**
 * Get the category shown in the manufacture list
 * @return the category
*/
std::string RuleManufacture::getCategory () const
{
	return _category;
}

/**
 * Returns the list of research required to
 * manufacture this object.
 * @return List of research IDs.
 */
const std::vector<std::string> &RuleManufacture::getRequirements() const
{
	return _requires;
}

/**
 * Get the required workspace
 * @return the required workspace to start production
*/
int RuleManufacture::getRequiredSpace () const
{
	return _space;
}

/**
 * Get the time needed to manufacture one object
 * @return The time needed to manufacture one object(in man/hour)
*/
int RuleManufacture::getManufactureTime () const
{
	return _time;
}


/**
 * Get the cost of one object
 * @return the cost of manufacturing one object
*/
int RuleManufacture::getManufactureCost () const
{
	return _cost;
}

/**
 * Get the number of objects to manufacture instead of one (it is 1 in most cases)
 * @return the number
*/
int RuleManufacture::getProduceQty() const
{
	return _produceQty;
}

/**
 * Get the list of items required to manufacture one object
 * @return the list of items required to manufacture one object
*/
const std::map<std::string, int> & RuleManufacture::getRequiredItems() const
{
	return _requiredItems;
}

}
