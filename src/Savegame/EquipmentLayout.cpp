/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "EquipmentLayout.h"
#include "EquipmentLayoutItem.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a new soldier-equipment layout from YAML.
 * @param node YAML node.
 */
EquipmentLayout::EquipmentLayout(const YAML::Node &node) : _name(L""), _id(0)
{
	load(node);
}

/**
 * Initializes a new soldier-equipment layout.
 * @param name The layout's name.
 * @param id The layout's unique ID.
 */
EquipmentLayout::EquipmentLayout(const std::wstring &name, int id) : _name(name), _id(id)
{
}

/**
 * Initializes a new custom owned (id=0) soldier-equipment layout by copying the given layout.
 * @param layout The layout to copy.
 */
EquipmentLayout::EquipmentLayout(EquipmentLayout *layout) : _name(layout->getName()), _id(0)
{
	copyLayoutItems(layout);
}

/**
 *
 */
EquipmentLayout::~EquipmentLayout()
{
	eraseItems();
}

/**
 * Copies the layout-items of the given layout into this layout.
 * @param layout The layout to copy.
 */
void EquipmentLayout::copyLayoutItems(EquipmentLayout *layout)
{
	eraseItems();
	if (layout == 0) return;
	std::vector<EquipmentLayoutItem*> *items = layout->getItems();
	for (std::vector<EquipmentLayoutItem*>::iterator i = items->begin(); i != items->end(); ++i)
	{
		_items.push_back(new EquipmentLayoutItem((*i)->getItemType(), (*i)->getSlot(), (*i)->getSlotX(), (*i)->getSlotY(), (*i)->getAmmoItem(), (*i)->getFuseTimer()));
	}
}

/**
 * Loads the soldier-equipment layout from a YAML file.
 * @param node YAML node.
 */
void EquipmentLayout::load(const YAML::Node &node)
{
	_name = Language::utf8ToWstr(node["name"].as<std::string>());
	_id = node["id"].as<int>(_id);
	if (const YAML::Node &items = node["items"])
	{
		for (YAML::const_iterator i = items.begin(); i != items.end(); ++i)
		{
			_items.push_back(new EquipmentLayoutItem(*i));
		}
	}
}

/**
 * Saves the soldier-equipment layout to a YAML file.
 * @return YAML node.
 */
YAML::Node EquipmentLayout::save() const
{
	YAML::Node node;
	node["name"] = Language::wstrToUtf8(_name);
	node["id"] = _id;
	if (!_items.empty())
	{
		for (std::vector<EquipmentLayoutItem*>::const_iterator i = _items.begin(); i != _items.end(); ++i)
		{
			node["items"].push_back((*i)->save());
		}
	}
	return node;
}

/**
 * Changes the layout's name.
 * @param name layout name.
 */
void EquipmentLayout::setName(const std::wstring &name)
{
	_name = name;
}

/**
 * Returns the layout's name.
 * @return layout name.
 */
std::wstring EquipmentLayout::getName() const
{
	return _name;
}

/**
 * Returns the layout's unique ID. Each layout
 * can be identified by its ID. (not it's name)
 * @return Unique ID.
 */
int EquipmentLayout::getId() const
{
	return _id;
}

/**
 * Returns the vector of EquipmentLayoutItems of a layout.
 * @return Pointer to the EquipmentLayoutItem vector.
 */
std::vector<EquipmentLayoutItem*> *EquipmentLayout::getItems()
{
	return &_items;
}

/**
 * Erases the layout's items.
 */
void EquipmentLayout::eraseItems()
{
	for (std::vector<EquipmentLayoutItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete *i;
	}
	_items.clear();
}

}
