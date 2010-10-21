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
#ifndef OPENXCOM_CRAFT_H
#define OPENXCOM_CRAFT_H

#include "MovingTarget.h"
#include <map>
#include <vector>
#include <string>
#include "../Resource/LangString.h"

using namespace std;

class RuleCraft;
class Base;
class Soldier;
class CraftWeapon;
class Item;

/**
 * Represents a craft stored in a base.
 * Contains variable info about a craft like
 * position, fuel, damage, etc.
 * @sa RuleCraft
 */
class Craft : public MovingTarget
{
private:
	RuleCraft *_rules;
	Base *_base;
	int _id, _fuel, _damage, _speed;
	vector<CraftWeapon*> _weapons;
	map<LangString, Item*> _items;
	LangString _status;
	bool _lowFuel;
public:
	/// Creates a craft of the specified type.
	Craft(RuleCraft *rules, map<LangString, int> *id, Base *base);
	/// Cleans up the craft.
	~Craft();
	/// Gets the craft's ruleset.
	RuleCraft *getRules();
	/// Gets the craft's ID.
	int getId();
	/// Gets the craft's name.
	string getName(Language *lang);
	/// Gets the craft's base.
	Base *getBase();
	/// Sets the craft's base.
	void setBase(Base *base);
	/// Gets the craft's status.
	LangString getStatus();
	/// Sets the craft's status.
	void setStatus(LangString status);
	/// Sets the craft's destination.
	void setDestination(Target *dest);
	/// Gets the craft's amount of weapons.
	int getNumWeapons();
	/// Gets the craft's amount of soldiers.
	int getNumSoldiers();
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
	/// Gets whether the craft is running out of fuel.
	bool getLowFuel();
	/// Sets whether the craft is running out of fuel.
	void setLowFuel(bool low);
	/// Gets the craft's distance from its base.
	double getDistanceFromBase();
	/// Returns the craft to its base.
	void returnToBase();
	/// Checks if a target is inside the craft's radar.
	bool insideRadarRange(Target *target);
	/// Handles craft logic.
	void think();
};

#endif
