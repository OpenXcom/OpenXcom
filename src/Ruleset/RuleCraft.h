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
#ifndef OPENXCOM_RULECRAFT_H
#define OPENXCOM_RULECRAFT_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;

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
	std::vector<std::string> _requires;
	int _sprite;
	int _fuelMax, _damageMax, _speedMax, _accel, _weapons, _soldiers, _vehicles, _costBuy, _costRent, _costSell;
	std::string _refuelItem;
	int _repairRate, _refuelRate, _radarRange, _radarChance, _sightRange, _transferTime, _score;
	RuleTerrain *_battlescapeTerrainData;
	bool _spacecraft;
	int _listOrder, _maxItems;
	std::vector<std::vector <int> > _deployment;
public:
	/// Creates a blank craft ruleset.
	RuleCraft(const std::string &type);
	/// Cleans up the craft ruleset.
	~RuleCraft();
	/// Loads craft data from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset, int modIndex, int nextCraftIndex);
	/// Gets the craft's type.
	std::string getType() const;
	/// Gets the craft's requirements.
	const std::vector<std::string> &getRequirements () const;
	/// Gets the craft's sprite.
	int getSprite() const;
	/// Gets the craft's maximum fuel.
	int getMaxFuel() const;
	/// Gets the craft's maximum damage.
	int getMaxDamage() const;
	/// Gets the craft's maximum speed.
	int getMaxSpeed() const;
	/// Gets the craft's acceleration.
	int getAcceleration() const;
	/// Gets the craft's weapon capacity.
	unsigned int getWeapons() const;
	/// Gets the craft's soldier capacity.
	int getSoldiers() const;
	/// Gets the craft's vehicle capacity.
	int getVehicles() const;
	/// Gets the craft's cost.
	int getBuyCost() const;
	/// Gets the craft's rent for a month.
	int getRentCost() const;
	/// Gets the craft's value.
	int getSellCost() const;
	/// Gets the craft's refuel item.
	std::string getRefuelItem() const;
	/// Gets the craft's repair rate.
	int getRepairRate() const;
	/// Gets the craft's refuel rate.
	int getRefuelRate() const;
	/// Gets the craft's radar range.
	int getRadarRange() const;
	/// Gets the craft's radar chance.
	inline int getRadarChance() const {return _radarChance;}
	/// Gets the craft's sight range.
	int getSightRange() const;
	/// Gets the craft's transfer time.
	int getTransferTime() const;
	/// Gets the craft's score.
	int getScore() const;
	/// Gets the craft's terrain data.
	RuleTerrain *getBattlescapeTerrainData();
	/// Checks if this craft is capable of travelling to mars.
	bool getSpacecraft() const;
	/// Gets the list weight for this craft.
	int getListOrder() const;
	/// Gets the deployment priority for the craft.
	std::vector<std::vector<int> > &getDeployment();
	const int getMaxItems() const;
};

}

#endif
