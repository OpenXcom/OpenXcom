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
#include "RuleResearchProject.h"

namespace OpenXcom
{

RuleResearchProject::RuleResearchProject(const std::string & name, int cost) : _name(name), _cost(cost)
{
}

void RuleResearchProject::addDependency (RuleResearchProject * rp)
{
	_dependencys.push_back(rp);
}

int RuleResearchProject::getCost() const
{
	return _cost;
}

const std::string & RuleResearchProject::getName () const
{
	return _name;
}

const std::vector<RuleResearchProject *> & RuleResearchProject::getDependencys () const
{
	return _dependencys;
}

bool RuleResearchProject::needItem() const
{
	return _needItem;
}

void RuleResearchProject::setNeedItem(bool b)
{
	_needItem = b;
}

const std::vector<RuleResearchProject *> & RuleResearchProject::getUnlocked () const
{
	return _unlocks;
}

void RuleResearchProject::addUnlocked (RuleResearchProject * rp)
{
	_unlocks.push_back(rp);
}

}
