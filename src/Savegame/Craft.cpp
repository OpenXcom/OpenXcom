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
#define _USE_MATH_DEFINES
#include "Craft.h"
#include <cmath>
#include <sstream>
#include "../Engine/Language.h"
#include "../Ruleset/RuleCraft.h"
#include "CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/SavedGame.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "Base.h"
#include "Ufo.h"
#include "Waypoint.h"
#include "TerrorSite.h"
#include "AlienBase.h"
#include "Vehicle.h"
#include "ScriptedEventLocation.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param base Pointer to base of origin.
 * @param id ID to assign to the craft (0 to not assign).
 */
Craft::Craft(RuleCraft *rules, Base *base, int id) : MovingTarget(), _rules(rules), _base(base), _id(0), _fuel(0), _damage(0), _interceptionOrder(0), _takeoff(0), _weapons(), _status("STR_READY"), _lowFuel(false), _mission(false), _inBattlescape(false), _inDogfight(false), _name(L"")
{
	_items = new ItemContainer();
	if (id != 0)
	{
		_id = id;
	}
	for (unsigned int i = 0; i < _rules->getWeapons(); ++i)
	{
		_weapons.push_back(0);
	}
	setBase(base);
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
	for (std::vector<Vehicle*>::iterator i = _vehicles.begin(); i != _vehicles.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the craft from a YAML file.
 * @param node YAML node.
 * @param rule Ruleset for the saved game.
 * @param save Pointer to the saved game.
 */
void Craft::load(const YAML::Node &node, const Ruleset *rule, SavedGame *save)
{
	MovingTarget::load(node);
	_id = node["id"].as<int>(_id);
	_fuel = node["fuel"].as<int>(_fuel);
	_damage = node["damage"].as<int>(_damage);

	size_t j = 0;
	for (YAML::const_iterator i = node["weapons"].begin(); i != node["weapons"].end(); ++i)
	{
		if (_rules->getWeapons() > j)
		{
			std::string type = (*i)["type"].as<std::string>();
			if (type != "0" && rule->getCraftWeapon(type))
			{
				CraftWeapon *w = new CraftWeapon(rule->getCraftWeapon(type), 0);
				w->load(*i);
				_weapons[j] = w;
			}
			else
			{
				_weapons[j] = 0;
			}
			j++;
		}
	}

	_items->load(node["items"]);
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end();)
	{
		if (std::find(rule->getItemsList().begin(), rule->getItemsList().end(), i->first) == rule->getItemsList().end())
		{
			_items->getContents()->erase(i++);
		}
		else
		{
			++i;
		}
	}
	for (YAML::const_iterator i = node["vehicles"].begin(); i != node["vehicles"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (rule->getItem(type))
		{
			Vehicle *v = new Vehicle(rule->getItem(type), 0, 4);
			v->load(*i);
			_vehicles.push_back(v);
		}
	}
	_status = node["status"].as<std::string>(_status);
	_lowFuel = node["lowFuel"].as<bool>(_lowFuel);
	_mission = node["mission"].as<bool>(_mission);
	_interceptionOrder = node["interceptionOrder"].as<int>(_interceptionOrder);
	if (const YAML::Node name = node["name"])
	{
		_name = Language::utf8ToWstr(name.as<std::string>());
	}
	if (const YAML::Node &dest = node["dest"])
	{
		std::string type = dest["type"].as<std::string>();
		int id = dest["id"].as<int>();
		if (type == "STR_BASE")
		{
			returnToBase();
		}
		else if (type == "STR_UFO")
		{
			for (std::vector<Ufo*>::iterator i = save->getUfos()->begin(); i != save->getUfos()->end(); ++i)
			{
				if ((*i)->getId() == id)
				{
					setDestination(*i);
					break;
				}
			}
		}
		else if (type == "STR_WAYPOINT")
		{
			for (std::vector<Waypoint*>::iterator i = save->getWaypoints()->begin(); i != save->getWaypoints()->end(); ++i)
			{
				if ((*i)->getId() == id)
				{
					setDestination(*i);
					break;
				}
			}
		}
		else if (type == "STR_TERROR_SITE")
		{
			for (std::vector<TerrorSite*>::iterator i = save->getTerrorSites()->begin(); i != save->getTerrorSites()->end(); ++i)
			{
				if ((*i)->getId() == id)
				{
					setDestination(*i);
					break;
				}
			}
		}
		else if (type == "STR_ALIEN_BASE")
		{
			for (std::vector<AlienBase*>::iterator i = save->getAlienBases()->begin(); i != save->getAlienBases()->end(); ++i)
			{
				if ((*i)->getId() == id)
				{
					setDestination(*i);
					break;
				}
			}
		}
		else if (type == "STR_SCRIPTED_EVENT_LOCATION")
		{
			for (std::vector<ScriptedEventLocation*>::iterator i = save->getScriptedEventLocations()->begin(); i != save->getScriptedEventLocations()->end(); ++i)
			{
				if ((*i)->getId() == id)
				{
					setDestination(*i);
					break;
				}
			}
		}
	}
	_takeoff = node["takeoff"].as<int>(_takeoff);
	_inBattlescape = node["inBattlescape"].as<bool>(_inBattlescape);
	if (_inBattlescape)
		setSpeed(0);
}

/**
 * Saves the craft to a YAML file.
 * @return YAML node.
 */
YAML::Node Craft::save() const
{
	YAML::Node node = MovingTarget::save();
	node["type"] = _rules->getType();
	node["id"] = _id;
	node["fuel"] = _fuel;
	node["damage"] = _damage;
	for (std::vector<CraftWeapon*>::const_iterator i = _weapons.begin(); i != _weapons.end(); ++i)
	{
		YAML::Node subnode;
		if (*i != 0)
		{
			subnode = (*i)->save();
		}
		else
		{
			subnode["type"] = "0";
		}
		node["weapons"].push_back(subnode);
	}
	node["items"] = _items->save();
	for (std::vector<Vehicle*>::const_iterator i = _vehicles.begin(); i != _vehicles.end(); ++i)
	{
		node["vehicles"].push_back((*i)->save());
	}
	node["status"] = _status;
	if (_lowFuel)
		node["lowFuel"] = _lowFuel;
	if (_mission)
		node["mission"] = _mission;
	if (_inBattlescape)
		node["inBattlescape"] = _inBattlescape;
	if (_interceptionOrder != 0)
		node["interceptionOrder"] = _interceptionOrder;
	if (_takeoff != 0)
		node["takeoff"] = _takeoff;
	if (!_name.empty())
		node["name"] = Language::wstrToUtf8(_name);
	return node;
}

/**
 * Saves the craft's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node Craft::saveId() const
{
	YAML::Node node = MovingTarget::saveId();
	node["type"] = _rules->getType();
	node["id"] = _id;
	return node;
}

/**
 * Returns the ruleset for the craft's type.
 * @return Pointer to ruleset.
 */
RuleCraft *Craft::getRules() const
{
	return _rules;
}

/**
 * Changes the ruleset for the craft's type.
 * @param rules Pointer to ruleset.
 * @warning ONLY FOR NEW BATTLE USE!
 */
void Craft::changeRules(RuleCraft *rules)
{
	_rules = rules;
	_weapons.clear();
	for (unsigned int i = 0; i < _rules->getWeapons(); ++i)
	{
		_weapons.push_back(0);
	}
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
 * If there's no custom name, the language default is used.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Craft::getName(Language *lang) const
{
	if (_name.empty())
		return lang->getString("STR_CRAFTNAME").arg(lang->getString(_rules->getType())).arg(_id);
	return _name;
}

/**
 * Changes the craft's custom name.
 * @param newName New custom name. If set to blank, the language default is used.
 */
void Craft::setName(const std::wstring &newName)
{
	_name = newName;
}

/**
 * Returns the globe marker for the craft.
 * @return Marker sprite, -1 if none.
 */
int Craft::getMarker() const
{
	if (_status != "STR_OUT")
		return -1;
	return 1;
}

/**
 * Returns the base the craft belongs to.
 * @return Pointer to base.
 */
Base *Craft::getBase() const
{
	return _base;
}

/**
 * Changes the base the craft belongs to.
 * @param base Pointer to base.
 * @param move Move the craft to the base coordinates.
 */
void Craft::setBase(Base *base, bool move)
{
	_base = base;
	if (move)
	{
		_lon = base->getLongitude();
		_lat = base->getLatitude();
	}
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
void Craft::setStatus(const std::string &status)
{
	_status = status;
}

/**
 * Returns the current altitude of the craft.
 * @return Altitude.
 */
std::string Craft::getAltitude() const
{
	Ufo *u = dynamic_cast<Ufo*>(_dest);
	if (u && u->getAltitude() != "STR_GROUND")
	{
		return u->getAltitude();
	}
	else
	{
		return "STR_VERY_LOW";
	}
}


/**
 * Changes the destination the craft is heading to.
 * @param dest Pointer to new destination.
 */
void Craft::setDestination(Target *dest)
{
	if (_status != "STR_OUT")
	{
		_takeoff = 60;
	}
	if (dest == 0)
		setSpeed(_rules->getMaxSpeed()/2);
	else
		setSpeed(_rules->getMaxSpeed());
	MovingTarget::setDestination(dest);
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
 * Returns the amount of vehicles currently
 * contained in this craft.
 * @return Number of vehicles.
 */
int Craft::getNumVehicles() const
{
	return _vehicles.size();
}

/**
 * Returns the list of weapons currently equipped
 * in the craft.
 * @return Pointer to weapon list.
 */
std::vector<CraftWeapon*> *Craft::getWeapons()
{
	return &_weapons;
}

/**
 * Returns the list of items in the craft.
 * @return Pointer to the item list.
 */
ItemContainer *Craft::getItems()
{
	return _items;
}

/**
 * Returns the list of vehicles currently equipped
 * in the craft.
 * @return Pointer to vehicle list.
 */
std::vector<Vehicle*> *Craft::getVehicles()
{
	return &_vehicles;
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
 * Returns whether the craft has just done a ground mission,
 * and is forced to return to base.
 * @return True if it's returning, false otherwise.
 */
bool Craft::getMissionComplete() const
{
	return _mission;
}

/**
 * Changes whether the craft has just done a ground mission,
 * and is forced to return to base.
 * @param mission True if it's returning, false otherwise.
 */
void Craft::setMissionComplete(bool mission)
{
	_mission = mission;
}

/**
 * Returns the current distance between the craft
 * and the base it belongs to.
 * @return Distance in radian.
 */
double Craft::getDistanceFromBase() const
{
	return getDistance(_base);
}

/**
 * Returns the amount of fuel the craft uses up
 * while it's on the air, based on its speed.
 * @return Fuel amount.
 */
int Craft::getFuelConsumption() const
{
	if (_rules->getRefuelItem() != "")
		return 1;
	return (int)floor(_speed / 100.0);
}

/**
 * Returns the minimum required fuel for the
 * craft to make it back to base.
 * @return Fuel amount.
 */
int Craft::getFuelLimit() const
{
	return getFuelLimit(_base);
}

/**
 * Returns the minimum required fuel for the
 * craft to go to a base.
 * @param base Pointer to target base.
 * @return Fuel amount.
 */
int Craft::getFuelLimit(Base *base) const
{
	return (int)floor(getFuelConsumption() * getDistance(base) / (_speedRadian * 120));
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
	if (_takeoff == 0)
	{
		move();
	}
	else
	{
		_takeoff--;
	}
	if (reachedDestination() && _dest == (Target*)_base)
	{
		setInterceptionOrder(0);
		checkup();
		setDestination(0);
		setSpeed(0);
		_lowFuel = false;
		_mission = false;
		_takeoff = 0;
	}
}

/**
 * Checks the condition of all the craft's systems
 * to define its new status (eg. when arriving at base).
 */
void Craft::checkup()
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
}

/**
 * Returns if a certain target is detected by the craft's
 * radar, taking in account the range and chance.
 * @param target Pointer to target to compare.
 * @return True if it's detected, False otherwise.
 */
bool Craft::detect(Target *target) const
{
	if (_rules->getRadarRange() == 0)
		return false;
	double range = _rules->getRadarRange() * (1 / 60.0) * (M_PI / 180);
	return (getDistance(target) <= range);
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
		for (std::vector<CraftWeapon*>::iterator i = _weapons.begin(); i != _weapons.end(); ++i)
		{
			if (*i && (*i)->isRearming())
			{
				_status = "STR_REARMING";
				break;
			}
		}
	}
}

/**
 * Rearms the craft's weapons by adding ammo every hour
 * while it's docked in the base.
 * @param rules Pointer to ruleset.
 * @return The ammo ID missing for rearming, or "" if none.
 */
std::string Craft::rearm(Ruleset *rules)
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
			std::string clip = (*i)->getRules()->getClipItem();
			int available = _base->getItems()->getItem(clip);
			if (clip == "")
			{
				(*i)->rearm(0, 0);
			}
			else if (available > 0)
			{
				int used = (*i)->rearm(available, rules->getItem(clip)->getClipSize());

				if (used == available && (*i)->isRearming())
				{
					ammo = clip;
					(*i)->setRearming(false);
				}

				_base->getItems()->removeItem(clip, used);
			}
			else
			{
				ammo = clip;
				(*i)->setRearming(false);
			}
			break;
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
	if (inbattle)
		setSpeed(0);
	_inBattlescape = inbattle;
}

/// Returns the craft destroyed status.
/**
 * If the amount of damage the craft take
 * is more than it's health it will be
 * destroyed.
 * @return Is the craft destroyed?
 */
bool Craft::isDestroyed() const
{
	return (_damage >= _rules->getMaxDamage());
}

/**
 * Returns the amount of space available for
 * soldiers and vehicles.
 * @return Space available.
 */
int Craft::getSpaceAvailable() const
{
	return _rules->getSoldiers() - getSpaceUsed();
}

/**
 * Returns the amount of space in use by
 * soldiers and vehicles.
 * @return Space used.
 */
int Craft::getSpaceUsed() const
{
	int vehicleSpaceUsed = 0;
	for (std::vector<Vehicle*>::const_iterator i = _vehicles.begin(); i != _vehicles.end(); ++i)
	{
		vehicleSpaceUsed += (*i)->getSize();
	}
	return getNumSoldiers() + vehicleSpaceUsed;
}

/**
 * Returns the total amount of vehicles of
 * a certain type stored in the craft.
 * @param vehicle Vehicle type.
 * @return Number of vehicles.
 */
int Craft::getVehicleCount(const std::string &vehicle) const
{
	int total = 0;
	for (std::vector<Vehicle*>::const_iterator i = _vehicles.begin(); i != _vehicles.end(); ++i)
	{
		if ((*i)->getRules()->getType() == vehicle)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the craft's dogfight status.
 * @return Is the craft ion a dogfight?
 */
bool Craft::isInDogfight() const
{
	return _inDogfight;
}

/**
 * Changes the craft's dogfight status.
 * @param inDogfight True if it's in dogfight, False otherwise.
 */
void Craft::setInDogfight(bool inDogfight)
{
	_inDogfight = inDogfight;
}

/**
 * Sets interception order (first craft to leave the base gets 1, second 2, etc.).
 * @param order Interception order.
 */
void Craft::setInterceptionOrder(const int order)
{
	_interceptionOrder = order;
}

/**
 * Gets interception order.
 * @return Interception order.
 */
int Craft::getInterceptionOrder() const
{
	return _interceptionOrder;
}

}
