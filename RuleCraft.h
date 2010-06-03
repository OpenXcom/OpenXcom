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
#ifndef OPENXCOM__RULECRAFT_H
#define OPENXCOM__RULECRAFT_H

#include "LangString.h"

/**
 * Represents a specific type of craft.
 * Contains constant info about a craft like
 * costs, speed, capacities, consumptions, etc.
 * @sa Craft
 */
class RuleCraft
{
private:
	LangString _type;
	int _sprite;
	int _fuelMax, _healthMax, _speed, _accel, _weapons, _soldiers, _hwp, _monthlyFee;
public:
	/// Creates a blank craft ruleset.
	RuleCraft(LangString type);
	/// Cleans up the craft ruleset.
	~RuleCraft();

	LangString getType();
};

#endif