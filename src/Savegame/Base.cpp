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
#include "../Ruleset/RuleManufacture.h"
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
				if ((*k)==(*j)) { _vehicles.erase(k); break; } // to avoid calling a vehicle's desctructor twice
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
void Base::load(const YAML::Node &node, SavedGame *save, bool newGame)
{
	Target::load(node);
	std::string name;
	node["name"] >> name;
	_name = Language::utf8ToWstr(name);

	if (!newGame || !Options::getBool("customInitialBase") )
	{
		for (YAML::Iterator i = node["facilities"].begin(); i != node["facilities"].end(); ++i)
		{
			std::string type;
			(*i)["type"] >> type;
			BaseFacility *f = new BaseFacility(_rule->getBaseFacility(type), this);
			f->load(*i);
			_facilities.push_back(f);
		}
	}

	for (YAML::Iterator i = node["crafts"].begin(); i != node["crafts"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		Craft *c = new Craft(_rule->getCraft(type), this);
		c->load(*i, _rule, save);		
		_crafts.push_back(c);
	}

	for (YAML::Iterator i = node["soldiers"].begin(); i != node["soldiers"].end(); ++i)
	{
		Soldier *s = new Soldier(_rule->getSoldier("XCOM"), _rule->getArmor("STR_NONE_UC"));
		s->load(*i, _rule);
		if (const YAML::Node *pName = (*i).FindValue("craft"))
		{
			std::string type;
			int id;
			(*pName)["type"] >> type;
			(*pName)["id"] >> id;
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

	node["scientists"] >> _scientists;
	node["engineers"] >> _engineers;
	node["inBattlescape"] >> _inBattlescape;

	for (YAML::Iterator i = node["transfers"].begin(); i != node["transfers"].end(); ++i)
	{
		int hours;
		(*i)["hours"] >> hours;
		Transfer *t = new Transfer(hours);
		t->load(*i, this, _rule);
		_transfers.push_back(t);
	}

	for (YAML::Iterator i = node["research"].begin(); i != node["research"].end(); ++i)
	{
		std::string research;
		(*i)["project"] >> research;
		ResearchProject *r = new ResearchProject(_rule->getResearch(research));
		r->load(*i);
		_research.push_back(r);
	}

	for (YAML::Iterator i = node["productions"].begin(); i != node["productions"].end(); ++i)
	{
		std::string item;
		(*i)["item"] >> item;
		Production *p = new Production(_rule->getManufacture(item), 0);
		p->load(*i);
		_productions.push_back(p);
	}

	if (const YAML::Node *pNode = node.FindValue("retaliationTarget"))
	{
		*pNode >> _retaliationTarget;
	}
}

/**
 * Saves the base to a YAML file.
 * @param out YAML emitter.
 */
void Base::save(YAML::Emitter &out) const
{
	Target::save(out);
	out << YAML::Key << "name" << YAML::Value << Language::wstrToUtf8(_name);
	out << YAML::Key << "facilities" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "soldiers" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "crafts" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "items" << YAML::Value;
	_items->save(out);
	out << YAML::Key << "scientists" << YAML::Value << _scientists;
	out << YAML::Key << "engineers" << YAML::Value << _engineers;
	out << YAML::Key << "inBattlescape" << YAML::Value << _inBattlescape;
	out << YAML::Key << "transfers" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "research" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<ResearchProject*>::const_iterator i = _research.begin(); i != _research.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "productions" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Production*>::const_iterator i = _productions.begin(); i != _productions.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;
	out << YAML::Key << "retaliationTarget" << YAML::Value << _retaliationTarget;
	out << YAML::EndMap;
}

/**
 * Saves the base's unique identifiers to a YAML file.
 * @param out YAML emitter.
 */
void Base::saveId(YAML::Emitter &out) const
{
	Target::saveId(out);
	out << YAML::Key << "type" << YAML::Value << "STR_BASE";
	out << YAML::Key << "id" << YAML::Value << 0;
	out << YAML::EndMap;
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
 * @return True if it's within range, False otherwise.
 */
bool Base::detect(Target *target) const
{
	int chance = 0;
	double distance = getDistance(target);
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getRadarRange() * (1 / 60.0) * (M_PI / 180) >= distance)
		{
			if ((*i)->getRules()->isHyperwave())
			{
				return true;
			}
			chance += (*i)->getRules()->getRadarChance();
		}
	}
	if (chance == 0)
		return false;

	Ufo *u = dynamic_cast<Ufo*>(target);
	if (u != 0)
	{
		chance = (chance * 100 + u->getVisibility()) / 100;
	}

	int detection = RNG::generate(0, 100);
	return (detection < chance);
}

/**
 * Returns if a certain target is inside the base's
 * radar range, taking in account the positions of both.
 * @param target Pointer to target to compare.
 * @return True if it's inside, False otherwise.
 */
bool Base::insideRadarRange(Target *target) const
{
	double range = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			range = std::max(range, (*i)->getRules()->getRadarRange() * (1 / 60.0) * (M_PI / 180));
		}
	}
	
	return (getDistance(target) <= range);
}

