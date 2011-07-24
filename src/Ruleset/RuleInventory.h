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
#ifndef OPENXCOM_RULEINVENTORY_H
#define OPENXCOM_RULEINVENTORY_H

#include <string>
#include <vector>

namespace OpenXcom
{

/**
 * Represents a specific section of the inventory,
 * containing information like available slots and
 * screen position.
 */
class RuleInventory
{
private:
	std::string _type;
	int _x, _y;
	bool _hand;
	std::vector<int> _slotX, _slotY;
public:
	/// Creates a blank inventory ruleset.
	RuleInventory(std::string type);
	/// Cleans up the inventory ruleset.
	~RuleInventory();
	/// Gets the inventory's type.
	std::string getType() const;
	/// Gets the X position of the inventory.
	int getX() const;
	/// Sets the X position of the inventory.
	void setX(int x);
	/// Gets the Y position of the inventory.
	int getY() const;
	/// Sets the Y position of the inventory.
	void setY(int y);
	/// Gets if the inventory is a hand.
	bool getHand() const;
	/// Sets if the inventory is a hand.
	void setHand(bool hand);
	/// Adds a slot to the inventory.
	void addSlot(int x, int y);
};

}

#endif
