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

Base::Base(double lat, double lon) : _lat(lat), _lon(lon), _name("")
{
}

Base::~Base()
{
}

string Base::getName()
{
	return _name;
}

void Base::setName(string name)
{
	_name = name;
}

double Base::getLatitude()
{
	return _lat;
}

double Base::getLongitude()
{
	return _lon;
}

vector<BaseFacility*> *Base::getFacilities()
{
	return &_facilities;
}

vector<Soldier*> *Base::getSoldiers()
{
	return &_soldiers;
}

vector<Craft*> *Base::getCrafts()
{
	return &_crafts;
}