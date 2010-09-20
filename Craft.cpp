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
#include <sstream>
#include "Language.h"
#include "RuleCraft.h"
#include "CraftWeapon.h"
#include "Item.h"
#include "Soldier.h"

#define RANGE_FACTOR 0.0003
#define CRAFT_RANGE 600

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param id List of craft IDs.
 */
Craft::Craft(RuleCraft *rules, map<LangString, int> *id, Base *base) : Target(), _rules(rules), _target(0), _base(base), _speedLon(0.0), _speedLat(0.0), _fuel(0), _damage(0), _speed(0), _weapons(), _items(), _status(STR_READY)
{
	_id = (*id)[_rules->getType()];
	(*id)[_rules->getType()]++;
	for (int i = 0; i < _rules->getWeapons(); i++)
		_weapons.push_back(0);
	setSpeed(_rules->getMaxSpeed());
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
 * @param Language to get strings from.
 * @return Full name.
 */
string Craft::getName(Language *lang)
{
	stringstream name;
	name << lang->getString(_rules->getType()) << "-" << _id;
	return name.str();
}

/**
 * Returns the target the craft is following.
 */
Target *Craft::getTarget()
{
	return _target;
}

/**
 * Changes the target the craft is following.
 */
void Craft::setTarget(Target *target)
{
	_target = target;
	calculateSpeed();
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

/**
 * Returns the speed of the craft.
 * @return Speed in kilometers.
 */
int Craft::getSpeed()
{
	return _speed;
}

/**
 * Changes the speed of the craft.
 * @param speed Speed in kilometers.
 */
void Craft::setSpeed(int speed)
{
	_speed = speed;
	calculateSpeed();
}

/**
 * Calculates the speed vector and direction for the UFO
 * based on the current raw speed and target destination.
 */
void Craft::calculateSpeed()
{
	double newSpeed = _speed * SPEED_FACTOR;
	if (_target != 0)
	{
		double dLon = _target->getLongitude() - _lon;
		double dLat = _target->getLatitude() - _lat;
		double length = sqrt(dLon * dLon + dLat * dLat);
		_speedLon = dLon / length * newSpeed;
		_speedLat = dLat / length * newSpeed;
	}
	else
	{
		_speedLon = 0;
		_speedLat = 0;
	}
}

/**
 * Moves the craft to its target.
 */
void Craft::think()
{
	if (_target != 0)
	{
		calculateSpeed();
	}
	_lon += _speedLon;
	_lat += _speedLat;
	if (((_speedLon > 0 && _lon > _target->getLongitude()) || (_speedLon < 0 && _lon < _target->getLongitude())) &&
		((_speedLat > 0 && _lat > _target->getLatitude()) || (_speedLat < 0 && _lat < _target->getLatitude())))
	{
		_lon = _target->getLongitude();
		_lat = _target->getLatitude();
		setSpeed(0);
		if (_target == (Target*)_base)
		{
			setTarget(0);
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
	double newrange = CRAFT_RANGE * RANGE_FACTOR;
	double dLon = pointLon - _lon;
	double dLat = pointLat - _lat;
    return (dLon * dLon + dLat * dLat <= newrange * newrange);
}
