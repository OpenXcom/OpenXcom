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
#include "RuleUfo.h"
#include <cmath>

/**
 * Initializes a UFO of the specified type.
 * @param rules Pointer to ruleset.
 */
Ufo::Ufo(RuleUfo *rules) : _rules(rules), _lat(0.0), _lon(0.0), _targetLat(0.0), _targetLon(0.0), _id(0), _damage(0), _speed(0)
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
 * Calculates the speed vector for the UFO based
 * on the current raw speed and target destination.
 */
void Ufo::calculateSpeed()
{
	double newspeed = _speed / 1000000.0;
	double dLon = _targetLon - _lon;
	double dLat = _targetLat - _lat;
	double length = sqrt(dLon * dLon + dLat * dLat);
	if (length > 0)
	{
		_speedLon = dLon / length * newspeed;
		_speedLat = dLat / length * newspeed;
	}
	else
	{
		_speedLon = 0;
		_speedLat = 0;
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
