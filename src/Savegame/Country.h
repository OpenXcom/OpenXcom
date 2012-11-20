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
#ifndef OPENXCOM_COUNTRY_H
#define OPENXCOM_COUNTRY_H

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
	int _change;
	std::vector<int> _funding, _activityXcom, _activityAlien;
public:
	/// Creates a new country of the specified type.
	Country(RuleCountry *rules, bool gen = true);
	/// Cleans up the country.
	~Country();
	/// Loads the country from YAML.
	void load(const YAML::Node& node);
	/// Saves the country to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the country's ruleset.
	RuleCountry *const getRules() const;
	/// Gets the country's funding.
	int getFunding(int month) const;
	/// Sets the country's funding.
	void setFunding(int funding);
	/// Gets the country's funding change.
	int getChange() const;
	/// get the country's satisfaction level
	int getSatisfaction();
	/// add xcom activity in this country
	void addActivityXcom(int activity);
	/// add alien activity in this country
	void addActivityAlien(int activity);
	/// get xcom activity to this country
	int getActivityXcom(int month) const;
	/// get xcom activity to this country
	int getActivityAlien(int month) const;
	/// store last month's counters, start new counters, set this month's change.
	void newMonth();
};

}

#endif
