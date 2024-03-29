/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../fmath.h"
#include <stack>
#include <algorithm>
#include <functional>
#include "BaseFacility.h"
#include "../Mod/RuleBaseFacility.h"
#include "Craft.h"
#include "CraftWeapon.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleCraftWeapon.h"
#include "../Mod/Mod.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "../Engine/Language.h"
#include "../Mod/RuleItem.h"
#include "../Mod/Armor.h"
#include "../Mod/RuleManufacture.h"
#include "../Mod/RuleResearch.h"
#include "Transfer.h"
#include "ResearchProject.h"
#include "Production.h"
#include "Vehicle.h"
#include "Target.h"
#include "Ufo.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Mod/RuleSoldier.h"
#include "../Engine/Logger.h"

namespace OpenXcom
{

/**
 * Initializes an empty base.
 * @param mod Pointer to mod.
 */
Base::Base(const Mod *mod) : Target(), _mod(mod), _scientists(0), _engineers(0), _inBattlescape(false), _retaliationTarget(false)
{
	_items = new ItemContainer();
}

/**
 * Deletes the contents of the base from memory.
 */
Base::~Base()
{
	for (std::vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Soldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Craft*>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		for (std::vector<Vehicle*>::iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
			for (std::vector<Vehicle*>::iterator k = _vehicles.begin(); k != _vehicles.end(); ++k)
				if ((*k)==(*j)) { _vehicles.erase(k); break; } // to avoid calling a vehicle's destructor twice
		delete *i;
	}
	for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Production *>::iterator i = _productions.begin(); i != _productions.end(); ++i)
	{
		delete *i;
	}
	delete _items;
	for (std::vector<ResearchProject*>::iterator i = _research.begin(); i != _research.end(); ++i)
	{
		delete *i;
	}
	for (std::vector<Vehicle*>::iterator i = _vehicles.begin(); i != _vehicles.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the base from a YAML file.
 * @param node YAML node.
 * @param save Pointer to saved game.
 * @param newGame Is this the first base of a new game?
 * @param newBattleGame Is this the base of a skirmish game?
 */
void Base::load(const YAML::Node &node, SavedGame *save, bool newGame, bool newBattleGame)
{
	Target::load(node);

	if (!newGame || !Options::customInitialBase || newBattleGame)
	{
		for (YAML::const_iterator i = node["facilities"].begin(); i != node["facilities"].end(); ++i)
		{
			std::string type = (*i)["type"].as<std::string>();
			if (_mod->getBaseFacility(type))
			{
				BaseFacility *f = new BaseFacility(_mod->getBaseFacility(type), this);
				f->load(*i);
				_facilities.push_back(f);
			}
			else
			{
				Log(LOG_ERROR) << "Failed to load facility " << type;
			}
		}
	}

	for (YAML::const_iterator i = node["crafts"].begin(); i != node["crafts"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		if (_mod->getCraft(type))
		{
			Craft *c = new Craft(_mod->getCraft(type), this);
			c->load(*i, _mod, save);
			_crafts.push_back(c);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load craft " << type;
		}
	}

	for (YAML::const_iterator i = node["soldiers"].begin(); i != node["soldiers"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>(_mod->getSoldiersList().front());
		if (_mod->getSoldier(type))
		{
			Soldier *s = new Soldier(_mod->getSoldier(type), 0);
			s->load(*i, _mod, save);
			s->setCraft(0);
			if (const YAML::Node &craft = (*i)["craft"])
			{
				CraftId craftId = Craft::loadId(craft);
				for (std::vector<Craft*>::iterator j = _crafts.begin(); j != _crafts.end(); ++j)
				{
					if ((*j)->getUniqueId() == craftId)
					{
						s->setCraft(*j);
						break;
					}
				}
			}
			_soldiers.push_back(s);
		}
		else
		{
			Log(LOG_ERROR) << "Failed to load soldier " << type;
		}
	}

	_items->load(node["items"]);
	// Some old saves have bad items, better get rid of them to avoid further bugs
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end();)
	{
		if (_mod->getItem(i->first) == 0)
		{
			Log(LOG_ERROR) << "Failed to load item " << i->first;
			_items->getContents()->erase(i++);
		}
		else
		{
			++i;
		}
	}

	_scientists = node["scientists"].as<int>(_scientists);
	_engineers = node["engineers"].as<int>(_engineers);
	_inBattlescape = node["inBattlescape"].as<bool>(_inBattlescape);

	for (YAML::const_iterator i = node["transfers"].begin(); i != node["transfers"].end(); ++i)
	{
		int hours = (*i)["hours"].as<int>();
		Transfer *t = new Transfer(hours);
		if (t->load(*i, this, _mod, save))
		{
			_transfers.push_back(t);
		}
	}

	for (YAML::const_iterator i = node["research"].begin(); i != node["research"].end(); ++i)
	{
		std::string research = (*i)["project"].as<std::string>();
		if (_mod->getResearch(research))
		{
			ResearchProject *r = new ResearchProject(_mod->getResearch(research));
			r->load(*i);
			_research.push_back(r);
		}
		else
		{
			_scientists += (*i)["assigned"].as<int>(0);
			Log(LOG_ERROR) << "Failed to load research " << research;
		}
	}

	for (YAML::const_iterator i = node["productions"].begin(); i != node["productions"].end(); ++i)
	{
		std::string item = (*i)["item"].as<std::string>();
		if (_mod->getManufacture(item))
		{
			Production *p = new Production(_mod->getManufacture(item), 0);
			p->load(*i);
			_productions.push_back(p);
		}
		else
		{
			_engineers += (*i)["assigned"].as<int>(0);
			Log(LOG_ERROR) << "Failed to load manufacture " << item;
		}
	}

	_retaliationTarget = node["retaliationTarget"].as<bool>(_retaliationTarget);

	isOverlappingOrOverflowing(); // don't crash, just report in the log file...
}

/**
 * Tests whether the base facilities are within the base boundaries and not overlapping.
 * @return True if the base has a problem.
 */
bool Base::isOverlappingOrOverflowing()
{
	bool result = false;
	BaseFacility* grid[BASE_SIZE][BASE_SIZE];

	// i don't think i NEED to do this for a pointer array, but who knows what might happen on weird archaic linux distros if i don't?
	for (int x = 0; x < BASE_SIZE; ++x)
	{
		for (int y = 0; y < BASE_SIZE; ++y)
		{
			grid[x][y] = 0;
		}
	}

	for (std::vector<BaseFacility*>::iterator f = _facilities.begin(); f != _facilities.end(); ++f)
	{
		RuleBaseFacility *rules = (*f)->getRules();
		int facilityX = (*f)->getX();
		int facilityY = (*f)->getY();
		int facilitySize = rules->getSize();

		if (facilityX < 0 || facilityY < 0 || facilityX + (facilitySize - 1) >= BASE_SIZE || facilityY + (facilitySize - 1) >= BASE_SIZE)
		{
			Log(LOG_ERROR) << "Facility " << rules->getType() << " at [" << facilityX << ", " << facilityY << "] (size " << facilitySize << ") is outside of base boundaries.";
			result = true;
			continue;
		}

		for (int x = facilityX; x < facilityX + facilitySize; ++x)
		{
			for (int y = facilityY; y < facilityY + facilitySize; ++y)
			{
				if (grid[x][y] != 0)
				{
					Log(LOG_ERROR) << "Facility " << rules->getType() << " at [" << facilityX << ", " << facilityY << "] (size " << facilitySize
						<< ") overlaps with " << grid[x][y]->getRules()->getType() << " at [" << x << ", " << y << "] (size " << grid[x][y]->getRules()->getSize() << ")";
					result = true;
				}
				grid[x][y] = *f;
			}
		}
	}

	return result;
}

/**
 * Saves the base to a YAML file.
 * @return YAML node.
 */
YAML::Node Base::save() const
{
	YAML::Node node = Target::save();
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		node["facilities"].push_back((*i)->save());
	}
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		node["soldiers"].push_back((*i)->save());
	}
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		node["crafts"].push_back((*i)->save());
	}
	node["items"] = _items->save();
	node["scientists"] = _scientists;
	node["engineers"] = _engineers;
	if (_inBattlescape)
		node["inBattlescape"] = _inBattlescape;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		node["transfers"].push_back((*i)->save());
	}
	for (std::vector<ResearchProject*>::const_iterator i = _research.begin(); i != _research.end(); ++i)
	{
		node["research"].push_back((*i)->save());
	}
	for (std::vector<Production*>::const_iterator i = _productions.begin(); i != _productions.end(); ++i)
	{
		node["productions"].push_back((*i)->save());
	}
	if (_retaliationTarget)
		node["retaliationTarget"] = _retaliationTarget;
	return node;
}

