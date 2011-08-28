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

ResearchProject::ResearchProject(RuleResearchProject * p) : _project(p), _spent(0), _assigned(0)
{
}

bool ResearchProject::step()
{
	_spent += _assigned;
	if (_spent >= _project->getCost())
	{
		return true;
	}
	return false;
}
  
void ResearchProject::setAssigned (int nb)
{
	_assigned = nb;
}

const RuleResearchProject * ResearchProject::getRuleResearchProject () const
{
	return _project;
}

int ResearchProject::getAssigned () const
{
	return _assigned;
}

float ResearchProject::getSpent () const
{
	return _spent;
}

void ResearchProject::setSpent (float f)
{
	_spent = f;
}

void ResearchProject::load(const YAML::Node& node, const Ruleset *rule)
{
	int assigned;
	float spent;
	node["assigned"] >> assigned;
	node["spent"] >> spent;
	setAssigned(assigned);
	setSpent(spent);
}

void ResearchProject::save(YAML::Emitter& out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "project" << YAML::Value << getRuleResearchProject ()->getName ();
	out << YAML::Key << "assigned" << YAML::Value << getAssigned ();
	out << YAML::Key << "spent" << YAML::Value << getSpent ();
	out << YAML::EndMap;
}
}
