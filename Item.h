/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__ITEM_H
#define OPENXCOM__ITEM_H

#include "LangString.h"

class RuleItem;

/**
 * Represents an item stored in a base.
 * Contains variable info about an item like quantity.
 * @sa RuleItem
 */
class Item
{
private:
	RuleItem *_rules;
	int _qty;
public:
	/// Creates a item of the specified type.
	Item(RuleItem *rules, int qty);
	/// Cleans up the item.
	~Item();
	/// Gets the item's ruleset.
	RuleItem *getRules();
	/// Gets the item's quantity
	int getQuantity();
	/// Sets the item's quantity.
	void setQuantity(int qty);
};

#endif
