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
#ifndef OPENXCOM_ITEM_H
#define OPENXCOM_ITEM_H

#include <string>

namespace OpenXcom
{

class RuleItem;

/**
 * Represents a set of a stored item.
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
	RuleItem *const getRules() const;
	/// Gets the item's quantity
	int getQuantity() const;
	/// Sets the item's quantity.
	void setQuantity(int qty);
	/// Gets the item's total size.
	double getTotalSize() const;
};

}

#endif
