/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../fmath.h"
#include "SerializationHelper.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes a moving target with blank coordinates.
 */
MovingTarget::MovingTarget() : Target(), _dest(0), _speedLon(0.0), _speedLat(0.0), _speedRadian(0.0), _meetPointLon(0.0), _meetPointLat(0.0), _speed(0)
{
}

/**
 * Make sure to cleanup the target's destination followers.
 */
MovingTarget::~MovingTarget()
{
	if (_dest != 0 && !_dest->getFollowers()->empty())
	{
		for (std::vector<Target*>::iterator i = _dest->getFollowers()->begin(); i != _dest->getFollowers()->end(); ++i)
		{
			if ((*i) == this)
			{
				_dest->getFollowers()->erase(i);
				break;
			}
		}
	}
}

/**
 * Loads the moving target from a YAML file.
 * @param node YAML node.
 */
void MovingTarget::load(const YAML::Node &node)
{
	Target::load(node);
	_speedLon = node["speedLon"].as<double>(_speedLon);
	_speedLat = node["speedLat"].as<double>(_speedLat);
	_speedRadian = node["speedRadian"].as<double>(_speedRadian);
	_speed = node["speed"].as<int>(_speed);
}

/**
 * Saves the moving target to a YAML file.
 * @return YAML node.
 */
YAML::Node MovingTarget::save() const
{
	YAML::Node node = Target::save();
	if (_dest != 0)
	{
		node["dest"] = _dest->saveId();
	}
	node["speedLon"] = serializeDouble(_speedLon);
	node["speedLat"] = serializeDouble(_speedLat);
	node["speedRadian"] = serializeDouble(_speedRadian);
	node["speed"] = _speed;
	return node;
}

/**
 * Returns the destination the moving target is heading to.
 * @return Pointer to destination.
 */
Target *MovingTarget::getDestination() const
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
		for (std::vector<Target*>::iterator i = _dest->getFollowers()->begin(); i != _dest->getFollowers()->end(); ++i)
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
int MovingTarget::getSpeed() const
{
	return _speed;
}

/**
 * Returns the radial speed of the moving target.
 * @return Speed in 1 / 5 sec.
 */
double MovingTarget::getSpeedRadian() const
{
	return _speedRadian;
}

/**
 * Converts a speed in degrees to a speed in radians.
 * Each nautical mile is 1/60th of a degree.
 * Each hour contains 720 5-seconds.
 * @param speed Speed in degrees.
 * @return Speed in radians.
 */
double MovingTarget::calculateRadianSpeed(int speed)
{
	return speed * (1 / 60.0) * (M_PI / 180.0) / 720.0;
}

/**
 * Changes the speed of the moving target
 * and converts it from standard knots (nautical miles per hour)
 * into radians per 5 in-game seconds.
 * @param speed Speed in knots.
 */
void MovingTarget::setSpeed(int speed)
{
	_speed = speed;
	_speedRadian = calculateRadianSpeed(_speed);
	calculateSpeed();
}

/**
 * Calculates the speed vector based on the
 * great circle distance to destination and
 * current raw speed.
 */
void MovingTarget::calculateSpeed()
{
	calculateMeetPoint();
	if (_dest != 0)
	{
		double dLon, dLat, length;
		dLon = sin(_meetPointLon - _lon) * cos(_meetPointLat);
		dLat = cos(_lat) * sin(_meetPointLat) - sin(_lat) * cos(_meetPointLat) * cos(_meetPointLon - _lon);
		length = sqrt(dLon * dLon + dLat * dLat);
		_speedLat = dLat / length * _speedRadian;
		_speedLon = dLon / length * _speedRadian / cos(_lat + _speedLat);

		// Check for invalid speeds when a division by zero occurs due to near-zero values
		if (!(_speedLon == _speedLon) || !(_speedLat == _speedLat))
		{
			_speedLon = 0;
			_speedLat = 0;
		}
	}
	else
	{
		_speedLon = 0;
		_speedLat = 0;
	}
}

/**
 * Checks if the moving target has reached its destination.
 * @return True if it has, False otherwise.
 */