/**
 * Returns the base's unique type used for
 * savegame purposes.
 * @return ID.
 */
std::string Base::getType() const
{
	return "STR_BASE";
}

/**
 * Returns the custom name for the base.
 * @param lang Language to get strings from (unused).
 * @return Name.
 */
std::string Base::getName(Language *) const
{
	return _name;
}

/**
 * Returns the globe marker for the base.
 * @return Marker sprite, -1 if none.
 */
int Base::getMarker() const
{
	// Cheap hack to hide bases when they haven't been placed yet
	if (AreSame(_lon, 0.0) && AreSame(_lat, 0.0))
		return -1;
	return 0;
}

/**
 * Returns the list of facilities in the base.
 * @return Pointer to the facility list.
 */
std::vector<BaseFacility*> *Base::getFacilities()
{
	return &_facilities;
}

/**
 * Returns the list of soldiers in the base.
 * @return Pointer to the soldier list.
 */
std::vector<Soldier*> *Base::getSoldiers()
{
	return &_soldiers;
}

/**
 * Returns the list of crafts in the base.
 * @return Pointer to the craft list.
 */
std::vector<Craft*> *Base::getCrafts()
{
	return &_crafts;
}

/**
 * Returns the list of transfers destined
 * to this base.
 * @return Pointer to the transfer list.
 */
std::vector<Transfer*> *Base::getTransfers()
{
	return &_transfers;
}

/**
 * Returns the list of items in the base storage rooms.
 * Does NOT return items assigned to craft or in transfer.
 * @return Pointer to the item list.
 */
ItemContainer *Base::getStorageItems()
{
	return _items;
}

/**
 * Returns the amount of scientists currently in the base.
 * @return Number of scientists.
 */
int Base::getScientists() const
{
	return _scientists;
}

/**
 * Changes the amount of scientists currently in the base.
 * @param scientists Number of scientists.
 */
void Base::setScientists(int scientists)
{
	 _scientists = scientists;
}

/**
 * Returns the amount of engineers currently in the base.
 * @return Number of engineers.
 */
int Base::getEngineers() const
{
	return _engineers;
}

/**
 * Changes the amount of engineers currently in the base.
 * @param engineers Number of engineers.
 */
void Base::setEngineers(int engineers)
{
	 _engineers = engineers;
}

/**
 * Returns if a certain target is covered by the base's
 * radar range, taking in account the range and chance.
 * @param target Pointer to target to compare.
 * @return 0 - not detected, 1 - detected by conventional radar, 2 - detected by hyper-wave decoder.
 */
