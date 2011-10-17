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
#ifndef OPENXCOM_RULEMANUFACTUREINFO_H
#define OPENXCOM_RULEMANUFACTUREINFO_H

#include <string>
#include <map>

namespace OpenXcom
{
/**
 * Represents information needed to manufacture a RuleItem
*/
class RuleManufactureInfo
{
private:
	std::map<std::string, int> _neededItems;
	int _space, _time, _cost;
	std::string _category, _name;
public:
	/// Create ManufactureInfo
	RuleManufactureInfo(const std::string &name);
	///Get the manufacture name
	std::string getName () const;
	///Set the manufacture category
	void setCategory (const std::string &category);
	///Get the manufacture category
	std::string getCategory () const;
	///Set the required workshop space
	void setRequiredSpace (int);
	///Get the required workshop space
	int getRequiredSpace () const;
	///Set the time required to manufacture one item
	void setManufactureTime (int);
	///Get the time required to manufacture one item
	int getManufactureTime () const;
	///Set the cost of manufacturing one item
	void setManufactureCost (int);
	///Get the cost of manufacturing one item
	int getManufactureCost () const;
	///Get the list of needed items to manufacture one item
	const std::map<std::string, int> & getNeededItems() const;
	///Add one required item
	void addNeededItem (std::string type, int quantity);
};

}
#endif
