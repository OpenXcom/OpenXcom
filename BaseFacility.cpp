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
#include "BaseFacility.h"
#include "RuleBaseFacility.h"

#define RANGE_FACTOR 0.0003

/**
 * Initializes a base facility of the specified type.
 * @param rules Pointer to ruleset.
 * @param x X position in grid squares.
 * @param y Y position in grid squares.
 */
BaseFacility::BaseFacility(RuleBaseFacility *rules, int x, int y) : _rules(rules), _x(x), _y(y), _buildTime(0)
{
}

/**
 *
 */
BaseFacility::~BaseFacility()
{
}

/**
 * Returns the ruleset for the base facility's type.
 * @return Pointer to ruleset.
 */
RuleBaseFacility *BaseFacility::getRules()
{
	return _rules;
}

/**
 * Returns the base facility's X position on the
 * base grid that it's placed on.
 * @return X position in grid squares.
 */
int BaseFacility::getX()
{
	return _x;
}

/**
 * Returns the base facility's Y position on the
 * base grid that it's placed on.
 * @return Y position in grid squares.
 */
int BaseFacility::getY()
{
	return _y;
}

/**
 * Returns the base facility's remaining time
 * until it's finished building (0 = complete).
 * @return Time left in days.
 */
int BaseFacility::getBuildTime()
{
	return _buildTime;
}

/**
 * Changes the base facility's remaining time
 * until it's finished building.
 * @param time Time left in days.
 */
void BaseFacility::setBuildTime(int time)
{
	_buildTime = time;
}

/**
 * Returns if a certain point is covered by the facility's
 * radar range, taking in account the positions of both.
 * @param baseLon Base longitude.
 * @param baseLat Base latitude.
 * @param pointLon Point longitude.
 * @param pointLat Point latitude.
 * @return True if it's within range, False otherwise.
 */
bool BaseFacility::insideRadarRange(double baseLon, double baseLat, double pointLon, double pointLat)
{
	if (_rules->getRadarRange() == 0)
		return false;
	double newrange = _rules->getRadarRange() * RANGE_FACTOR;
	double dLon = pointLon - baseLon;
	double dLat = pointLat - baseLat;
    return (dLon * dLon + dLat * dLat <= newrange * newrange);
}
