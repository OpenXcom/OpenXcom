/*
 * Copyright 2010 OpenXcom Developers.
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
#include "BattleItem.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes a item of the specified type.
 * @param rules Pointer to ruleset.
 */
BattleItem::BattleItem(RuleItem *rules) : _rules(rules)
{
	_itemProperty[0] = 0;
	_itemProperty[1] = 0;
	_itemProperty[2] = 0;
}

/**
 *
 */
BattleItem::~BattleItem()
{
}

/**
 * Loads the item from a YAML file.
 * @param node YAML node.
 */
void BattleItem::load(const YAML::Node &node)
{
/*	int a;

	node["id"] >> _id;
	std::string name;
	node["name"] >> name;
	_name = Language::utf8ToWstr(name);
	node["faction"] >> a;
	_faction = (UnitFaction)a;
	node["status"] >> a;
	_status = (UnitStatus)a;

	node["X"] >> _pos.x;
	node["Y"] >> _pos.y;
	node["Z"] >> _pos.z;
	node["direction"] >> _direction;

	node["tu"] >> _tu;
	node["health"] >> _health;
	node["energy"] >> _energy;
	node["morale"] >> _morale;*/
}

/**
 * Saves the item to a YAML file.
 * @param out YAML emitter.
 */
void BattleItem::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

/*	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "name" << YAML::Value << Language::wstrToUtf8(_name);
	out << YAML::Key << "faction" << YAML::Value << _faction;
	out << YAML::Key << "status" << YAML::Value << _status;

	out << YAML::Key << "X" << YAML::Value << _pos.x;
	out << YAML::Key << "Y" << YAML::Value << _pos.y;
	out << YAML::Key << "Z" << YAML::Value << _pos.z;
	out << YAML::Key << "direction" << YAML::Value << _direction;

	out << YAML::Key << "tu" << YAML::Value << _tu;
	out << YAML::Key << "health" << YAML::Value << _health;
	out << YAML::Key << "energy" << YAML::Value << _energy;
	out << YAML::Key << "morale" << YAML::Value << _morale;*/

	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the item's type.
 * @return Pointer to ruleset.
 */
RuleItem *const BattleItem::getRules() const
{
	return _rules;
}

/**
 * Returns the quantity of ammo in this item.
 * @return Ammo quantity.
 */
int BattleItem::getAmmoQuantity() const
{
	return _itemProperty[0];
}

/**
 * Changes the quantity of ammo in this item.
 * @param qty Ammo quantity.
 */
void BattleItem::setAmmoQuantity(int qty)
{
	_itemProperty[0] = qty;
}

/// Gets the item's owner.
BattleUnit *BattleItem::getOwner() const
{
	return _owner;
}

/// Sets the item's owner.
void BattleItem::setOwner(BattleUnit *owner)
{
	_owner = owner;
}

/// Gets the item's inventory slot.
InventorySlot BattleItem::getSlot() const
{
	return _inventorySlot;
}

/// Sets the item's inventory slot.
void BattleItem::setSlot(InventorySlot slot)
{
	_inventorySlot = slot;
}

}
