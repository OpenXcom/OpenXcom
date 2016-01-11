#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleItem;

/**
 * Represents a vehicle (tanks etc.) kept in a craft.
 * Contains variable info about a vehicle like ammo.
 * @sa RuleItem
 */
class Vehicle
{
private:
	RuleItem *_rules;
	int _ammo, _size;
public:
	/// Creates a vehicle of the specified type.
	Vehicle(RuleItem *rules, int ammo, int size);
	/// Cleans up the vehicle.
	~Vehicle();
	/// Loads the vehicle from YAML.
	void load(const YAML::Node& node);
	/// Saves the vehicle to YAML.
	YAML::Node save() const;
	/// Gets the vehicle's ruleset.
	RuleItem *getRules() const;
	/// Gets the vehicle's ammo.
	int getAmmo() const;
	/// Sets the vehicle's ammo.
	void setAmmo(int ammo);
	/// Gets the vehicle's size.
	int getSize() const;
};

}
