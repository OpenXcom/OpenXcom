/*
 * Copyright 2010 OpenXcom Developers.
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
#include <cmath>
#include "BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/Ruleset.h"
#include "ItemContainer.h"
#include "Soldier.h"
#include "SavedGame.h"
#include "Ufo.h"
#include "Waypoint.h"
#include "../Engine/Language.h"
#include "../Ruleset/RuleItem.h"
#include "Transfer.h"
#include <algorithm>
#include "ResearchProject.h"
#include "../Ruleset/RuleResearchProject.h"

namespace OpenXcom
{

/**
 * Initializes an empty base.
 * @param rule Pointer to ruleset.
 */
Base::Base(const Ruleset *rule) : Target(), _rule(rule), _name(L""), _facilities(), _soldiers(), _crafts(), _scientists(0), _engineers(0)
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
		delete *i;
	}
	delete _items;
	for (std::vector<ResearchProject*>::iterator i = _research.begin(); i != _research.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Loads the base from a YAML file.
 * @param node YAML node.
 * @param save Pointer to saved game.
 */
void Base::load(const YAML::Node &node, SavedGame *save)
{
	Target::load(node);
	std::string name;
	node["name"] >> name;
	_name = Language::utf8ToWstr(name);

	for (YAML::Iterator i = node["facilities"].begin(); i != node["facilities"].end(); ++i)
	{
		int x, y;
		(*i)["x"] >> x;
		(*i)["y"] >> y;
		std::string type;
		(*i)["type"] >> type;
		BaseFacility *f = new BaseFacility(_rule->getBaseFacility(type), this, x, y);
		f->load(*i);
		_facilities.push_back(f);
	}

	for (YAML::Iterator i = node["crafts"].begin(); i != node["crafts"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		Craft *c = new Craft(_rule->getCraft(type), this);
		c->load(*i, _rule);
		if (const YAML::Node *pName = (*i).FindValue("dest"))
		{
			std::string type;
			int id;
			(*pName)["type"] >> type;
			(*pName)["id"] >> id;
			if (type == "STR_BASE")
			{
				c->returnToBase();
			}
			else if (type == "STR_UFO")
			{
				for (std::vector<Ufo*>::iterator j = save->getUfos()->begin(); j != save->getUfos()->end(); ++j)
				{
					if ((*j)->getId() == id)
					{
						c->setDestination(*j);
						break;
					}
				}
			}
			else if (type == "STR_WAYPOINT")
			{
				for (std::vector<Waypoint*>::iterator j = save->getWaypoints()->begin(); j != save->getWaypoints()->end(); ++j)
				{
					if ((*j)->getId() == id)
					{
						c->setDestination(*j);
						break;
					}
				}
			}
		}
		_crafts.push_back(c);
	}

	for (YAML::Iterator i = node["soldiers"].begin(); i != node["soldiers"].end(); ++i)
	{
		Soldier *s = new Soldier(_rule->getSoldier("XCOM"), _rule->getArmor("STR_NONE_UC"));
		s->load(*i);
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
	node["scientists"] >> _scientists;
	node["engineers"] >> _engineers;

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
		ResearchProject *r = new ResearchProject(_rule->getResearchProject(research));
		r->load(*i);
		_research.push_back(r);
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
std::wstring Base::getName(Language *lang) const
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
std::vector<BaseFacility*> *const Base::getFacilities()
{
	return &_facilities;
}

/**
 * Returns the list of soldiers in the base.
 * @return Pointer to the soldier list.
 */
std::vector<Soldier*> *const Base::getSoldiers()
{
	return &_soldiers;
}

/**
 * Returns the list of crafts in the base.
 * @return Pointer to the craft list.
 */
std::vector<Craft*> *const Base::getCrafts()
{
	return &_crafts;
}

/**
 * Returns the list of transfers destined
 * to this base.
 * @return Pointer to the transfer list.
 */
std::vector<Transfer*> *const Base::getTransfers()
{
	return &_transfers;
}

/**
 * Returns the list of items in the base.
 * @return Pointer to the item list.
 */
ItemContainer *const Base::getItems()
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
	int total = _soldiers.size();
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
	int nbFreeScientist = getScientists();
	const std::vector<ResearchProject *> & researchs (getResearch());
	for (std::vector<ResearchProject *>::const_iterator itResearch = researchs.begin ();
	     itResearch != researchs.end ();
	     itResearch++)
	{
		nbFreeScientist -= (*itResearch)->getAssigned ();
	}
       
	return nbFreeScientist;
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
	return total;
}

/**
 * Returns the amount of engineers contained
 * in the base without any assignments.
 * @return Number of engineers.
 */
int Base::getAvailableEngineers() const
{
	return _engineers;
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
	const std::vector<ResearchProject *> & researchs (getResearch());
	int usedLabSpace = 0;
	for (std::vector<ResearchProject *>::const_iterator itResearch = researchs.begin ();
	     itResearch != researchs.end ();
	     itResearch++)
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
	return 0;
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
	int total = _crafts.size();
	for (std::vector<Transfer*>::const_iterator i = _transfers.begin(); i != _transfers.end(); ++i)
	{
		if ((*i)->getType() == TRANSFER_CRAFT)
		{
			total += (*i)->getQuantity();
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
 * Returns the total defence value of all
 * the facilities in the base.
 * @return Defence value.
 */
int Base::getDefenceValue() const
{
	int total = 0;
	for (std::vector<BaseFacility*>::const_iterator i = _facilities.begin(); i != _facilities.end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			total += (*i)->getRules()->getDefenceValue();
		}
	}
	return total;
}

/**
 * Returns the total amount of short range
 * detection facilities in the base.
 * @return Defence value.
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
 * @return Defence value.
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
		total += (*i)->getRules()->getCost();
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
	std::vector<ResearchProject *>::iterator iter = std::find (_research.begin (), _research.end (), project);
	if(iter == _research.end ())
	{
		return ;
	}
	_research.erase(iter);
}

/**
 * Return the number of scientist not assigned to a ResearchProject
 * @return the number of scientist not assigned to a ResearchProject
*/
int Base::getFreeScientist () const
{
	return getScientists() - getUsedLaboratories();
}

/**
 * Return laboratories space not used by a ResearchProject
 * @return laboratories space not used by a ResearchProject
*/
int Base::getFreeLaboratories () const
{
	return getAvailableLaboratories() - getUsedLaboratories();
}

}
