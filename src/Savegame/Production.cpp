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
#include "Production.h"
#include "../Ruleset/RuleManufacture.h"
#include "Base.h"
#include "SavedGame.h"
#include "ItemContainer.h"
#include "Craft.h"
#include "CraftWeapon.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/RuleCraftWeapon.h"
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

bool Production::haveEnoughMoneyForOneMoreUnit(SavedGame * g)
{
	return (g->getFunds() >= _rules->getManufactureCost ());
}

bool Production::haveEnoughMaterialsForOneMoreUnit(Base * b)
{
	for (std::map<std::string,int>::const_iterator iter = _rules->getRequiredItems().begin(); iter != _rules->getRequiredItems().end(); ++iter)
		if (b->getItems()->getItem(iter->first) < iter->second)
			return false;
	return true;
}

productionProgress_e Production::step(Base * b, SavedGame * g, const Ruleset *r)
{
	int done = getAmountProduced ();
	_timeSpent += _engineers;
	if (done < getAmountProduced ())
	{
		bool allowAutoSellProduction = Options::getBool("allowAutoSellProduction");
		bool canManufactureMoreItemsPerHour = Options::getBool("canManufactureMoreItemsPerHour");
		int produced = std::min(getAmountProduced(), _amount) - done; // std::min is required because we don't want to overproduce
		int count = 0;
		do
		{
			if (_rules->getCategory() == "STR_CRAFT")
			{
				Craft *craft = new Craft(r->getCraft(_rules->getName()), b, g->getId(_rules->getName()));
				craft->setStatus("STR_REFUELLING");
				b->getCrafts()->push_back(craft);
			}
			else
			{
				// Check if it's ammo to reload a craft
				if (r->getItem(_rules->getName())->getBattleType() == BT_NONE)
				{
					for (std::vector<Craft*>::iterator c = b->getCrafts()->begin(); c != b->getCrafts()->end(); ++c)
					{
						if ((*c)->getStatus() != "STR_READY")
							continue;
						for (std::vector<CraftWeapon*>::iterator w = (*c)->getWeapons()->begin(); w != (*c)->getWeapons()->end(); ++w)
						{
							if ((*w) != 0 && (*w)->getRules()->getClipItem() == _rules->getName() && (*w)->getAmmo() < (*w)->getRules()->getAmmoMax())
							{
								(*w)->setRearming(true);
								(*c)->setStatus("STR_REARMING");
							}
						}
					}
				}
				if (allowAutoSellProduction && getAmountTotal() == std::numeric_limits<int>::max())
					g->setFunds(g->getFunds() + r->getItem(_rules->getName())->getSellCost());
				else
					b->getItems()->addItem(_rules->getName(), 1);
			}
			if (!canManufactureMoreItemsPerHour) break;
			count++;
			if (count < produced)
			{
				// We need to ensure that player has enough cash/item to produce a new unit
				if (!haveEnoughMoneyForOneMoreUnit(g)) return PROGRESS_NOT_ENOUGH_MONEY;
				if (!haveEnoughMaterialsForOneMoreUnit(b)) return PROGRESS_NOT_ENOUGH_MATERIALS;
				startItem(b,g);
			}
		}
		while (count < produced);
	}
	if (getAmountProduced () >= _amount) return PROGRESS_COMPLETE;
	if (done < getAmountProduced ())
	{
		// We need to ensure that player has enough cash/item to produce a new unit
		if (!haveEnoughMoneyForOneMoreUnit(g)) return PROGRESS_NOT_ENOUGH_MONEY;
		if (!haveEnoughMaterialsForOneMoreUnit(b)) return PROGRESS_NOT_ENOUGH_MATERIALS;
		startItem(b,g);
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

YAML::Node Production::save() const
{
	YAML::Node node;
	node["item"] = getRules ()->getName ();
	node["assigned"] = getAssignedEngineers ();
	node["spent"] = getTimeSpent ();
	node["amount"] = getAmountTotal ();
	return node;
}

void Production::load(const YAML::Node &node)
{
	setAssignedEngineers(node["assigned"].as<int>());
	setTimeSpent(node["spent"].as<int>());
	setAmountTotal(node["amount"].as<int>());
}
};
