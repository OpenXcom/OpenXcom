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
#include "ResearchProject.h"
#include "../Mod/RuleResearch.h"

namespace OpenXcom
{
const float PROGRESS_LIMIT_UNKNOWN = 0.333f;
const float PROGRESS_LIMIT_POOR = 0.07f;
const float PROGRESS_LIMIT_AVERAGE = 0.13f;
const float PROGRESS_LIMIT_GOOD = 0.25f;

ResearchProject::ResearchProject(RuleResearch * p, int c) : _project(p), _assigned(0), _spent(0), _cost(c)
{
}

/**
 * Called every day to compute time spent on this ResearchProject
 * @return true if the ResearchProject is finished
 */
bool ResearchProject::step()
{
	_spent += _assigned;
	return isFinished();
}

/**
 * gets state of project.
 */
bool ResearchProject::isFinished()
{
	return _spent >= getCost();
}

/**
 * Changes the number of scientist to the ResearchProject
 * @param nb number of scientist assigned to this ResearchProject
 */
void ResearchProject::setAssigned (int nb)
{
	_assigned = nb;
}

const RuleResearch * ResearchProject::getRules() const
{
	return _project;
}

/**
 * Returns the number of scientist assigned to this project
 * @return Number of assigned scientist.
 */
int ResearchProject::getAssigned() const
{
	return _assigned;
}

/**
 * Returns the time already spent on this project
 * @return the time already spent on this ResearchProject(in man/day)
 */
int ResearchProject::getSpent() const
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
 */
void ResearchProject::load(const YAML::Node& node)
{
	setAssigned(node["assigned"].as<int>(getAssigned()));
	setSpent(node["spent"].as<int>(getSpent()));
	setCost(node["cost"].as<int>(getCost()));
}

/**
 * Saves the research project to a YAML file.
 * @return YAML node.
 */
YAML::Node ResearchProject::save() const
{
	YAML::Node node;
	node["project"] = getRules()->getName();
	node["assigned"] = getAssigned();
	node["spent"] = getSpent();
	node["cost"] = getCost();
	return node;
}

/**
 * Return a string describing Research progress.
 * @return a string describing Research progress.
 */
std::string ResearchProject::getResearchProgress() const
{
	float progress = (float)getSpent() / getRules()->getCost();
	if (getAssigned() == 0)
	{
		return "STR_NONE";
	}
	else if (progress <= PROGRESS_LIMIT_UNKNOWN)
	{
		return "STR_UNKNOWN";
	}
	else
	{
		float rating = (float)getAssigned();
		rating /= getRules()->getCost();
		if (rating <= PROGRESS_LIMIT_POOR)
		{
			return "STR_POOR";
		}
		else if (rating <= PROGRESS_LIMIT_AVERAGE)
		{
			return "STR_AVERAGE";
		}
		else if (rating <= PROGRESS_LIMIT_GOOD)
		{
			return "STR_GOOD";
		}
		return "STR_EXCELLENT";
	}
}

}
