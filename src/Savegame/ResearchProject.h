#pragma once
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
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleResearch;
class Mod;

/**
   Represent a ResearchProject
   Contain information about assigned scientist, time already spent and cost of the project.
 */
class ResearchProject
{
	RuleResearch * _project;
	int _assigned;
	int _spent;
	int _cost;
public:
	ResearchProject(RuleResearch * p, int c = 0);
	/// Game logic. Called every new day to compute time spent.
	bool step();
	/// gets state of project.
	bool isFinished();
	/// set the number of scientist assigned to this ResearchProject
	void setAssigned (int nb);
	/// get the number of scientist assigned to this ResearchProject
	int getAssigned() const;
	/// get time already spent on this ResearchProject
	int getSpent() const;
	/// set time already spent on this ResearchProject
	void setSpent (int spent);
	/// get time cost of this ResearchProject
	int getCost() const;
	/// set time cost of this ResearchProject
	void setCost(int f);
	/// get the ResearchProject Mod
	const RuleResearch * getRules() const;
	/// load the ResearchProject from YAML
	void load(const YAML::Node& node);
	/// save the ResearchProject to YAML
	YAML::Node save() const;
	/// Get a string describing current progress.
	std::string getResearchProgress() const;
};

}
