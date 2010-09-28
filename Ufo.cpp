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
#include <cmath>
#include <sstream>
#include "Language.h"
#include "RuleUfo.h"
#include "RNG.h"

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules) : MovingTarget(), _rules(rules), _id(0), _damage(0), _altitude(0), _direction(STR_NORTH), _detected(false)
{
	setSpeed(RNG::generate(_rules->getMaxSpeed() / 2, _rules->getMaxSpeed()));
}

/**
 *
 */
Ufo::~Ufo()
{
	delete _dest;
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
 * Returns the UFO's unique ID. If it's 0,
 * this UFO has never been detected.
 * @return Unique ID.
 */
int Ufo::getId()
{
	return _id;
}

/**
 * Changes the UFO's unique ID.
 * @param id Unique ID.
 */
void Ufo::setId(int id)
{
	_id = id;
}

/**
 * Returns the UFO's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
string Ufo::getName(Language *lang)
{
	stringstream name;
	name << lang->getString(STR_UFO_) << _id;
	return name.str();
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

/**
 * Returns whether this UFO has been detected by radars.
 * @return Detection status.
 */
bool Ufo::getDetected()
{
	return _detected;
}

/**
 * Changes whether this UFO has been detected by radars.
 * @param detected Detection status.
 */
void Ufo::setDetected(bool detected)
{
	_detected = detected;
}

/**
 * Returns the current direction the UFO is heading in.
 * @return Direction.
 */
LangString Ufo::getDirection()
{
	return _direction;
}

/**
 * Returns the current altitude of the UFO.
 * @return Altitude.
 */
LangString Ufo::getAltitude()
{
	return STR_HIGH;
}

/**
 * Calculates the direction for the UFO based
 * on the current raw speed and destination.
 */
void Ufo::calculateSpeed()
{
	MovingTarget::calculateSpeed();
	if (_speedLon > 0)
	{
		if (_speedLat > 0)
		{
			_direction = STR_SOUTH_EAST;
		}
		else if (_speedLat < 0)
		{
			_direction = STR_NORTH_EAST;
		}
		else
		{
			_direction = STR_EAST;
		}
	}
	else if (_speedLon < 0)
	{
		if (_speedLat > 0)
		{
			_direction = STR_SOUTH_WEST;
		}
		else if (_speedLat < 0)
		{
			_direction = STR_NORTH_WEST;
		}
		else
		{
			_direction = STR_WEST;
		}
	}
	else
	{
		if (_speedLat > 0)
		{
			_direction = STR_SOUTH;
		}
		else if (_speedLat < 0)
		{
			_direction = STR_NORTH;
		}
	}
}

/**
 * Moves the UFO to its destination.
 */
void Ufo::think()
{
	setLongitude(_lon + _speedLon);
	setLatitude(_lat + _speedLat);
	if (reachedDestination())
	{
		_lon = _dest->getLongitude();
		_lat = _dest->getLatitude();
		setSpeed(0);
	}
}