/**
 * Returns the amount of soldiers contained
 * in the base without any assignments.
 * @return Number of soldiers.
 */
int Base::getAvailableSoldiers() const
{
	int total = 0;
	for (std::vector<Soldier*>::const_iterator i = _soldiers.begin(); i != _soldiers.end(); ++i)
	{
		if ((*i)->getCraft() == 0)
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
	for (std::vector<Production *>::const_iterator iter = _productions.begin (); iter != _productions.end (); ++iter)
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
 * Returns the amount of stores used up
 * by equipment in the base.
 * @return Storage space.
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
 * Returns the amount of laboratories used up
 * by research projects in the base.
 * @return Laboratory space.
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
	for (std::vector<Production *>::const_iterator iter = _productions.begin (); iter != _productions.end (); ++iter)
	{
		usedWorkShop += ((*iter)->getAssignedEngineers() + (*iter)->getRules()->getRequiredSpace ());
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
 * @return Storage space.
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
int Base::getFreeLaboratories () const
{
	return getAvailableLaboratories() - getUsedLaboratories();
}

/**
 * Return workshop space not used by a Production
 * @return workshop space not used by a Production
*/
int Base::getFreeWorkshops () const
{
	return getAvailableWorkshops() - getUsedWorkshops();
}

/**
 * Returns the amount of scientists currently in use.
 * @return Amount of scientists.
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
 * Returns the amount of engineers currently in use.
 * @return Amount of engineers.
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
 * @return Defense value.
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
 * @return Number of craft.
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
 * @return Maintenance costs.
 */
int Base::getCraftMaintenance() const
{
	int total = 0;
	for (std::vector<Craft*>::const_iterator i = _crafts.begin(); i != _crafts.end(); ++i)
	{
		total += (*i)->getRules()->getBuyCost();
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
	size_t total = 0;
	total += _soldiers.size() * _rule->getSoldierCost();
	total += _engineers * _rule->getEngineerCost();
	total += _scientists * _rule->getScientistCost();
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
	std::vector<ResearchProject *>::iterator iter = std::find (_research.begin (), _research.end (), project);
	if(iter != _research.end ())
	{
		_research.erase(iter);
	}
}

/**
 * Remove a Production from the Base
 * @param p A pointer to a Production
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
 * Get the list of Base Production's
 * @return the list of Base Production's
 */
const std::vector<Production *> & Base::getProductions () const
{
	return _productions;
}


/**
 * Returns whether or not this base
 * is equipped with hyper-wave
 * detection facilities.
 */
bool Base::getHyperDetection() const
{
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->isHyperwave())
		{
			return true;
		}		
	}
	return false;
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
 * Calculate the detection chance of this base.
 * Big bases without mindshields are easier to detect.
 * @return The detection chance.
 */
unsigned Base::getDetectionChance() const
{
	unsigned mindShields = std::count_if(_facilities.begin(), _facilities.end(), isMindShield());
	return (_facilities.size()/6 + 16) / (mindShields + 1);
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
			if (rule->getClipSize() == -1) // so this vehicle does not need ammo
			{
				for (int j=0; j<iqty; ++j) _vehicles.push_back(new Vehicle(rule, 255));
				_items->removeItem(itemId, iqty);
			}
			else // so this vehicle needs ammo
			{
				RuleItem *ammo = _rule->getItem(rule->getCompatibleAmmo()->front());
				int baqty = _items->getItem(ammo->getType()); // Ammo Quantity for this vehicle-type on the base
				if (0 >= baqty || 0 >= iqty) { ++i; continue; }
				int canBeAdded = std::min(iqty, baqty);
				int newAmmoPerVehicle = std::min(baqty / canBeAdded, ammo->getClipSize());;
				int remainder = 0;
				if (ammo->getClipSize() > newAmmoPerVehicle) remainder = baqty - (canBeAdded * newAmmoPerVehicle);
				int newAmmo;
				for (int j=0; j<canBeAdded; ++j)
				{
					newAmmo = newAmmoPerVehicle;
					if (j<remainder) ++newAmmo;
					_vehicles.push_back(new Vehicle(rule, newAmmo));
					_items->removeItem(ammo->getType(), newAmmo);
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
}
