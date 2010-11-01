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
#include "Terrain.h"

/**
 * Creates a blank ruleset for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleCraft::RuleCraft(LangString type) : _type(type), _sprite(-1), _fuelMax(0), _damageMax(0), _speedMax(0), _accel(0), _weapons(0), _soldiers(0), _hwps(0), _fee(0), _repair(1), _refuel(1)
{
	_battlescapeTerrainData = NULL;
}

/**
 *
 */
RuleCraft::~RuleCraft()
{
	if (_battlescapeTerrainData != NULL)
		delete _battlescapeTerrainData;
}

/**
 * Returns the language string that names
 * this craft. Each craft type has a unique name.
 * @return Craft name.
 */
LangString RuleCraft::getType() const
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
 * Returns the monthly rental fee that needs to be
 * paid as long as this craft is owned (0 = no rental).
 * @return Monthly fee.
 */
int RuleCraft::getMonthlyFee() const
{
	return _fee;
}

/**
 * Changes the monthly rental fee that needs to be
 * paid as long as this craft is owned.
 * @param fee Monthly fee.
 */
void RuleCraft::setMonthlyFee(int fee)
{
	_fee = fee;
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
 * Returns the terrain data needed to draw the Craft in the battlescape.
 * @return Terrain.
 */
Terrain* RuleCraft::getBattlescapeTerrainData()
{
	return _battlescapeTerrainData;
}

/**
 * Changes the terrain data needed to draw the Craft in the battlescape.
 * @param t Terrain.
 */
void RuleCraft::setBattlescapeTerrainData(Terrain *t)
{
	_battlescapeTerrainData = t;
}