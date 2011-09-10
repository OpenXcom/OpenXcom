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
#include "ResearchProject.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Ruleset/Ruleset.h"
#include <algorithm>

namespace OpenXcom
{

ResearchProject::ResearchProject(RuleResearchProject * p, int c) : _project(p), _spent(0), _assigned(0), _cost(c)
{
}

/**
 * Called every day to compute time spent on this ResearchProject
 * @return true if the ResearchProject is finished
*/
bool ResearchProject::step()
{
	_spent += _assigned;
	if (_spent >= getCost())
	{
		return true;
	}
	return false;
}
  
/**
 * Changes the number of scientist to the ResearchProject
 * @param nb number of scientist assigned to this ResearchProject
 */
void ResearchProject::setAssigned (int nb)
{
	_assigned = nb;
}

const RuleResearchProject * ResearchProject::getRuleResearchProject () const
{
	return _project;
}

/**
 * Returns the number of scientist assigned to this project
 * @return Number of assigned scientist.
 */
int ResearchProject::getAssigned () const
{
	return _assigned;
}

/**
 * Returns the time already spent on this project
 * @return the time already spent on this ResearchProject(in man/day)
 */
int ResearchProject::getSpent () const
{
	return _spent;
}

/**
 * Changes the cost of the ResearchProject
 * @param spent new project cost(in man/day)
 */
void ResearchProject::setSpent (int spent)
{
	_spent = spent;
}

/**
 * Returns the cost of the ResearchProject
 * @return the cost of the ResearchProject(in man/day)
 */
int ResearchProject::getCost() const
{
	return _cost;
}

/**
 * Changes the cost of the ResearchProject
 * @param f new project cost(in man/day)
 */
void ResearchProject::setCost(int f)
{
	_cost = f;
}

/**
 * Loads the research project from a YAML file.
 * @param node YAML node.
 * @param rule Ruleset for the saved game.
 */
void ResearchProject::load(const YAML::Node& node, const Ruleset *rule)
{
	int assigned;
	int spent;
	int cost;
	node["assigned"] >> assigned;
	node["spent"] >> spent;
	node["cost"] >> cost;
	setAssigned(assigned);
	setSpent(spent);
	setCost(cost);
}

/**
 * Saves the research project to a YAML file.
 * @param out YAML emitter.
 */
void ResearchProject::save(YAML::Emitter& out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "project" << YAML::Value << getRuleResearchProject ()->getName ();
	out << YAML::Key << "assigned" << YAML::Value << getAssigned ();
	out << YAML::Key << "spent" << YAML::Value << getSpent ();
	out << YAML::Key << "cost" << YAML::Value << getCost ();
	out << YAML::EndMap;
}
}
