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
#ifndef OPENXCOM_MOVINGTARGET_H
#define OPENXCOM_MOVINGTARGET_H

#include "Target.h"

/**
 * Base class for moving targets on the globe
 * with a certain speed and destination.
 */
class MovingTarget : public Target
{
protected:
	Target *_dest;
	double _speedLon, _speedLat;
	int _speed;
	
	/// Has the moving target finished its route?
	bool finishedRoute();
public:
	/// Creates a moving target.
	MovingTarget();
	/// Cleans up the moving target.
	virtual ~MovingTarget();
	/// Gets the moving target's destination.
	Target *getDestination();
	/// Sets the moving target's destination.
	virtual void setDestination(Target *dest);
	/// Gets the moving target's speed.
	int getSpeed();
	/// Sets the moving target's speed.
	void setSpeed(int speed);
	/// Gets the moving target's speed in radian.
	double getRadianSpeed();
	/// Gets the distance to another target.
	double getDistance(Target *target, double *dLon, double *dLat);
	/// Calculates a new speed vector to the destination.
	virtual void calculateSpeed();
	/// Has the moving target reached its destination?
	bool reachedDestination();
};

#endif
