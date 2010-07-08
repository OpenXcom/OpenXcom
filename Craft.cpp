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
#include "Craft.h"

/**
 * Initializes a craft of the specified type and
 * assigns it the latest craft ID available.
 * @param rules Pointer to ruleset.
 * @param id List of craft IDs.
 * @param lat Latitude in radian.
 * @param lon Longitude in radian.
 */
Craft::Craft(RuleCraft *rules, map<LangString, int> *id, double lat, double lon) : _rules(rules), _lat(lat), _lon(lon), _fuel(0), _health(0)
{
	_id = (*id)[rules->getType()];
	(*id)[rules->getType()]++;
}

/**
 *
 */
Craft::~Craft()
{
}

/**
 * Returns the ruleset for the craft's type.
 * @return Pointer to ruleset.
 */
RuleCraft *Craft::getRules()
{
	return _rules;
}

/**
 * Returns the craft's unique ID. Each craft
 * can be identified by its type and ID.
 */
int Craft::getId()
{
	return _id;
}

/**
 * Returns the amount of weapons currently
 * equipped on this craft.
 * @return Number of weapons.
 */
int Craft::getWeapons()
{
	return 0;
}

/**
 * Returns the amount of soldiers from a list
 * that are currently attached to this craft.
 * @return Number of soldiers.
 */
int Craft::getSoldiers(vector<Soldier*> *soldiers)
{
	if (_rules->getSoldiers() == 0)
		return 0;

	int total = 0;

	for (vector<Soldier*>::iterator i = soldiers->begin(); i != soldiers->end(); i++)
	{
		if ((*i)->getCraft() == this)
			total++;
	}

	return total;
}

/**
 * Returns the amount of HWPs currently
 * contained in this craft.
 * @return Number of HWPs.
 */
int Craft::getHWPs()
{
	return 0;
}
