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
#ifndef OPENXCOM_RESEARCHPROJECT
#define OPENXCOM_RESEARCHPROJECT
#include "yaml.h"

namespace OpenXcom
{
class RuleResearchProject;
class Ruleset;

/**
   Represent a ResearchProject
   Contain information about assigned scientist, time already spent and cost of the project.
*/
class ResearchProject
{
	RuleResearchProject * _project;
	int _assigned;
	float _spent;
	float _cost;
public:
	ResearchProject(RuleResearchProject * p, float f = 0.0f);
	/// Game logic. Called every new day to compute time spent.
	bool step();
	/// set the number of scientist assigned to this ResearchProject
	void setAssigned (int nb);
	/// get the number of scientist assigned to this ResearchProject
	int getAssigned () const;
	/// get time already spent on this ResearchProject
	float getSpent () const;
	/// set time already spent on this ResearchProject
	void setSpent (float f);
	/// get time cost of this ResearchProject
	float getCost() const;
	/// set time cost of this ResearchProject
	void setCost(float f);
	/// get the ResearchProject Ruleset
	const RuleResearchProject * getRuleResearchProject () const;
	/// load the ResearchProject from YAML
	void load(const YAML::Node& node, const Ruleset *rule);
	/// save the ResearchProject to YAML
	void save(YAML::Emitter& out) const;
};
}
#endif
