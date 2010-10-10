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
#ifndef OPENXCOM__RULECRAFTWEAPON_H
#define OPENXCOM__RULECRAFTWEAPON_H

#include "LangString.h"

/**
 * Represents a specific type of craft weapon.
 * Contains constant info about a craft weapon like
 * damage, range, accuracy, items used, etc.
 * @sa CraftWeapon
 */
class RuleCraftWeapon
{
private:
	LangString _type;
	int _sprite, _sound, _damage, _range, _accuracy, _reloadCautious, _reloadStandard, _reloadAggressive, _ammoMax, _rearm;
	LangString _launcher, _clip;
public:
	/// Creates a blank craft weapon ruleset.
	RuleCraftWeapon(LangString type);
	/// Cleans up the craft weapon ruleset.
	~RuleCraftWeapon();
	/// Gets the craft weapon's type.
	LangString getType();
	/// Gets the craft weapon's sprite.
	int getSprite();
	/// Sets the craft weapon's sprite.
	void setSprite(int sprite);
	/// Gets the craft weapon's sound.
	int getSound();
	/// Sets the craft weapon's sound.
	void setSound(int sound);
	/// Gets the craft weapon's damage.
	int getDamage();
	/// Sets the craft weapon's damage.
	void setDamage(int damage);
	/// Gets the craft weapon's range.
	int getRange();
	/// Sets the craft weapon's range.
	void setRange(int range);
	/// Gets the craft weapon's accuracy.
	int getAccuracy();
	/// Sets the craft weapon's accuracy.
	void setAccuracy(int accuracy);
	/// Gets the craft weapon's cautious reload time.
	int getCautiousReload();
	/// Sets the craft weapon's cautious reload time.
	void setCautiousReload(int reload);
	/// Gets the craft weapon's standard reload time.
	int getStandardReload();
	/// Sets the craft weapon's standard reload time.
	void setStandardReload(int reload);
	/// Gets the craft weapon's aggressive reload time.
	int getAggressiveReload();
	/// Sets the craft weapon's aggressive reload time.
	void setAggressiveReload(int reload);
	/// Gets the craft weapon's maximum ammo.
	int getAmmoMax();
	/// Sets the craft weapon's maximum ammo.
	void setAmmoMax(int ammo);
	/// Gets the craft weapon's rearm rate.
	int getRearmRate();
	/// Sets the craft weapon's rearm rate.
	void setRearmRate(int rearm);
	/// Gets the craft weapon's launcher item.
	LangString getLauncherItem();
	/// Sets the craft weapon's launcher item.
	void setLauncherItem(LangString launcher);
	/// Gets the craft weapon's clip item.
	LangString getClipItem();
	/// Sets the craft weapon's clip item.
	void setClipItem(LangString clip);
};

#endif
