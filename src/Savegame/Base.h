/*
 * Copyright 2010-2014 OpenXcom Developers.
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
class Vehicle;

/**
 * Represents a player base on the globe.
 * Bases can contain facilities, personnel, crafts and equipment.
 */
class Base : public Target
{
private:
	static const int BASE_SIZE = 6;
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
	bool _inBattlescape;
	bool _retaliationTarget;
	std::vector<Vehicle*> _vehicles;
	std::vector<BaseFacility*> _defenses;
	/// Determines space taken up by ammo clips about to rearm craft.
	double getIgnoredStores();
public:
	/// Creates a new base.
	Base(const Ruleset *rule);
	/// Cleans up the base.
	~Base();
	/// Loads the base from YAML.
	void load(const YAML::Node& node, SavedGame *save, bool newGame, bool newBattleGame = false);
	/// Saves the base to YAML.
	YAML::Node save() const;
	/// Saves the base's ID to YAML.
	YAML::Node saveId() const;
	/// Gets the base's name.
	std::wstring getName(Language* lang = 0) const;
	/// Sets the base's name.
	void setName(const std::wstring &name);
	/// Gets the base's marker.
	int getMarker() const;
	/// Gets the base's facilities.
	std::vector<BaseFacility*> *getFacilities();
	/// Gets the base's soldiers.
	std::vector<Soldier*> *getSoldiers();
	/// Gets the base's crafts.
	std::vector<Craft*> *getCrafts();
	/// Gets the base's transfers.
	std::vector<Transfer*> *getTransfers();
	/// Gets the base's items.
	ItemContainer *getItems();
	/// Gets the base's scientists.
	int getScientists() const;
	/// Sets the base's scientists.
	void setScientists(int scientists);
	/// Gets the base's engineers.
	int getEngineers() const;
	/// Sets the base's engineers.
	void setEngineers(int engineers);
	/// Checks if a target is detected by the base's radar.
	int detect(Target *target) const;
	/// Checks if a target is inside the base's radar range.
	int insideRadarRange(Target *target) const;
	/// Gets the base's available soldiers.
	int getAvailableSoldiers(bool checkCombatReadiness = false) const;
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
	double getUsedStores();
	/// Checks if the base's stores are overfull.
	bool storesOverfull(double offset = 0.0);
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
	/// Checks if this base is hyper-wave equipped.
	bool getHyperDetection() const;
	/// Gets the base's used psi lab space.
	int getUsedPsiLabs() const;
	/// Gets the base's total available psi lab space.
	int getAvailablePsiLabs() const;
	/// Gets the base's total free psi lab space.
	int getFreePsiLabs() const;
	/// Gets the total amount of Containment Space
	int getAvailableContainment() const;
	/// Gets the total amount of used Containment Space
	int getUsedContainment() const;
	/// Sets the craft's battlescape status.
	void setInBattlescape(bool inbattle);
	/// Gets if the craft is in battlescape.
	bool isInBattlescape() const;
	/// Mark this base for alien retaliation.
	void setRetaliationTarget(bool mark = true);
	/// Gets the retaliation status of this base.
	bool getRetaliationTarget() const;
	/// Get the detection chance for this base.
	size_t getDetectionChance(int difficulty) const;
	/// Gets how many Grav Shields the base has
	int getGravShields() const;
	/// Setup base defenses.
	void setupDefenses();
	/// Get a list of Defensive Facilities
	std::vector<BaseFacility*> *getDefenses();
	/// Gets the base's vehicles.
	std::vector<Vehicle*> *getVehicles();
	/// Destroys all disconnected facilities in the base.
	void destroyDisconnectedFacilities();
	/// Gets a sorted list of the facilities(=iterators) NOT connected to the Access Lift.
	std::list<std::vector<BaseFacility*>::iterator> getDisconnectedFacilities(BaseFacility *remove);
	/// destroy a facility and deal with the side effects.
	void destroyFacility(std::vector<BaseFacility*>::iterator facility);
	/// Get the number of a particular facility at this base
	int countAvailableFacilities(const std::string &facility) const;
};

}

#endif
