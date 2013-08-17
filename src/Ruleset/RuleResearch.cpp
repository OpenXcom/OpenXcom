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
#include "RuleResearch.h"

namespace OpenXcom
{

RuleResearch::RuleResearch(const std::string & name) : _name(name), _lookup(""), _cost(0), _points(0), _getOneFree(0), _requires(0), _needItem(false), _listOrder(0)
{
}

/**
 * Loads the research project from a YAML file.
 * @param node YAML node.
 * @param listOrder the list weight for this research.
 */
void RuleResearch::load(const YAML::Node &node, int listOrder)
{
	_name = node["name"].as<std::string>(_name);
	_lookup = node["lookup"].as<std::string>(_lookup);
	_cost = node["cost"].as<int>(_cost);
	_points = node["points"].as<int>(_points);
	_dependencies = node["dependencies"].as< std::vector<std::string> >(_dependencies);
	_unlocks = node["unlocks"].as< std::vector<std::string> >(_unlocks);
	_getOneFree = node["getOneFree"].as< std::vector<std::string> >(_getOneFree);
	_requires = node["requires"].as< std::vector<std::string> >(_requires);
	_needItem = node["needItem"].as<bool>(_needItem);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
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
const std::vector<std::string> & RuleResearch::getDependencies () const
{
	return _dependencies;
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
   @return The list of ResearchProject unlocked by this research project.
*/
const std::vector<std::string> & RuleResearch::getUnlocked () const
{
	return _unlocks;
}

/**
 * Get the points earned for this ResearchProject
 * @return points for this ResearchProject
 */
int RuleResearch::getPoints() const
{
	return _points;
}

/**
 *@return The list of ResearchProject unlocked by this research project.
 */
const std::vector<std::string> & RuleResearch::getGetOneFree () const
{
	return _getOneFree;
}

/**
 * @return what article to look up on the ufopaedia
 */
const std::string RuleResearch::getLookup () const
{
	return _lookup;
}

/**
 * @return The requirement for this research.
 */
const std::vector<std::string> & RuleResearch::getRequirements() const
{
	return _requires;
}

/**
 * @return the list weight for this research item.
 */
int RuleResearch::getListOrder() const
{
	return _listOrder;
}

}
