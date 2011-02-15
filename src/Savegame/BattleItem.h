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
#ifndef OPENXCOM_BATTLEITEM_H
#define OPENXCOM_BATTLEITEM_H

#include "../Battlescape/Position.h"

enum InventorySlot { RIGHT_HAND, LEFT_HAND };

namespace OpenXcom
{

class Item;
class RuleItem;
class BattleUnit;

/**
 * Represents a single item in the battlescape.
 * Contains battle-related info about an item like the position, ammo quantity, ...
 * @sa RuleItem
 * @sa Item
 */
class BattleItem
{
private:
	RuleItem *_rules;
	Position _position;
	BattleUnit *_owner;
	InventorySlot _inventorySlot;
	BattleItem *_parentItem;
	int _itemProperty[3];
public:
	/// Creates a item of the specified type.
	BattleItem(RuleItem *rules);
	/// Cleans up the item.
	~BattleItem();
	/// Gets the item's ruleset.
	RuleItem *const getRules() const;
	/// Gets the item's ammo quantity
	int getAmmoQuantity() const;
	/// Sets the item's ammo quantity.
	void setAmmoQuantity(int qty);
	/// Gets the item's owner.
	BattleUnit *getOwner() const;
	/// Sets the item's owner.
	void setOwner(BattleUnit *owner);
	/// Gets the item's inventory slot.
	InventorySlot getSlot() const;
	/// Sets the item's inventory slot.
	void setSlot(InventorySlot slot);
};

}

#endif
