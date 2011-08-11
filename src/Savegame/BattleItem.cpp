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
BattleItem::BattleItem(RuleItem *rules) : _rules(rules), _owner(0), _previousOwner(0), _inventorySlot(""), _inventoryX(0), _inventoryY(0), _ammoItem(0)
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
	node["X"] >> _position.x;
	node["Y"] >> _position.y;
	node["Z"] >> _position.z;

	node["inventoryslot"] >> _inventorySlot;
	node["inventoryX"] >> _inventoryX;
	node["inventoryY"] >> _inventoryY;
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
	if (_owner)
		out << YAML::Key << "owner" << YAML::Value << _owner->getId();
	out << YAML::Key << "inventoryslot" << YAML::Value << _inventorySlot;
	out << YAML::Key << "inventoryX" << YAML::Value << _inventoryX;
	out << YAML::Key << "inventoryY" << YAML::Value << _inventoryY;

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
 * Returns the turn to explode on. 0 = unprimed grenade
 * @return Explode turn.
 */
int BattleItem::getExplodeTurn() const
{
	return _itemProperty[0];
}

/**
 * Set the turn to explode on.
 * @param turn Turn to explode on.
 */
void BattleItem::setExplodeTurn(int turn)
{
	_itemProperty[0] = turn;
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

/**
 * Gets the item's owner. 
 * @return pointer to Battleunit
 */
BattleUnit *BattleItem::getOwner() const
{
	return _owner;
}

/// Gets the item's previous owner.
BattleUnit *BattleItem::getPreviousOwner() const
{
	return _previousOwner;
}

/**
 * Sets the item's owner. 
 * @param owner pointer to Battleunit
 */
void BattleItem::setOwner(BattleUnit *owner)
{
	_previousOwner = _owner;
	_owner = owner;
	if (_previousOwner != 0)
	{
		for (std::vector<BattleItem*>::iterator i = _previousOwner->getInventoryItems()->begin(); i != _previousOwner->getInventoryItems()->end(); ++i)
		{
			if ((*i) == this)
			{
				_previousOwner->getInventoryItems()->erase(i);
				break;
			}
		}
	}
	if (_owner != 0)
	{
		_owner->getInventoryItems()->push_back(this);
	}
}

/**
 * Gets the item's inventory slot.
 * @return slot id
 */
std::string BattleItem::getSlot() const
{
	return _inventorySlot;
}

/**
 * Sets the item's inventory slot.
 * @param slot slot id
 */
void BattleItem::setSlot(std::string slot)
{
	_inventorySlot = slot;
}

/**
 * Gets the item's inventory X position.
 * @return X position.
 */
int BattleItem::getSlotX() const
{
	return _inventoryX;
}

/**
 * Sets the item's inventory X position.
 * @param x X position.
 */
void BattleItem::setSlotX(int x)
{
	_inventoryX = x;
}

/**
 * Gets the item's inventory Y position.
 * @return Y position.
 */
int BattleItem::getSlotY() const
{
	return _inventoryY;
}

/**
 * Sets the item's inventory Y position.
 * @param y Y position.
 */
void BattleItem::setSlotY(int y)
{
	_inventoryY = y;
}

/**
 * Checks if the item is covering a
 * certain inventory slot.
 * @param x Slot X position.
 * @param y Slot Y position.
 * @return Whether it's covering or not.
 */
bool BattleItem::occupiesSlot(int x, int y) const
{
	return (x >= _inventoryX && x <= _inventoryX + _rules->getInventoryWidth() - 1 &&
			y >= _inventoryY && y <= _inventoryY + _rules->getInventoryHeight() - 1);
}

/**
 * Gets the item's ammo item.
 * @return BattleItem
 */
BattleItem *BattleItem::getAmmoItem()
{
	return _ammoItem;
}

/**
 * Sets the item's ammo item.
 * @return -2 when ammo doesn't fit, or -1 when weapon already contains ammo
 */
int BattleItem::setAmmoItem(BattleItem *item)
{
	if (item == 0)
	{
		_ammoItem = 0;
		return 0;
	}

	if (_ammoItem)
		return -1;

	for (std::vector<std::string>::iterator i = _rules->getCompatibleAmmo()->begin(); i != _rules->getCompatibleAmmo()->end(); ++i)
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
