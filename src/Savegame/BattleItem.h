/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_BATTLEITEM_H
#define OPENXCOM_BATTLEITEM_H

#include "../Battlescape/Position.h"
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleItem;
class RuleInventory;
class BattleUnit;
class Tile;

/**
 * Represents a single item in the battlescape.
 * Contains battle-related info about an item like the position, ammo quantity, ...
 * @sa RuleItem
 * @sa Item
 */
class BattleItem
{
private:
	int _id;
	RuleItem *_rules;
	BattleUnit *_owner, *_previousOwner;
	BattleUnit *_unit;
	Tile *_tile;
	RuleInventory *_inventorySlot;
	int _inventoryX, _inventoryY;
	BattleItem *_ammoItem;
	int _fuseTimer, _ammoQuantity;
	int _painKiller, _heal, _stimulant;
	bool _XCOMProperty, _droppedOnAlienTurn;
public:
	/// Creates a item of the specified type.
	BattleItem(RuleItem *rules, int *id);
	/// Cleans up the item.
	~BattleItem();
	/// Loads the item from YAML.
	void load(const YAML::Node& node);
	/// Saves the item to YAML.
	YAML::Node save() const;
	/// Gets the item's ruleset.
	RuleItem *getRules() const;
	/// Gets the item's ammo quantity
	int getAmmoQuantity() const;
	/// Sets the item's ammo quantity.
	void setAmmoQuantity(int qty);
	/// Gets the turn until explosion
	int getFuseTimer() const;
	/// Sets the turns until explosion.
	void setFuseTimer(int turns);
	/// Spend one bullet.
	bool spendBullet();
	/// Gets the item's owner.
	BattleUnit *getOwner() const;
	/// Gets the item's previous owner.
	BattleUnit *getPreviousOwner() const;
	/// Sets the owner.
	void setOwner(BattleUnit *owner);
	/// Removes the item from previous owner and moves to new owner.
	void moveToOwner(BattleUnit *owner);
	/// Gets the item's inventory slot.
	RuleInventory *getSlot() const;
	/// Sets the item's inventory slot.
	void setSlot(RuleInventory *slot);
	/// Gets the item's inventory X position.
	int getSlotX() const;
	/// Sets the item's inventory X position.
	void setSlotX(int x);
	/// Gets the item's inventory Y position.
	int getSlotY() const;
	/// Sets the item's inventory Y position.
	void setSlotY(int y);
	/// Checks if the item is occupying a slot.
	bool occupiesSlot(int x, int y, BattleItem *item = 0) const;
	/// Gets the item's ammo item.
	BattleItem *getAmmoItem();
	/// Determines if this item uses ammo.
	bool needsAmmo() const;
	/// Sets the item's ammo item.
	int setAmmoItem(BattleItem *item);
	/// Gets the item's tile.
	Tile *getTile() const;
	/// Sets the tile.
	void setTile(Tile *tile);
	/// Gets it's unique id.
	int getId() const;
	/// Gets the corpse's unit.
	BattleUnit *getUnit() const;
	/// Sets the corpse's unit.
	void setUnit(BattleUnit *unit);
	/// Set medikit Heal quantity
	void setHealQuantity (int heal);
	/// Get medikit heal quantity
	int getHealQuantity() const;
	/// Set medikit pain killers quantity
	void setPainKillerQuantity (int pk);
	/// Get medikit pain killers quantity
	int getPainKillerQuantity() const;
	/// Set medikit stimulant quantity
	void setStimulantQuantity (int stimulant);
	/// Get medikit stimulant quantity
	int getStimulantQuantity() const;
	/// Set xcom property flag
	void setXCOMProperty (bool flag);
	/// Get xcom property flag
	bool getXCOMProperty() const;
	/// get the flag representing "not dropped on player turn"
	bool getTurnFlag() const;
	/// set the flag representing "not dropped on player turn"
	void setTurnFlag(bool flag);
	/// Sets the item's ruleset.
	void convertToCorpse(RuleItem *rules);

};

}

#endif
