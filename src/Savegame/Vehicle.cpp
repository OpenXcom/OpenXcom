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
#include "Vehicle.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes a vehicle of the specified type.
 * @param rules Pointer to ruleset.
 * @param ammo Initial ammo.
 */
Vehicle::Vehicle(RuleItem *rules, int ammo) : _rules(rules), _ammo(ammo)
{
}

/**
 *
 */
Vehicle::~Vehicle()
{
}

/**
 * Loads the vehicle from a YAML file.
 * @param node YAML node.
 */
void Vehicle::load(const YAML::Node &node)
{
	node["ammo"] >> _ammo;
}

/**
 * Saves the base to a YAML file.
 * @param out YAML emitter.
 */
void Vehicle::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "ammo" << YAML::Value << _ammo;
	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the vehicle's type.
 * @return Pointer to ruleset.
 */
RuleItem *Vehicle::getRules() const
{
	return _rules;
}

/**
 * Returns the ammo contained in this vehicle.
 * @return Weapon ammo.
 */
int Vehicle::getAmmo() const
{
	return _ammo;
}

/**
 * Changes the ammo contained in this vehicle.
 * @param ammo Weapon ammo.
 */
void Vehicle::setAmmo(int ammo)
{
	_ammo = ammo;
}

}
