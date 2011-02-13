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
	int _inventorySlot;
	BattleItem *_parentItem;
	int _itemProperty1, _itemProperty2, _itemProperty3;
public:
	/// Creates a item of the specified type.
	BattleItem(RuleItem *rules, int ammoQty = 0);
	/// Cleans up the item.
	~BattleItem();
	/// Gets the item's ruleset.
	RuleItem *const getRules() const;
	/// Gets the item's ammo quantity
	int getAmmoQuantity() const;
	/// Sets the item's ammo quantity.
	void setAmmoQuantity(int qty);
};

}

#endif
