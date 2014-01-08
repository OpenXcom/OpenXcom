/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#define _USE_MATH_DEFINES
#include "Base.h"
#include <cmath>
#include <algorithm>
#include "BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/Ruleset.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "../Engine/Language.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Ruleset/RuleResearch.h"
#include "Transfer.h"
#include "ResearchProject.h"
#include "Production.h"
#include "Vehicle.h"
#include "Target.h"
#include "Ufo.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes an empty base.
 * @param rule Pointer to ruleset.
 */
Base::Base(const Ruleset *rule) : Target(), _rule(rule), _name(L""), _scientists(0), _engineers(0), _inBattlescape(false), _retaliationTarget(false)
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
	for (std::vector<Production *>::iterator i = _productions.begin (); i != _productions.end (); ++i)
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
 */
void Base::load(const YAML::Node &node, SavedGame *save, bool newGame, bool newBattleGame)
{
	Target::load(node);
	_name = Language::utf8ToWstr(node["name"].as<std::string>(""));

	if (!newGame || !Options::getBool("customInitialBase") || newBattleGame)
	{
		for (YAML::const_iterator i = node["facilities"].begin(); i != node["facilities"].end(); ++i)
		{
			std::string type = (*i)["type"].as<std::string>();
			BaseFacility *f = new BaseFacility(_rule->getBaseFacility(type), this);
			f->load(*i);
			_facilities.push_back(f);
		}
	}

	for (YAML::const_iterator i = node["crafts"].begin(); i != node["crafts"].end(); ++i)
	{
		std::string type = (*i)["type"].as<std::string>();
		Craft *c = new Craft(_rule->getCraft(type), this);
		c->load(*i, _rule, save);
		_crafts.push_back(c);
	}

	for (YAML::const_iterator i = node["soldiers"].begin(); i != node["soldiers"].end(); ++i)
	{
		Soldier *s = new Soldier(_rule->getSoldier("XCOM"), _rule->getArmor("STR_NONE_UC"));
		s->load(*i, _rule);
		if (const YAML::Node &craft = (*i)["craft"])
		{
			std::string type = craft["type"].as<std::string>();
			int id = craft["id"].as<int>();
			for (std::vector<Craft*>::iterator j = _crafts.begin(); j != _crafts.end(); ++j)
			{
				if ((*j)->getRules()->getType() == type && (*j)->getId() == id)
				{
					s->setCraft(*j);
					break;
				}
			}
		}
		else
		{
			s->setCraft(0);
		}
		_soldiers.push_back(s);
	}

	_items->load(node["items"]);
	// Some old saves have bad items, better get rid of them to avoid further bugs
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end();)
	{
		if (std::find(_rule->getItemsList().begin(), _rule->getItemsList().end(), i->first) == _rule->getItemsList().end())
		{
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
		t->load(*i, this, _rule);
		_transfers.push_back(t);
	}

	for (YAML::const_iterator i = node["research"].begin(); i != node["research"].end(); ++i)
	{
		std::string research = (*i)["project"].as<std::string>();
		ResearchProject *r = new ResearchProject(_rule->getResearch(research));
		r->load(*i);
		_research.push_back(r);
	}

	for (YAML::const_iterator i = node["productions"].begin(); i != node["productions"].end(); ++i)
	{
		std::string item = (*i)["item"].as<std::string>();
		Production *p = new Production(_rule->getManufacture(item), 0);
		p->load(*i);
		_productions.push_back(p);
	}

	_retaliationTarget = node["retaliationTarget"].as<bool>(_retaliationTarget);
}

/**
 * Saves the base to a YAML file.
 * @return YAML node.
 */
YAML::Node Base::save() const
{
	YAML::Node node = Target::save();
	node["name"] = Language::wstrToUtf8(_name);
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
	node["retaliationTarget"] = _retaliationTarget;
	return node;
}

/**
 * Saves the base's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node Base::saveId() const
{
	YAML::Node node = Target::saveId();
	node["type"] = "STR_BASE";
	node["id"] = 0;
	return node;
}

/**
 * Returns the custom name for the base.
 * @param lang Language to get strings from.
 * @return Name.
 */
std::wstring Base::getName(Language *) const
{
	return _name;
}

/**
 * Changes the custom name for the base.
 * @param name Name.
 */
