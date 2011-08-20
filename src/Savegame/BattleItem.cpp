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
#include "../Ruleset/RuleInventory.h"

namespace OpenXcom
{

/**
 * Initializes a item of the specified type.
 * @param rules Pointer to ruleset.
 */
BattleItem::BattleItem(RuleItem *rules) : _rules(rules), _position(-1, -1, -1), _owner(0), _previousOwner(0), _inventorySlot(0), _inventoryX(0), _inventoryY(0), _ammoItem(0), _explodeTurn(0), _ammoQuantity(0)
{
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

	//node["inventoryslot"] >> _inventorySlot;
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
	out << YAML::Key << "inventoryslot" << YAML::Value << _inventorySlot->getId();
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
	return _explodeTurn;
}

/**
 * Set the turn to explode on.
 * @param turn Turn to explode on.
 */
void BattleItem::setExplodeTurn(int turn)
{
	_explodeTurn = turn;
}

/**
 * Returns the quantity of ammo in this item.
 * @return Ammo quantity.
 */
int BattleItem::getAmmoQuantity() const
{
	return _ammoQuantity;
}

/**
 * Changes the quantity of ammo in this item.
 * @param qty Ammo quantity.
 */
void BattleItem::setAmmoQuantity(int qty)
{
	_ammoQuantity = qty;
}

/**
 * Changes the quantity of ammo in this item.
 * @param qty Ammo quantity.
 * @return bool Got bullets left?
 */
bool BattleItem::spendBullet()
{
	_ammoQuantity--;
	if (_ammoQuantity == 0)
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
}

/**
 * Removes the item from previous owner and moves to new owner.
 * @param owner pointer to Battleunit
 */
void BattleItem::moveToOwner(BattleUnit *owner)
{
	_previousOwner = _owner;
	_owner = owner;
	if (_previousOwner != 0)
	{
		for (std::vector<BattleItem*>::iterator i = _previousOwner->getInventory()->begin(); i != _previousOwner->getInventory()->end(); ++i)
		{
			if ((*i) == this)
			{
				_previousOwner->getInventory()->erase(i);
				break;
			}
		}
	}
	if (_owner != 0)
	{
		_owner->getInventory()->push_back(this);
	}
}

/**
 * Gets the item's inventory slot.
 * @return slot id
 */
RuleInventory *BattleItem::getSlot() const
{
	return _inventorySlot;
}

/**
 * Sets the item's inventory slot.
 * @param slot slot id
 */
void BattleItem::setSlot(RuleInventory *slot)
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
 * Checks if the item is covering certain inventory slot(s).
 * @param x Slot X position.
 * @param y Slot Y position.
 * @param item Item to check for overlap, or NULL if none.
 * @return Whether it's covering or not.
 */
bool BattleItem::occupiesSlot(int x, int y, BattleItem *item) const
{
	if (item == this)
		return false;
	if (_inventorySlot->getType() == INV_HAND)
		return true;
	if (item == 0)
	{
		return (x >= _inventoryX && x < _inventoryX + _rules->getInventoryWidth() &&
				y >= _inventoryY && y < _inventoryY + _rules->getInventoryHeight());
	}
	else
	{
		return !(x >= _inventoryX + _rules->getInventoryWidth() ||
				x + item->getRules()->getInventoryWidth() <= _inventoryX ||
				y >= _inventoryY + _rules->getInventoryHeight() ||
				y + item->getRules()->getInventoryHeight() <= _inventoryY);
	}
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
