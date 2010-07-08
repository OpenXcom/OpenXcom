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
#ifndef OPENXCOM__CRAFT_H
#define OPENXCOM__CRAFT_H

#include <map>
#include <vector>
#include "RuleCraft.h"
#include "Soldier.h"

using namespace std;

/**
 * Represents a craft stored in a base.
 * Contains variable info about a craft like
 * position, fuel, damage, etc.
 * @sa RuleCraft
 */
class Craft
{
private:
	RuleCraft *_rules;
	double _lat, _lon;
	int _id, _fuel, _health;
public:
	/// Creates a craft of the specified type.
	Craft(RuleCraft *rules, map<LangString, int> *id, double lat, double lon);
	/// Cleans up the craft.
	~Craft();
	/// Gets the craft's ruleset.
	RuleCraft *getRules();
	/// Gets the craft's ID.
	int getId();
	/// Gets the craft's amount of weapons.
	int getWeapons();
	/// Gets the craft's amount of soldiers.
	int getSoldiers(vector<Soldier*> *soldiers);
	/// Gets the craft's amount of HWPs.
	int getHWPs();
};

#endif