void Base::setName(const std::wstring &name)
{
	_name = name;
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
 * Returns the list of items in the base.
 * @return Pointer to the item list.
 */
ItemContainer *Base::getItems()
{
	return _items;
}

/**
 * Returns the number of scientists currently in the base.
 * @return Number of scientists.
 */
int Base::getScientists() const
{
	return _scientists;
}

/**
 * Changes the number of scientists currently in the base.
 * @param scientists Number of scientists.
 */
void Base::setScientists(int scientists)
{
	 _scientists = scientists;
}

/**
 * Returns the number of engineers currently in the base.
 * @return Number of engineers.
 */
int Base::getEngineers() const
{
	return _engineers;
}

/**
 * Changes the number of engineers currently in the base.
 * @param engineers Number of engineers.
 */
void Base::setEngineers(int engineers)
{
	 _engineers = engineers;
}

/**
 * Returns if a certain target is covered by the base's
 * radar range, taking into account the range and chance.
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
			if ((*i)->getRules()->isHyperwave())
			{
				return 2;
			}
			chance += (*i)->getRules()->getRadarChance();
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
 * radar range, taking into account the positions of both.
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
 * @param checkCombatReadiness Does what it says on the tin.
 * @return The number of soldiers.
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
 * @return The number of soldiers.
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
 * @return The number of unassigned scientists.
 */
int Base::getAvailableScientists() const
{
	return getScientists();
}

/**
 * Returns the total amount of scientists contained
 * in the base.
 * @return The number of scientists.
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
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin ();
		 itResearch != research.end ();
		 ++itResearch)
	{
		total += (*itResearch)->getAssigned ();
	}
	return total;
}

/**
 * Returns the amount of engineers contained
 * in the base without any assignments.
 * @return The number of unassigned engineers.
 */
int Base::getAvailableEngineers() const
{
	return getEngineers();
}

/**
 * Returns the total number of engineers contained
 * in the base.
 * @return The number of engineers.
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
	for (std::vector<Production *>::const_iterator iter = _productions.begin (); iter != _productions.end (); ++iter)
	{
		total += (*iter)->getAssignedEngineers();
	}
	return total;
}

/**
 * Returns the amount of living quarters used up
 * by personnel in the base.
 * @return The amount of used living space.
 */
int Base::getUsedQuarters() const
{
	return getTotalSoldiers() + getTotalScientists() + getTotalEngineers();
}

/**
 * Returns the total amount of living quarters
 * available in the base.
 * @return The amount of available living space.
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
 * Returns the amount of stores used up
 * by equipment in the base.
 * @return The amount of used storage space.
 */
int Base::getUsedStores() const
{
	double total = _items->getTotalSize(_rule);
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		total += (*i)->getItems()->getTotalSize(_rule);
		for (std::vector<Vehicle*>::const_iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
		{
			total += (*j)->getRules()->getSize();
		}
	}
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_ITEM)
		{
			total += (*i)->getQuantity() * _rule->getItem((*i)->getItems())->getSize();
		}
	}
	return (int)floor(total);
}

/**
 * Returns the total amount of stores
 * available in the base.
 * @return The amount of storage space.
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
 * Returns the amount of laboratory space used up
 * by research projects in the base.
 * @return The amount of aboratory space.
 */
int Base::getUsedLaboratories() const
{
	const std::vector<ResearchProject *> & research (getResearch());
	int usedLabSpace = 0;
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin ();
		 itResearch != research.end ();
		 ++itResearch)
	{
		usedLabSpace += (*itResearch)->getAssigned ();
	}
	return usedLabSpace;
}

/**
 * Returns the total amount of laboratory space
 * available in the base.
 * @return The amount of available laboratory space.
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
 * Returns the amount of workshop space used up
 * by manufacturing projects in the base.
 * @return The amount of used workshop space.
 */
int Base::getUsedWorkshops() const
{
	int usedWorkShop = 0;
	for (std::vector<Production *>::const_iterator iter = _productions.begin (); iter != _productions.end (); ++iter)
	{
		usedWorkShop += ((*iter)->getAssignedEngineers() + (*iter)->getRules()->getRequiredSpace ());
	}
	return usedWorkShop;
}

