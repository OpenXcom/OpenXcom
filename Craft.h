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
#ifndef OPENXCOM__CRAFT_H
#define OPENXCOM__CRAFT_H

#include "Target.h"
#include <map>
#include <vector>
#include "LangString.h"

using namespace std;

class RuleCraft;
class Soldier;
class CraftWeapon;
class Item;

/**
 * Represents a craft stored in a base.
 * Contains variable info about a craft like
 * position, fuel, damage, etc.
 * @sa RuleCraft
 */
class Craft : public Target
{
private:
	RuleCraft *_rules;
	Target *_target;
	double _speedLon, _speedLat;
	int _id, _fuel, _damage, _speed;
	vector<CraftWeapon*> _weapons;
	map<LangString, Item*> _items;
	LangString _status;
	/// Calculates the new speed vector.
	void calculateSpeed();
public:
	/// Creates a craft of the specified type.
	Craft(RuleCraft *rules, map<LangString, int> *id);
	/// Cleans up the craft.
	~Craft();
	/// Gets the craft's ruleset.
	RuleCraft *getRules();
	/// Gets the craft's target.
	Target *getTarget();
	/// Sets the craft's target.
	void setTarget(Target *target);
	/// Gets the craft's ID.
	int getId();
	/// Gets the craft's status.
	LangString getStatus();
	/// Sets the craft's status.
	void setStatus(LangString status);
	/// Gets the craft's amount of weapons.
	int getNumWeapons();
	/// Gets the craft's amount of soldiers.
	int getNumSoldiers(vector<Soldier*> *soldiers);
	/// Gets the craft's amount of equipment.
	int getNumEquipment();
	/// Gets the craft's amount of HWPs.
	int getNumHWPs();
	/// Gets the craft's weapons.
	vector<CraftWeapon*> *getWeapons();
	/// Gets the craft's items.
	map<LangString, Item*> *getItems();
	/// Gets the craft's amount of fuel.
	int getFuel();
	/// Sets the craft's amount of fuel.
	void setFuel(int fuel);
	/// Gets the craft's percentage of fuel.
	int getFuelPercentage();
	/// Gets the craft's amount of damage.
	int getDamage();
	/// Sets the craft's amount of damage.
	void setDamage(int damage);
	/// Gets the craft's percentage of damage.
	int getDamagePercentage();
	/// Gets the craft's speed.
	int getSpeed();
	/// Sets the craft's speed.
	void setSpeed(int speed);
	/// Handles craft logic.
	void think();
};

#endif
