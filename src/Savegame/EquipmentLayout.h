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
#ifndef OPENXCOM_EQUIPMENTLAYOUT_H
#define OPENXCOM_EQUIPMENTLAYOUT_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class EquipmentLayoutItem;

/**
 * Represents a soldier-equipment layout which is used
 * on the beginning of the Battlescape.
 */
class EquipmentLayout
{
private:
	std::wstring _name;
	int _id;
	std::vector<EquipmentLayoutItem*> _items;
public:
	/// Creates a new soldier-equipment layout and loads its contents from YAML.
	EquipmentLayout(const YAML::Node& node);
	/// Creates a new soldier-equipment layout.
	EquipmentLayout(const std::wstring &name, int id = 0);
	/// Creates a new custom owned (id=0) soldier-equipment layout by copying the given layout.
	EquipmentLayout(EquipmentLayout *layout);
	/// Cleans up the soldier-equipment layout.
	~EquipmentLayout();
/// Copies the layout-items of the given layout into this layout.
	void copyLayoutItems(EquipmentLayout *layout);
	/// Loads the soldier-equipment layout from YAML.
	void load(const YAML::Node& node);
	/// Saves the soldier-equipment layout to YAML.
	YAML::Node save() const;
	/// Sets the layout's name.
	void setName(const std::wstring &name);
	/// Gets the layout's name.
	std::wstring getName() const;
	/// Gets the layout's unique ID.
	int getId() const;
	/// Gets the layout's items.
	std::vector<EquipmentLayoutItem*> *getItems();
	/// Erases the layout's items.
	void eraseItems();
};

}

#endif
