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
#include "BattleUnit.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes a item of the specified type.
 * @param rules Pointer to ruleset.
 */
BattleItem::BattleItem(RuleItem *rules) : _rules(rules), _owner(0), _ammoItem(0)
{
	_itemProperty[0] = 0;
	_itemProperty[1] = 0;
	_itemProperty[2] = 0;
	if (_rules->getBattleType() == BT_AMMO)
	{
		setAmmoQuantity(_rules->getClipSize());
	}
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
	int a;

	node["X"] >> _position.x;
	node["Y"] >> _position.y;
	node["Z"] >> _position.z;

	node["inventoryslot"] >> a;
	_inventorySlot = (InventorySlot)a;
}

/**
 * Saves the item to a YAML file.
 * @param out YAML emitter.
 */
void BattleItem::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "X" << YAML::Value << _position.x;
	out << YAML::Key << "Y" << YAML::Value << _position.y;
	out << YAML::Key << "Z" << YAML::Value << _position.z;
	out << YAML::Key << "owner" << YAML::Value << _owner->getId();
	out << YAML::Key << "inventoryslot" << YAML::Value << (int)_inventorySlot;

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

/**
 * Changes the quantity of ammo in this item.
 * @param qty Ammo quantity.
 */
bool BattleItem::spendBullet()
{
	_itemProperty[0]--;
	if (_itemProperty[0] == 0)
		return false;
	else
		return true;
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

/// Gets the item's ammo item.
BattleItem *BattleItem::getAmmoItem()
{
	return _ammoItem;
}

/// Sets the item's ammo item. Return -2 when ammo doesn't fit, or -1 when weapon already contains ammo?
int BattleItem::setAmmoItem(BattleItem *item)
{
	if (_ammoItem)
		return -1;

	for (std::vector<std::string>::iterator i = _rules->getCompatibleAmmo()->begin(); i != _rules->getCompatibleAmmo()->end(); i++)
	{
		if (*i == item->getRules()->getType())
		{
			_ammoItem = item;
			return 0;
		}
	}

	return -2;
}

}
