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
#ifndef OPENXCOM_RULEMANUFACTURE_H
#define OPENXCOM_RULEMANUFACTURE_H

#include <string>
#include <map>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{
/**
 * Represents information needed to manufacture an object
*/
class RuleManufacture
{
private:
	std::string _name, _producedItem, _category;
	std::vector<std::string> _requires;
	int _space, _time, _cost, _produceQty;
	std::map<std::string, int> _requiredItems;
public:
	/// Create ManufactureInfo
	RuleManufacture(const std::string &name);
	/// Create ManufactureInfo
	RuleManufacture(const std::string &name, const std::string &producedItem, const std::string &category, const std::vector<std::string> &requires, int space, int time, int cost, int produceQty, const std::map<std::string, int> &requiredItems);
	/// Loads the manufacture from YAML.
	void load(const YAML::Node& node);
	/// Saves the manufacture to YAML.
	void save(YAML::Emitter& out) const;
	///Get the manufacture name
	std::string getName () const;
	///Get the name of the produced item (it is the same as getName in most cases)
	std::string getProducedItem() const;
	///Get the manufacture category
	std::string getCategory () const;
	/// Gets the manufacture's requirements.
	const std::vector<std::string> &getRequirements () const;
	///Get the required workshop space
	int getRequiredSpace () const;
	///Get the time required to manufacture one object
	int getManufactureTime () const;
	///Get the cost of manufacturing one object
	int getManufactureCost () const;
	///Get the number of objects to manufacture instead of one (it is 1 in most cases)
	int getProduceQty() const;
	///Get the list of items required to manufacture one object
	const std::map<std::string, int> & getRequiredItems() const;
};

}
#endif
