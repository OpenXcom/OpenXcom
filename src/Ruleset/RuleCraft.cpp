/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "RuleCraft.h"
#include "RuleTerrain.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleCraft::RuleCraft(const std::string &type) : 
    _type(type), _sprite(-1), _fuelMax(0), _damageMax(0), _speedMax(0), _accel(0), 
    _weapons(0), _soldiers(0), _vehicles(0), _costBuy(0), _costRent(0), _costSell(0),
	_refuelItem(""), _repairRate(1), _refuelRate(1), _radarRange(600), _transferTime(0),
	_score(0), _battlescapeTerrainData(0), _spacecraft(false), _listOrder(0)
{

}

/**
 *
 */
RuleCraft::~RuleCraft()
{
	delete _battlescapeTerrainData;
}

/**
 * Loads the craft from a YAML file.
 * @param node YAML node.
 * @param ruleset Ruleset for the craft.
 * @param modIndex offsets the sounds and sprite values to avoid conflicts.
 * @param listOrder the list weight for this craft.
 */
void RuleCraft::load(const YAML::Node &node, Ruleset *ruleset, int modIndex, int listOrder)
{
	_type = node["type"].as<std::string>(_type);
	_requires = node["requires"].as< std::vector<std::string> >(_requires);
	_sprite = node["sprite"].as<int>(_sprite);
	// this is an offset in BASEBITS.PCK, and two in INTICONS.PCK
	if (_sprite > 4)
		_sprite += modIndex;
	_fuelMax = node["fuelMax"].as<int>(_fuelMax);
	_damageMax = node["damageMax"].as<int>(_damageMax);
	_speedMax = node["speedMax"].as<int>(_speedMax);
	_accel = node["accel"].as<int>(_accel);
	_weapons = node["weapons"].as<int>(_weapons);
	_soldiers = node["soldiers"].as<int>(_soldiers);
	_vehicles = node["vehicles"].as<int>(_vehicles);
	_costBuy = node["costBuy"].as<int>(_costBuy);
	_costRent = node["costRent"].as<int>(_costRent);
	_costSell = node["costSell"].as<int>(_costSell);
	_refuelItem = node["refuelItem"].as<std::string>(_refuelItem);
	_repairRate = node["repairRate"].as<int>(_repairRate);
	_refuelRate = node["refuelRate"].as<int>(_refuelRate);
	_radarRange = node["radarRange"].as<int>(_radarRange);
	_transferTime = node["transferTime"].as<int>(_transferTime);
	_score = node["score"].as<int>(_score);
	if (const YAML::Node &terrain = node["battlescapeTerrainData"])
	{
		RuleTerrain *rule = new RuleTerrain(terrain["name"].as<std::string>());
		rule->load(terrain, ruleset);
		_battlescapeTerrainData = rule;
	}
	_spacecraft = node["spacecraft"].as<bool>(_spacecraft);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Returns the language string that names
 * this craft. Each craft type has a unique name.
 * @return Craft name.
 */
std::string RuleCraft::getType() const
{
	return _type;
}

/**
 * Returns the list of research required to
 * acquire this craft.
 * @return List of research IDs.
 */
const std::vector<std::string> &RuleCraft::getRequirements() const
{
	return _requires;
}

/**
 * Returns the ID of the sprite used to draw the craft
 * in the Basescape and Equip Craft screens.
 * @return Sprite ID.
 */
int RuleCraft::getSprite() const
{
	return _sprite;
}

/**
 * Returns the maximum fuel the craft can contain.
 * @return Fuel amount.
 */
int RuleCraft::getMaxFuel() const
{
	return _fuelMax;
}

/**
 * Returns the maximum damage (damage the craft can take)
 * of the craft.
 * @return Damage.
 */
int RuleCraft::getMaxDamage() const
{
	return _damageMax;
}

/**
 * Returns the maximum speed of the craft flying
 * around the Geoscape.
 * @return Speed in knots.
 */
int RuleCraft::getMaxSpeed() const
{
	return _speedMax;
}

/**
 * Returns the acceleration of the craft for
 * taking off / stopping.
 * @return Acceleration.
 */
int RuleCraft::getAcceleration() const
{
	return _accel;
}

/**
 * Returns the maximum number of weapons that
 * can be equipped onto the craft.
 * @return Weapon capacity.
 */
int RuleCraft::getWeapons() const
{
	return _weapons;
}

/**
 * Returns the maximum number of soldiers that
 * the craft can carry.
 * @return Soldier capacity.
 */
int RuleCraft::getSoldiers() const
{
	return _soldiers;
}

/**
 * Returns the maximum number of vehicles that
 * the craft can carry.
 * @return vehicle capacity.
 */
int RuleCraft::getVehicles() const
{
	return _vehicles;
}

/**
 * Returns the cost of this craft for
 * purchase/rent (0 if not purchasable).
 * @return Cost.
 */
int RuleCraft::getBuyCost() const
{
	return _costBuy;
}

/**
 * Returns the cost of rent for a month.
 * @return Cost.
 */
int RuleCraft::getRentCost() const
{
	return _costRent;
}

/**
 * Returns sell value of this craft
 * Rented craft should use 0.
 * @return Cost.
 */
int RuleCraft::getSellCost() const
{
	return _costSell;
}

/**
 * Returns what item is required while
 * the craft is refuelling.
 * @return Item ID or "" if none.
 */
std::string RuleCraft::getRefuelItem() const
{
	return _refuelItem;
}

/**
 * Returns how much damage is removed from the
 * craft while repairing.
 * @return Amount of damage.
 */
int RuleCraft::getRepairRate() const
{
	return _repairRate;
}

/**
 * Returns how much fuel is added to the
 * craft while refuelling.
 * @return Amount of fuel.
 */
int RuleCraft::getRefuelRate() const
{
	return _refuelRate;
}

/**
 * Returns the craft's radar range
 * for detecting UFOs.
 * @return Range in nautical miles.
 */
int RuleCraft::getRadarRange() const
{
	return _radarRange;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleCraft::getTransferTime() const
{
	return _transferTime;
}

/**
 * Returns the amount of score you lose
 * when this craft is destroyed.
 * @return Score in points.
 */
int RuleCraft::getScore() const
{
	return _score;
}

/**
 * Returns the terrain data needed to draw the Craft in the battlescape.
 * @return Terrain.
 */
RuleTerrain *RuleCraft::getBattlescapeTerrainData()
{
	return _battlescapeTerrainData;
}

/**
 * @return if this ship is capable of going to mars.
 */
bool RuleCraft::getSpacecraft() const
{
	return _spacecraft;
}

/**
 * @return the list weight for this research item.
 */
int RuleCraft::getListOrder() const
{
	 return _listOrder;
}
}

