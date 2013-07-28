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
#include "EquipmentLayoutItem.h"

namespace OpenXcom
{

/**
 * Initializes a new soldier-equipment layout item from YAML.
 * @param node YAML node.
 */
EquipmentLayoutItem::EquipmentLayoutItem(const YAML::Node &node)
{
	load(node);
}

/**
 * Initializes a new soldier-equipment layout item.
 * @param itemType Item's type.
 * @param slot Occupied slot's id.
 * @param slotX Position-X in the occupied slot.
 * @param slotY Position-Y in the occupied slot.
 * @param ammoItem The ammo has to be loaded into the item. (it's type)
 * @param explodeTurn The turn when the item explodes. (if it's an activated grenade-type)
 */
EquipmentLayoutItem::EquipmentLayoutItem(std::string itemType, std::string slot, int slotX, int slotY, std::string ammoItem, int explodeTurn) : _itemType(itemType), _slot(slot), _slotX(slotX), _slotY(slotY), _ammoItem(ammoItem), _explodeTurn(explodeTurn)
{
}

/**
 *
 */
EquipmentLayoutItem::~EquipmentLayoutItem()
{
}

/**
 * Returns the item's type which has to be in a slot.
 * @return item type.
 */
std::string EquipmentLayoutItem::getItemType() const
{
	return _itemType;
}

/**
 * Returns the slot to be occupied.
 * @return slot name.
 */
std::string EquipmentLayoutItem::getSlot() const
{
	return _slot;
}

/**
 * Returns the position-X in the slot to be occupied.
 * @return slot-X.
 */
int EquipmentLayoutItem::getSlotX() const
{
	return _slotX;
}

/**
 * Returns the position-Y in the slot to be occupied.
 * @return slot-Y.
 */
int EquipmentLayoutItem::getSlotY() const
{
	return _slotY;
}

/**
 * Returns the ammo has to be loaded into the item.
 * @return ammo type.
 */
std::string EquipmentLayoutItem::getAmmoItem() const
{
	return _ammoItem;
}

/**
 * Returns the turn when the item explodes. (if it's an activated grenade-type)
 * @return turn count.
 */
int EquipmentLayoutItem::getExplodeTurn() const
{
	return _explodeTurn;
}

/**
 * Loads the soldier-equipment layout item from a YAML file.
 * @param node YAML node.
 */
void EquipmentLayoutItem::load(const YAML::Node &node)
{
	node["itemType"] >> _itemType;
	node["slot"] >> _slot;
	node["slotX"] >> _slotX;
	node["slotY"] >> _slotY;
	node["ammoItem"] >> _ammoItem;
	node["explodeTurn"] >> _explodeTurn;
}

/**
 * Saves the soldier-equipment layout item to a YAML file.
 * @param out YAML emitter.
 */
void EquipmentLayoutItem::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "itemType" << YAML::Value << _itemType;
	out << YAML::Key << "slot" << YAML::Value << _slot;
	out << YAML::Key << "slotX" << YAML::Value << _slotX;
	out << YAML::Key << "slotY" << YAML::Value << _slotY;
	out << YAML::Key << "ammoItem" << YAML::Value << _ammoItem;
	out << YAML::Key << "explodeTurn" << YAML::Value << _explodeTurn;
	out << YAML::EndMap;
}

}
