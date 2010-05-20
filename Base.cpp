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
#include "Base.h"

/**
 * Initializes an empty base at specific globe coordinates.
 * @param lat Latitude in radian.
 * @param lon Longitude in radian.
 */
Base::Base(double lat, double lon) : _lat(lat), _lon(lon), _name(""), _facilities(), _soldiers(), _crafts()
{
}

/**
 *
 */
Base::~Base()
{
}

/**
 * Returns the custom name for the base.
 * @return Name.
 */
string Base::getName()
{
	return _name;
}

/**
 * Changes the custom name for the base.
 * @param name Name.
 */
void Base::setName(string name)
{
	_name = name;
}

/**
 * Returns the latitude coordinate of the base.
 * @return Latitude in radian.
 */
double Base::getLatitude()
{
	return _lat;
}

/**
 * Returns the longitude coordinate of the base.
 * @return Longitude in radian.
 */
double Base::getLongitude()
{
	return _lon;
}

/**
 * Returns the list of facilities in the base.
 * @return Pointer to the facility list.
 */
vector<BaseFacility*> *Base::getFacilities()
{
	return &_facilities;
}

/**
 * Returns the list of soldiers in the base.
 * @return Pointer to the soldier list.
 */
vector<Soldier*> *Base::getSoldiers()
{
	return &_soldiers;
}

/**
 * Returns the list of crafts in the base.
 * @return Pointer to the craft list.
 */
vector<Craft*> *Base::getCrafts()
{
	return &_crafts;
}