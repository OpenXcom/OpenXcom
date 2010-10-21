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
#ifndef OPENXCOM_RULECRAFT_H
#define OPENXCOM_RULECRAFT_H

#include "../Resource/LangString.h"

/**
 * Represents a specific type of craft.
 * Contains constant info about a craft like
 * costs, speed, capacities, consumptions, etc.
 * @sa Craft
 */
class RuleCraft
{
private:
	LangString _type;
	int _sprite;
	int _fuelMax, _damageMax, _speedMax, _accel, _weapons, _soldiers, _hwps, _fee, _repair, _refuel;
public:
	/// Creates a blank craft ruleset.
	RuleCraft(LangString type);
	/// Cleans up the craft ruleset.
	~RuleCraft();
	/// Gets the craft's type.
	LangString getType();
	/// Gets the craft's sprite.
	int getSprite();
	/// Sets the craft's sprite.
	void setSprite(int sprite);
	/// Gets the craft's maximum fuel.
	int getMaxFuel();
	/// Sets the craft's maximum fuel.
	void setMaxFuel(int fuel);
	/// Gets the craft's maximum damage.
	int getMaxDamage();
	/// Sets the craft's maximum damage.
	void setMaxDamage(int damage);
	/// Gets the craft's maximum speed.
	int getMaxSpeed();
	/// Sets the craft's maximum speed.
	void setMaxSpeed(int speed);
	/// Gets the craft's acceleration.
	int getAcceleration();
	/// Sets the craft's acceleration.
	void setAcceleration(int accel);
	/// Gets the craft's weapon capacity.
	int getWeapons();
	/// Sets the craft's weapon capacity.
	void setWeapons(int weapons);
	/// Gets the craft's soldier capacity.
	int getSoldiers();
	/// Sets the craft's soldier capacity.
	void setSoldiers(int soldiers);
	/// Gets the craft's HWP capacity.
	int getHWPs();
	/// Sets the craft's HWP capacity.
	void setHWPs(int hwps);
	/// Gets the craft's monthly fee.
	int getMonthlyFee();
	/// Sets the craft's monthly fee.
	void setMonthlyFee(int fee);
	/// Gets the craft's repair rate.
	int getRepairRate();
	/// Sets the craft's repair rate.
	void setRepairRate(int repair);
	/// Gets the craft's refuel rate.
	int getRefuelRate();
	/// Sets the craft's refuel rate.
	void setRefuelRate(int refuel);
};

#endif
