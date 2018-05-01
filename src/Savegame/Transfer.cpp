/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "Transfer.h"
#include "Base.h"
#include "Soldier.h"
#include "Craft.h"
#include "ItemContainer.h"
#include "../Engine/Language.h"
#include "../Mod/Mod.h"
#include "../Engine/Logger.h"

namespace OpenXcom
{

/**
 * Initializes a transfer.
 * @param hours Hours in-transit.
 */
Transfer::Transfer(int hours) : _hours(hours), _soldier(0), _craft(0), _itemQty(0), _scientists(0), _engineers(0), _delivered(false)
{
}

/**
 * Cleans up undelivered transfers.
 */
Transfer::~Transfer()
{
	if (!_delivered)
	{
		delete _soldier;
		delete _craft;
	}
}

/**
 * Loads the transfer from a YAML file.
 * @param node YAML node.
 * @param base Destination base.
 * @param rule Game mod.
 * @param save Pointer to savegame.
 * @return Was the transfer content valid?
 */
bool Transfer::load(const YAML::Node& node, Base *base, const Mod *mod, SavedGame *save)
{
	_hours = node["hours"].as<int>(_hours);
	if (const YAML::Node &soldier = node["soldier"])
	{
		std::string type = soldier["type"].as<std::string>(mod->getSoldiersList().front());
		if (mod->getSoldier(type) != 0)
		{
			_soldier = new Soldier(mod->getSoldier(type), 0);
			_soldier->load(soldier, mod, save);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load soldier " << type;
			delete this;
			return false;
		}
	}
	if (const YAML::Node &craft = node["craft"])
	{
		std::string type = craft["type"].as<std::string>();
		if (mod->getCraft(type) != 0)
		{
			_craft = new Craft(mod->getCraft(type), base);
			_craft->load(craft, mod, 0);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load craft " << type;
			delete this;
			return false;
		}

	}
	if (const YAML::Node &item = node["itemId"])
	{
		_itemId = item.as<std::string>(_itemId);
		if (mod->getItem(_itemId) == 0)
		{
			Log(LOG_ERROR) << "Failed to load item " << _itemId;
			delete this;
			return false;
		}
	}
	_itemQty = node["itemQty"].as<int>(_itemQty);
	_scientists = node["scientists"].as<int>(_scientists);
	_engineers = node["engineers"].as<int>(_engineers);
	_delivered = node["delivered"].as<bool>(_delivered);
	return true;
}

/**
 * Saves the transfer to a YAML file.
 * @return YAML node.
 */
YAML::Node Transfer::save() const
{
	YAML::Node node;
	node["hours"] = _hours;
	if (_soldier != 0)
	{
		node["soldier"] = _soldier->save();
	}
	else if (_craft != 0)
	{
		node["craft"] = _craft->save();
	}
	else if (_itemQty != 0)
	{
		node["itemId"] = _itemId;
		node["itemQty"] = _itemQty;
	}
	else if (_scientists != 0)
	{
		node["scientists"] = _scientists;
	}
	else if (_engineers != 0)
	{
		node["engineers"] = _engineers;
	}
	if (_delivered)
	{
		node["delivered"] = _delivered;
	}
	return node;
}

/**
 * Changes the soldier being transferred.
 * @param soldier Pointer to soldier.
 */
void Transfer::setSoldier(Soldier *soldier)
{
	_soldier = soldier;
}

/**
 * Changes the craft being transferred.
 * @param craft Pointer to craft.
 */
void Transfer::setCraft(Craft *craft)
{
	_craft = craft;
}

/**
 * Gets the craft being transferred.
 * @return a Pointer to craft.
 */
Craft *Transfer::getCraft()
{
	return _craft;
}

/**
 * Returns the items being transferred.
 * @return Item ID.
 */
std::string Transfer::getItems() const
{
	return _itemId;
}

/**
 * Changes the items being transferred.
 * @param id Item identifier.
 * @param qty Item quantity.
 */
void Transfer::setItems(const std::string &id, int qty)
{
	_itemId = id;
	_itemQty = qty;
}

/**
 * Changes the scientists being transferred.
 * @param scientists Amount of scientists.
 */
void Transfer::setScientists(int scientists)
{
	_scientists = scientists;
}

/**
 * Changes the engineers being transferred.
 * @param engineers Amount of engineers.
 */
void Transfer::setEngineers(int engineers)
{
	_engineers = engineers;
}

/**
 * Returns the name of the contents of the transfer.
 * @param lang Language to get strings from.
 * @return Name string.
 */
std::wstring Transfer::getName(Language *lang) const
{
	if (_soldier != 0)
	{
		return _soldier->getName();
	}
	else if (_craft != 0)
	{
		return _craft->getName(lang);
	}
	else if (_scientists != 0)
	{
		return lang->getString("STR_SCIENTISTS");
	}
	else if (_engineers != 0)
	{
		return lang->getString("STR_ENGINEERS");
	}
	return lang->getString(_itemId);
}

/**
 * Returns the time remaining until the
 * transfer arrives at its destination.
 * @return Amount of hours.
 */
int Transfer::getHours() const
{
	return _hours;
}

/**
 * Returns the quantity of items in the transfer.
 * @return Amount of items.
 */
int Transfer::getQuantity() const
{
	if (_itemQty != 0)
	{
		return _itemQty;
	}
	else if (_scientists != 0)
	{
		return _scientists;
	}
	else if (_engineers != 0)
	{
		return _engineers;
	}
	return 1;
}

/**
 * Returns the type of the contents of the transfer.
 * @return TransferType.
 */
TransferType Transfer::getType() const
{
	if (_soldier != 0)
	{
		return TRANSFER_SOLDIER;
	}
	else if (_craft != 0)
	{
		return TRANSFER_CRAFT;
	}
	else if (_scientists != 0)
	{
		return TRANSFER_SCIENTIST;
	}
	else if (_engineers != 0)
	{
		return TRANSFER_ENGINEER;
	}
	return TRANSFER_ITEM;
}

/**
 * Advances the transfer and takes care of
 * the delivery once it's arrived.
 * @param base Pointer to destination base.
 */
void Transfer::advance(Base *base)
{
	_hours--;
	if (_hours <= 0)
	{
		if (_soldier != 0)
		{
			base->getSoldiers()->push_back(_soldier);
		}
		else if (_craft != 0)
		{
			base->getCrafts()->push_back(_craft);
			_craft->setBase(base);
			_craft->checkup();
		}
		else if (_itemQty != 0)
		{
			base->getStorageItems()->addItem(_itemId, _itemQty);
		}
		else if (_scientists != 0)
		{
			base->setScientists(base->getScientists() + _scientists);
		}
		else if (_engineers != 0)
		{
			base->setEngineers(base->getEngineers() + _engineers);
		}
		_delivered = true;
	}
}

/**
 * Get a pointer to the soldier being transferred.
 * @return a pointer to the soldier being moved.
 */
Soldier *Transfer::getSoldier()
{
	return _soldier;
}

}
