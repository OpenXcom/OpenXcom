/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

/**
 * Initializes a transfer.
 * @param hours Hours in-transit.
 */
Transfer::Transfer(int hours) : _hours(hours), _soldier(0), _craft(0), _itemId(""), _itemQty(0), _scientists(0), _engineers(0), _delivered(false)
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
 * @param rule Game ruleset.
 */
void Transfer::load(const YAML::Node &node, Base *base, const Ruleset *rule)
{
	node["hours"] >> _hours;
	if (const YAML::Node *pName = node.FindValue("soldier"))
	{
		_soldier = new Soldier(rule->getSoldier("XCOM"), rule->getArmor("STR_NONE_UC"));
		_soldier->load(*pName, rule);
	}
	else if (const YAML::Node *pName = node.FindValue("craft"))
	{
		std::string type;
		(*pName)["type"] >> type;
		_craft = new Craft(rule->getCraft(type), base);
		_craft->load(*pName, rule, 0);
	}
	else if (const YAML::Node *pName = node.FindValue("itemId"))
	{
		*pName >> _itemId;
		node["itemQty"] >> _itemQty;
	}
	else if (const YAML::Node *pName = node.FindValue("scientists"))
	{
		*pName >> _scientists;
	}
	else if (const YAML::Node *pName = node.FindValue("engineers"))
	{
		*pName >> _engineers;
	}
	node["delivered"] >> _delivered;
}

/**
 * Saves the transfer to a YAML file.
 * @param out YAML emitter.
 */
void Transfer::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "hours" << YAML::Value << _hours;
	if (_soldier != 0)
	{
		out << YAML::Key << "soldier" << YAML::Value;
		_soldier->save(out);
	}
	else if (_craft != 0)
	{
		out << YAML::Key << "craft" << YAML::Value;
		_craft->save(out);
	}
	else if (_itemQty != 0)
	{
		out << YAML::Key << "itemId" << YAML::Value << _itemId;
		out << YAML::Key << "itemQty" << YAML::Value << _itemQty;
	}
	else if (_scientists != 0)
	{
		out << YAML::Key << "scientists" << YAML::Value << _scientists;
	}
	else if (_engineers != 0)
	{
		out << YAML::Key << "engineers" << YAML::Value << _engineers;
	}
	out << YAML::Key << "delivered" << YAML::Value << _delivered;
	out << YAML::EndMap;
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
	if (_hours == 0)
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
			base->getItems()->addItem(_itemId, _itemQty);
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

}
