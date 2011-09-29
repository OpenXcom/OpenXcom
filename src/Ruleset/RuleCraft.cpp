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
#include "RuleCraft.h"
#include "RuleTerrain.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleCraft::RuleCraft(const std::string &type) : _type(type), _sprite(-1), _fuelMax(0), _damageMax(0), _speedMax(0), _accel(0), _weapons(0), _soldiers(0), _hwps(0), _cost(0), _repair(1), _refuel(1), _range(600), _time(0), _score(0), _battlescapeTerrainData(0)
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
 * Changes the ID of the sprite used to draw the craft
 * in the Basescape and Equip Craft screens.
 * @param sprite Sprite ID.
 */
void RuleCraft::setSprite(int sprite)
{
	_sprite = sprite;
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
 * Changes the maximum fuel the craft can contain.
 * @param fuel Fuel amount.
 */
void RuleCraft::setMaxFuel(int fuel)
{
	_fuelMax = fuel;
}

/**
 * Returns the maximum damage (damage the craft can take) const
 * of the craft.
 * @return Damage.
 */
int RuleCraft::getMaxDamage() const
{
	return _damageMax;
}

/**
 * Changes the maximum damage (damage the craft can take)
 * of the craft.
 * @param damage Damage.
 */
void RuleCraft::setMaxDamage(int damage)
{
	_damageMax = damage;
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
 * Changes the maximum speed of the craft flying
 * around the Geoscape.
 * @param speed Speed in knots.
 */
void RuleCraft::setMaxSpeed(int speed)
{
	_speedMax = speed;
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
 * Changes the acceleration of the craft for
 * taking off / stopping.
 * @param accel Acceleration.
 */
void RuleCraft::setAcceleration(int accel)
{
	_accel = accel;
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
 * Changes the maximum number of weapons that
 * can be equipped onto the craft.
 * @param weapons Weapon capacity.
 */
void RuleCraft::setWeapons(int weapons)
{
	_weapons = weapons;
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
 * Changes the maximum number of soldiers that
 * the craft can carry.
 * @param soldiers Soldier capacity.
 */
void RuleCraft::setSoldiers(int soldiers)
{
	_soldiers = soldiers;
}

/**
 * Returns the maximum number of HWPs that
 * the craft can carry.
 * @return HWP capacity.
 */
int RuleCraft::getHWPs() const
{
	return _hwps;
}

/**
 * Changes the maximum number of HWPs that
 * the craft can carry.
 * @param hwps HWP capacity.
 */
void RuleCraft::setHWPs(int hwps)
{
	_hwps = hwps;
}

/**
 * Returns the cost of this craft for
 * purchase/maintenance.
 * @return Cost.
 */
int RuleCraft::getCost() const
{
	return _cost;
}

/**
 * Changes the cost of this craft for
 * purchase/maintenance.
 * @param cost Cost.
 */
void RuleCraft::setCost(int cost)
{
	_cost = cost;
}

/**
 * Returns how much damage is removed from the
 * craft while repairing.
 * @return Amount of damage.
 */
int RuleCraft::getRepairRate() const
{
	return _repair;
}

/**
 * Changes how much damage is removed from the
 * craft while repairing.
 * @param repair Amount of damage.
 */
void RuleCraft::setRepairRate(int repair)
{
	_repair = repair;
}

/**
 * Returns how much fuel is added to the
 * craft while refuelling.
 * @return Amount of fuel.
 */
int RuleCraft::getRefuelRate() const
{
	return _refuel;
}

/**
 * Changes how much fuel is added to the
 * craft while refuelling.
 * @param refuel Amount of fuel.
 */
void RuleCraft::setRefuelRate(int refuel)
{
	_refuel = refuel;
}

/**
 * Returns the craft's radar range
 * for detecting UFOs.
 * @return Range in nautical miles.
 */
int RuleCraft::getRadarRange() const
{
	return _range;
}

/**
 * Changes the craft's radar range
 * for detecting UFOs.
 * @param range Range in nautical miles.
 */
void RuleCraft::setRadarRange(int range)
{
	_range = range;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleCraft::getTransferTime() const
{
	return _time;
}

/**
 * Changes the amount of time this item
 * takes to arrive at a base.
 * @param time Time in hours.
 */
void RuleCraft::setTransferTime(int time)
{
	_time = time;
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
 * Changes the amount of score you lose
 * when this craft is destroyed.
 * @param score Score in points.
 */
void RuleCraft::setScore(int score)
{
	_score = score;
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
 * Changes the terrain data needed to draw the Craft in the battlescape.
 * @param terrain pointer to a RuleTerrain.
 */
void RuleCraft::setBattlescapeTerrainData(RuleTerrain *terrain)
{
	_battlescapeTerrainData = terrain;
}

<<<<<<< HEAD
=======
/**
 * Loads the craft from a YAML file.
 * @param node YAML node.
 */
void RuleCraft::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		if (key == "sprite")
		{
			i.second() >> _sprite;
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
		else if (key == "hwps")
		{
			i.second() >> _hwps;
		}
		else if (key == "cost")
		{
			i.second() >> _cost;
		}
		else if (key == "repair")
		{
			i.second() >> _repair;
		}
		else if (key == "refuel")
		{
			i.second() >> _refuel;
		}
		else if (key == "time")
		{
			i.second() >> _time;
		}
		else if (key == "score")
		{
			i.second() >> _score;
		}
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
	out << YAML::Key << "hwps" << YAML::Value << _hwps;
	out << YAML::Key << "cost" << YAML::Value << _cost;
	out << YAML::Key << "repair" << YAML::Value << _repair;
	out << YAML::Key << "refuel" << YAML::Value << _refuel;
	out << YAML::Key << "range" << YAML::Value << _range;
	out << YAML::Key << "time" << YAML::Value << _time;
	out << YAML::Key << "score" << YAML::Value << _score;
	out << YAML::EndMap;
}


>>>>>>> d153778... Adding the YAML save part to the RuleCraft rule set.
}