/**
 * Returns the total amount of workshop space
 * available in the base.
 * @return The amount of available workshop space.
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
 * Returns the numbers of hangars used up
 * by craft in the base.
 * @return The number of used hangars.
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
			total += ((*i)->getAmountTotal() - (*i)->getAmountProduced());
		}
	}
	return total;
}

/**
 * Returns the total number of hangars
 * available in the base.
 * @return The number of hangars.
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
 * Returns the laboratory space not used by a ResearchProject.
 * @return The unused laboratory space.
 */
int Base::getFreeLaboratories () const
{
	return getAvailableLaboratories() - getUsedLaboratories();
}

/**
 * Returns the workshop space not used by a Production.
 * @return The unused workshop space.
 */
int Base::getFreeWorkshops () const
{
	return getAvailableWorkshops() - getUsedWorkshops();
}

/**
 * Returns the number of scientists currently in use.
 * @return The number of scientists.
 */
int Base::getAllocatedScientists() const
{
	int total = 0;
	const std::vector<ResearchProject *> & research (getResearch());
	for (std::vector<ResearchProject *>::const_iterator itResearch = research.begin ();
		 itResearch != research.end ();
		 ++itResearch)
	{
		total += (*itResearch)->getAssigned ();
	}
	return total;
}

/**
 * Returns the number of engineers currently in use.
 * @return The number of engineers.
 */
int Base::getAllocatedEngineers() const
{
	int total = 0;
	for (std::vector<Production *>::const_iterator iter = _productions.begin (); iter != _productions.end (); ++iter)
	{
		total += (*iter)->getAssignedEngineers();
	}
	return total;
}

/**
 * Returns the total defense value of all
 * the facilities in the base.
 * @return The defense value.
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
 * @return The short range dectection value.
 */
int Base::getShortRangeDetection() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getRadarRange() == 1500)
		{
			total++;
		}
	}
	return total;
}

/**
 * Returns the total amount of long range
 * detection facilities in the base.
 * @return The long range detection value.
 */
int Base::getLongRangeDetection() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getRadarRange() > 1500)
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
 * @return The number of craft.
 */
int Base::getCraftCount(const std::string &craft) const
{
	int total = 0;
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
 * @return The maintenance costs.
 */
int Base::getCraftMaintenance() const
{
	int total = 0;
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		total += (*i)->getRules()->getRentCost();
	}
	return total;
}

/**
 * Returns the total amount of monthly costs
 * for maintaining the personnel in the base.
 * @return The maintenance costs.
 */
int Base::getPersonnelMaintenance() const
{
	size_t total = 0;
	total += _soldiers.size() * _rule->getSoldierCost();
	total += getTotalEngineers() * _rule->getEngineerCost();
	total += getTotalScientists() * _rule->getScientistCost();
	return total;
}

/**
 * Returns the total amount of monthly costs
 * for maintaining the facilities in the base.
 * @return The maintenance costs.
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
 * @return The maintenance costs.
 */
int Base::getMonthlyMaintenace() const
{
	return getCraftMaintenance() + getPersonnelMaintenance() + getFacilityMaintenance();
}

/**
 * Returns the list of all the base's ResearchProjects.
 * @return A list of the base's ResearchProjects.
 */
const std::vector<ResearchProject *> & Base::getResearch() const
{
	return _research;
}

/**
 * Adds a new production to the base.
 * @param p A pointer to a Production.
 */
void Base::addProduction (Production * p)
{
	_productions.push_back(p);
}

/**
 * Adds a new ResearchProject to the base.
 * @param project The ResearchProject to add.
 */
void Base::addResearch(ResearchProject * project)
{
	_research.push_back(project);
}

/**
 * Removes a ResearchProject from the base.
 * @param project The ResearchProject to remove.
 */
void Base::removeResearch(ResearchProject * project)
{
	_scientists += project->getAssigned();
	std::vector<ResearchProject *>::iterator iter = std::find (_research.begin (), _research.end (), project);
	if(iter != _research.end ())
	{
		_research.erase(iter);
	}
}

/**
 * Removes a Production from the base.
 * @param p A pointer to a Production.
 */
void Base::removeProduction (Production * p)
{
	_engineers += p->getAssignedEngineers();
	std::vector<Production *>::iterator iter = std::find (_productions.begin (), _productions.end (), p);
	if (iter != _productions.end ())
	{
		_productions.erase(iter);
	}
}

/**
 * Gets the list of Base Productions.
 * @return The list of Base Productions.
 */
