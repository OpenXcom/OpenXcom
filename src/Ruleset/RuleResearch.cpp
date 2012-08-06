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

RuleResearch::RuleResearch(const std::string & name) : _name(name), _cost(0), _needItem(false)
{
}

/**
 * Loads the research project from a YAML file.
 * @param node YAML node.
 */
void RuleResearch::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "cost")
		{
			i.second() >> _cost;
		}
		else if (key == "needItem")
		{
			i.second() >> _needItem;
		}
		else if (key == "dependencies")
		{
			i.second() >> _dependencies;
		}
		else if (key == "unlocks")
		{
			i.second() >> _unlocks;
		}
	}
}

/**
 * Saves the research project to a YAML file.
 * @param out YAML emitter.
 */
void RuleResearch::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "cost" << YAML::Value << _cost;
	out << YAML::Key << "needItem" << YAML::Value << _needItem;
	out << YAML::Key << "dependencies" << YAML::Value << _dependencies;
	out << YAML::Key << "unlocks" << YAML::Value << _unlocks;
	out << YAML::EndMap;
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

}
