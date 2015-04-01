/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "CraftWeaponProjectile.h"

namespace OpenXcom
{

/**
 * Initializes a craft weapon of the specified type.
 * @param rules Pointer to ruleset.
 * @param ammo Initial ammo.
 */
CraftWeapon::CraftWeapon(RuleCraftWeapon *rules, int ammo) : _rules(rules), _ammo(ammo), _rearming(false)
{
}

/**
 *
 */
CraftWeapon::~CraftWeapon()
{
}

/**
 * Loads the craft weapon from a YAML file.
 * @param node YAML node.
 */
void CraftWeapon::load(const YAML::Node &node)
{
	_ammo = node["ammo"].as<int>(_ammo);
	_rearming = node["rearming"].as<bool>(_rearming);
}

/**
 * Saves the base to a YAML file.
 * @return YAML node.
 */
YAML::Node CraftWeapon::save() const
{
	YAML::Node node;
	node["type"] = _rules->getType();
	node["ammo"] = _ammo;
	if (_rearming)
		node["rearming"] = _rearming;
	return node;
}

/**
 * Returns the ruleset for the craft weapon's type.
 * @return Pointer to ruleset.
 */
RuleCraftWeapon *CraftWeapon::getRules() const
{
	return _rules;
}

/**
 * Returns the ammo contained in this craft weapon.
 * @return Weapon ammo.
 */
int CraftWeapon::getAmmo() const
{
	return _ammo;
}

/**
 * Changes the ammo contained in this craft weapon.
 * @param ammo Weapon ammo.
 * @return If the weapon ran out of ammo.
 */
bool CraftWeapon::setAmmo(int ammo)
{
	_ammo = ammo;
	if (_ammo < 0)
	{
		_ammo = 0;
		return false;
	}
	if (_ammo > _rules->getAmmoMax())
	{
		_ammo = _rules->getAmmoMax();
	}
	return true;
}

/**
 * Returns whether this craft weapon needs rearming.
 * @return Rearming status.
 */
bool CraftWeapon::isRearming() const
{
	return _rearming;
}

/**
 * Changes whether this craft weapon needs rearming
 * (for example, in case there's no more ammo).
 * @param rearming Rearming status.
 */
void CraftWeapon::setRearming(bool rearming)
{
	_rearming = rearming;
}

/**
 * Rearms this craft weapon's ammo.
 * @param available number of clips available.
 * @param clipSize number of rounds in said clips.
 * @return number of clips used.
 */
int CraftWeapon::rearm(const int available, const int clipSize)
{
	int ammoUsed = _rules->getRearmRate();

	if (clipSize > 0)
	{	// +(clipSize - 1) correction for rounding up
		int needed = std::min(_rules->getRearmRate(), _rules->getAmmoMax() - _ammo + clipSize - 1) / clipSize;
		ammoUsed = ((needed > available)? available : needed) * clipSize;
	}

	setAmmo(_ammo + ammoUsed);

	_rearming = _ammo < _rules->getAmmoMax();

	return (clipSize <= 0)? 0 : ammoUsed / clipSize;
}

/*
 * Fires a projectile from crafts weapon.
 * @return Pointer to the new projectile.
 */
CraftWeaponProjectile* CraftWeapon::fire() const
{
	CraftWeaponProjectile *p = new CraftWeaponProjectile();
	p->setType(this->getRules()->getProjectileType());
	p->setSpeed(this->getRules()->getProjectileSpeed());
	p->setAccuracy(this->getRules()->getAccuracy());
	p->setDamage(this->getRules()->getDamage());
	p->setRange(this->getRules()->getRange());
	return p;
}

/*
 * get how many clips are loaded into this weapon.
 * @param ruleset a pointer to the core ruleset.
 * @return number of clips loaded.
 */
int CraftWeapon::getClipsLoaded(Ruleset *ruleset)
{
	int retVal = (int)floor((double)_ammo / _rules->getRearmRate());
	RuleItem *clip = ruleset->getItem(_rules->getClipItem());

	if (clip && clip->getClipSize() > 0)
	{
		retVal = (int)floor((double)_ammo / clip->getClipSize());
	}

	return retVal;
}

}
