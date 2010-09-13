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
#include "RuleUfo.h"

#define SPEED_FACTOR 0.0000001

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules) : Target(), _rules(rules), _targetLat(0.0), _targetLon(0.0), _speedLon(0.0), _speedLat(0.0), _id(0), _damage(0), _speed(0), _altitude(0), _direction(STR_NORTH), _detected(false)
{
	setSpeed(_rules->getMaxSpeed());
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
 * Returns the latitude of the target destination of the UFO.
 * @return Latitude in radian.
 */
double Ufo::getTargetLatitude()
{
	return _targetLat;
}

/**
 * Changes the latitude of the target destination of the UFO.
 * @param lat Latitude in radian.
 */
void Ufo::setTargetLatitude(double lat)
{
	_targetLat = lat;
	calculateSpeed();
}

/**
 * Returns the longitude of the target destination of the UFO.
 * @return Longitude in radian.
 */
double Ufo::getTargetLongitude()
{
	return _targetLon;
}

/**
 * Changes the longitude of the target destination of the UFO.
 * @param lon Longitude in radian.
 */
void Ufo::setTargetLongitude(double lon)
{
	_targetLon = lon;
	calculateSpeed();
}

/**
 * Returns the speed of the UFO.
 * @return Speed in kilometers.
 */
int Ufo::getSpeed()
{
	return _speed;
}

/**
 * Changes the speed of the UFO.
 * @param speed Speed in kilometers.
 */
void Ufo::setSpeed(int speed)
{
	_speed = speed;
	calculateSpeed();
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
 * Calculates the speed vector and direction for the UFO
 * based on the current raw speed and target destination.
 */
void Ufo::calculateSpeed()
{
	double newSpeed = _speed * SPEED_FACTOR;
	double dLon = _targetLon - _lon;
	double dLat = _targetLat - _lat;
	double length = sqrt(dLon * dLon + dLat * dLat);
	if (length > 0)
	{
		_speedLon = dLon / length * newSpeed;
		_speedLat = dLat / length * newSpeed;
	}
	else
	{
		_speedLon = 0;
		_speedLat = 0;
	}

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
 * Moves the UFO to its target.
 */
void Ufo::think()
{
	_lon += _speedLon;
	_lat += _speedLat;
	if (((_speedLon > 0 && _lon > _targetLon) || (_speedLon < 0 && _lon < _targetLon)) &&
		((_speedLat > 0 && _lat > _targetLat) || (_speedLat < 0 && _lat < _targetLat)))
	{
		_lon = _targetLon;
		_lat = _targetLat;
		setSpeed(0);
	}
}
