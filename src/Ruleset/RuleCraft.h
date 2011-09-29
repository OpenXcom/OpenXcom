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
#ifndef OPENXCOM_RULECRAFT_H
#define OPENXCOM_RULECRAFT_H

#include <vector>
#include <string>
#include "yaml.h"

namespace OpenXcom
{

class RuleTerrain;

/**
 * Represents a specific type of craft.
 * Contains constant info about a craft like
 * costs, speed, capacities, consumptions, etc.
 * @sa Craft
 */
class RuleCraft
{
private:
	std::string _type;
	int _sprite;
	int _fuelMax, _damageMax, _speedMax, _accel, _weapons, _soldiers, _hwps, _cost, _repair, _refuel, _range, _time, _score;
	// battlescape:
	RuleTerrain *_battlescapeTerrainData;
public:
	/// Creates a blank craft ruleset.
	RuleCraft(const std::string &type);
	/// Cleans up the craft ruleset.
	~RuleCraft();
	/// Gets the craft's type.
	std::string getType() const;
	/// Gets the craft's sprite.
	int getSprite() const;
	/// Sets the craft's sprite.
	void setSprite(int sprite);
	/// Gets the craft's maximum fuel.
	int getMaxFuel() const;
	/// Sets the craft's maximum fuel.
	void setMaxFuel(int fuel);
	/// Gets the craft's maximum damage.
	int getMaxDamage() const;
	/// Sets the craft's maximum damage.
	void setMaxDamage(int damage);
	/// Gets the craft's maximum speed.
	int getMaxSpeed() const;
	/// Sets the craft's maximum speed.
	void setMaxSpeed(int speed);
	/// Gets the craft's acceleration.
	int getAcceleration() const;
	/// Sets the craft's acceleration.
	void setAcceleration(int accel);
	/// Gets the craft's weapon capacity.
	int getWeapons() const;
	/// Sets the craft's weapon capacity.
	void setWeapons(int weapons);
	/// Gets the craft's soldier capacity.
	int getSoldiers() const;
	/// Sets the craft's soldier capacity.
	void setSoldiers(int soldiers);
	/// Gets the craft's HWP capacity.
	int getHWPs() const;
	/// Sets the craft's HWP capacity.
	void setHWPs(int hwps);
	/// Gets the craft's cost.
	int getCost() const;
	/// Sets the craft's cost.
	void setCost(int cost);
	/// Gets the craft's repair rate.
	int getRepairRate() const;
	/// Sets the craft's repair rate.
	void setRepairRate(int repair);
	/// Gets the craft's refuel rate.
	int getRefuelRate() const;
	/// Sets the craft's refuel rate.
	void setRefuelRate(int refuel);
	/// Gets the craft's radar range.
	int getRadarRange() const;
	/// Sets the craft's radar range.
	void setRadarRange(int range);
	/// Gets the craft's transfer time.
	int getTransferTime() const;
	/// Sets the craft's transfer time.
	void setTransferTime(int time);
	/// Gets the craft's score.
	int getScore() const;
	/// Sets the craft's score.
	void setScore(int score);
	/// Loads craft data from YAML.
	void load(const YAML::Node& node);
	/// Saves the craft data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the craft's terrain data.
	RuleTerrain *getBattlescapeTerrainData();
	/// Sets the craft's terrain data.
	void setBattlescapeTerrainData(RuleTerrain *t);
};

}

#endif
