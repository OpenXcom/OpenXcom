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

	bool step();

	void setAssigned (int nb);
	int getAssigned () const;

	float getSpent () const;
	void setSpent (float f);

	float getCost() const;
	void setCost(float f);

	const RuleResearchProject * getRuleResearchProject () const;

	void load(const YAML::Node& node, const Ruleset *rule);
	void save(YAML::Emitter& out) const;
};
}
#endif
