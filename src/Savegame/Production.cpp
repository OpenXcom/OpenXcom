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
#include "Production.h"
#include <algorithm>
#include "../Mod/RuleManufacture.h"
#include "Base.h"
#include "SavedGame.h"
#include "ItemContainer.h"
#include "Craft.h"
#include "CraftWeapon.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleItem.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleCraftWeapon.h"
#include <limits>
#include "BaseFacility.h"

namespace OpenXcom
{
Production::Production(const RuleManufacture * rules, int amount) : _rules(rules), _amount(amount), _infinite(false), _timeSpent(0), _engineers(0), _sell(false)
{
}

int Production::getAmountTotal() const
{
	return _amount;
}

void Production::setAmountTotal (int amount)
{
	_amount = amount;
}

bool Production::getInfiniteAmount() const
{
	return _infinite;
}

void Production::setInfiniteAmount (bool inf)
{
	_infinite = inf;
}

int Production::getTimeSpent() const
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

bool Production::getSellItems() const
{
	return _sell;
}

void Production::setSellItems (bool sell)
{
	_sell = sell;
}

bool Production::haveEnoughMoneyForOneMoreUnit(SavedGame * g) const
{
	return (g->getFunds() >= _rules->getManufactureCost());
}

bool Production::haveEnoughMaterialsForOneMoreUnit(Base * b, const Mod *m) const
{
	for (std::map<std::string, int>::const_iterator iter = _rules->getRequiredItems().begin(); iter != _rules->getRequiredItems().end(); ++iter)
	{
		if (m->getItem(iter->first) != 0 && b->getStorageItems()->getItem(iter->first) < iter->second)
			return false;
		else if (m->getCraft(iter->first) != 0 && b->getCraftCount(iter->first) < iter->second)
			return false;
	}
	return true;
}

productionProgress_e Production::step(Base * b, SavedGame * g, const Mod *m)
{
	int done = getAmountProduced();
	_timeSpent += _engineers;

	if (done < getAmountProduced())
	{
		int produced;
		if (!getInfiniteAmount())
		{
			produced = std::min(getAmountProduced(), _amount) - done; // std::min is required because we don't want to overproduce
		}
		else
		{
			produced = getAmountProduced() - done;
		}
		int count = 0;
		do
		{
			for (std::map<std::string,int>::const_iterator i = _rules->getProducedItems().begin(); i != _rules->getProducedItems().end(); ++i)
			{
				if (_rules->getCategory() == "STR_CRAFT")
				{
					Craft *craft = new Craft(m->getCraft(i->first), b, g->getId(i->first));
					craft->setStatus("STR_REFUELLING");
					b->getCrafts()->push_back(craft);
					break;
				}
				else
				{
					// Check if it's ammo to reload a craft
					if (m->getItem(i->first)->getBattleType() == BT_NONE)
					{
						for (std::vector<Craft*>::iterator c = b->getCrafts()->begin(); c != b->getCrafts()->end(); ++c)
						{
							if ((*c)->getStatus() != "STR_READY")
								continue;
							for (std::vector<CraftWeapon*>::iterator w = (*c)->getWeapons()->begin(); w != (*c)->getWeapons()->end(); ++w)
							{
								if ((*w) != 0 && (*w)->getRules()->getClipItem() == i->first && (*w)->getAmmo() < (*w)->getRules()->getAmmoMax())
								{
									(*w)->setRearming(true);
									(*c)->setStatus("STR_REARMING");
								}
							}
						}
					}
					// Check if it's fuel to refuel a craft
					if (m->getItem(i->first)->getBattleType() == BT_NONE)
					{
						for (std::vector<Craft*>::iterator c = b->getCrafts()->begin(); c != b->getCrafts()->end(); ++c)
						{
							if ((*c)->getStatus() != "STR_READY")
								continue;
							if ((*c)->getRules()->getRefuelItem() == i->first && 100 > (*c)->getFuelPercentage())
								(*c)->setStatus("STR_REFUELLING");
						}
					}
					if (getSellItems())
						g->setFunds(g->getFunds() + (m->getItem(i->first)->getSellCost() * i->second));
					else
						b->getStorageItems()->addItem(i->first, i->second);
				}
			}
			count++;
			if (count < produced)
			{
				// We need to ensure that player has enough cash/item to produce a new unit
				if (!haveEnoughMoneyForOneMoreUnit(g)) return PROGRESS_NOT_ENOUGH_MONEY;
				if (!haveEnoughMaterialsForOneMoreUnit(b, m)) return PROGRESS_NOT_ENOUGH_MATERIALS;
				startItem(b, g, m);
			}
		}
		while (count < produced);
	}
	if (getAmountProduced() >= _amount && !getInfiniteAmount()) return PROGRESS_COMPLETE;
	if (done < getAmountProduced())
	{
		// We need to ensure that player has enough cash/item to produce a new unit
		if (!haveEnoughMoneyForOneMoreUnit(g)) return PROGRESS_NOT_ENOUGH_MONEY;
		if (!haveEnoughMaterialsForOneMoreUnit(b, m)) return PROGRESS_NOT_ENOUGH_MATERIALS;
		startItem(b, g, m);
	}
	return PROGRESS_NOT_COMPLETE;
}

int Production::getAmountProduced() const
{
	if (_rules->getManufactureTime() > 0)
		return _timeSpent / _rules->getManufactureTime();
	else
		return _amount;
}

const RuleManufacture * Production::getRules() const
{
	return _rules;
}

void Production::startItem(Base * b, SavedGame * g, const Mod *m)
{
	g->setFunds(g->getFunds() - _rules->getManufactureCost());
	for (std::map<std::string,int>::const_iterator iter = _rules->getRequiredItems().begin(); iter != _rules->getRequiredItems().end(); ++iter)
	{
		if (m->getItem(iter->first) != 0)
		{
			b->getStorageItems()->removeItem(iter->first, iter->second);			
		}
		else if (m->getCraft(iter->first) != 0)
		{
			// Find suitable craft
			for (std::vector<Craft*>::iterator c = b->getCrafts()->begin(); c != b->getCrafts()->end(); ++c)
			{
				if ((*c)->getRules()->getType() == iter->first)
				{
					// Unload craft
					(*c)->unload(m);

					// Clear hangar
					for (std::vector<BaseFacility*>::iterator f = b->getFacilities()->begin(); f != b->getFacilities()->end(); ++f)
					{
						if ((*f)->getCraft() == (*c))
						{
							(*f)->setCraft(0);
							break;
						}
					}

					// Remove craft
					b->getCrafts()->erase(c);
					break;
				}
			}
		}
	}
}

YAML::Node Production::save() const
{
	YAML::Node node;
	node["item"] = getRules()->getName();
	node["assigned"] = getAssignedEngineers();
	node["spent"] = getTimeSpent();
	node["amount"] = getAmountTotal();
	node["infinite"] = getInfiniteAmount();
	if (getSellItems())
		node["sell"] = getSellItems();
	return node;
}

void Production::load(const YAML::Node &node)
{
	setAssignedEngineers(node["assigned"].as<int>(getAssignedEngineers()));
	setTimeSpent(node["spent"].as<int>(getTimeSpent()));
	setAmountTotal(node["amount"].as<int>(getAmountTotal()));
	setInfiniteAmount(node["infinite"].as<bool>(getInfiniteAmount()));
	setSellItems(node["sell"].as<bool>(getSellItems()));
	// backwards compatibility
	if (getAmountTotal() == std::numeric_limits<int>::max())
	{
		setAmountTotal(999);
		setInfiniteAmount(true);
		setSellItems(true);
	}
}
}
