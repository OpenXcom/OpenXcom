/*
 * Copyright 2010 OpenXcom Developers.
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
#include "../Engine/Language.h"
#include "../Ruleset/RuleCraft.h"
#include "CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Ruleset/Ruleset.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "Base.h"

namespace OpenXcom
{

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param base Pointer to base of origin.
 * @param ids List of craft IDs (Leave NULL for no ID).
 */
Craft::Craft(RuleCraft *rules, Base *base, std::map<std::string, int> *ids) : MovingTarget(), _rules(rules), _base(base), _id(0), _fuel(0), _damage(0), _weapons(), _status("STR_READY"), _lowFuel(false), _inBattlescape(false)
{
	_items = new ItemContainer();
	if (ids != 0)
	{
		_id = (*ids)[_rules->getType()];
		(*ids)[_rules->getType()]++;
	}
	for (int i = 0; i < _rules->getWeapons(); ++i)
	{
		_weapons.push_back(0);
	}
}

/**
 * Delete the contents of the craft from memory.
 */
Craft::~Craft()
{
	for (std::vector<CraftWeapon*>::iterator i = _weapons.begin(); i != _weapons.end(); ++i)
	{
		delete *i;
	}
	delete _items;
}

/**
 * Loads the craft from a YAML file.
 * @param node YAML node.
 * @param rule Ruleset for the saved game.
 */
void Craft::load(const YAML::Node &node, const Ruleset *rule)
{
	unsigned int size = 0;

	MovingTarget::load(node);
	node["id"] >> _id;
	node["fuel"] >> _fuel;
	node["damage"] >> _damage;

	size = node["weapons"].size();
	for (unsigned int i = 0; i < size; ++i)
	{
		std::string type;
		node["weapons"][i]["type"] >> type;
		if (type != "0")
		{
			CraftWeapon *w = new CraftWeapon(rule->getCraftWeapon(type), 0);
			w->load(node["weapons"][i]);
			_weapons[i] = w;
		}
	}

	_items->load(node["items"]);
	node["status"] >> _status;
	node["lowFuel"] >> _lowFuel;
	node["inBattlescape"] >> _inBattlescape;
}

/**
 * Saves the craft to a YAML file.
 * @param out YAML emitter.
 */
void Craft::save(YAML::Emitter &out) const
{
	MovingTarget::save(out);
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::Key << "fuel" << YAML::Value << _fuel;
	out << YAML::Key << "damage" << YAML::Value << _damage;
	out << YAML::Key << "weapons" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<CraftWeapon*>::const_iterator i = _weapons.begin(); i != _weapons.end(); ++i)
	{
		if (*i != 0)
		{
			(*i)->save(out);
		}
		else
		{
			out << YAML::BeginMap;
			out << YAML::Key << "type" << YAML::Value << "0";
			out << YAML::EndMap;
		}
	}
	out << YAML::EndSeq;
	out << YAML::Key << "items" << YAML::Value;
	_items->save(out);
	out << YAML::Key << "status" << YAML::Value << _status;
	out << YAML::Key << "lowFuel" << YAML::Value << _lowFuel;
	out << YAML::Key << "inBattlescape" << YAML::Value << _inBattlescape;
	out << YAML::EndMap;
}

/**
 * Saves the craft's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void Craft::saveId(YAML::Emitter &out) const
{
	MovingTarget::saveId(out);
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "id" << YAML::Value << _id;
	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the craft's type.
 * @return Pointer to ruleset.
 */
RuleCraft *const Craft::getRules() const
{
	return _rules;
}

/**
 * Returns the craft's unique ID. Each craft
 * can be identified by its type and ID.
 * @return Unique ID.
 */
int Craft::getId() const
{
	return _id;
}

/**
 * Returns the craft's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Craft::getName(Language *lang) const
{
	std::wstringstream name;
	name << lang->getString(_rules->getType()) << "-" << _id;
	return name.str();
}

/**
 * Returns the base the craft belongs to.
 * @return Pointer to base.
 */
Base *const Craft::getBase() const
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
std::string Craft::getStatus() const
{
	return _status;
}

/**
 * Changes the current status of the craft.
 * @param status Status string.
 */
void Craft::setStatus(std::string status)
{
	_status = status;
}

/**
 * Changes the destination the craft is heading to.
 * @param dest Pointer to new destination.
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
int Craft::getNumWeapons() const
{
	if (_rules->getWeapons() == 0)
	{
		return 0;
	}

	int total = 0;

	for (std::vector<CraftWeapon*>::const_iterator i = _weapons.begin(); i != _weapons.end(); ++i)
	{
		if ((*i) != 0)
		{
			total++;
		}
	}

	return total;
}

/**
 * Returns the amount of soldiers from a list
 * that are currently attached to this craft.
 * @return Number of soldiers.
 */
int Craft::getNumSoldiers() const
{
	if (_rules->getSoldiers() == 0)
		return 0;

	int total = 0;

	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
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
int Craft::getNumEquipment() const
{
	return _items->getTotalQuantity();
}

/**
 * Returns the amount of HWPs currently
 * contained in this craft.
 * @return Number of HWPs.
 */
int Craft::getNumHWPs() const
{
	return 0;
}

/**
 * Returns the list of weapons currently equipped
 * in the craft.
 * @return Pointer to weapon list.
 */
std::vector<CraftWeapon*> *const Craft::getWeapons()
{
	return &_weapons;
}

/**
 * Returns the list of items in the craft.
 * @return Pointer to the item list.
 */
ItemContainer *const Craft::getItems()
{
	return _items;
}

/**
 * Returns the amount of fuel currently contained
 * in this craft.
 * @return Amount of fuel.
 */
int Craft::getFuel() const
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
	{
		_fuel = _rules->getMaxFuel();
	}
	else if (_fuel < 0)
	{
		_fuel = 0;
	}
}

