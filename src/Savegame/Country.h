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
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleCountry;

/**
 * Represents a country that funds the player.
 * Contains variable info about a country like
 * monthly funding and various activities.
 */
class Country
{
private:
	RuleCountry *_rules;
	bool _pact, _newPact;
	std::vector<int> _funding, _activityXcom, _activityAlien;
	int _satisfaction;
public:
	/// Creates a new country of the specified type.
	Country(RuleCountry *rules, bool gen = true);
	/// Cleans up the country.
	~Country();
	/// Loads the country from YAML.
	void load(const YAML::Node& node);
	/// Saves the country to YAML.
	YAML::Node save() const;
	/// Gets the country's ruleset.
	RuleCountry *getRules() const;
	/// Gets the country's funding.
	std::vector<int> &getFunding();
	/// Sets the country's funding.
	void setFunding(int funding);
	/// get the country's satisfaction level
	int getSatisfaction() const;
	/// add xcom activity in this country
	void addActivityXcom(int activity);
	/// add alien activity in this country
	void addActivityAlien(int activity);
	/// get xcom activity to this country
	std::vector<int> &getActivityXcom();
	/// get xcom activity to this country
	std::vector<int> &getActivityAlien();
	/// store last month's counters, start new counters, set this month's change.
	void newMonth(int xcomTotal, int alienTotal, int pactScore);
	/// are we signing a new pact?
	bool getNewPact() const;
	/// sign a pact at the end of this month.
	void setNewPact();
	/// have we signed a pact?
	bool getPact() const;
	/// sign a pact immediately
	void setPact();
};

}
