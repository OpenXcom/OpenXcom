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
#ifndef OPENXCOM__RULEITEM_H
#define OPENXCOM__RULEITEM_H

#include "LangString.h"

/**
 * Represents a specific type of item.
 * Contains constant info about an item like
 * storage size, sell price, etc.
 * @sa Item
 */
class RuleItem
{
private:
	LangString _type;
	double _size;
	int _value, _time;
	bool _equip;
public:
	/// Creates a blank item ruleset.
	RuleItem(LangString type);
	/// Cleans up the item ruleset.
	~RuleItem();
	/// Gets the item's type.
	LangString getType();
	/// Gets the item's size.
	double getSize();
	/// Sets the item's size.
	void setSize(double size);
	/// Gets the item's sale value.
	int getSaleValue();
	/// Sets the item's sale value.
	void setSaleValue(int value);
	/// Gets the item's transfer time.
	int getTransferTime();
	/// Sets the item's transfer time.
	void setTransferTime(int time);
	/// Gets if the item is equippable.
	bool getEquippable();
	/// Sets if the item is equippable.
	void setEquippable(bool equip);
};

#endif