const std::vector<Production *> & Base::getProductions () const
{
	return _productions;
}

/**
 * Returns whether or not this base is equipped with hyper-wave
 * detection facilities.
 * @return True, if this base is equipped with hyperwave detection.
 */
bool Base::getHyperDetection() const
{
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getRules()->isHyperwave() && (*i)->getBuildTime() == 0)
		{
			return true;
		}
	}
	return false;
}

/**
 * Returns the total amount of Psi Lab space
 * available in the base.
 * @return The amount of available Psi Lab space.
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
 * Psi Lab space in the base.
 * @return The amount of used Psi Lab space.
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
 * containment space in the base.
 * @return The amount of used containment space.
 */
int Base::getUsedContainment() const
{
	int total = 0;
	for (std::map<std::string, int>::iterator i = _items->getContents()->begin(); i != _items->getContents()->end(); ++i)
	{
		if (_rule->getItem((i)->first)->getAlien())
		{
			total += (i)->second;
		}
	}
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_ITEM)
		{
			if(_rule->getItem((*i)->getItems())->getAlien())
			{
				total += (*i)->getQuantity();
			}
		}
	}
	if (Options::getBool("alienContainmentLimitEnforced"))
	{
		for (std::vector<ResearchProject*>::const_iterator i = _research.begin(); i != _research.end(); ++i)
		{
			const RuleResearch *projRules = (*i)->getRules();
			if (projRules->needItem() && _rule->getUnit(projRules->getName()))
			{
				++total;
			}
		}
	}
	return total;
}

/**
 * Returns the total amount of containment space
 * available in the base.
 * @return The amount of available containment space.
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
 * @return True, if the craft is on the battlescape.
 */
bool Base::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Changes the base's battlescape status.
 * @param inbattle True if it's in battle, false otherwise.
 */
void Base::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

/**
 * Marks the base as a valid alien retaliation target.
 * @param mark Mark (if @c true) or unmark (if @c false) the base.
 */
void Base::setRetaliationTarget(bool mark)
{
	_retaliationTarget = mark;
}

/**
 * Gets the base's retaliation status.
 * @return True, if the base is a valid target for alien retaliation.
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
 * Checks if this facility is a mind shield.
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
 * Checks Facilities for construction completion.
 * @param facility Pointer to the facility to check.
 * @return If @a facility has completed construction.
 */
bool isCompleted::operator()(const BaseFacility *facility) const
{
	return (facility->getBuildTime() == 0);
}
/**
 * Calculates the detection chance of this base.
 * Big bases without mindshields are easier to detect.
 * @return The detection chance.
 */
unsigned Base::getDetectionChance() const
{
	unsigned mindShields = std::count_if(_facilities.begin(), _facilities.end(), isMindShield());
	unsigned completedFacilities = std::count_if(_facilities.begin(), _facilities.end(), isCompleted());
	return (completedFacilities / 6 + 15) / (mindShields + 1);
}

/**
 * Returns the number of gravity shields protecting the base.
 * @return Number of gravity shields.
 */
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

/**
 * Sets up the bases defenses.
 */
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

	_vehicles.clear();
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
		std::string itemId=(i)->first;
		int iqty=(i)->second;
		RuleItem *rule = _rule->getItem(itemId);
		if (rule->isFixed())
		{
			int size = 4;
			if (_rule->getUnit(itemId))
			{
				size = _rule->getArmor(_rule->getUnit(itemId)->getArmor())->getSize();
			}
			if (rule->getCompatibleAmmo()->empty()) // so this vehicle does not need ammo
			{
				for (int j=0; j<iqty; ++j) _vehicles.push_back(new Vehicle(rule, rule->getClipSize(), size));
				_items->removeItem(itemId, iqty);
			}
			else // so this vehicle needs ammo
			{
				RuleItem *ammo = _rule->getItem(rule->getCompatibleAmmo()->front());
				int baqty = _items->getItem(ammo->getType()); // Ammo Quantity for this vehicle-type on the base
				if (0 >= baqty || 0 >= iqty) { ++i; continue; }
				int canBeAdded = std::min(iqty, baqty);
				int newAmmoPerVehicle = std::min(baqty / canBeAdded, ammo->getClipSize());
				int remainder = 0;
				if (ammo->getClipSize() > newAmmoPerVehicle) remainder = baqty - (canBeAdded * newAmmoPerVehicle);
				int newAmmo;
				for (int j=0; j<canBeAdded; ++j)
				{
					newAmmo = newAmmoPerVehicle;
					if (j<remainder) ++newAmmo;
					_vehicles.push_back(new Vehicle(rule, newAmmo, size));
					_items->removeItem(ammo->getType(), newAmmo);
				}
				_items->removeItem(itemId, canBeAdded);
			}

			i = _items->getContents()->begin(); // we have to start over because iterator is broken because of the removeItem
		}
		else ++i;
	}
}

