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
#include "MovingTarget.h"

#define PI 3.141592653589793238461

/**
 * Initializes a moving target with blank coordinates.
 */
MovingTarget::MovingTarget() : Target(), _dest(0), _speedLon(0.0), _speedLat(0.0), _speed(0)
{
}

MovingTarget::~MovingTarget()
{
}

/**
 * Returns the destination the moving target is heading to.
 * @return Pointer to destination.
 */
Target *MovingTarget::getDestination()
{
	return _dest;
}

/**
 * Changes the destination the moving target is heading to.
 * @param dest Pointer to destination.
 */
void MovingTarget::setDestination(Target *dest)
{
	// Remove moving target from old destination's followers
	if (_dest != 0)
	{
		for (vector<Target*>::iterator i = _dest->getFollowers()->begin(); i != _dest->getFollowers()->end(); i++)
		{		
			if ((*i) == this)
			{
				_dest->getFollowers()->erase(i);
				break;
			}
		}
	}	
	_dest = dest;
	// Add moving target to new destination's followers
	if (_dest != 0)
	{
		_dest->getFollowers()->push_back(this);
	}
	calculateSpeed();
}

/**
 * Returns the speed of the moving target.
 * @return Speed in knots.
 */
int MovingTarget::getSpeed()
{
	return _speed;
}

/**
 * Changes the speed of the moving target.
 * @param speed Speed in knots.
 */
void MovingTarget::setSpeed(int speed)
{
	_speed = speed;
	calculateSpeed();
}

/**
 * Converts the speed from the standard knots (nautical miles per hour),
 * and converts it into radians per 5 in-game seconds.
 * @return Speed in radians.
 */
double MovingTarget::getRadianSpeed()
{
	// Each nautical mile is 1/60th of a degree.
	// Each hour contains 300 5-seconds.
	return _speed * (1 / 60.0) * (PI / 180) / 300.0;
}

/**
 * Calculates the speed vector for the moving target
 * based on the current raw speed and destination.
 */
void MovingTarget::calculateSpeed()
{
	if (_dest != 0)
	{
		double dLon = _dest->getLongitude() - _lon;
		double dLat = _dest->getLatitude() - _lat;
		double length = sqrt(dLon * dLon + dLat * dLat);
		_speedLon = dLon / length * getRadianSpeed();
		_speedLat = dLat / length * getRadianSpeed();
	}
	else
	{
		_speedLon = 0;
		_speedLat = 0;
	}
}

/**
 * Checks if the moving target has reached its destination by checking
 * if it has exceeded the destination position based on the speed vector.
 */
bool MovingTarget::reachedDestination()
{
	return (((_speedLon > 0 && _lon >= _dest->getLongitude()) || (_speedLon < 0 && _lon <= _dest->getLongitude())) &&
			((_speedLat > 0 && _lat >= _dest->getLatitude()) || (_speedLat < 0 && _lat <= _dest->getLatitude())));
}