int Base::detect(Target *target) const
{
	int chance = 0;
	double distance = getDistance(target) * 60.0 * (180.0 / M_PI);
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getRules()->getRadarRange() >= distance && (*i)->getBuildTime() == 0)
		{
			int radarChance = (*i)->getRules()->getRadarChance();
			if ((*i)->getRules()->isHyperwave())
			{
				if (radarChance == 100 || RNG::percent(radarChance))
				{
					return 2;
				}
			}
			else
			{
				chance += radarChance;
			}
		}
	}
	if (chance == 0) return 0;

	Ufo *u = dynamic_cast<Ufo*>(target);
	if (u != 0)
	{
		chance = chance * (100 + u->getVisibility()) / 100;
	}

	return RNG::percent(chance)? 1 : 0;
}

/**
 * Returns if a certain target is inside the base's
 * radar range, taking in account the positions of both.
 * @param target Pointer to target to compare.
 * @return 0 - outside radar range, 1 - inside conventional radar range, 2 - inside hyper-wave decoder range.
 */
int Base::insideRadarRange(Target *target) const
{
	bool insideRange = false;
	double distance = getDistance(target) * 60.0 * (180.0 / M_PI);
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getRules()->getRadarRange() >= distance && (*i)->getBuildTime() == 0)
		{
			if ((*i)->getRules()->isHyperwave())
			{
				return 2;
			}
			insideRange = true;
		}
	}

	return insideRange? 1 : 0;
}

/**
 * Returns the amount of soldiers contained
 * in the base without any assignments.
 * @param checkCombatReadiness does what it says on the tin.
 * @return Number of soldiers.
 */
int Base::getAvailableSoldiers(bool checkCombatReadiness) const
{
	int total = 0;
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		if (!checkCombatReadiness && (*i)->getCraft() == 0)
		{
			total++;
		}
		else if (checkCombatReadiness && (((*i)->getCraft() != 0 && (*i)->getCraft()->getStatus() != "STR_OUT") ||
			((*i)->getCraft() == 0 && (*i)->getWoundRecovery() == 0)))
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of soldiers contained
 * in the base.
 * @return Number of soldiers.
 */
int Base::getTotalSoldiers() const
{
	size_t total = _soldiers.size();
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_SOLDIER)
		{
			total += (*i)->getQuantity();
		}
	}
	return total;
}

/**
 * Returns the amount of scientists contained
 * in the base without any assignments.
 * @return Number of scientists.
 */
int Base::getAvailableScientists() const
{
	return getScientists();
}

/**
 * Returns the total amount of scientists contained
 * in the base.
 * @return Number of scientists.
 */
int Base::getTotalScientists() const
{
	int total = _scientists;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_SCIENTIST)
		{
			total += (*i)->getQuantity();
		}
	}
	const std::vector<ResearchProject *> & research (getResearch());
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin();
		 itResearch != research.end();
		 ++itResearch)
	{
		total += (*itResearch)->getAssigned();
	}
	return total;
}

/**
 * Returns the amount of engineers contained
 * in the base without any assignments.
 * @return Number of engineers.
 */
int Base::getAvailableEngineers() const
{
	return getEngineers();
}

/**
 * Returns the total amount of engineers contained
 * in the base.
 * @return Number of engineers.
 */
int Base::getTotalEngineers() const
{
	int total = _engineers;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_ENGINEER)
		{
			total += (*i)->getQuantity();
		}
	}
	for (std::vector<Production *>::const_iterator iter = _productions.begin(); iter != _productions.end(); ++iter)
	{
		total += (*iter)->getAssignedEngineers();
	}
	return total;
}

/**
 * Returns the amount of living quarters used up
 * by personnel in the base.
 * @return Living space.
 */
int Base::getUsedQuarters() const
{
	return getTotalSoldiers() + getTotalScientists() + getTotalEngineers();
}

/**
 * Returns the total amount of living quarters
 * available in the base.
 * @return Living space.
 */
int Base::getAvailableQuarters() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getPersonnel();
		}
	}
	return total;
}

/**
 * Returns the amount of stores used up by equipment in the base,
 * and equipment about to arrive.
 * @return Storage space.
 */
double Base::getUsedStores()
{
	double total = _items->getTotalSize(_mod);
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		total += (*i)->getItems()->getTotalSize(_mod);
		for (std::vector<Vehicle*>::const_iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
		{
			total += (*j)->getRules()->getSize();
		}
	}
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_ITEM)
		{
			total += (*i)->getQuantity() * _mod->getItem((*i)->getItems(), true)->getSize();
		}
		else if ((*i)->getType() == TRANSFER_CRAFT)
		{
			Craft *craft = (*i)->getCraft();
			total += craft->getItems()->getTotalSize(_mod);
		}
	}
	total -= getIgnoredStores();
	return total;
}

/**
 * Checks if the base's stores are overfull.
 *
 * Supplying an offset will add/subtract to the used capacity before performing the check.
 * A positive offset simulates adding items to the stores, whereas a negative offset
 * can be used to check whether sufficient items have been removed to stop the stores overflowing.
 * @param offset Adjusts the used capacity.
 * @return True if the base's stores are over their limit.
 */
bool Base::storesOverfull(double offset)
{
	int capacity = getAvailableStores() * 100;
	double used = (getUsedStores() + offset) * 100;
	return (int)used > capacity;
}

/**
 * Returns the total amount of stores
 * available in the base.
 * @return Storage space.
 */
int Base::getAvailableStores() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getStorage();
		}
	}
	return total;
}

/**
 * Determines space taken up by ammo clips about to rearm craft.
 * @return Ignored storage space.
 */
