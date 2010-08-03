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
class Craft
{
private:
	RuleCraft *_rules;
	double _lat, _lon;
	int _id, _fuel, _damage;
	vector<CraftWeapon*> _weapons;
	map<LangString, Item*> _items;
public:
	/// Creates a craft of the specified type.
	Craft(RuleCraft *rules, map<LangString, int> *id);
	/// Cleans up the craft.
	~Craft();
	/// Gets the craft's ruleset.
	RuleCraft *getRules();
	/// Gets the craft's ID.
	int getId();
	/// Gets the craft's latitude.
	double getLatitude();
	/// Sets the craft's latitude.
	void setLatitude(double lat);
	/// Gets the craft's longitude.
	double getLongitude();
	/// Sets the craft's longitude.
	void setLongitude(double lon);
	/// Gets the craft's amount of weapons.
	int getNumWeapons();
	/// Gets the craft's amount of soldiers.
	int getNumSoldiers(vector<Soldier*> *soldiers);
	/// Gets the craft's amount of HWPs.
	int getNumHWPs();
	/// Gets the craft's weapon in a certain slot.
	CraftWeapon* *getWeapon(int i);
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
};

#endif