bool MovingTarget::reachedDestination() const
{
	if (_dest == 0)
	{
		return false;
	}
	return ( AreSame(_dest->getLongitude(), _lon) && AreSame(_dest->getLatitude(), _lat) );
}

/**
 * Executes a movement cycle for the moving target.
 */
void MovingTarget::move()
{
	calculateSpeed();
	if (_dest != 0)
	{
		if (getDistance(_dest) > _speedRadian)
		{
			setLongitude(_lon + _speedLon);
			setLatitude(_lat + _speedLat);
		}
		else
		{
			setLongitude(_dest->getLongitude());
			setLatitude(_dest->getLatitude());
		}
	}
}

/**
 * Calculate meeting point with the target.
 */
void MovingTarget::calculateMeetPoint()
{
	// Initialize
	if (_dest != 0)
	{
		_meetPointLat = _dest->getLatitude();
		_meetPointLon = _dest->getLongitude();
	}
	else
	{
		_meetPointLat = _lat;
		_meetPointLon = _lon;
	}

	if (!_dest || !Options::meetingPoint) return;

	MovingTarget *u = dynamic_cast<MovingTarget*>(_dest);
	if (!u || !u->getDestination()) return;

	// Speed ratio
	if (AreSame(u->getSpeedRadian(), 0.0)) return;
	const double speedRatio = _speedRadian/ u->getSpeedRadian();

	// The direction pseudovector
	double	nx = cos(u->getLatitude())*sin(u->getLongitude())*sin(u->getDestination()->getLatitude()) -
					sin(u->getLatitude())*cos(u->getDestination()->getLatitude())*sin(u->getDestination()->getLongitude()),
			ny = sin(u->getLatitude())*cos(u->getDestination()->getLatitude())*cos(u->getDestination()->getLongitude()) -
					cos(u->getLatitude())*cos(u->getLongitude())*sin(u->getDestination()->getLatitude()),
			nz = cos(u->getLatitude())*cos(u->getDestination()->getLatitude())*sin(u->getDestination()->getLongitude() - u->getLongitude());
	// Normalize and multiplex with radian speed
	double	nk = _speedRadian/sqrt(nx*nx+ny*ny+nz*nz);
	nx *= nk;
	ny *= nk;
	nz *= nk;

	// Finding the meeting point. Don't search further than halfway across the
	// globe (distance from interceptor's current point >= 1), as that may 
	// cause the interceptor to go the wrong way later.
	for (double path = 0, distance = 1;
		path < M_PI && distance - path*speedRatio > 0 && path*speedRatio < 1;
		path += _speedRadian)
	{
		_meetPointLat += nx*sin(_meetPointLon) - ny*cos(_meetPointLon);
		if (std::abs(_meetPointLat) < M_PI_2) _meetPointLon += nz - (nx*cos(_meetPointLon) + ny*sin(_meetPointLon))*tan(_meetPointLat); else _meetPointLon += M_PI;

		distance = acos(cos(_lat) * cos(_meetPointLat) * cos(_meetPointLon - _lon) + sin(_lat) * sin(_meetPointLat));
	}

	// Correction overflowing angles
	double lonSign = Sign(_meetPointLon);
	double latSign = Sign(_meetPointLat);
	while (std::abs(_meetPointLon) > M_PI) _meetPointLon -= lonSign * 2 * M_PI;
	while (std::abs(_meetPointLat) > M_PI) _meetPointLat -= latSign * 2 * M_PI;
	if (std::abs(_meetPointLat) > M_PI_2) { _meetPointLat = latSign * std::abs(2 * M_PI - std::abs(_meetPointLat)); _meetPointLon -= lonSign * M_PI; }
}

/**
 * Returns the latitude of the meeting point.
 * @return Angle in rad.
 */
double MovingTarget::getMeetLatitude() const
{
	return _meetPointLat;
}

/**
 * Returns the longitude of the meeting point.
 * @return Angle in rad.
 */
double MovingTarget::getMeetLongitude() const
{
	return _meetPointLon;
}

}
