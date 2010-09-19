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
#include "Target.h"
#include "Language.h"

/**
 * Initializes a target with blank coordinates.
 */
Target::Target() : _lat(0.0), _lon(0.0)
{
}

Target::~Target()
{
}

/**
 * Returns the latitude coordinate of the target.
 * @return Latitude in radian.
 */
double Target::getLatitude()
{
	return _lat;
}

/**
 * Changes the latitude coordinate of the target.
 * @param lat Latitude in radian.
 */
void Target::setLatitude(double lat)
{
	_lat = lat;
}

/**
 * Returns the longitude coordinate of the target.
 * @return Longitude in radian.
 */
double Target::getLongitude()
{
	return _lon;
}

/**
 * Changes the longitude coordinate of the target.
 * @param lon Longitude in radian.
 */
void Target::setLongitude(double lon)
{
	_lon = lon;
}
