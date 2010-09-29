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
#define _USE_MATH_DEFINES
#include "Craft.h"
#include <cmath>
#include <sstream>
#include "Language.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "Item.h"
#include "Soldier.h"
#include "Base.h"

#define CRAFT_RANGE 600

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param id List of craft IDs.
 * @param base Pointer to base of origin.
 */
Craft::Craft(RuleCraft *rules, map<LangString, int> *id, Base *base) : MovingTarget(), _rules(rules), _base(base), _fuel(0), _damage(0), _weapons(), _items(), _status(STR_READY), _lowFuel(false)
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
 * Returns the craft's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
string Craft::getName(Language *lang)
{
	stringstream name;
	name << lang->getString(_rules->getType()) << "-" << _id;
	return name.str();
}

/**
 * Returns the base the craft belongs to.
 * @return Pointer to base.
 */
Base *Craft::getBase()
{
	return _base;
}

/**
 * Changes the base the craft belongs to.
 * @param base Pointer to base.
 */
void Craft::setBase(Base *base)
{
	_base = base;
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
 * Changes the destination the craft is heading to.
 */
void Craft::setDestination(Target *dest)
{
	MovingTarget::setDestination(dest);
	setSpeed(_rules->getMaxSpeed());
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
int Craft::getNumSoldiers()
{
	if (_rules->getSoldiers() == 0)
		return 0;

	int total = 0;

	for (vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); i++)
	{
		if ((*i)->getCraft() == this)
			total++;
	}

	return total;
}

/**
 * Returns the amount of equipment currently
 * equipped on this craft.
 * @return Number of items.
 */
int Craft::getNumEquipment()
{
	int total = 0;

	for (map<LangString, Item*>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		total += i->second->getQuantity();
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
	else if (_fuel < 0)
		_fuel = 0;
}

/**
 * Returns the ratio between the amount of fuel currently
 * contained in this craft and the total it can carry.
 * @return Percentage of fuel.
 */
int Craft::getFuelPercentage()
{
	return (int)floor((double)_fuel / _rules->getMaxFuel() * 100.0);
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

/**
 * Returns whether the craft is currently low on fuel
 * (only has enough to head back to base).
 * @return True if it's low, false otherwise.
 */
bool Craft::getLowFuel()
{
	return _lowFuel;
}

/**
 * Changes whether the craft is currently low on fuel
 * (only has enough to head back to base).
 * @param low True if it's low, false otherwise.
 */
void Craft::setLowFuel(bool low)
{
	_lowFuel = low;
}

/**
 * Returns the current distance between the craft
 * and the base it belongs to.
 * @return Distance in radian.
 */
double Craft::getDistanceFromBase()
{
	double dLon = _base->getLongitude() - _lon;
	double dLat = _base->getLatitude() - _lat;
	double length = sqrt(dLon * dLon + dLat * dLat);
	return length;
}

/**
 * Moves the craft to its destination.
 */
void Craft::think()
{
	if (_dest != 0)
	{
		calculateSpeed();
	}
	setLongitude(_lon + _speedLon);
	setLatitude(_lat + _speedLat);
	if (reachedDestination())
	{
		_lon = _dest->getLongitude();
		_lat = _dest->getLatitude();

		if (_dest == (Target*)_base)
		{
			if (_damage > 0)
			{
				setStatus(STR_REPAIRS);
			}
			else if (_fuel < _rules->getMaxFuel())
			{
				setStatus(STR_REFUELLING);
			}
			else
			{
				setStatus(STR_REARMING);
			}
			setSpeed(0);
			setDestination(0);
			_lowFuel = false;
		}
	}
}

/**
 * Returns if a certain point is covered by the craft's
 * radar range, taking in account the positions of both.
 * @param pointLon Point longitude.
 * @param pointLat Point latitude.
 * @return True if it's within range, False otherwise.
 */
bool Craft::insideRadarRange(double pointLon, double pointLat)
{
	double newrange = CRAFT_RANGE * (1 / 60.0) * (M_PI / 180);
	double dLon = pointLon - _lon;
	double dLat = pointLat - _lat;
    return (dLon * dLon + dLat * dLat <= newrange * newrange);
}
