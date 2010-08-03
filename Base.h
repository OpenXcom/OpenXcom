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
#include <map>
#include "LangString.h"

using namespace std;

class BaseFacility;
class Soldier;
class Craft;
class Item;

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
	map<LangString, Item*> _items;
	int _scientists, _engineers;
public:
	/// Creates a new base.
	Base();
	/// Cleans up the base.
	~Base();
	/// Gets the base's name.
	string getName();
	/// Sets the base's name.
	void setName(string name);
	/// Gets the base's latitude.
	double getLatitude();
	/// Sets the base's latitude.
	void setLatitude(double lat);
	/// Gets the base's longitude.
	double getLongitude();
	/// Sets the base's longitude.
	void setLongitude(double lon);
	/// Gets the base's facilities.
	vector<BaseFacility*> *getFacilities();
	/// Gets the base's soldiers.
	vector<Soldier*> *getSoldiers();
	/// Gets the base's crafts.
	vector<Craft*> *getCrafts();
	/// Gets the base's items.
	map<LangString, Item*> *getItems();
	/// Sets the base's scientists.
	void setScientists(int scientists);
	/// Sets the base's engineers.
	void setEngineers(int engineers);
	/// Gets the base's available soldiers.
	int getAvailableSoldiers();
	/// Gets the base's total soldiers.
	int getTotalSoldiers();
	/// Gets the base's available scientists.
	int getAvailableScientists();
	/// Gets the base's total scientists.
	int getTotalScientists();
	/// Gets the base's available engineers.
	int getAvailableEngineers();
	/// Gets the base's total engineers.
	int getTotalEngineers();
	/// Gets the base's used living quarters.
	int getUsedQuarters();
	/// Gets the base's available living quarters.
	int getAvailableQuarters();
	/// Gets the base's used storage space.
	int getUsedStores();
	/// Gets the base's available storage space.
	int getAvailableStores();
	/// Gets the base's used laboratory space.
	int getUsedLaboratories();
	/// Gets the base's available laboratory space.
	int getAvailableLaboratories();
	/// Gets the base's used workshop space.
	int getUsedWorkshops();
	/// Gets the base's available workshop space.
	int getAvailableWorkshops();
	/// Gets the base's used hangars.
	int getUsedHangars();
	/// Gets the base's available hangars.
	int getAvailableHangars();
	/// Gets the base's defence value.
	int getDefenceValue();
	/// Gets the base's short range detection.
	int getShortRangeDetection();
	/// Gets the base's long range detection.
	int getLongRangeDetection();
	/// Gets the base's crafts of a certain type.
	int getCraftCount(LangString craft);
	/// Gets the base's craft maintenance.
	int getCraftMaintenance();
	/// Gets the base's personnel maintenance.
	int getPersonnelMaintenance();
	/// Gets the base's facility maintenance.
	int getFacilityMaintenance();
	/// Gets the base's total monthly maintenance.
	int getMonthlyMaintenace();
};

#endif