double Base::getIgnoredStores()
{
	double space = 0;
	for (std::vector<Craft*>::iterator c = getCrafts()->begin(); c != getCrafts()->end(); ++c)
	{
		if ((*c)->getStatus() == "STR_REARMING")
		{
			for (std::vector<CraftWeapon*>::iterator w = (*c)->getWeapons()->begin(); w != (*c)->getWeapons()->end() ; ++w)
			{
				if (*w != 0 && (*w)->isRearming())
				{
					std::string clip = (*w)->getRules()->getClipItem();
					int available = getStorageItems()->getItem(clip);
					if (!clip.empty() && available > 0)
					{
						int clipSize = _mod->getItem(clip, true)->getClipSize();
						int needed = 0;
						if (clipSize > 0)
						{
							needed = ((*w)->getRules()->getAmmoMax() - (*w)->getAmmo()) / clipSize;
						}
						space += std::min(available, needed) * _mod->getItem(clip, true)->getSize();
					}
				}
			}
		}
	}
	return space;
}

/**
 * Returns the amount of laboratories used up
 * by research projects in the base.
 * @return Laboratory space.
 */
int Base::getUsedLaboratories() const
{
	const std::vector<ResearchProject *> & research (getResearch());
	int usedLabSpace = 0;
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin();
		 itResearch != research.end();
		 ++itResearch)
	{
		usedLabSpace += (*itResearch)->getAssigned();
	}
	return usedLabSpace;
}

/**
 * Returns the total amount of laboratories
 * available in the base.
 * @return Laboratory space.
 */
int Base::getAvailableLaboratories() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getLaboratories();
		}
	}
	return total;
}

/**
 * Returns the amount of workshops used up
 * by manufacturing projects in the base.
 * @return Storage space.
 */
int Base::getUsedWorkshops() const
{
	int usedWorkShop = 0;
	for (std::vector<Production *>::const_iterator iter = _productions.begin(); iter != _productions.end(); ++iter)
	{
		usedWorkShop += ((*iter)->getAssignedEngineers() + (*iter)->getRules()->getRequiredSpace());
	}
	return usedWorkShop;
}

/**
 * Returns the total amount of workshops
 * available in the base.
 * @return Workshop space.
 */
int Base::getAvailableWorkshops() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getWorkshops();
		}
	}
	return total;
}

/**
 * Returns the amount of hangars used up
 * by crafts in the base.
 * @return Number of hangars.
 */
int Base::getUsedHangars() const
{
	size_t total = _crafts.size();
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_CRAFT)
		{
			total += (*i)->getQuantity();
		}
	}
	for (std::vector<Production*>::const_iterator i = _productions.begin(); i != _productions.end(); ++i)
	{
		if ((*i)->getRules()->getCategory() == "STR_CRAFT")
		{
			// This should be fixed on the case when (*i)->getInfiniteAmount() == TRUE
			total += ((*i)->getAmountTotal() - (*i)->getAmountProduced());
		}
	}
	return total;
}

/**
 * Returns the total amount of hangars
 * available in the base.
 * @return Number of hangars.
 */
int Base::getAvailableHangars() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getCrafts();
		}
	}
	return total;
}

/**
 * Return laboratories space not used by a ResearchProject
 * @return laboratories space not used by a ResearchProject
 */
int Base::getFreeLaboratories() const
{
	return getAvailableLaboratories() - getUsedLaboratories();
}

/**
 * Return workshop space not used by a Production
 * @return workshop space not used by a Production
 */
int Base::getFreeWorkshops() const
{
	return getAvailableWorkshops() - getUsedWorkshops();
}

/**
 * Return psilab space not in use
 * @return psilab space not in use
 */
int Base::getFreePsiLabs() const
{
	return getAvailablePsiLabs() - getUsedPsiLabs();
}

/**
 * Return containment space not in use
 * @return containment space not in use
 */
int Base::getFreeContainment() const
{
	return getAvailableContainment() - getUsedContainment();
}

/**
 * Returns the amount of scientists currently in use.
 * @return Amount of scientists.
 */
int Base::getAllocatedScientists() const
{
	int total = 0;
	const std::vector<ResearchProject *> & research (getResearch());
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin();
		 itResearch != research.end();
		 ++itResearch)
	{
		total += (*itResearch)->getAssigned();
	}
	return total;
}

/**
 * Returns the amount of engineers currently in use.
 * @return Amount of engineers.
 */
int Base::getAllocatedEngineers() const
{
	int total = 0;
	for (std::vector<Production *>::const_iterator iter = _productions.begin(); iter != _productions.end(); ++iter)
	{
		total += (*iter)->getAssignedEngineers();
	}
	return total;
}

/**
 * Returns the total defense value of all
 * the facilities in the base.
 * @return Defense value.
 */
int Base::getDefenseValue() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getDefenseValue();
		}
	}
	return total;
}

/**
 * Returns the total amount of short range
 * detection facilities in the base.
 * @return Defense value.
 */
int Base::getShortRangeDetection() const
{
	int total = 0;
	int minRadarRange = _mod->getMinRadarRange();

	if (minRadarRange == 0) return 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getRules()->getRadarRange() == minRadarRange && (*i)->getBuildTime() == 0)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of long range
 * detection facilities in the base.
 * @return Defense value.
 */
int Base::getLongRangeDetection() const
{
	int total = 0;
	int minRadarRange = _mod->getMinRadarRange();

	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getRules()->getRadarRange() > minRadarRange && (*i)->getBuildTime() == 0)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of craft of
 * a certain type stored in the base.
 * @param craft Craft type.
 * @return Number of craft.
 */
int Base::getCraftCount(const std::string &craft) const
{
	int total = 0;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_CRAFT && (*i)->getCraft()->getRules()->getType() == craft)
		{
			total++;
		}
	}
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		if ((*i)->getRules()->getType() == craft)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of monthly costs
 * for maintaining the craft in the base.
 * @return Maintenance costs.
 */
int Base::getCraftMaintenance() const
{
	int total = 0;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_CRAFT)
		{
			total += (*i)->getCraft()->getRules()->getRentCost();
		}
	}
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		total += (*i)->getRules()->getRentCost();
	}
	return total;
}

