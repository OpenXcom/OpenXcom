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
#ifndef OPENXCOM_UNIT_H
#define OPENXCOM_UNIT_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

enum SpecialAbility { SPECAB_NONE = 0, SPECAB_EXPLODEONDEATH, SPECAB_BURNFLOOR, SPECAB_RESPAWN };
/**
 * This struct holds some plain unit attribute data together.
 */
struct UnitStats
{
	int tu, stamina, health, bravery, reactions, firing, throwing, strength, psiStrength, psiSkill, melee;
};

/**
 * Represents the static data for a unit that is generated on the battlescape, this includes: HWPs, aliens and civilians.
 * @sa Soldier BattleUnit
 */
class Unit
{
private:
	std::string _type;
	std::string _race;
	std::string _rank;
	UnitStats _stats;
	std::string _armor;
	int _standHeight, _kneelHeight, _floatHeight;
	int _value, _deathSound, _aggroSound, _moveSound;
	int _intelligence, _aggression;
	int _extraWeight;
	SpecialAbility _specab;
	std::string _zombieUnit, _spawnUnit;
	bool _livingWeapon;
public:
	/// Creates a blank unit ruleset.
	Unit(const std::string &type, std::string race, std::string rank);
	/// Cleans up the unit ruleset.
	~Unit();
	/// Loads the unit data from YAML.
	void load(const YAML::Node& node);
	/// Gets the unit's type.
	std::string getType() const;
	/// Gets the unit's stats.
	UnitStats *getStats();
	/// Gets the unit's height when standing.
	int getStandHeight() const;
	/// Gets the unit's height when kneeling.
	int getKneelHeight() const;
	/// Gets the unit's float elevation.
	int getFloatHeight() const;
	/// Gets the armor type.
	std::string getArmor() const;
	/// Gets the alien race type.
	std::string getRace() const;
	/// Gets the alien rank.
	std::string getRank() const;
	/// Gets the value - for score calculation.
	int getValue() const;
	/// Gets the death sound id.
	int getDeathSound() const;
	/// Gets the move sound id.
	int getMoveSound() const;
	/// Gets the intelligence. This is the number of turns AI remembers your troop positions.
	int getIntelligence() const;
	/// Gets the aggression. Determines the chance of revenge and taking cover.
	int getAggression() const;
	/// Gets the alien's special ability.
	int getSpecialAbility() const;
	/// Gets the unit's zombie unit.
	std::string getZombieUnit() const;
	/// Gets the unit's spawn unit.
	std::string getSpawnUnit() const;
	/// Gets the unit's war cry.
	int getAggroSound() const;
	/// Checks if this unit has a built in weapon.
	bool isLivingWeapon() const;
	/// Gets the unit's extra weight.
	int getExtraWeight() const;
};

}

namespace YAML
{
	template<>
	struct convert<OpenXcom::UnitStats>
	{
		static Node encode(const OpenXcom::UnitStats& rhs)
		{
			Node node;
			node["tu"] = rhs.tu;
			node["stamina"] = rhs.stamina;
			node["health"] = rhs.health;
			node["bravery"] = rhs.bravery;
			node["reactions"] = rhs.reactions;
			node["firing"] = rhs.firing;
			node["throwing"] = rhs.throwing;
			node["strength"] = rhs.strength;
			node["psiStrength"] = rhs.psiStrength;
			node["psiSkill"] = rhs.psiSkill;
			node["melee"] = rhs.melee;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::UnitStats& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.tu = node["tu"].as<int>(rhs.tu);
			rhs.stamina = node["stamina"].as<int>(rhs.stamina);
			rhs.health = node["health"].as<int>(rhs.health);
			rhs.bravery = node["bravery"].as<int>(rhs.bravery);
			rhs.reactions = node["reactions"].as<int>(rhs.reactions);
			rhs.firing = node["firing"].as<int>(rhs.firing);
			rhs.throwing = node["throwing"].as<int>(rhs.throwing);
			rhs.strength = node["strength"].as<int>(rhs.strength);
			rhs.psiStrength = node["psiStrength"].as<int>(rhs.psiStrength);
			rhs.psiSkill = node["psiSkill"].as<int>(rhs.psiSkill);
			rhs.melee = node["melee"].as<int>(rhs.melee);
			return true;
		}
	};
}

#endif
