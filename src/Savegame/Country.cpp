/*
 * Copyright 2010 OpenXcom Developers.
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
#include "Country.h"

namespace OpenXcom
{

/**
 * Initializes a country with a starting amount of monthly funding.
 * @param funding Starting monthly funding.
 */
Country::Country(int funding) : _funding(funding), _change(0)
{
}

/**
 *
 */
Country::~Country()
{
}

/**
 * Returns the country's current monthly funding.
 * @return Monthly funding.
 */
int Country::getFunding() const
{
	return _funding;
}

/**
 * Changes the country's current monthly funding.
 * @param funding Monthly funding.
 */
void Country::setFunding(int funding)
{
	_funding = funding;
}

/**
 * Returns the country's funding change since last month.
 * @return Funding change.
 */
int Country::getChange() const
{
	return _change;
}

/**
 * Returns the longitude of the country's label on the globe.
 * @return Longitude in radians.
 */
double Country::getLabelLongitude() const
{
	return _labelLon;
}

/**
 * Changes the longitude of the country's label on the globe.
 * @param lon Longitude in radians.
 */
void Country::setLabelLongitude(double lon)
{
	_labelLon = lon;
}

/**
 * Returns the latitude of the country's label on the globe.
 * @return Latitude in radians.
 */
double Country::getLabelLatitude() const
{
	return _labelLat;
}

/**
 * Changes the latitude of the country's label on the globe.
 * @param lat Latitude in radians.
 */
void Country::setLabelLatitude(double lat)
{
	_labelLat = lat;
}

}
