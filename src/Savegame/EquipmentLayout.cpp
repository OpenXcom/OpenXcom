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
#include "EquipmentLayout.h"

namespace OpenXcom
{

/**
 * Initializes a new soldier-equipment layout from YAML.
 * @param node YAML node.
 */
EquipmentLayout::EquipmentLayout(const YAML::Node &node)
{
	load(node);
}

/**
 * Initializes a new soldier-equipment layout.
 */
EquipmentLayout::EquipmentLayout()
{
}

/**
 *
 */
EquipmentLayout::~EquipmentLayout()
{
}

/**
 * Loads the soldier-equipment layout from a YAML file.
 * @param node YAML node.
 */
void EquipmentLayout::load(const YAML::Node &node)
{
}

/**
 * Saves the soldier-equipment layout to a YAML file.
 * @return YAML node.
 */
YAML::Node EquipmentLayout::save() const
{
	YAML::Node node;
	return node;
}

}
