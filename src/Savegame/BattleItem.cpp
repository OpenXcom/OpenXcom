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
#include "BattleItem.h"
#include "BattleUnit.h"
#include "Tile.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleInventory.h"

namespace OpenXcom
{

/**
 * Initializes a item of the specified type.
 * @param rules Pointer to ruleset.
 */
BattleItem::BattleItem(RuleItem *rules, int *id) : _id(*id), _rules(rules), _owner(0), _previousOwner(0), _unit(0), _tile(0), _inventorySlot(0), _inventoryX(0), _inventoryY(0), _ammoItem(0), _explodeTurn(0), _ammoQuantity(0), _painKiller(0), _heal(0), _stimulant(0), _XCOMProperty(false)
{
	if (_rules && _rules->getBattleType() == BT_AMMO)
	{
		setAmmoQuantity(_rules->getClipSize());
	} else if (_rules && _rules->getBattleType() == BT_MEDIKIT)
	{
		setHealQuantity (_rules->getHealQuantity ());
		setPainKillerQuantity (_rules->getPainKillerQuantity ());
		setStimulantQuantity (_rules->getStimulantQuantity ());
	}
	(*id)++;

	// weapon does not need ammo, ammo item points to weapon
	if (_rules && _rules->getClipSize() == -1)
	{
		_ammoItem = this;
		setAmmoQuantity(99999);
	}
}

/**
 *
 */
BattleItem::~BattleItem()
{
}

/**
 * Loads the item from a YAML file.
 * @param node YAML node.
 */
void BattleItem::load(const YAML::Node &node)
{
	//node["inventoryslot"] >> _inventorySlot;
	node["inventoryX"] >> _inventoryX;
	node["inventoryY"] >> _inventoryY;
	node["ammoqty"] >> _ammoQuantity;
	node["painKiller"] >> _painKiller;
	node["heal"] >> _heal;
	node["stimulant"] >> _stimulant;
	node["explodeTurn"] >> _explodeTurn;
}

/**
 * Saves the item to a YAML file.
 * @param out YAML emitter.
 */
void BattleItem::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	if (_owner)
	{
		out << YAML::Key << "owner" << YAML::Value << _owner->getId();
	}
	else
	{
		out << YAML::Key << "owner" << YAML::Value << -1;
	}
	if (_unit)
	{
		out << YAML::Key << "unit" << YAML::Value << _unit->getId();
	}
	else
	{
		out << YAML::Key << "unit" << YAML::Value << -1;
	}

	if (_inventorySlot)
	{
		out << YAML::Key << "inventoryslot" << YAML::Value << _inventorySlot->getId();
	}
	else
	{
		out << YAML::Key << "inventoryslot" << YAML::Value << "NULL";
	}
	out << YAML::Key << "inventoryX" << YAML::Value << _inventoryX;
	out << YAML::Key << "inventoryY" << YAML::Value << _inventoryY;

	out << YAML::Key << "position" << YAML::Value << YAML::Flow;
	if (_tile)
	{
		out << YAML::BeginSeq << _tile->getPosition().x << _tile->getPosition().y << _tile->getPosition().z << YAML::EndSeq;
	}
	else
	{
		out << YAML::BeginSeq << -1 << -1 << -1 << YAML::EndSeq;
	}
	out << YAML::Key << "ammoqty" << YAML::Value << _ammoQuantity;
	if (_ammoItem)
	{
		out << YAML::Key << "ammoItem" << YAML::Value << _ammoItem->getId();
	}
	else
	{
		out << YAML::Key << "ammoItem" << YAML::Value << -1;
	}

