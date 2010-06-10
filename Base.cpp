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
Base::Base(double lat, double lon) : _lat(lat), _lon(lon), _name(""), _facilities(), _soldiers(), _crafts(), _scientists(0), _engineers(0)
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

int Base::getScientists()
{
	return _scientists;
}

void Base::setScientists(int scientists)
{
	 _scientists = scientists;
}

int Base::getEngineers()
{
	return _engineers;
}

void Base::setEngineers(int engineers)
{
	 _engineers = engineers;
}

int Base::getAvailableSoldiers()
{
	return 0;
}

int Base::getTotalSoldiers()
{
	return _soldiers.size();
}

int Base::getAvailableScientists()
{
	return 0;
}

int Base::getTotalScientists()
{
	return _scientists;
}

int Base::getAvailableEngineers()
{
	return 0;
}

int Base::getTotalEngineers()
{
	return _engineers;
}

int Base::getUsedQuarters()
{
	return getTotalSoldiers() + getTotalScientists() + getTotalEngineers();
}

int Base::getAvailableQuarters()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getPersonnel();
	}
	return total;
}

int Base::getUsedStores()
{
	return 0;
}

int Base::getAvailableStores()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getStorage();
	}
	return total;
}

int Base::getUsedLaboratories()
{
	return 0;
}

int Base::getAvailableLaboratories()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getLaboratories();
	}
	return total;
}

int Base::getUsedWorkshops()
{
	return 0;
}

int Base::getAvailableWorkshops()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getWorkshops();
	}
	return total;
}

int Base::getUsedHangars()
{
	return _crafts.size();
}

int Base::getAvailableHangars()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getCrafts();
	}
	return total;
}

int Base::getDefenceValue()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getDefenceValue();
	}
	return total;
}

int Base::getShortRangeDetection()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			if ((*i)->getRules()->getRadarRange() == 2000)
				total++;
	}
	return total;
}

int Base::getLongRangeDetection()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			if ((*i)->getRules()->getRadarRange() > 2000)
				total++;
	}
	return total;
}

int Base::getCraftCount(LangString craft)
{
	int total = 0;
	for (vector<Craft*>::iterator i = _crafts.begin(); i != _crafts.end(); i++)
	{
		if ((*i)->getRules()->getType() == craft)
			total++;
	}
	return total;
}

int Base::getCraftMaintenance()
{
	int total = 0;
	for (vector<Craft*>::iterator i = _crafts.begin(); i != _crafts.end(); i++)
	{
		total += (*i)->getRules()->getMonthlyFee();
	}
	return total;
}

int Base::getPersonnelMaintenance()
{
	int total = 0;
	total += _soldiers.size() * 20000;
	total += _engineers * 25000;
	total += _scientists * 30000;
	return total;
}

int Base::getFacilityMaintenance()
{
	int total = 0;
	for (vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); i++)
	{
		if ((*i)->getBuildTime() == 0)
			total += (*i)->getRules()->getMonthlyCost();
	}
	return total;
}

int Base::getMonthlyMaintenace()
{
	return getCraftMaintenance() + getPersonnelMaintenance() + getFacilityMaintenance();
}