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
#include "BaseFacility.h"
#include "Soldier.h"
#include "Craft.h"

using namespace std;

class Base
{
private:
	double _lat, _lon;
	string _name;
	vector<BaseFacility*> _facilities;
	vector<Soldier*> _soldiers;
	vector<Craft*> _crafts;
public:
	Base(double lat, double lon);
	~Base();
	string getName();
	void setName(string name);
	double getLatitude();
	double getLongitude();
	vector<BaseFacility*> *getFacilities();
	vector<Soldier*> *getSoldiers();
	vector<Craft*> *getCrafts();
};

#endif