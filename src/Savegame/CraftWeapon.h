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
#ifndef OPENXCOM_CRAFTWEAPON_H
#define OPENXCOM_CRAFTWEAPON_H

#include "../Resource/LangString.h"

class RuleCraftWeapon;

/**
 * Represents a craft weapon equipped by a craft.
 * Contains variable info about a craft weapon like ammo.
 * @sa RuleCraftWeapon
 */
class CraftWeapon
{
private:
	RuleCraftWeapon *_rules;
	int _ammo;
public:
	/// Creates a craft weapon of the specified type.
	CraftWeapon(RuleCraftWeapon *rules, int ammo);
	/// Cleans up the craft weapon.
	~CraftWeapon();
	/// Gets the craft weapon's ruleset.
	RuleCraftWeapon *getRules();
	/// Gets the craft weapon's ammo
	int getAmmo();
	/// Sets the craft weapon's ammo.
	void setAmmo(int ammo);
};

#endif
