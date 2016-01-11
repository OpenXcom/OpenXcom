#pragma once
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
#include <string>
#include <yaml-cpp/yaml.h>
#include "../Savegame/CraftWeaponProjectile.h"

namespace OpenXcom
{

class Mod;

/**
 * Represents a specific type of craft weapon.
 * Contains constant info about a craft weapon like
 * damage, range, accuracy, items used, etc.
 * @sa CraftWeapon
 */
class RuleCraftWeapon
{
private:
	std::string _type;
	int _sprite, _sound, _damage, _range, _accuracy, _reloadCautious, _reloadStandard, _reloadAggressive, _ammoMax, _rearmRate, _projectileSpeed;
	CraftWeaponProjectileType _projectileType;
	std::string _launcher, _clip;
	bool _underwaterOnly;
public:
	/// Creates a blank craft weapon ruleset.
	RuleCraftWeapon(const std::string &type);
	/// Cleans up the craft weapon ruleset.
	~RuleCraftWeapon();
	/// Loads craft weapon data from YAML.
	void load(const YAML::Node& node, Mod *mod);
	/// Gets the craft weapon's type.
	std::string getType() const;
	/// Gets the craft weapon's sprite.
	int getSprite() const;
	/// Gets the craft weapon's sound.
	int getSound() const;
	/// Gets the craft weapon's damage.
	int getDamage() const;
	/// Gets the craft weapon's range.
	int getRange() const;
	/// Gets the craft weapon's accuracy.
	int getAccuracy() const;
	/// Gets the craft weapon's cautious reload time.
	int getCautiousReload() const;
	/// Gets the craft weapon's standard reload time.
	int getStandardReload() const;
	/// Gets the craft weapon's aggressive reload time.
	int getAggressiveReload() const;
	/// Gets the craft weapon's maximum ammo.
	int getAmmoMax() const;
	/// Gets the craft weapon's rearm rate.
	int getRearmRate() const;
	/// Gets the craft weapon's launcher item.
	std::string getLauncherItem() const;
	/// Gets the craft weapon's clip item.
	std::string getClipItem() const;
	/// Gets the craft weapon's projectile's type.
	CraftWeaponProjectileType getProjectileType() const;
	/// Gets the craft weapon's projectile speed.
	int getProjectileSpeed() const;
	/// Is this item restricted to use underwater?
	bool isWaterOnly() const;
};

}