	out << YAML::Key << "painKiller" << YAML::Value << _painKiller;
	out << YAML::Key << "heal" << YAML::Value << _heal;
	out << YAML::Key << "stimulant" << YAML::Value << _stimulant;
	out << YAML::Key << "explodeTurn" << YAML::Value << _explodeTurn;

	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the item's type.
 * @return Pointer to ruleset.
 */
RuleItem *BattleItem::getRules() const
{
	return _rules;
}

/**
 * Returns the turn to explode on. 0 = unprimed grenade
 * @return Explode turn.
 */
int BattleItem::getExplodeTurn() const
{
	return _explodeTurn;
}

/**
 * Set the turn to explode on.
 * @param turn Turn to explode on.
 */
void BattleItem::setExplodeTurn(int turn)
{
	_explodeTurn = turn;
}

/**
 * Returns the quantity of ammo in this item.
 * @return Ammo quantity.
 */
int BattleItem::getAmmoQuantity() const
{
	return _ammoQuantity;
}

/**
 * Changes the quantity of ammo in this item.
 * @param qty Ammo quantity.
 */
void BattleItem::setAmmoQuantity(int qty)
{
	_ammoQuantity = qty;
}

/**
 * Spends a bullet from the ammo in this item.
 * @return bool Got bullets left?
 */
bool BattleItem::spendBullet()
{
	_ammoQuantity--;
	if (_ammoQuantity == 0)
		return false;
	else
		return true;
}

/**
 * Gets the item's owner.
 * @return pointer to Battleunit
 */
BattleUnit *BattleItem::getOwner() const
{
	return _owner;
}

/// Gets the item's previous owner.
BattleUnit *BattleItem::getPreviousOwner() const
{
	return _previousOwner;
}

/**
 * Sets the item's owner.
 * @param owner pointer to Battleunit
 */
void BattleItem::setOwner(BattleUnit *owner)
{
	_previousOwner = _owner;
	_owner = owner;
}

/**
 * Removes the item from previous owner and moves to new owner.
 * @param owner pointer to Battleunit
 */
void BattleItem::moveToOwner(BattleUnit *owner)
{
	
	_previousOwner = _owner ? _owner:owner;
	_owner = owner;
	if (_previousOwner != 0)
	{
		for (std::vector<BattleItem*>::iterator i = _previousOwner->getInventory()->begin(); i != _previousOwner->getInventory()->end(); ++i)
		{
			if ((*i) == this)
			{
				_previousOwner->getInventory()->erase(i);
				break;
			}
		}
	}
	if (_owner != 0)
	{
		_owner->getInventory()->push_back(this);
	}
}

/**
 * Gets the item's inventory slot.
 * @return slot id
 */
RuleInventory *BattleItem::getSlot() const
{
	return _inventorySlot;
}

/**
 * Sets the item's inventory slot.
 * @param slot slot id
 */
void BattleItem::setSlot(RuleInventory *slot)
{
	_inventorySlot = slot;
}

/**
 * Gets the item's inventory X position.
 * @return X position.
 */
int BattleItem::getSlotX() const
{
	return _inventoryX;
}

/**
 * Sets the item's inventory X position.
 * @param x X position.
 */
void BattleItem::setSlotX(int x)
{
	_inventoryX = x;
}

/**
 * Gets the item's inventory Y position.
 * @return Y position.
 */
int BattleItem::getSlotY() const
{
	return _inventoryY;
}

/**
 * Sets the item's inventory Y position.
 * @param y Y position.
 */
void BattleItem::setSlotY(int y)
{
	_inventoryY = y;
}

/**
 * Checks if the item is covering certain inventory slot(s).
 * @param x Slot X position.
 * @param y Slot Y position.
 * @param item Item to check for overlap, or NULL if none.
 * @return Whether it's covering or not.
 */
bool BattleItem::occupiesSlot(int x, int y, BattleItem *item) const
{
	if (item == this)
		return false;
	if (_inventorySlot->getType() == INV_HAND)
		return true;
	if (item == 0)
	{
		return (x >= _inventoryX && x < _inventoryX + _rules->getInventoryWidth() &&
				y >= _inventoryY && y < _inventoryY + _rules->getInventoryHeight());
	}
	else
	{
		return !(x >= _inventoryX + _rules->getInventoryWidth() ||
				x + item->getRules()->getInventoryWidth() <= _inventoryX ||
				y >= _inventoryY + _rules->getInventoryHeight() ||
				y + item->getRules()->getInventoryHeight() <= _inventoryY);
	}
}

/**
 * Gets the item's ammo item.
 * @return BattleItem
 */
BattleItem *BattleItem::getAmmoItem()
{
	return _ammoItem;
}

/**
 * Gets the item's ammo item.
 * @return BattleItem
 */
bool BattleItem::needsAmmo() const
{
	return !(_ammoItem == this); // no ammo for this weapon is needed
}

/**
 * Sets the item's ammo item.
 * @param item BattleItem
 * @return -2 when ammo doesn't fit, or -1 when weapon already contains ammo
 */
int BattleItem::setAmmoItem(BattleItem *item)
{
	if (!needsAmmo()) return -2;

	if (item == 0)
	{
		_ammoItem = 0;
		return 0;
	}

	if (_ammoItem)
		return -1;

	for (std::vector<std::string>::iterator i = _rules->getCompatibleAmmo()->begin(); i != _rules->getCompatibleAmmo()->end(); ++i)
	{
		if (*i == item->getRules()->getType())
		{
			_ammoItem = item;
			return 0;
		}
	}

	return -2;
}


/**
 * Gets the item's tile.
 * @return Tile
 */
Tile *BattleItem::getTile() const
{
	return _tile;
}

/**
 * Sets the item's tile.
 * @param Tile
 */
void BattleItem::setTile(Tile *tile)
{
	_tile = tile;
}

int BattleItem::getId() const
{
	return _id;
}

/**
 * Gets the corpse's unit.
 * @return BattleUnit
 */
BattleUnit *BattleItem::getUnit() const
{
	return _unit;
}

/**
 * Sets the corpse's unit.
 * @param unit
 */
void BattleItem::setUnit(BattleUnit *unit)
{
	_unit = unit;
}

/**
 * Set the heal quantity of item
 * @param heal the new heal quantity
 */
void BattleItem::setHealQuantity (int heal)
{
	_heal = heal;
}

/**
 * Get the heal quantity of item
 * @return The new heal quantity
 */
int BattleItem::getHealQuantity () const
{
	return _heal;
}

/**
 * Set the pain killer quantity of item
 * @param pk the new pain killer quantity
 */
void BattleItem::setPainKillerQuantity (int pk)
{
	_painKiller = pk;
}

/**
 * Get the pain killer quantity of item
 * @return The new pain killer quantity
 */
int BattleItem::getPainKillerQuantity () const
{
	return _painKiller;
}

/**
 * Set the stimulant quantity of item
 * @param stimulant the new stimulant quantity
 */
void BattleItem::setStimulantQuantity (int stimulant)
{
	_stimulant = stimulant;
}

/**
 * Get the stimulant quantity of item
 * @return The new stimulant quantity
 */
int BattleItem::getStimulantQuantity () const
{
	return _stimulant;
}

/**
 * Set the XCOM property flag. This is to determine at debriefing what goes into base/craft
 * @param bool
 */
void BattleItem::setXCOMProperty (bool flag)
{
	_XCOMProperty = flag;
}
/**
 * Get the XCOM property flag. This is to determine at debriefing what goes into base/craft
 * @return bool
 */
bool BattleItem::getXCOMProperty () const
{
	return _XCOMProperty;
}
}
