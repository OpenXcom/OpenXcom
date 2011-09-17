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
#ifndef OPENXCOM_RULRESEARCHPROJECT_H
#define OPENXCOM_RULRESEARCHPROJECT_H

#include <string>
#include <vector>

namespace OpenXcom
{
/**
   Represent one research project.
   Dependency and unlock. Dependency is the list of RuleResearchProject which must be discovered before a RuleResearchProject became available. Unlock  are used to immediately unlock a RuleResearchProject(even if not all dependency have been researched).

   Fake ResearchProject. A RuleResearchProject is fake one, if t's cost is 0. They are used to to create check point in the dependency tree.
   For example if we have a Research E which need either A & B or C & D. We create 2 fake research project:
   * F which need A & B
   * G which need C & D
   both F and G can unlock E.
*/
class RuleResearchProject
{
 private:
	std::string _name;
	std::wstring _description;
	int _cost;
	std::vector<RuleResearchProject *> _dependencys;
	std::vector<RuleResearchProject *> _unlocks;
	bool _needItem;
public:
	RuleResearchProject(const std::string & name, int cost);
	/// Add a Dependency
	void addDependency (RuleResearchProject * rp);
	/// Get time needed to discover this ResearchProject
	int getCost() const;
	/// Get the research name
	const std::string & getName () const;
	/// Get the research dependencys
	const std::vector<RuleResearchProject *> & getDependencys () const;
	/// Does this ResearchProject need a corresponding Item to be researched ?
	bool needItem() const;
	/// Set if this ResearchProject need a corresponding Item to be researched.
	void setNeedItem(bool b);
	/// Get the list of ResearchProjects unlocked by this research
	const std::vector<RuleResearchProject *> & getUnlocked () const;
	/// Add a ResearchProject which can be unlocked
	void addUnlocked (RuleResearchProject * rp);
};
}

#endif