/**
 * Returns the total amount of soldiers of
 * a certain type stored in the base.
 * @param soldier Soldier type.
 * @return Number of soldiers.
 */
int Base::getSoldierCount(const std::string &soldier) const
{
	int total = 0;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_SOLDIER && (*i)->getSoldier()->getRules()->getType() == soldier)
		{
			total++;
		}
	}
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		if ((*i)->getRules()->getType() == soldier)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of monthly costs
 * for maintaining the personnel in the base.
 * @return Maintenance costs.
 */
int Base::getPersonnelMaintenance() const
{
	int total = 0;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_SOLDIER)
		{
			total += (*i)->getSoldier()->getRules()->getSalaryCost();
		}
	}
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		total += (*i)->getRules()->getSalaryCost();
	}
	total += getTotalEngineers() * _mod->getEngineerCost();
	total += getTotalScientists() * _mod->getScientistCost();
	return total;
}

/**
 * Returns the total amount of monthly costs
 * for maintaining the facilities in the base.
 * @return Maintenance costs.
 */
int Base::getFacilityMaintenance() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getMonthlyCost();
		}
	}
	return total;
}

/**
 * Returns the total amount of all the maintenance
 * monthly costs in the base.
 * @return Maintenance costs.
 */
int Base::getMonthlyMaintenace() const
{
	return getCraftMaintenance() + getPersonnelMaintenance() + getFacilityMaintenance();
}

/**
 * Returns the list of all base's ResearchProject
 * @return list of base's ResearchProject
 */
const std::vector<ResearchProject *> & Base::getResearch() const
{
	return _research;
}

/**
 * Add a new Production to the Base
 * @param p A pointer to a Production
 */
void Base::addProduction (Production * p)
{
	_productions.push_back(p);
}

/**
 * Add A new ResearchProject to Base
 * @param project The project to add
 */
void Base::addResearch(ResearchProject * project)
{
	_research.push_back(project);
}

/**
 * Remove a ResearchProject from base
 * @param project the project to remove
 */
void Base::removeResearch(ResearchProject * project)
{
	_scientists += project->getAssigned();
	std::vector<ResearchProject *>::iterator iter = std::find (_research.begin(), _research.end(), project);
	if (iter != _research.end())
	{
		_research.erase(iter);
	}

	const RuleResearch *ruleResearch = project->getRules();
	if (!project->isFinished())
	{
		if (ruleResearch->needItem() && ruleResearch->destroyItem())
		{
			getStorageItems()->addItem(ruleResearch->getName(), 1);
		}
	}
	delete project;
}

/**
 * Remove a Production from the Base
 * @param p A pointer to a Production
 */
void Base::removeProduction(Production * p)
{
	_engineers += p->getAssignedEngineers();
	std::vector<Production *>::iterator iter = std::find (_productions.begin(), _productions.end(), p);
	if (iter != _productions.end())
	{
		_productions.erase(iter);
	}
	delete p;
}

/**
 * Get the list of Base Production's
 * @return the list of Base Production's
 */
const std::vector<Production *> & Base::getProductions() const
{
	return _productions;
}

/**
 * Returns the total amount of Psi Lab Space
 * available in the base.
 * @return Psi Lab space.
 */
int Base::getAvailablePsiLabs() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getPsiLaboratories();
		}
	}
	return total;
}

/**
 * Returns the total amount of used
 * Psi Lab Space in the base.
 * @return used Psi Lab space.
 */
int Base::getUsedPsiLabs() const
{
	int total = 0;
	for (std::vector<Soldier*>::const_iterator s = _soldiers.begin(); s != _soldiers.end(); ++s)
	{
		if ((*s)->isInPsiTraining())
		{
			total ++;
		}
	}
	return total;
}

/**
 * Returns the total amount of used
 * Containment Space in the base.
 * @return Containment Lab space.
 */
int Base::getUsedContainment() const
{
	int total = 0;
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end(); ++i)
	{
		if (_mod->getItem((i)->first, true)->isAlien())
		{
			total += (i)->second;
		}
	}
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_ITEM)
		{
			if (_mod->getItem((*i)->getItems(), true)->isAlien())
			{
				total += (*i)->getQuantity();
			}
		}
	}
	for (std::vector<ResearchProject*>::const_iterator i = _research.begin(); i != _research.end(); ++i)
	{
		const RuleResearch *projRules = (*i)->getRules();
		if (projRules->needItem() && _mod->getUnit(projRules->getName()))
		{
			++total;
		}
	}
	return total;
}

/**
 * Returns the total amount of Containment Space
 * available in the base.
 * @return Containment Lab space.
 */
int Base::getAvailableContainment() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getAliens();
		}
	}
	return total;
}

/**
 * Returns the base's battlescape status.
 * @return Is the craft on the battlescape?
 */
bool Base::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Changes the base's battlescape status.
 * @param inbattle True if it's in battle, False otherwise.
 */
void Base::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

/**
 * Mark the base as a valid alien retaliation target.
 * @param mark Mark (if @c true) or unmark (if @c false) the base.
 */
void Base::setRetaliationTarget(bool mark)
{
	_retaliationTarget = mark;
}

/**
 * Get the base's retaliation status.
 * @return If the base is a valid target for alien retaliation.
 */
bool Base::getRetaliationTarget() const
{
	return _retaliationTarget;
}

/**
 * Functor to check for mind shield capability.
 */
struct isMindShield: public std::unary_function<BaseFacility*, bool>
{
	/// Check isMindShield() for @a facility.
	bool operator()(const BaseFacility *facility) const;
};

/**
 * Only fully operational facilities are checked.
 * @param facility Pointer to the facility to check.
 * @return If @a facility can act as a mind shield.
 */
