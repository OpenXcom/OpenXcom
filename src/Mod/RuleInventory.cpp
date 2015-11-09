/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "RuleInventory.h"
#include <cmath>
#include "RuleItem.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::RuleSlot>
	{
		static Node encode(const OpenXcom::RuleSlot& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, OpenXcom::RuleSlot& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			return true;
		}
	};
}

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of inventory section.
 * @param id String defining the id.
 */
RuleInventory::RuleInventory(const std::string &id): _id(id), _x(0), _y(0), _type(INV_SLOT), _listOrder(0)
{
}

RuleInventory::~RuleInventory()
{
}

/**
 * Loads the inventory from a YAML file.
 * @param node YAML node.
 * @param listOrder The list weight for this inventory.
 */
void RuleInventory::load(const YAML::Node &node, int listOrder)
{
	_id = node["id"].as<std::string>(_id);
	_x = node["x"].as<int>(_x);
	_y = node["y"].as<int>(_y);
	_type = (InventoryType)node["type"].as<int>(_type);
	_slots = node["slots"].as< std::vector<RuleSlot> >(_slots);
	_costs = node["costs"].as< std::map<std::string, int> >(_costs);
	_listOrder = node["listOrder"].as<int>(listOrder);
}

/**
 * Gets the language string that names
 * this inventory section. Each section has a unique name.
 * @return The section name.
 */
std::string RuleInventory::getId() const
{
	return _id;
}

/**
 * Gets the X position of the inventory section on the screen.
 * @return The X position in pixels.
 */
int RuleInventory::getX() const
{
	return _x;
}

/**
 * Gets the Y position of the inventory section on the screen.
 * @return The Y position in pixels.
 */
int RuleInventory::getY() const
{
	return _y;
}

/**
 * Gets the type of the inventory section.
 * Slot-based contain a limited number of slots.
 * Hands only contain one slot but can hold any item.
 * Ground can hold infinite items but don't attach to soldiers.
 * @return The inventory type.
 */
InventoryType RuleInventory::getType() const
{
	return _type;
}

/**
 * Gets all the slots in the inventory section.
 * @return The list of slots.
 */
std::vector<struct RuleSlot> *RuleInventory::getSlots()
{
	return &_slots;
}

/**
 * Gets the slot located in the specified mouse position.
 * @param x Mouse X position. Returns the slot's X position.
 * @param y Mouse Y position. Returns the slot's Y position.
 * @return True if there's a slot there.
 */
bool RuleInventory::checkSlotInPosition(int *x, int *y) const
{
	int mouseX = *x, mouseY = *y;
	if (_type == INV_HAND)
	{
		for (int xx = 0; xx < HAND_W; ++xx)
		{
			for (int yy = 0; yy < HAND_H; ++yy)
			{
				if (mouseX >= _x + xx * SLOT_W && mouseX < _x + (xx + 1) * SLOT_W &&
					mouseY >= _y + yy * SLOT_H && mouseY < _y + (yy + 1) * SLOT_H)
				{
					*x = 0;
					*y = 0;
					return true;
				}
			}
		}
	}
	else if (_type == INV_GROUND)
	{
		if (mouseX >= _x && mouseX < 320 && mouseY >= _y && mouseY < 200)
		{
			*x = (int)floor(double(mouseX - _x) / SLOT_W);
			*y = (int)floor(double(mouseY - _y) / SLOT_H);
			return true;
		}
	}
	else
	{
		for (std::vector<RuleSlot>::const_iterator i = _slots.begin(); i != _slots.end(); ++i)
		{
			if (mouseX >= _x + i->x * SLOT_W && mouseX < _x + (i->x + 1) * SLOT_W &&
				mouseY >= _y + i->y * SLOT_H && mouseY < _y + (i->y + 1) * SLOT_H)
			{
				*x = i->x;
				*y = i->y;
				return true;
			}
		}
	}
	return false;
}

/**
 * Checks if an item completely fits when
 * placed in a certain slot.
 * @param item Pointer to item ruleset.
 * @param x Slot X position.
 * @param y Slot Y position.
 * @return True if there's a slot there.
 */
bool RuleInventory::fitItemInSlot(RuleItem *item, int x, int y) const
{
	if (_type == INV_HAND)
	{
		return true;
	}
	else if (_type == INV_GROUND)
	{
		int width = (320 - _x) / SLOT_W;
		int height = (200 - _y) / SLOT_H;
		int xOffset = 0;
		while (x >= xOffset + width)
			xOffset += width;
		for (int xx = x; xx < x + item->getInventoryWidth(); ++xx)
		{
			for (int yy = y; yy < y + item->getInventoryHeight(); ++yy)
			{
				if (!(xx >= xOffset && xx < xOffset + width && yy >= 0 && yy < height))
					return false;
			}
		}
		return true;
	}
	else
	{
		int totalSlots = item->getInventoryWidth() * item->getInventoryHeight();
		int foundSlots = 0;
		for (std::vector<RuleSlot>::const_iterator i = _slots.begin(); i != _slots.end() && foundSlots < totalSlots; ++i)
		{
			if (i->x >= x && i->x < x + item->getInventoryWidth() &&
				i->y >= y && i->y < y + item->getInventoryHeight())
			{
				foundSlots++;
			}
		}
		return (foundSlots == totalSlots);
	}
}

/**
 * Gets the time unit cost to place an item in another section.
 * @param slot The new section id.
 * @return The time unit cost.
 */
int RuleInventory::getCost(RuleInventory* slot) const
{
	if (slot == this)
		return 0;
	return _costs.find(slot->getId())->second;
}

int RuleInventory::getListOrder() const
{
	return _listOrder;
}
}
