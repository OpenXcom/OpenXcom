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
#include "RuleResearch.h"

namespace OpenXcom
{

RuleResearch::RuleResearch(const std::string & name, int cost) : _name(name), _cost(cost)
{
}

/**
 * Add a ResearchProject must be discovered before this researchProject
 * @param rp a pointer to a ResearchProject
*/
void RuleResearch::addDependency (RuleResearch * rp)
{
	_dependencys.push_back(rp);
}

/**
   Get the cost of this ResearchProject
   @return cost of this ResearchProject(in man/day)
*/
int RuleResearch::getCost() const
{
	return _cost;
}

/**
   Get the name of this ResearchProject
   @return name of this ResearchProject
*/
const std::string & RuleResearch::getName () const
{
	return _name;
}

/**
   Get the list of dependencies
   @return the list of ResearchProject that must be discovered before this one
*/
const std::vector<RuleResearch *> & RuleResearch::getDependencys () const
{
	return _dependencys;
}

/**
   Does this ResearchProject need a corresponding Item to be researched ?
   @return true if the ResearchProject need a corresponding item
*/
bool RuleResearch::needItem() const
{
	return _needItem;
}

/**
   Change if this ResearchProject need a corresponding Item to be researched.
   @param b true if the ResearchProject need a corresponding item
*/
void RuleResearch::setNeedItem(bool b)
{
	_needItem = b;
}

/**
   @return The list of ResearchProject unlocked by this research project.
*/
const std::vector<RuleResearch *> & RuleResearch::getUnlocked () const
{
	return _unlocks;
}

/**
 * Add a ResearchProject which can be unlocked by this researchProject
 * @param rp a pointer to a ResearchProject
*/
void RuleResearch::addUnlocked (RuleResearch * rp)
{
	_unlocks.push_back(rp);
}

}
