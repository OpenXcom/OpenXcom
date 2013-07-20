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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "sprite")
		{
			i.second() >> _sprite;
			// this is an offset in BASEBITS.PCK, and two in INTICONS.PCK
			if (_sprite > 4)
				_sprite += modIndex;
		}
		else if (key == "fuelMax")
		{
			i.second() >> _fuelMax;
		}
		else if (key == "damageMax")
		{
			i.second() >> _damageMax;
		}
		else if (key == "speedMax")
		{
			i.second() >> _speedMax;
		}
		else if (key == "accel")
		{
			i.second() >> _accel;
		}
		else if (key == "weapons")
		{
			i.second() >> _weapons;
		}
		else if (key == "soldiers")
		{
			i.second() >> _soldiers;
		}
		else if (key == "vehicles")
		{
			i.second() >> _vehicles;
		}
		else if (key == "costBuy")
		{
			i.second() >> _costBuy;
		}
		else if (key == "costRent")
		{
			i.second() >> _costRent;
		}
		else if (key == "costSell")
		{
			i.second() >> _costSell;
		}
		else if (key == "refuelItem")
		{
			i.second() >> _refuelItem;
		}
		else if (key == "repairRate")
		{
			i.second() >> _repairRate;
		}
		else if (key == "refuelRate")
		{
			i.second() >> _refuelRate;
		}
		else if (key == "radarRange")
		{
			i.second() >> _radarRange;
		}
		else if (key == "transferTime")
		{
			i.second() >> _transferTime;
		}
		else if (key == "score")
		{
			i.second() >> _score;
		}
		else if (key == "battlescapeTerrainData")
		{
			std::string name;
			i.second()["name"] >> name;
			RuleTerrain *rule = new RuleTerrain(name);
			rule->load(i.second(), ruleset);
			_battlescapeTerrainData = rule;
		}
		else if (key == "spacecraft")
		{
			i.second() >> _spacecraft;
		}
		else if (key == "listOrder")
		{
			i.second() >> _listOrder;
		}
	}
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Saves the craft to a YAML file.
 * @param out YAML emitter.
 */
void RuleCraft::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "sprite" << YAML::Value << _sprite;
	out << YAML::Key << "fuelMax" << YAML::Value << _fuelMax;
	out << YAML::Key << "damageMax" << YAML::Value << _damageMax;
	out << YAML::Key << "speedMax" << YAML::Value << _speedMax;
	out << YAML::Key << "accel" << YAML::Value << _accel;
	out << YAML::Key << "weapons" << YAML::Value << _weapons;
	out << YAML::Key << "soldiers" << YAML::Value << _soldiers;
	out << YAML::Key << "vehicles" << YAML::Value << _vehicles;
	out << YAML::Key << "costBuy" << YAML::Value << _costBuy;
	out << YAML::Key << "costRent" << YAML::Value << _costRent;
	out << YAML::Key << "costSell" << YAML::Value << _costSell;
	out << YAML::Key << "refuelItem" << YAML::Value << _refuelItem;
	out << YAML::Key << "repairRate" << YAML::Value << _repairRate;
	out << YAML::Key << "refuelRate" << YAML::Value << _refuelRate;
	out << YAML::Key << "radarRange" << YAML::Value << _radarRange;
	out << YAML::Key << "transferTime" << YAML::Value << _transferTime;
	out << YAML::Key << "score" << YAML::Value << _score;
	out << YAML::Key << "spacecraft" << YAML::Value << _spacecraft;
	if (_battlescapeTerrainData != 0)
	{
		out << YAML::Key << "battlescapeTerrainData" << YAML::Value;
		_battlescapeTerrainData->save(out);
	}
	out << YAML::EndMap;
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

