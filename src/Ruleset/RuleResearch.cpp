/*
 * Copyright 2010-2015 OpenXcom Developers.
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

RuleResearch::RuleResearch(const std::string & name) : _name(name), _cost(0), _points(0), _needItem(false), _listOrder(0)
{
}

/**
 * Loads the research project from a YAML file.
 * @param node YAML node.
 * @param listOrder The list weight for this research.
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
 * Gets the cost of this ResearchProject.
 * @return The cost of this ResearchProject (in man/day).
 */
int RuleResearch::getCost() const
{
	return _cost;
}

/**
 * Gets the name of this ResearchProject.
 * @return The name of this ResearchProject.
 */
const std::string & RuleResearch::getName() const
{
	return _name;
}

/**
 * Gets the list of dependencies, i.e. ResearchProjects, that must be discovered before this one.
 * @return The list of ResearchProjects.
 */
const std::vector<std::string> & RuleResearch::getDependencies() const
{
	return _dependencies;
}

/**
 * Checks if this ResearchProject needs a corresponding Item to be researched.
 *  @return True if the ResearchProject needs a corresponding item.
 */
bool RuleResearch::needItem() const
{
	return _needItem;
}

/**
 * Gets the list of ResearchProjects unlocked by this research.
 * @return The list of ResearchProjects.
 */
const std::vector<std::string> & RuleResearch::getUnlocked() const
{
	return _unlocks;
}

/**
 * Get the points earned for this ResearchProject.
 * @return The points earned for this ResearchProject.
 */
int RuleResearch::getPoints() const
{
	return _points;
}

/**
 * Gets the list of ResearchProjects granted at random for free by this research.
 * @return The list of ResearchProjects.
 */
const std::vector<std::string> & RuleResearch::getGetOneFree() const
{
	return _getOneFree;
}

/**
 * Gets what article to look up in the ufopedia.
 * @return The article to look up in the ufopaedia
 */
const std::string RuleResearch::getLookup() const
{
	return _lookup;
}

/**
 * Gets the requirements for this ResearchProject.
 * @return The requirement for this research.
 */
const std::vector<std::string> & RuleResearch::getRequirements() const
{
	return _requires;
}

/**
 * Gets the list weight for this research item.
 * @return The list weight for this research item.
 */
int RuleResearch::getListOrder() const
{
	return _listOrder;
}

}
