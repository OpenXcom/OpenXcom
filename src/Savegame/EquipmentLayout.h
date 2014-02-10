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

/**
 * Represents a soldier-equipment layout which is used
 * on the beginning of the Battlescape.
 */
class EquipmentLayout
{
private:
public:
	/// Creates a new soldier-equipment layout and loads its contents from YAML.
	EquipmentLayout(const YAML::Node& node);
	/// Creates a new soldier-equipment layout.
	EquipmentLayout();
	/// Cleans up the soldier-equipment layout.
	~EquipmentLayout();
	/// Loads the soldier-equipment layout from YAML.
	void load(const YAML::Node& node);
	/// Saves the soldier-equipment layout to YAML.
	YAML::Node save() const;
};

}

#endif
