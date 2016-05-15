/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "Unit.h"
#include "../Engine/Exception.h"
#include "Mod.h"

namespace OpenXcom
{

/**
 * Creates a certain type of unit.
 * @param type String defining the type.
 */
Unit::Unit(const std::string &type) : _type(type), _standHeight(0), _kneelHeight(0), _floatHeight(0), _value(0), _aggroSound(-1), _moveSound(-1), _intelligence(0), _aggression(0), _energyRecovery(30), _specab(SPECAB_NONE), _livingWeapon(false), _psiWeapon("ALIEN_PSI_WEAPON")
{
}

/**
 *
 */
Unit::~Unit()
{

}

/**
 * Loads the unit from a YAML file.
 * @param node YAML node.
 * @param mod Mod for the unit.
 */
void Unit::load(const YAML::Node &node, Mod *mod)
{
	_type = node["type"].as<std::string>(_type);
	_race = node["race"].as<std::string>(_race);
	_rank = node["rank"].as<std::string>(_rank);
	_stats.merge(node["stats"].as<UnitStats>(_stats));
	_armor = node["armor"].as<std::string>(_armor);
	_standHeight = node["standHeight"].as<int>(_standHeight);
	_kneelHeight = node["kneelHeight"].as<int>(_kneelHeight);
	_floatHeight = node["floatHeight"].as<int>(_floatHeight);
	if (_floatHeight + _standHeight > 25)
	{
		throw Exception("Error with unit "+ _type +": Unit height may not exceed 25");
	}
	_value = node["value"].as<int>(_value);
	_intelligence = node["intelligence"].as<int>(_intelligence);
	_aggression = node["aggression"].as<int>(_aggression);
	_energyRecovery = node["energyRecovery"].as<int>(_energyRecovery);
	_specab = (SpecialAbility)node["specab"].as<int>(_specab);
	_spawnUnit = node["spawnUnit"].as<std::string>(_spawnUnit);
	_livingWeapon = node["livingWeapon"].as<bool>(_livingWeapon);
	_meleeWeapon = node["meleeWeapon"].as<std::string>(_meleeWeapon);
	_psiWeapon = node["psiWeapon"].as<std::string>(_psiWeapon);
	_builtInWeapons = node["builtInWeaponSets"].as<std::vector<std::vector<std::string> > >(_builtInWeapons);
	if (node["builtInWeapons"])
	{
		_builtInWeapons.push_back(node["builtInWeapons"].as<std::vector<std::string> >());
	}
	if (node["deathSound"])
	{
		_deathSound.clear();
		if (node["deathSound"].IsSequence())
		{
			for (YAML::const_iterator i = node["deathSound"].begin(); i != node["deathSound"].end(); ++i)
			{
				_deathSound.push_back(mod->getSoundOffset(i->as<int>(), "BATTLE.CAT"));
			}
		}
		else
		{
			_deathSound.push_back(mod->getSoundOffset(node["deathSound"].as<int>(), "BATTLE.CAT"));
		}
	}
	if (node["aggroSound"])
	{
		_aggroSound = mod->getSoundOffset(node["aggroSound"].as<int>(_aggroSound), "BATTLE.CAT");
	}
	if (node["moveSound"])
	{
		_moveSound = mod->getSoundOffset(node["moveSound"].as<int>(_moveSound), "BATTLE.CAT");
	}
}

/**
 * Returns the language string that names
 * this unit. Each unit type has a unique name.
 * @return The unit's name.
 */
std::string Unit::getType() const
{
	return _type;
}

/**
 * Returns the unit's stats data object.
 * @return The unit's stats.
 */
UnitStats *Unit::getStats()
{
	return &_stats;
}

/**
 * Returns the unit's height at standing.
 * @return The unit's height.
 */
int Unit::getStandHeight() const
{
	return _standHeight;
}

/**
 * Returns the unit's height at kneeling.
 * @return The unit's kneeling height.
 */
int Unit::getKneelHeight() const
{
	return _kneelHeight;
}

/**
 * Returns the unit's floating elevation.
 * @return The unit's floating height.
 */
int Unit::getFloatHeight() const
{
	return _floatHeight;
}

/**
 * Gets the unit's armor type.
 * @return The unit's armor type.
 */
std::string Unit::getArmor() const
{
	return _armor;
}

/**
 * Gets the alien's race.
 * @return The alien's race.
 */
std::string Unit::getRace() const
{
	return _race;
}

/**
 * Gets the unit's rank.
 * @return The unit's rank.
 */
std::string Unit::getRank() const
{
	return _rank;
}

/**
 * Gets the unit's value - for scoring.
 * @return The unit's value.
 */
int Unit::getValue() const
{
	return _value;
}

/**
* Get the unit's death sounds.
* @return List of sound IDs.
*/
const std::vector<int> &Unit::getDeathSounds() const
{
	return _deathSound;
}

/**
 * Gets the unit's move sound.
 * @return The id of the unit's move sound.
 */
int Unit::getMoveSound() const
{
	return _moveSound;
}

/**
 * Gets the intelligence. This is the number of turns the AI remembers your troop positions.
 * @return The unit's intelligence.
 */
int Unit::getIntelligence() const
{
	return _intelligence;
}

/**
 * Gets the aggression. Determines the chance of revenge and taking cover.
 * @return The unit's aggression.
 */
int Unit::getAggression() const
{
	return _aggression;
}

/**
 * Gets the unit's special ability.
 * @return The unit's specab.
 */
int Unit::getSpecialAbility() const
{
	return (int)_specab;
}

/**
 * Gets the unit that is spawned when this one dies.
 * @return The unit's spawn unit.
 */
std::string Unit::getSpawnUnit() const
{
	return _spawnUnit;
}

/**
 * Gets the unit's war cry.
 * @return The id of the unit's aggro sound.
 */
int Unit::getAggroSound() const
{
	return _aggroSound;
}

/**
 * How much energy does this unit recover per turn?
 * @return energy recovery amount.
 */
int Unit::getEnergyRecovery() const
{
	return _energyRecovery;
}

/**
 * Checks if this unit is a living weapon.
 * a living weapon ignores any loadout that may be available to
 * its rank and uses the one associated with its race.
 * @return True if this unit is a living weapon.
 */
bool Unit::isLivingWeapon() const
{
	return _livingWeapon;
}

/**
 * What is this unit's built in melee weapon (if any).
 * @return the name of the weapon.
 */
std::string Unit::getMeleeWeapon() const
{
	return _meleeWeapon;
}

/**
* What is this unit's built in psi weapon (if any).
* @return the name of the weapon.
*/
std::string Unit::getPsiWeapon() const
{
	return _psiWeapon;
}

/**
 * What weapons does this unit have built in?
 * this is a vector of strings representing any
 * weapons that may be inherent to this creature.
 * note: unlike "livingWeapon" this is used in ADDITION to
 * any loadout or living weapon item that may be defined.
 * @return list of weapons that are integral to this unit.
 */
const std::vector<std::vector<std::string> > &Unit::getBuiltInWeapons() const
{
	return _builtInWeapons;
}

}