bool isMindShield::operator()(const BaseFacility *facility) const
{
	if (facility->getBuildTime() != 0)
	{
		// Still building this
		return false;
	}
	return (facility->getRules()->isMindShield());
}

/**
 * Functor to check for completed facilities.
 */
struct isCompleted: public std::unary_function<BaseFacility*, bool>
{
	/// Check isCompleted() for @a facility.
	bool operator()(const BaseFacility *facility) const;
};

/**
 * Facilities are checked for construction completion.
 * @param facility Pointer to the facility to check.
 * @return If @a facility has completed construction.
 */
bool isCompleted::operator()(const BaseFacility *facility) const
{
	return (facility->getBuildTime() == 0);
}

/**
 * Calculate the detection chance of this base.
 * Big bases without mindshields are easier to detect.
 * @param difficulty The savegame difficulty.
 * @return The detection chance.
 */
size_t Base::getDetectionChance() const
{
	size_t mindShields = std::count_if (_facilities.begin(), _facilities.end(), isMindShield());
	size_t completedFacilities = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			completedFacilities += (*i)->getRules()->getSize() * (*i)->getRules()->getSize();
		}
	}
	return ((completedFacilities / 6 + 15) / (mindShields + 1));
}

int Base::getGravShields() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->isGravShield())
		{
			++total;
		}
	}
	return total;
}

void Base::setupDefenses()
{
	_defenses.clear();
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getDefenseValue())
		{
			_defenses.push_back(*i);
		}
	}

	for (std::vector<Craft*>::iterator i = getCrafts()->begin(); i != getCrafts()->end(); ++i)
		for (std::vector<Vehicle*>::iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
			for (std::vector<Vehicle*>::iterator k = _vehicles.begin(); k != _vehicles.end(); ++k)
				if ((*k)==(*j)) { _vehicles.erase(k); break; } // to avoid calling a vehicle's destructor for tanks on crafts

	for (std::vector<Vehicle*>::iterator i = _vehicles.begin(); i != _vehicles.end();)
	{
		delete *i;
		i = _vehicles.erase(i);
	}

	// add vehicles that are in the crafts of the base, if it's not out
	for (std::vector<Craft*>::iterator c = getCrafts()->begin(); c != getCrafts()->end(); ++c)
	{
		if ((*c)->getStatus() != "STR_OUT")
		{
			for (std::vector<Vehicle*>::iterator i = (*c)->getVehicles()->begin(); i != (*c)->getVehicles()->end(); ++i)
			{
				_vehicles.push_back(*i);
			}
		}
	}

	// add vehicles left on the base
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end(); )
	{
		std::string itemId = (i)->first;
		int itemQty = (i)->second;
		RuleItem *rule = _mod->getItem(itemId, true);
		if (rule->isFixed())
		{
			int size = 4;
			if (_mod->getUnit(itemId))
			{
				size = _mod->getArmor(_mod->getUnit(itemId)->getArmor(), true)->getSize();
			}
			if (rule->getCompatibleAmmo()->empty()) // so this vehicle does not need ammo
			{
				for (int j = 0; j < itemQty; ++j)
				{
					_vehicles.push_back(new Vehicle(rule, rule->getClipSize(), size));
				}
				_items->removeItem(itemId, itemQty);
			}
			else // so this vehicle needs ammo
			{
				RuleItem *ammo = _mod->getItem(rule->getCompatibleAmmo()->front(), true);
				int ammoPerVehicle, clipSize;
				if (ammo->getClipSize() > 0 && rule->getClipSize() > 0)
				{
					clipSize = rule->getClipSize();
					ammoPerVehicle = clipSize / ammo->getClipSize();
				}
				else
				{
					clipSize = ammo->getClipSize();
					ammoPerVehicle = clipSize;
				}
				int baseQty = _items->getItem(ammo->getType()) / ammoPerVehicle;
				if (!baseQty)
				{
					++i;
					continue;
				}
				int canBeAdded = std::min(itemQty, baseQty);
				for (int j=0; j<canBeAdded; ++j)
				{
					_vehicles.push_back(new Vehicle(rule, clipSize, size));
					_items->removeItem(ammo->getType(), ammoPerVehicle);
				}
				_items->removeItem(itemId, canBeAdded);
			}

			i = _items->getContents()->begin(); // we have to start over because iterator is broken because of the removeItem
		}
		else ++i;
	}
}

std::vector<BaseFacility*> *Base::getDefenses()
{
	return &_defenses;
}

/**
 * Returns the list of vehicles currently equipped
 * in the base.
 * @return Pointer to vehicle list.
 */
std::vector<Vehicle*> *Base::getVehicles()
{
	return &_vehicles;
}

/**
 * Destroys all disconnected facilities in the base.
 */
void Base::destroyDisconnectedFacilities()
{
	std::list<std::vector<BaseFacility*>::iterator> disFacs = getDisconnectedFacilities(0);
	for (std::list<std::vector<BaseFacility*>::iterator>::reverse_iterator i = disFacs.rbegin(); i != disFacs.rend(); ++i)
	{
		destroyFacility(*i);
	}
}

/**
 * Gets a sorted list of the facilities(=iterators) NOT connected to the Access Lift.
 * @param remove Facility to ignore (in case of facility dismantling).
 * @return a sorted list of iterators pointing to elements in _facilities.
 */