/**
 * Returns the defenses available at this base.
 * @return Pointer to the defenses.
 */
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
 * Checks that all the base modules are connected in some way to the elevator.
 * If they are disconnected somehow, destroys them.
 */
void Base::checkModuleConnections()
{
	BaseFacility *connectionMap[BASE_SIZE][BASE_SIZE];
	for (int x = 0; x != BASE_SIZE; ++x)
	{
		for (int y = 0; y != BASE_SIZE; ++y)
		{
			connectionMap[x][y] = 0;
		}
	}

	for (std::vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		for (int x = 0; x != (*i)->getRules()->getSize(); ++x)
		{
			for (int y = 0; y != (*i)->getRules()->getSize(); ++y)
			{
				connectionMap[(*i)->getX() + x][(*i)->getY() + y] = *i;
			}
		}
	}
	for (std::vector<BaseFacility*>::iterator i = _facilities.begin(); i != _facilities.end();)
	{
		if (!checkConnected((*i)->getX(), (*i)->getY(), 0, connectionMap))
		{
			for (int x = 0; x != (*i)->getRules()->getSize(); ++x)
			{
				for (int y = 0; y != (*i)->getRules()->getSize(); ++y)
				{
					connectionMap[(*i)->getX() + x][(*i)->getY() + y] = 0;
				}
			}
			destroyFacility(i);
		}
		else
		{
			++i;
		}
	}
}

/**
 * Checks individual modules for connectivity to the elevator, essentially by pathfinding.
 * Thank $deity the base is only 6x6, or this could get out of hand.
 * @param x, y Coordinates on the grid.
 * @param grid This is defined within the function to keep track of which modules have been checked.
 * @param facilities Similar to the grid, but instead it contains a grid full of pointers.
 * @return True, if this facility is connected.
 */
bool Base::checkConnected(int x, int y, int **grid, BaseFacility *(&facilities)[BASE_SIZE][BASE_SIZE]) const
{

	bool newgrid = (grid == 0);

	// Create connection grid
	if (newgrid)
	{
		grid = new int*[BASE_SIZE];

		for (int xx = 0; xx < BASE_SIZE; ++xx)
		{
			grid[xx] = new int[BASE_SIZE];
			for (int yy = 0; yy < BASE_SIZE; ++yy)
			{
				if (facilities[xx][yy] == 0)
				{
					grid[xx][yy] = -1;
				}
				else
				{
					grid[xx][yy] = 0;
				}
			}
		}
	}

	// outside map, or already checked.
	if (x < 0 || x >= BASE_SIZE || y < 0 || y >= BASE_SIZE || grid[x][y] != 0)
	{
		return false;
	}

	// Add connected (neighbor) facilities to grid
	int total = 1;
	grid[x][y]++;
	bool retVal = facilities[x][y]->getRules()->isLift();

	// check neighbouring slots for a facility
	for (int i = -1; i < 2 && !retVal; i += 2)
	{
		if (!retVal && x + i >= 0 && facilities[x+i][y] != 0)
		{
			retVal = checkConnected(x + i, y, grid, facilities);
		}
		if (!retVal && y + i >= 0 && facilities[x][y + i] != 0)
		{
			retVal = checkConnected(x, y + i, grid, facilities);
		}
	}

	// Delete connection grid
	if (newgrid)
	{
		for (int xx = 0; xx < BASE_SIZE; ++xx)
		{
			delete[] grid[xx];
		}
		delete[] grid;
	}

	return retVal;
}


/**
 * Removes a base module, and deals with the ramifications thereof.
 * @param facility An iterator reference to the facility to destroy and remove.
 */
