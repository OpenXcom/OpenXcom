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
#include "Unit.h"
#include "StatStringCondition.h"

namespace OpenXcom
{

/**
 * For adding statStrings to the game.
 * See http://ufopaedia.org/index.php?title=Statstrings
 */

/*
NameStats in XCOMUTIL.CFG:
 
StatStrings, which were added in version 7.2, are used for automatically
renaming soldiers.  They are case sensitive and they are processed in the
order in which they are specified.  The format of these StatStrings is:
 
  string statid:[lower]-[upper] [statid:[lower]-[upper] [...] ]
 
  where string = string to add to name.  If length == 1, then the strings
				 accumulate.  Otherwise, success ends the search.  All
				 single character strings SHOULD come last, but that is
				 not required. / and ; are not a valid strings.  The maximum
				 string length is 19.
		statid = a = armor (front)
				 b = bravery
				 d = time units (dexterity)
				 e = stamina (endurance)
				 f = firing accuracy
				 h = health
				 k = psi/mc skill
				 p = psi/mc strength (if skill > 0)
				 q = psi/mc strength (regardless of skill)
				 r = reactions
				 s = strength
				 t = throwing accuracy
		 lower = lower limit of stat (inclusive), defaults to 0
		 upper = upper limit of stat (inclusive), defaults to 255
 
Stat ranges are ANDed together when testing for success.  To achieve a logical
OR, list the same string more than once.  For example, Wimp b:0-10 and
Wimp s:0-20 would designate a Wimp as either someone who is not brave or
someone who is very weak.  By arranging the string properly, you can put all
of your strengths before your weaknesses or list the stats in order, which is
the default.  
 
You may specify as many ranges for one StatString as you like, up to the limit
of the memory I have allocated.  There is enough space for a total of more
than 600 ranges to be defined.  Let me know if you need more.
 
The # character is a special string that will be replaced by numbers
corresponding to the values of the statistics listed, divided by 10.  For
example, # fr will generate the string 74 if the firing accuracy value is 72
and the reactions value is 48.  This is a cumulative string, so that you could
prefix your strings with letters by using statid:0-255 before it.  For
example P p:0-255 followed by # p would generate P6 for a Psi Strength of 64,
assuming that Psi Skill is non-zero.  Psi/MC Strength is always reported as
zero unless Psi/MC Skill is greater than 0.  To test Psi/MC Strength without
checking the Psi/MC Skill, use the q statid.
 
Since strings longer than one character will terminate the checking, these
strings are normally listed first.  More stats accumulating after Snpr would
ruin the usefulness of Snpr as an equipment type.  However, if you wanted to
place an indicator in front of Snpr as a warning of some critical weakness,
you could place a single character string at the start of the list.  For
example, if you wanted to know that your Snpr had a very low Psi/MC Strength
and had little resistance to alien control, you could put x p:0-30 or x q:0-30
at the start of the list to produce Snpr or xSnpr as your final code. This is
what I did in the default case.  See the XCOMUTIL.CFG file for examples.
 
If the resulting string exceeds the maximum length for a name, the first
name will be replaced by an initial.  If the string is still too long, the
first name will be removed entirely.  If this string is still too long, no
change will be made.  
 
If / is used as a statid, it indicates that this is the last required name
stat.  That is, if the name stats make the name too long, the total string
will be reduced to the size at the moment that the / statid was
encountered.  XcomUtil will again check the length of the name, reducing
or eliminating the first name as needed.  If this string is still too
long, nothing will be changed.
 */

class StatString
{
private:
	std::string _stringToBeAddedIfAllConditionsAreMet;
	std::vector<StatStringCondition*> _conditions;
	static StatStringCondition *getCondition(const std::string &conditionName, const YAML::Node &node);
public:
	/// Creates a blank StatString ruleset.
	StatString();
	/// Cleans up the StatString ruleset.
	virtual ~StatString();
	/// Loads the StatString from YAML.
	void load(const YAML::Node& node);
	/// Get the conditions for this StatString.
	const std::vector<StatStringCondition*> &getConditions() const;
	/// Get the StatString string.
	std::string getString() const;
	/// Calculate a StatString.
	static std::string calcStatString(UnitStats &currentStats, const std::vector<StatString*> &statStrings, bool psiStrengthEval, bool inTraining);
	/// Get the CurrentStats.
	static std::map<std::string, int> getCurrentStats(UnitStats &currentStats);
};

}
