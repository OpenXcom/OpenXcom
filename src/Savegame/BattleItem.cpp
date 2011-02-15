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
 * @param item Pointer to item.
 * @param qty Initial ammo quantity.
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
