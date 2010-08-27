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
#include "Ufo.h"

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules) : _rules(rules), _lat(0.0), _lon(0.0), _id(0), _damage(0), _speed(0)
{
}

/**
 * Delete the contents of the UFO from memory.
 */
Ufo::~Ufo()
{
}

/**
 * Returns the ruleset for the UFO's type.
 * @return Pointer to ruleset.
 */
RuleUfo *Ufo::getRules()
{
	return _rules;
}

/**
 * Returns the UFO's unique ID. Each UFO
 * can be identified by its type and ID.
 */
int Ufo::getId()
{
	return _id;
}

/**
 * Returns the latitude coordinate of the UFO.
 * @return Latitude in radian.
 */
double Ufo::getLatitude()
{
	return _lat;
}

/**
 * Changes the latitude coordinate of the UFO.
 * @param lat Latitude in radian.
 */
void Ufo::setLatitude(double lat)
{
	_lat = lat;
}

/**
 * Returns the longitude coordinate of the UFO.
 * @return Longitude in radian.
 */
double Ufo::getLongitude()
{
	return _lon;
}

/**
 * Changes the longitude coordinate of the UFO.
 * @param lon Longitude in radian.
 */
void Ufo::setLongitude(double lon)
{
	_lon = lon;
}

/**
 * Returns the amount of damage this UFO has taken.
 * @return Amount of damage.
 */
int Ufo::getDamage()
{
	return _damage;
}

/**
 * Changes the amount of damage this UFO has taken.
 * @param damage Amount of damage.
 */
void Ufo::setDamage(int damage)
{
	_damage = damage;
	if (_damage < 0)
		_damage = 0;
}
