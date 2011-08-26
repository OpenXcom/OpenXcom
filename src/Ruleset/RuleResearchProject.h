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
	void addDependency (RuleResearchProject * rp);
	int getCost() const;
	const std::string & getName () const;
	const std::vector<RuleResearchProject *> & getDependencys () const;
	bool needItem() const;
	void setNeedItem(bool b);
	const std::vector<RuleResearchProject *> & getUnlocked () const;
	void addUnlocked (RuleResearchProject * rp);
};

struct findRuleResearchProjectByString : public std::unary_function<RuleResearchProject *,
							    bool>
{
	std::string _toFind;
	findRuleResearchProjectByString(const std::string & toFind);
	bool operator()(RuleResearchProject *r) const;
};

}

#endif
