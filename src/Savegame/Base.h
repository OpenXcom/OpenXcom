/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_BASE_H
#define OPENXCOM_BASE_H

#include "Target.h"
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Ruleset;
class BaseFacility;
class Soldier;
class Craft;
class ItemContainer;
class Transfer;
class Language;
class Ruleset;
class SavedGame;
class ResearchProject;
class Production;

/**
 * Represents a player base on the globe.
 * Bases can contain facilities, personnel, crafts and equipment.
 */
class Base : public Target
{
private:
	const Ruleset *_rule;
	std::wstring _name;
	std::vector<BaseFacility*> _facilities;
	std::vector<Soldier*> _soldiers;
	std::vector<Craft*> _crafts;
	std::vector<Transfer*> _transfers;
	ItemContainer *_items;
	int _scientists, _engineers;
	std::vector<ResearchProject *> _research;
	std::vector<Production *> _productions;
public:
	/// Creates a new base.
	Base(const Ruleset *rule);
	/// Cleans up the base.
	~Base();
	/// Loads the base from YAML.
	void load(const YAML::Node& node, SavedGame *save, bool newGame);
	/// Saves the base to YAML.
	void save(YAML::Emitter& out) const;
	/// Saves the base's ID to YAML.
	void saveId(YAML::Emitter& out) const;
	/// Gets the base's name.
	std::wstring getName(Language* lang = 0) const;
	/// Sets the base's name.
	void setName(const std::wstring &name);
	/// Gets the base's facilities.
	std::vector<BaseFacility*> *const getFacilities();
	/// Gets the base's soldiers.
	std::vector<Soldier*> *const getSoldiers();
	/// Gets the base's crafts.
	std::vector<Craft*> *const getCrafts();
	/// Gets the base's transfers.
	std::vector<Transfer*> *const getTransfers();
	/// Gets the base's items.
	ItemContainer *const getItems();
	/// Gets the base's scientists.
	int getScientists() const;
	/// Sets the base's scientists.
	void setScientists(int scientists);
	/// Gets the base's engineers.
	int getEngineers() const;
	/// Sets the base's engineers.
	void setEngineers(int engineers);
	/// Checks if a target is detected by the base's radar.
	bool detect(Target *target) const;
	/// Checks if a target is inside the base's radar range.
	bool insideRadarRange(Target *target) const;
	/// Gets the base's available soldiers.
	int getAvailableSoldiers() const;
	/// Gets the base's total soldiers.
	int getTotalSoldiers() const;
	/// Gets the base's available scientists.
	int getAvailableScientists() const;
	/// Gets the base's total scientists.
	int getTotalScientists() const;
	/// Gets the base's available engineers.
	int getAvailableEngineers() const;
	/// Gets the base's total engineers.
	int getTotalEngineers() const;
	/// Gets the base's used living quarters.
	int getUsedQuarters() const;
	/// Gets the base's available living quarters.
	int getAvailableQuarters() const;
	/// Gets the base's used storage space.
	int getUsedStores() const;
	/// Gets the base's available storage space.
	int getAvailableStores() const;
	/// Gets the base's used laboratory space.
	int getUsedLaboratories() const;
	/// Gets the base's available laboratory space.
	int getAvailableLaboratories() const;
	/// Gets the base's used workshop space.
	int getUsedWorkshops() const;
	/// Gets the base's available workshop space.
	int getAvailableWorkshops() const;
	/// Gets the base's used hangars.
	int getUsedHangars() const;
	/// Gets the base's available hangars.
	int getAvailableHangars() const;
	/// Get the number of available space lab (not used by a ResearchProject)
	int getFreeLaboratories () const;
	/// Get the number of available space lab (not used by a Production)
	int getFreeWorkshops () const;

	int getAllocatedScientists() const;

	int getAllocatedEngineers() const;
	/// Gets the base's defense value.
	int getDefenseValue() const;
	/// Gets the base's short range detection.
	int getShortRangeDetection() const;
	/// Gets the base's long range detection.
	int getLongRangeDetection() const;
	/// Gets the base's crafts of a certain type.
	int getCraftCount(const std::string &craft) const;
	/// Gets the base's craft maintenance.
	int getCraftMaintenance() const;
	/// Gets the base's personnel maintenance.
	int getPersonnelMaintenance() const;
	/// Gets the base's facility maintenance.
	int getFacilityMaintenance() const;
	/// Gets the base's total monthly maintenance.
	int getMonthlyMaintenace() const;
	/// Get the list of base's ResearchProject
	const std::vector<ResearchProject *> & getResearch() const;
	/// Add a new ResearchProject to the Base
	void addResearch(ResearchProject *);
	/// Remove a ResearchProject from the Base
	void removeResearch(ResearchProject *);
	/// Add a new Production to Base
	void addProduction (Production * p);
	/// Remove a Base Production's
	void removeProduction (Production * p);
	/// Get the list of Base Production's
	const std::vector<Production *> & getProductions () const;
};

}

#endif