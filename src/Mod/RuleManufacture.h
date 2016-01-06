/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_RULEMANUFACTURE_H
#define OPENXCOM_RULEMANUFACTURE_H

#include <string>
#include <map>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents the information needed to manufacture an object.
 */
class RuleManufacture
{
private:
	std::string _name, _category;
	std::vector<std::string> _requires;
	int _space, _time, _cost;
	std::map<std::string, int> _requiredItems, _producedItems;
	int _listOrder;
public:
	/// Creates a new manufacture.
	RuleManufacture(const std::string &name);
	/// Loads the manufacture from YAML.
	void load(const YAML::Node& node, int listOrder);
	/// Gets the manufacture name.
	std::string getName() const;
	/// Gets the manufacture category.
	std::string getCategory() const;
	/// Gets the manufacture's requirements.
	const std::vector<std::string> &getRequirements() const;
	/// Gets the required workshop space.
	int getRequiredSpace() const;
	/// Gets the time required to manufacture one object.
	int getManufactureTime() const;
	/// Gets the cost of manufacturing one object.
	int getManufactureCost() const;
	/// Gets the list of items required to manufacture one object.
	const std::map<std::string, int> & getRequiredItems() const;
	/// Gets the list of items produced by completing "one object" of this project.
	/// by default: it contains only the "name" item with a value of 1.
	const std::map<std::string, int> & getProducedItems() const;
	/// Gets the list weight for this manufacture item.
	int getListOrder() const;
};

}
#endif
