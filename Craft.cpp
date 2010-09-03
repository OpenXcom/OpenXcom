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
#include "Craft.h"
#include <cmath>
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "Item.h"
#include "Soldier.h"

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param id List of craft IDs.
 */
Craft::Craft(RuleCraft *rules, map<LangString, int> *id) : _rules(rules), _lat(0.0), _lon(0.0), _fuel(0), _damage(0), _speed(0), _weapons(), _items(), _status(STR_READY)
{
	_id = (*id)[_rules->getType()];
	(*id)[_rules->getType()]++;
	for (int i = 0; i < _rules->getWeapons(); i++)
		_weapons.push_back(0);
}

/**
 * Delete the contents of the craft from memory.
 */
Craft::~Craft()
{
	for (vector<CraftWeapon*>::iterator i = _weapons.begin(); i != _weapons.end(); i++)
	{
		delete *i;
	}
	for (map<LangString, Item*>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		delete i->second;
	}
}

/**
 * Returns the ruleset for the craft's type.
 * @return Pointer to ruleset.
 */
RuleCraft *Craft::getRules()
{
	return _rules;
}

/**
 * Returns the craft's unique ID. Each craft
 * can be identified by its type and ID.
 * @return Unique ID.
 */
int Craft::getId()
{
	return _id;
}

/**
 * Returns the latitude coordinate of the craft.
 * @return Latitude in radian.
 */
double Craft::getLatitude()
{
	return _lat;
}

/**
 * Changes the latitude coordinate of the craft.
 * @param lat Latitude in radian.
 */
void Craft::setLatitude(double lat)
{
	_lat = lat;
}

/**
 * Returns the longitude coordinate of the craft.
 * @return Longitude in radian.
 */
double Craft::getLongitude()
{
	return _lon;
}

/**
 * Changes the longitude coordinate of the craft.
 * @param lon Longitude in radian.
 */
void Craft::setLongitude(double lon)
{
	_lon = lon;
}

/**
 * Returns the current status of the craft.
 * @return Status string.
 */
LangString Craft::getStatus()
{
	return _status;
}

/**
 * Changes the current status of the craft.
 * @param status Status string.
 */
void Craft::setStatus(LangString status)
{
	_status = status;
}

/**
 * Returns the amount of weapons currently
 * equipped on this craft.
 * @return Number of weapons.
 */
int Craft::getNumWeapons()
{
	if (_rules->getWeapons() == 0)
		return 0;

	int total = 0;

	for (vector<CraftWeapon*>::iterator i = _weapons.begin(); i != _weapons.end(); i++)
	{
		if ((*i) != 0)
			total++;
	}

	return total;
}

/**
 * Returns the amount of soldiers from a list
 * that are currently attached to this craft.
 * @return Number of soldiers.
 */
int Craft::getNumSoldiers(vector<Soldier*> *soldiers)
{
	if (_rules->getSoldiers() == 0)
		return 0;

	int total = 0;

	for (vector<Soldier*>::iterator i = soldiers->begin(); i != soldiers->end(); i++)
	{
		if ((*i)->getCraft() == this)
			total++;
	}

	return total;
}

/**
 * Returns the amount of HWPs currently
 * contained in this craft.
 * @return Number of HWPs.
 */
int Craft::getNumHWPs()
{
	return 0;
}

/**
 * Returns the list of weapons currently equipped
 * in the craft.
 * @return Pointer to weapon list.
 */
vector<CraftWeapon*> *Craft::getWeapons()
{
	return &_weapons;
}

/**
 * Returns the list of items in the craft.
 * @return Pointer to the item list.
 */
map<LangString, Item*> *Craft::getItems()
{
	return &_items;
}

/**
 * Returns the amount of fuel currently contained
 * in this craft.
 * @return Amount of fuel.
 */
int Craft::getFuel()
{
	return _fuel;
}

/**
 * Changes the amount of fuel currently contained
 * in this craft.
 * @param fuel Amount of fuel.
 */
void Craft::setFuel(int fuel)
{
	_fuel = fuel;
	if (_fuel > _rules->getMaxFuel())
		_fuel = _rules->getMaxFuel();
}

/**
 * Returns the ratio between the amount of fuel currently
 * contained in this craft and the total it can carry.
 * @return Percentage of fuel.
 */
int Craft::getFuelPercentage()
{
	return (int)floor((double)_fuel / _rules->getMaxFuel() * 100);
}

/**
 * Returns the amount of damage this craft has taken.
 * @return Amount of damage.
 */
int Craft::getDamage()
{
	return _damage;
}

/**
 * Changes the amount of damage this craft has taken.
 * @param damage Amount of damage.
 */
void Craft::setDamage(int damage)
{
	_damage = damage;
	if (_damage < 0)
		_damage = 0;
}

/**
 * Returns the ratio between the amount of damage this
 * craft can take and the total it can take before it's
 * destroyed.
 * @return Percentage of damage.
 */
int Craft::getDamagePercentage()
{
	return (int)floor((double)_damage / _rules->getMaxDamage() * 100);
}