std::list<std::vector<BaseFacility*>::iterator> Base::getDisconnectedFacilities(BaseFacility *remove)
{
	std::list<std::vector<BaseFacility*>::iterator> result;

	if (remove != 0 && remove->getRules()->isLift())
	{ // Theoretically this is impossible, but sanity check is good :)
		for (std::vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
		{
			if ((*i) != remove) result.push_back(i);
		}
		return result;
	}

	std::vector<std::pair<std::vector<BaseFacility*>::iterator, bool>*> facilitiesConnStates;
	std::pair<std::vector<BaseFacility*>::iterator, bool> *grid[BASE_SIZE][BASE_SIZE];
	BaseFacility *lift = 0;

	for (int x = 0; x < BASE_SIZE; ++x)
	{
		for (int y = 0; y < BASE_SIZE; ++y)
		{
			grid[x][y] = 0;
		}
	}

	// Ok, fill up the grid(+facilitiesConnStates), and search the lift
	for (std::vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i) != remove)
		{
			if ((*i)->getRules()->isLift()) lift = (*i);
			for (int x = 0; x != (*i)->getRules()->getSize(); ++x)
			{
				for (int y = 0; y != (*i)->getRules()->getSize(); ++y)
				{
					std::pair<std::vector<BaseFacility*>::iterator, bool> *p = new std::pair<std::vector<BaseFacility*>::iterator, bool>(i,false);
					facilitiesConnStates.push_back(p);
					grid[(*i)->getX() + x][(*i)->getY() + y] = p;
				}
			}
		}
	}

	// we're in real trouble if this happens...
	if (lift == 0)
	{
		//TODO: something clever.
		return result;
	}

	// Now make the recursion manually using a stack
	std::stack<std::pair<int, int> > stack;
	stack.push(std::make_pair(lift->getX(),lift->getY()));
	while (!stack.empty())
	{
		int x = stack.top().first, y = stack.top().second;
		stack.pop();
		if (x >= 0 && x < BASE_SIZE && y >= 0 && y < BASE_SIZE && grid[x][y] != 0 && !grid[x][y]->second)
		{
			grid[x][y]->second = true;
			BaseFacility *fac = *(grid[x][y]->first);
			BaseFacility *neighborLeft = (x-1 >= 0 && grid[x-1][y] != 0) ? *(grid[x-1][y]->first) : 0;
			BaseFacility *neighborRight = (x+1 < BASE_SIZE && grid[x+1][y] != 0) ? *(grid[x+1][y]->first) : 0;
			BaseFacility *neighborTop = (y-1 >= 0 && grid[x][y-1] != 0) ? *(grid[x][y-1]->first) : 0;
			BaseFacility *neighborBottom= (y+1 < BASE_SIZE && grid[x][y+1] != 0) ? *(grid[x][y+1]->first) : 0;
			if ((fac->getBuildTime() == 0) || (neighborLeft != 0 && (neighborLeft == fac || neighborLeft->getBuildTime() > neighborLeft->getRules()->getBuildTime()))) stack.push(std::make_pair(x-1,y));
			if ((fac->getBuildTime() == 0) || (neighborRight != 0 && (neighborRight == fac || neighborRight->getBuildTime() > neighborRight->getRules()->getBuildTime()))) stack.push(std::make_pair(x+1,y));
			if ((fac->getBuildTime() == 0) || (neighborTop != 0 && (neighborTop == fac || neighborTop->getBuildTime() > neighborTop->getRules()->getBuildTime()))) stack.push(std::make_pair(x,y-1));
			if ((fac->getBuildTime() == 0) || (neighborBottom != 0 && (neighborBottom == fac || neighborBottom->getBuildTime() > neighborBottom->getRules()->getBuildTime()))) stack.push(std::make_pair(x,y+1));
		}
	}

	BaseFacility *lastFacility = 0;
	for (std::vector<std::pair<std::vector<BaseFacility*>::iterator, bool>*>::iterator i = facilitiesConnStates.begin(); i != facilitiesConnStates.end(); ++i)
	{
		// Not a connected fac.? -> push its iterator into the list!
		// Oh, and we don't want duplicates (facilities with bigger sizes like hangar)
		if (*((*i)->first) != lastFacility && !(*i)->second) result.push_back((*i)->first);
		lastFacility = *((*i)->first);
		delete *i; // We don't need the pair anymore.
	}

	return result;
}

/**
 * Removes a base module, and deals with the ramifications thereof.
 * @param facility An iterator reference to the facility to destroy and remove.
 */
