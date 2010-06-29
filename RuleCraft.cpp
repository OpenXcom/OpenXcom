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
#include "RuleCraft.h"

/**
 * Creates a blank ruleset for a certain
 * type of craft.
 * @param type String defining the type.
 */
RuleCraft::RuleCraft(LangString type) : _type(type), _sprite(0), _fuelMax(0), _healthMax(0), _speed(0), _accel(0), _weapons(0), _soldiers(0), _hwps(0), _monthlyFee(0)
{
}

/**
 *
 */
RuleCraft::~RuleCraft()
{
}

/**
 * Returns the language string that names
 * this craft. Each craft type has a unique name.
 * @return Craft name.
 */
LangString RuleCraft::getType()
{
	return _type;
}

/**
 * Returns the ID of the sprite used to draw the craft
 * in the Basescape and Equip Craft screens.
 * @return Sprite ID.
 */
int RuleCraft::getSprite()
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
int RuleCraft::getMaxFuel()
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
 * Returns the maximum health (damage the craft can take)
 * of the craft.
 * @return Health.
 */
int RuleCraft::getMaxHealth()
{
	return _healthMax;
}

/**
 * Changes the maximum health (damage the craft can take)
 * of the craft.
 * @param health Health.
 */
void RuleCraft::setMaxHealth(int health)
{
	_healthMax = health;
}

/**
 * Returns the maximum speed of the craft flying
 * around the Geoscape.
 * @return Speed.
 */
int RuleCraft::getSpeed()
{
	return _speed;
}

/**
 * Changes the maximum speed of the craft flying
 * around the Geoscape.
 * @param speed Speed.
 */
void RuleCraft::setSpeed(int speed)
{
	_speed = speed;
}

/**
 * Returns the acceleration of the craft for
 * taking off / stopping.
 * @return Acceleration.
 */
int RuleCraft::getAcceleration()
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
int RuleCraft::getWeapons()
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
int RuleCraft::getSoldiers()
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
int RuleCraft::getHWPs()
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
int RuleCraft::getMonthlyFee()
{
	return _monthlyFee;
}

/**
 * Changes the monthly rental fee that needs to be
 * paid as long as this craft is owned.
 * @param fee Monthly fee.
 */
void RuleCraft::setMonthlyFee(int fee)
{
	_monthlyFee = fee;
}