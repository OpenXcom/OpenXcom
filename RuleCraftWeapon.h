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
	int _damage, _range, _accuracy, _reload, _ammoMax, _ammoClip;
	LangString _launcher, _clip;
public:
	/// Creates a blank craft weapon ruleset.
	RuleCraftWeapon(LangString type);
	/// Cleans up the craft weapon ruleset.
	~RuleCraftWeapon();
	/// Gets the craft weapon's type.
	LangString getType();
};

#endif
