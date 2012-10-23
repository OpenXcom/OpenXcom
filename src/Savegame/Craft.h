/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_CRAFT_H
#define OPENXCOM_CRAFT_H

#include "MovingTarget.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class RuleCraft;
class Base;
class Soldier;
class CraftWeapon;
class ItemContainer;
class Ruleset;
class SavedGame;
class Vehicle;

/**
 * Represents a craft stored in a base.
 * Contains variable info about a craft like
 * position, fuel, damage, etc.
 * @sa RuleCraft
 */
class Craft : public MovingTarget
{
private:
	RuleCraft *_rules;
	Base *_base;
	int _id, _fuel, _damage, _interceptionOrder;
	std::vector<CraftWeapon*> _weapons;
	ItemContainer *_items;
	std::vector<Vehicle*> _vehicles;
	std::string _status;
	bool _lowFuel;
	bool _inBattlescape;
	bool _inDogfight;
public:
	/// Creates a craft of the specified type.
	Craft(RuleCraft *rules, Base *base, int id = 0);
	/// Cleans up the craft.
	~Craft();
	/// Loads the craft from YAML.
	void load(const YAML::Node& node, const Ruleset *rule, SavedGame *save);
	/// Saves the craft to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the craft's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the craft's ruleset.
	RuleCraft *const getRules() const;
	/// Sets the craft's ruleset.
	void setRules(RuleCraft *rules);
	/// Gets the craft's ID.
	int getId() const;
	/// Gets the craft's name.
	std::wstring getName(Language *lang) const;
	/// Gets the craft's base.
	Base *const getBase() const;
	/// Sets the craft's base.
	void setBase(Base *base);
	/// Gets the craft's status.
	std::string getStatus() const;
	/// Sets the craft's status.
	void setStatus(const std::string &status);
	/// Gets the craft's altitude.
	std::string getAltitude() const;
	/// Sets the craft's destination.
	void setDestination(Target *dest);
	/// Gets the craft's amount of weapons.
	int getNumWeapons() const;
	/// Gets the craft's amount of soldiers.
	int getNumSoldiers() const;
	/// Gets the craft's amount of equipment.
	int getNumEquipment() const;
	/// Gets the craft's amount of vehicles.
	int getNumVehicles() const;
	/// Gets the craft's weapons.
	std::vector<CraftWeapon*> *const getWeapons();
	/// Gets the craft's items.
	ItemContainer *const getItems();
	/// Gets the craft's vehicles.
	std::vector<Vehicle*> *const getVehicles();
	/// Gets the craft's amount of fuel.
	int getFuel() const;
	/// Sets the craft's amount of fuel.
	void setFuel(int fuel);
	/// Gets the craft's percentage of fuel.
	int getFuelPercentage() const;
	/// Gets the craft's amount of damage.
	int getDamage() const;
	/// Sets the craft's amount of damage.
	void setDamage(int damage);
	/// Gets the craft's percentage of damage.
	int getDamagePercentage() const;
	/// Gets whether the craft is running out of fuel.
	bool getLowFuel() const;
	/// Sets whether the craft is running out of fuel.
	void setLowFuel(bool low);
	/// Gets the craft's distance from its base.
	double getDistanceFromBase() const;
	/// Gets the craft's fuel consumption.
	int getFuelConsumption() const;
	/// Gets the craft's minimum fuel limit.
	int getFuelLimit() const;
	/// Returns the craft to its base.
	void returnToBase();
	/// Checks if a target is detected by the craft's radar.
	bool detect(Target *target) const;
	/// Handles craft logic.
	void think();
	/// Does a craft full checkup.
	void checkup();
	/// Consumes the craft's fuel.
	void consumeFuel();
	/// Repairs the craft.
	void repair();
	/// Refuels the craft.
	void refuel();
	/// Rearms the craft.
	std::string rearm();
	/// Sets the craft's battlescape status.
	void setInBattlescape(bool inbattle);
	/// Gets if the craft is in battlescape.
	bool isInBattlescape() const;
	/// Gets if craft is destroyed during dogfights.
	bool isDestroyed() const;
	/// Gets the amount of space available inside a craft.
	int getSpaceAvailable() const;
	/// Gets the amount of space used inside a craft.
	int getSpaceUsed() const;
	/// Gets the craft's vehicles of a certain type.
	int getVehicleCount(const std::string &vehicle) const;
	/// Sets the craft's dogfight status.
	void setInDogfight(const bool inDogfight);
	/// Gets if the craft is in dogfight.
	bool isInDogfight() const;
	/// Sets interception order (first craft to leave the base gets 1, second 2, etc.).
	void setInterceptionOrder(const int order);
	/// Gets interception number.
	int getInterceptionOrder() const;
};

}

#endif