void Base::destroyFacility(std::vector<BaseFacility*>::iterator facility)
{
	if ((*facility)->getRules()->getCrafts() > 0)
	{
		// hangar destruction - destroy crafts and any production of crafts
		// if this will mean there is no hangar to contain it
		if ((*facility)->getCraft())
		{
			// remove all soldiers
			if ((*facility)->getCraft()->getNumSoldiers())
			{
				for (std::vector<Soldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
				{
					if ((*i)->getCraft() == (*facility)->getCraft())
					{
						(*i)->setCraft(0);
					}
				}
			}
			// remove all items
			while (!(*facility)->getCraft()->getItems()->getContents()->empty())
			{
				std::map<std::string, int>::iterator i = (*facility)->getCraft()->getItems()->getContents()->begin();
				_items->addItem(i->first, i->second);
				(*facility)->getCraft()->getItems()->removeItem(i->first, i->second);
			}
			for (std::vector<Craft*>::iterator i = _crafts.begin(); i != _crafts.end(); ++i)
			{
				if (*i == (*facility)->getCraft())
				{
					delete (*i);
					_crafts.erase(i);
					break;
				}
			}
		}
		else
		{
			bool remove = true;
			// no craft - check productions.
			for (std::vector<Production*>::iterator i = _productions.begin(); i != _productions.end(); ++i)
			{
				if (getAvailableHangars() - getUsedHangars() - (*facility)->getRules()->getCrafts() < 0 && (*i)->getRules()->getCategory() == "STR_CRAFT")
				{
					_engineers += (*i)->getAssignedEngineers();
					delete *i;
					_productions.erase(i);
					remove = false;
					break;
				}
			}
			if (remove && !_transfers.empty())
			{
				for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); ++i)
				{
					if ((*i)->getType() == TRANSFER_CRAFT)
					{
						delete (*i)->getCraft();
						delete *i;
						_transfers.erase(i);
						break;
					}
				}
			}
		}
	}
	if ((*facility)->getRules()->getPsiLaboratories() > 0)
	{
		// psi lab destruction: remove any soldiers over the maximum allowable from psi training.
		int toRemove = (*facility)->getRules()->getPsiLaboratories() - getFreePsiLabs();
		for (std::vector<Soldier*>::iterator i = _soldiers.begin(); i != _soldiers.end() && toRemove > 0; ++i)
		{
			if ((*i)->isInPsiTraining())
			{
				(*i)->setPsiTraining(false);
				--toRemove;
			}
		}
	}
	if ((*facility)->getRules()->getLaboratories())
	{
		// lab destruction: enforce lab space limits. take scientists off projects until
		// it all evens out. research is not cancelled.
		int toRemove = (*facility)->getRules()->getLaboratories() - getFreeLaboratories();
		for (std::vector<ResearchProject*>::iterator i = _research.begin(); i != _research.end() && toRemove > 0;)
		{
			if ((*i)->getAssigned() >= toRemove)
			{
				(*i)->setAssigned((*i)->getAssigned() - toRemove);
				_scientists += toRemove;
				break;
			}
			else
			{
				toRemove -= (*i)->getAssigned();
				_scientists += (*i)->getAssigned();
				(*i)->setAssigned(0);
				++i;
			}
		}
	}
	if ((*facility)->getRules()->getWorkshops())
	{
		// workshop destruction: similar to lab destruction, but we'll lay off engineers instead
		// in this case, however, production IS cancelled, as it takes up space in the workshop.
		int toRemove = (*facility)->getRules()->getWorkshops() - getFreeWorkshops();
		for (std::vector<Production*>::iterator i = _productions.begin(); i != _productions.end() && toRemove > 0;)
		{
			if ((*i)->getAssignedEngineers() > toRemove)
			{
				(*i)->setAssignedEngineers((*i)->getAssignedEngineers() - toRemove);
				_engineers += toRemove;
				break;
			}
			else
			{
				toRemove -= (*i)->getAssignedEngineers();
				_engineers += (*i)->getAssignedEngineers();
				delete *i;
				i = _productions.erase(i);
			}
		}
	}
	if ((*facility)->getRules()->getStorage())
	{
		// we won't destroy the items physically AT the base,
		// but any items in transit will end up at the dead letter office.
		if (storesOverfull((*facility)->getRules()->getStorage()) && !_transfers.empty())
		{
			for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); )
			{
				if ((*i)->getType() == TRANSFER_ITEM)
				{
					delete *i;
					i = _transfers.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
	}
	if ((*facility)->getRules()->getPersonnel())
	{
		// as above, we won't actually fire people, but we'll block any new ones coming in.
		if ((getAvailableQuarters() - getUsedQuarters()) - (*facility)->getRules()->getPersonnel() < 0 && !_transfers.empty())
		{
			for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); )
			{
				// let soldiers arrive, but block workers.
				if ((*i)->getType() == TRANSFER_ENGINEER || (*i)->getType() == TRANSFER_SCIENTIST)
				{
					delete *i;
					i = _transfers.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
	}
	delete *facility;
	_facilities.erase(facility);
}

/**
 * Cleans up the defenses vector and optionally reclaims the tanks and their ammo.
 * @param reclaimItems determines whether the HWPs should be returned to storage.
 */
void Base::cleanupDefenses(bool reclaimItems)
{
	_defenses.clear();

	for (std::vector<Craft*>::iterator i = getCrafts()->begin(); i != getCrafts()->end(); ++i)
		for (std::vector<Vehicle*>::iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
			for (std::vector<Vehicle*>::iterator k = _vehicles.begin(); k != _vehicles.end(); ++k)
				if ((*k)==(*j)) { _vehicles.erase(k); break; } // to avoid calling a vehicle's destructor for tanks on crafts

	for (std::vector<Vehicle*>::iterator i = _vehicles.begin(); i != _vehicles.end();)
	{
		if (reclaimItems)
		{
			RuleItem *rule = (*i)->getRules();
			std::string type = rule->getType();
			_items->addItem(type);
			if (!rule->getCompatibleAmmo()->empty())
			{
				RuleItem *ammo = _mod->getItem(rule->getCompatibleAmmo()->front(), true);
				int ammoPerVehicle;
				if (ammo->getClipSize() > 0 && rule->getClipSize() > 0)
				{
					ammoPerVehicle = rule->getClipSize() / ammo->getClipSize();
				}
				else
				{
					ammoPerVehicle = ammo->getClipSize();
				}
				_items->addItem(ammo->getType(), ammoPerVehicle);
			}
		}
		delete *i;
		i = _vehicles.erase(i);
	}
}

/**
 * Removes the craft and all associations from the base (does not destroy it!).
 * @param craft Pointer to craft.
 * @param unload Unload craft contents before removing.
 */
std::vector<Craft*>::iterator Base::removeCraft(Craft *craft, bool unload)
{
	// Unload craft
	if (unload)
	{
		craft->unload(_mod);
	}

	// Clear hangar
	for (std::vector<BaseFacility*>::iterator f = _facilities.begin(); f != _facilities.end(); ++f)
	{
		if ((*f)->getCraft() == craft)
		{
			(*f)->setCraft(0);
			break;
		}
	}

	// Remove craft
	std::vector<Craft*>::iterator c;
	for (c = _crafts.begin(); c != _crafts.end(); ++c)
	{
		if (*c == craft)
		{
			return _crafts.erase(c);
		}
	}
	return c;
}

}
