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

Craft::Craft(RuleCraft *rules, map<LangString, int> *id, double lat, double lon) : _rules(rules), _lat(lat), _lon(lon)
{
	_id = (*id)[rules->getType()];
	(*id)[rules->getType()]++;
}

Craft::~Craft()
{
}

RuleCraft *Craft::getRules()
{
	return _rules;
}

int Craft::getId()
{
	return _id;
}