void Base::destroyFacility(std::vector<BaseFacility*>::iterator &facility)
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
				_items->addItem((*i).first, (*i).second);
				(*facility)->getCraft()->getItems()->removeItem((*i).first, (*i).second);
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
			for (std::vector<Production*>::iterator i = _productions.begin(); i != _productions.end();)
			{
				if (getAvailableHangars() - getUsedHangars() - (*facility)->getRules()->getCrafts() < 0 && (*i)->getRules()->getCategory() == "STR_CRAFT")
				{
					_engineers += (*i)->getAssignedEngineers();
					delete *i;
					_productions.erase(i);
					remove = false;
					break;
				}
				else
				{
					++i;
				}
			}
			if (remove && !_transfers.empty())
			{
				for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); )
				{
					if ((*i)->getType() == TRANSFER_CRAFT)
					{
						delete *i;
						_transfers.erase(i);
						break;
					}
				}
			}
		}
	}
	else if ((*facility)->getRules()->getPsiLaboratories() > 0)
	{
		// psi lab destruction: remove any soldiers over the maximum allowable from psi training.
		int toRemove = getUsedPsiLabs() - (getAvailablePsiLabs() - (*facility)->getRules()->getPsiLaboratories());
		for (std::vector<Soldier*>::iterator i = _soldiers.begin(); i != _soldiers.end() && toRemove > 0; ++i)
		{
			if ((*i)->isInPsiTraining())
			{
				(*i)->setPsiTraining();
				--toRemove;
			}
		}
	}
	else if ((*facility)->getRules()->getLaboratories())
	{
		// lab destruction: enforce lab space limits. take scientists off projects until
		// it all evens out.
		int toRemove = getUsedLaboratories() - (getAvailableLaboratories() - (*facility)->getRules()->getLaboratories());
		for (std::vector<ResearchProject*>::iterator i = _research.begin(); i != _research.end() && toRemove > 0;)
		{
			if ((*i)->getAssigned() > toRemove)
			{
				(*i)->setAssigned((*i)->getAssigned() - toRemove);
				break;
			}
			else
			{
				toRemove -= (*i)->getAssigned();
				_scientists += (*i)->getAssigned();
				delete *i;
				i = _research.erase(i);
			}
		}
	}
	else if ((*facility)->getRules()->getWorkshops())
	{
		// workshop destruction: similar to lab destruction, but we'll lay off engineers instead
		int toRemove = getUsedWorkshops() - (getAvailableWorkshops() - (*facility)->getRules()->getWorkshops());
		for (std::vector<Production*>::iterator i = _productions.begin(); i != _productions.end() && toRemove > 0;)
		{
			if ((*i)->getAssignedEngineers() > toRemove)
			{
				(*i)->setAssignedEngineers((*i)->getAssignedEngineers() - toRemove);
				break;
			}
			else
			{
				toRemove -= (*i)->getAssignedEngineers();
				_scientists += (*i)->getAssignedEngineers();
				delete *i;
				i = _productions.erase(i);
			}
		}
	}
	else if ((*facility)->getRules()->getStorage())
	{
		// we won't destroy the items physically AT the base,
		// but any items in transit will end up at the dead letter office.
		if (getUsedStores() - (getAvailableStores() - (*facility)->getRules()->getStorage()) < 0 && !_transfers.empty())
		{
			for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); )
			{
				if ((*i)->getType() == TRANSFER_ITEM)
				{
					delete *i;
					i = _transfers.erase(i);
				}
			}
		}
	}
	else if ((*facility)->getRules()->getPersonnel())
	{
		// as above, we won't actually fire people, but we'll block any new ones coming in.
		if (getUsedQuarters() - (getAvailableQuarters() - (*facility)->getRules()->getPersonnel()) < 0 && !_transfers.empty())
		{
			for (std::vector<Transfer*>::iterator i = _transfers.begin(); i != _transfers.end(); )
			{
				bool del = false;
				if ((*i)->getType() == TRANSFER_ENGINEER)
				{
					_engineers -= (*i)->getQuantity();
					del = true;
				}
				else if ((*i)->getType() == TRANSFER_SCIENTIST)
				{
					_scientists -= (*i)->getQuantity();
					del = true;
				}
				else if ((*i)->getType() == TRANSFER_SOLDIER)
				{
					del = true;
				}
				if (del)
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
	facility = _facilities.erase(facility);
}
}
