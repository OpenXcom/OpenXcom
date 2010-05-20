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
#ifndef OPENXCOM__BASE_H
#define OPENXCOM__BASE_H

#include <string>
#include <vector>

class BaseFacility;
class Soldier;
class Craft;

using namespace std;

/**
 * Represents a player base on the globe.
 * Bases can contain facilities, personnel, crafts and equipment.
 */
class Base
{
private:
	double _lat, _lon;
	string _name;
	vector<BaseFacility*> _facilities;
	vector<Soldier*> _soldiers;
	vector<Craft*> _crafts;
public:
	/// Creates a base on the specified coordinates.
	Base(double lat, double lon);
	/// Cleans up the base.
	~Base();
	/// Gets the base's name.
	string getName();
	/// Sets the base's name.
	void setName(string name);
	/// Gets the base's latitude.
	double getLatitude();
	/// Gets the base's longitude.
	double getLongitude();
	/// Gets the base's facilities.
	vector<BaseFacility*> *getFacilities();
	/// Gets the base's soldiers.
	vector<Soldier*> *getSoldiers();
	/// Gets the base's crafts.
	vector<Craft*> *getCrafts();
};

#endif