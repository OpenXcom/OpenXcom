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
#include "Production.h"
#include "../Ruleset/RuleManufacture.h"
#include "Base.h"
#include "SavedGame.h"
#include "ItemContainer.h"
#include "Craft.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Options.h"
#include <limits>

namespace OpenXcom
{
Production::Production (const RuleManufacture * rules, int amount) : _rules(rules), _amount(amount), _timeSpent(0), _engineers(0)
{
}

int Production::getAmountTotal () const
{
	return _amount;
}

void Production::setAmountTotal (int amount)
{
	_amount = amount;
}

int Production::getTimeSpent () const
{
	return _timeSpent;
}

void Production::setTimeSpent (int done)
{
	_timeSpent = done;
}

int Production::getAssignedEngineers() const
{
	return _engineers;
}

void Production::setAssignedEngineers (int engineers)
{
	_engineers = engineers;
}

productionProgress_e Production::step(Base * b, SavedGame * g, const Ruleset *r)
{
	int done = getAmountProduced ();
	_timeSpent += _engineers;
	if (done < getAmountProduced ())
	{
		if (_rules->getCategory() == "STR_CRAFT")
		{
			Craft *craft = new Craft(r->getCraft(_rules->getName()), b, g->getId(_rules->getName()));
			craft->setStatus("STR_REFUELLING");
			b->getCrafts()->push_back(craft);
		}
		else
		{
			if (Options::getBool("allowAutoSellProduction") && getAmountTotal() == std::numeric_limits<int>::max())
				g->setFunds(g->getFunds() + r->getItem(_rules->getName())->getSellCost());
			else
				b->getItems()->addItem(_rules->getName(), 1);
		}
	}
	if (getAmountProduced () >= _amount)
	{
		return PROGRESS_COMPLETE;
	}
	else if (done < getAmountProduced ())
	{
		// We need to ensure that player has enough cash/item to produce a new unit
		if(g->getFunds() < _rules->getManufactureCost ())
		{
			return PROGRESS_NOT_ENOUGH_MONEY;
		}
		for(std::map<std::string,int>::const_iterator iter = _rules->getRequiredItems ().begin (); iter != _rules->getRequiredItems ().end (); ++iter)
		{
			if (b->getItems ()->getItem(iter->first) < iter->second)
			{
				return PROGRESS_NOT_ENOUGH_MATERIALS;
			}
		}
		//if (done < getAmountProduced ())
		{
			startItem(b, g);
		}
	}
	return PROGRESS_NOT_COMPLETE;
}

int Production::getAmountProduced () const
{
	return _timeSpent / _rules->getManufactureTime ();
}

const RuleManufacture * Production::getRules() const
{
	return _rules;
}

void Production::startItem(Base * b, SavedGame * g)
{
	g->setFunds(g->getFunds() - _rules->getManufactureCost ());
	for(std::map<std::string,int>::const_iterator iter = _rules->getRequiredItems ().begin (); iter != _rules->getRequiredItems ().end (); ++iter)
	{
		b->getItems ()->removeItem(iter->first, iter->second);
	}
}

void Production::save(YAML::Emitter &out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "item" << YAML::Value << getRules ()->getName ();
	out << YAML::Key << "assigned" << YAML::Value << getAssignedEngineers ();
	out << YAML::Key << "spent" << YAML::Value << getTimeSpent ();
	out << YAML::Key << "amount" << YAML::Value << getAmountTotal ();
	out << YAML::EndMap;
}

void Production::load(const YAML::Node &node)
{
	int assigned;
	int spent;
	int amount;
	node["assigned"] >> assigned;
	node["spent"] >> spent;
	node["amount"] >> amount;
	setAssignedEngineers(assigned);
	setTimeSpent(spent);
	setAmountTotal(amount);
}
};