/**
 * Returns the ratio between the amount of fuel currently
 * contained in this craft and the total it can carry.
 * @return Percentage of fuel.
 */
int Craft::getFuelPercentage() const
{
	return (int)floor((double)_fuel / _rules->getMaxFuel() * 100.0);
}

/**
 * Returns the amount of damage this craft has taken.
 * @return Amount of damage.
 */
int Craft::getDamage() const
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
	{
		_damage = 0;
	}
}

/**
 * Returns the ratio between the amount of damage this
 * craft can take and the total it can take before it's
 * destroyed.
 * @return Percentage of damage.
 */
int Craft::getDamagePercentage() const
{
	return (int)floor((double)_damage / _rules->getMaxDamage() * 100);
}

/**
 * Returns whether the craft is currently low on fuel
 * (only has enough to head back to base).
 * @return True if it's low, false otherwise.
 */
bool Craft::getLowFuel() const
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
double Craft::getDistanceFromBase() const
{
	double dLon, dLat;
	return getDistance(_base, &dLon, &dLat);
}

/**
 * Returns the amount of fuel the craft uses up
 * while it's on the air, based on its speed.
 * @return Fuel amount.
 */
int Craft::getFuelConsumption() const
{
	return (int)floor(_speed / 100.0);
}

/**
 * Returns the minimum required fuel for the
 * craft to make it back to base.
 * @return Fuel amount.
 */
int Craft::getFuelLimit() const
{
	return (int)floor(getFuelConsumption() * getDistanceFromBase() / (getRadianSpeed() * 120));
}

/**
 * Sends the craft back to its origin base.
 */
void Craft::returnToBase()
{
	setDestination(_base);
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
	if (_dest != 0 && finishedRoute())
	{
		_lon = _dest->getLongitude();
		_lat = _dest->getLatitude();

		if (_dest == (Target*)_base)
		{
			int available = 0, full = 0;
			for (std::vector<CraftWeapon*>::iterator i = _weapons.begin(); i != _weapons.end(); ++i)
			{
				if ((*i) == 0)
					continue;
				available++;
				if ((*i)->getAmmo() >= (*i)->getRules()->getAmmoMax())
				{
					full++;
				}
				else
				{
					(*i)->setRearming(true);
				}
			}

			if (_damage > 0)
			{
				_status = "STR_REPAIRS";
			}
			else if (available != full)
			{
				_status = "STR_REARMING";
			}
			else
			{
				_status = "STR_REFUELLING";
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
 * @param target Pointer to target to compare.
 * @return True if it's within range, False otherwise.
 */
bool Craft::insideRadarRange(Target *target) const
{
	bool inside = false;
	double newrange = _rules->getRadarRange() * (1 / 60.0) * (M_PI / 180);
	for (double lon = target->getLongitude() - 2*M_PI; lon <= target->getLongitude() + 2*M_PI + 0.01; lon += 2*M_PI)
	{
		double dLon = lon - _lon;
		double dLat = target->getLatitude() - _lat;
		inside = inside || (dLon * dLon + dLat * dLat <= newrange * newrange);
	}
    return inside;
}

/**
 * Consumes the craft's fuel every 10 minutes
 * while it's on the air.
 */
void Craft::consumeFuel()
{
	setFuel(_fuel - getFuelConsumption());
}

/**
 * Repairs the craft's damage every hour
 * while it's docked in the base.
 */
void Craft::repair()
{
	setDamage(_damage - _rules->getRepairRate());
	if (_damage <= 0)
	{
		_status = "STR_REARMING";
	}
}

/**
 * Refuels the craft every 30 minutes
 * while it's docked in the base.
 */
void Craft::refuel()
{
	setFuel(_fuel + _rules->getRefuelRate());
	if (_fuel >= _rules->getMaxFuel())
	{
		_status = "STR_READY";
	}
}

/**
 * Rearms the craft's weapons by adding ammo every hour
 * while it's docked in the base.
 * @return The ammo ID missing for rearming, or "" if none.
 */
std::string Craft::rearm()
{
	std::string ammo = "";
	for (std::vector<CraftWeapon*>::iterator i = _weapons.begin(); ; ++i)
	{
		if (i == _weapons.end())
		{
			_status = "STR_REFUELLING";
			break;
		}
		if (*i != 0 && (*i)->isRearming())
		{
			if (_base->getItems()->getItem((*i)->getRules()->getClipItem()) > 0)
			{
				(*i)->rearm();
				_base->getItems()->removeItem((*i)->getRules()->getClipItem());
				break;
			}
			else
			{
				ammo = (*i)->getRules()->getClipItem();
				(*i)->setRearming(false);
			}
		}
	}
	return ammo;
}

/**
 * Returns the craft's battlescape status.
 * @return Is the craft on the battlescape?
 */
bool Craft::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Changes the craft's battlescape status.
 * @param inbattle True if it's in battle, False otherwise.
 */
void Craft::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

}
