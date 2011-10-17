/*
 * Copyright 2011 OpenXcom Developers.
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
#include "../Ruleset/RuleManufactureInfo.h"
#include "Base.h"
#include "SavedGame.h"
#include "ItemContainer.h"

namespace OpenXcom
{
Production::Production (RuleManufactureInfo * item, int todo) : _item(item), _todo(todo), _timeSpent(0), _engineers(0)
{
}

int Production::getNumberOfItemTodo () const
{
	return _todo;
}

void Production::setNumberOfItemTodo (int todo)
{
	_todo = todo;
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

productionProgress_e Production::step(Base * b, SavedGame * g)
{
	int done = getNumberOfItemDone ();
	_timeSpent += _engineers;
	if (done < getNumberOfItemDone ())
	{
		b->getItems ()->addItem(_item->getName (), 1);
	}
	if (getNumberOfItemDone () >= _todo)
	{
		return PRODUCTION_PROGRESS_COMPLETE;
	}
	else if (done < getNumberOfItemDone ())
	{
		// We need to ensure that player has enough cash/item to produce a new unit
		if(g->getFunds() < _item->getManufactureCost ())
		{
			return PRODUCTION_PROGRESS_NOT_ENOUGH_MONEY;
		}
		for(std::map<std::string,int>::const_iterator iter = _item->getNeededItems ().begin (); iter != _item->getNeededItems ().end (); ++iter)
		{
			if (b->getItems ()->getItem(iter->first) < iter->second)
			{
				return PRODUCTION_PROGRESS_NOT_ENOUGH_MATERIALS;
			}
		}
		//if (done < getNumberOfItemDone ())
		{
			startItem(b, g);
		}
	}
	return PRODUCTION_PROGRESS_NOT_COMPLETE;
}

int Production::getNumberOfItemDone () const
{
	return _timeSpent / _item->getManufactureTime ();
}

const RuleManufactureInfo * Production::getRuleManufactureInfo() const
{
	return _item;
}

void Production::startItem(Base * b, SavedGame * g)
{
	g->setFunds(g->getFunds() - _item->getManufactureCost ());
	for(std::map<std::string,int>::const_iterator iter = _item->getNeededItems ().begin (); iter != _item->getNeededItems ().end (); ++iter)
	{
		b->getItems ()->removeItem(iter->first, iter->second);
	}
}

void Production::save(YAML::Emitter &out)
{
	out << YAML::BeginMap;
	out << YAML::Key << "item" << YAML::Value << getRuleManufactureInfo ()->getName ();
	out << YAML::Key << "assigned" << YAML::Value << getAssignedEngineers ();
	out << YAML::Key << "spent" << YAML::Value << getTimeSpent ();
	out << YAML::Key << "todo" << YAML::Value << getNumberOfItemTodo ();
	out << YAML::EndMap;
}

void Production::load(const YAML::Node &node)
{
	int assigned;
	int spent;
	int todo;
	node["assigned"] >> assigned;
	node["spent"] >> spent;
	node["todo"] >> todo;
	setAssignedEngineers(assigned);
	setTimeSpent(spent);
	setNumberOfItemTodo(todo);
}
};
