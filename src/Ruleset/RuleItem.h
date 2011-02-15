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
#ifndef OPENXCOM_RULEITEM_H
#define OPENXCOM_RULEITEM_H

#include <string>

namespace OpenXcom
{

/**
 * Represents a specific type of item.
 * Contains constant info about an item like
 * storage size, sell price, etc.
 * @sa Item
 */
class RuleItem
{
private:
	std::string _type;
	float _size;
	int _value, _time;
	bool _equip;
	int _bigSprite, _floorSprite, _handSprite;
public:
	/// Creates a blank item ruleset.
	RuleItem(std::string type);
	/// Cleans up the item ruleset.
	~RuleItem();
	/// Gets the item's type.
	std::string getType() const;
	/// Gets the item's size.
	float getSize() const;
	/// Sets the item's size.
	void setSize(float size);
	/// Gets the item's sale value.
	int getSaleValue() const;
	/// Sets the item's sale value.
	void setSaleValue(int value);
	/// Gets the item's transfer time.
	int getTransferTime() const;
	/// Sets the item's transfer time.
	void setTransferTime(int time);
	/// Gets if the item is equippable.
	bool getEquippable() const;
	/// Sets if the item is equippable.
	void setEquippable(bool equip);
	/// Gets the item's reference in BIGOBS.PCK for use in inventory.
	int getBigSprite() const;
	/// Sets the item's reference in BIGOBS.PCK for use in inventory.
	void setBigSprite(int value);

};

}

#endif
