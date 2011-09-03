#include "Production.h"
#include "../Ruleset/RuleManufactureInfo.h"
#include "../Ruleset/RuleItem.h"
#include "Base.h"
#include "SavedGame.h"
#include "ItemContainer.h"

namespace OpenXcom
{
Production::Production (RuleItem * item, int todo) : _item(item), _todo(todo), _timeSpent(0), _engineers(0)
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

bool Production::step(Base * b, SavedGame * g)
{
	int done = getNumberOfItemDone ();
	_timeSpent += _engineers;
	if (done < getNumberOfItemDone ())
	{
		b->getItems ()->addItem(_item->getType (), 1);
		g->setFunds(g->getFunds() - _item->getManufactureInfo()->getManufactureCost ());
	}
	if (getNumberOfItemDone () >= _todo)
	{
		return true;
	}
	return false;
}

int Production::getNumberOfItemDone () const
{
	return _timeSpent / _item->getManufactureInfo()->getManufactureTime ();
}

const RuleItem * Production::getRuleItem() const
{
	return _item;
}
};
