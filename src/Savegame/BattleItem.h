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

enum Location {LOC_GROUND, LOC_LEFT_ARM, LOC_RIGHT_ARM, LOC_LEFT_LEG, LOC_RIGHT_LEG, LOC_BELT, LOC_BACKPACK};

/**
 * Represents a single item in the battlescape.
 * Contains variable info about an item like the ammo quantity.
 * @sa RuleItem
 * @sa Item
 */
class BattleItem
{
private:
	Item *_item;
	int _ammoQty;
	Position _position;
	Location _location;
	BattleItem *_ammoItem;
public:
	/// Creates a item of the specified type.
	BattleItem(Item *item, int ammoQty = 0);
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
