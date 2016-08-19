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
#include "RuleCraftWeapon.h"
#include "Mod.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of craft weapon.
 * @param type String defining the type.
 */
RuleCraftWeapon::RuleCraftWeapon(const std::string &type) : _type(type), _sprite(-1), _sound(-1), _damage(0), _range(0), _accuracy(0), _reloadCautious(0), _reloadStandard(0), _reloadAggressive(0), _ammoMax(0), _rearmRate(1), _projectileSpeed(0), _projectileType(CWPT_CANNON_ROUND), _underwaterOnly(false)
{
}

/**
 *
 */
RuleCraftWeapon::~RuleCraftWeapon()
{
}

/**
 * Loads the craft weapon from a YAML file.
 * @param node YAML node.
 * @param mod Mod for the craft weapon.
 */
void RuleCraftWeapon::load(const YAML::Node &node, Mod *mod)
{
	_type = node["type"].as<std::string>(_type);
	if (node["sprite"])
	{
		_sprite = node["sprite"].as<int>(_sprite);
		// this one is an offset within INTICONS.PCK
		if (_sprite > 5)
			_sprite += mod->getModOffset();
	}
	if (node["sound"])
	{
		_sound = mod->getSoundOffset(node["sound"].as<int>(_sound), "GEO.CAT");
	}
	_damage = node["damage"].as<int>(_damage);
	_range = node["range"].as<int>(_range);
	_accuracy = node["accuracy"].as<int>(_accuracy);
	_reloadCautious = node["reloadCautious"].as<int>(_reloadCautious);
	_reloadStandard = node["reloadStandard"].as<int>(_reloadStandard);
	_reloadAggressive = node["reloadAggressive"].as<int>(_reloadAggressive);
	_ammoMax = node["ammoMax"].as<int>(_ammoMax);
	_rearmRate = node["rearmRate"].as<int>(_rearmRate);
	_projectileType = (CraftWeaponProjectileType)node["projectileType"].as<int>(_projectileType);
	_projectileSpeed = node["projectileSpeed"].as<int>(_projectileSpeed);
	_launcher = node["launcher"].as<std::string>(_launcher);
	_clip = node["clip"].as<std::string>(_clip);
	_underwaterOnly = node["underwaterOnly"].as<bool>(_underwaterOnly);
}

/**
 * Gets the language string that names this craft weapon.
 * Each craft weapon type has a unique name.
 * @return The craft weapon's name.
 */
std::string RuleCraftWeapon::getType() const
{
	return _type;
}

/**
 * Gets the ID of the sprite used to draw the craft weapon
 * in the Equip Craft and Interception screens.
 * @return The sprite ID.
 */
int RuleCraftWeapon::getSprite() const
{
	return _sprite;
}

/**
 * Gets the ID of the sound used when firing the weapon
 * in the Dogfight screen.
 * @return The sound ID.
 */
int RuleCraftWeapon::getSound() const
{
	return _sound;
}

/**
 * Gets the amount of damage this craft weapon
 * inflicts on enemy crafts.
 * @return The damage amount.
 */
int RuleCraftWeapon::getDamage() const
{
	return _damage;
}

/**
 * Gets the maximum range of this craft weapon.
 * @return The range in km.
 */
int RuleCraftWeapon::getRange() const
{
	return _range;
}

/**
 * Gets the percentage chance of each shot of
 * this craft weapon hitting an enemy craft.
 * @return The accuracy as a percentage.
 */
int RuleCraftWeapon::getAccuracy() const
{
	return _accuracy;
}

/**
 * Gets the amount of time the craft weapon takes to
 * reload in cautious mode.
 * @return The time in game seconds.
 */
int RuleCraftWeapon::getCautiousReload() const
{
	return _reloadCautious;
}

/**
 * Gets the amount of time the craft weapon takes to
 * reload in standard mode.
 * @return The time in game seconds.
 */
int RuleCraftWeapon::getStandardReload() const
{
	return _reloadStandard;
}

/**
 * Gets the amount of time the craft weapon takes to
 * reload in aggressive mode.
 * @return The time in game seconds.
 */
int RuleCraftWeapon::getAggressiveReload() const
{
	return _reloadAggressive;
}

/**
 * Gets the maximum amount of ammo the craft weapon
 * can carry.
 * @return The amount of ammo.
 */
int RuleCraftWeapon::getAmmoMax() const
{
	return _ammoMax;
}

/**
 * Gets how much ammo is added to the craft weapon
 * while rearming (the amount of ammo in each clip item).
 * @return The amount of ammo.
 */
int RuleCraftWeapon::getRearmRate() const
{
	return _rearmRate;
}

/**
 * Gets the language string of the item used to
 * equip this craft weapon.
 * @return The item name.
 */
std::string RuleCraftWeapon::getLauncherItem() const
{
	return _launcher;
}

/**
 * Gets the language string of the item used to
 * load this craft weapon with ammo.
 * @return The item name.
 */
std::string RuleCraftWeapon::getClipItem() const
{
	return _clip;
}

/**
 * Gets the Projectile Type this weapon will fire
 * @return The projectile type.
 */
CraftWeaponProjectileType RuleCraftWeapon::getProjectileType() const
{
	return _projectileType;
}

/**
 * Gets the speed of the projectile fired by this weapon
 * @return The projectile speed.
 */
int RuleCraftWeapon::getProjectileSpeed() const
{
	return _projectileSpeed;
}

/**
 * Can this item be used on land or is it underwater only?
 * @return if this is an underwater weapon or not.
 */
bool RuleCraftWeapon::isWaterOnly() const
{
	return _underwaterOnly;
}

}
