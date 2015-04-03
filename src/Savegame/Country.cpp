/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "Country.h"
#include "../Ruleset/RuleCountry.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Initializes a country of the specified type.
 * @param rules Pointer to ruleset.
 * @param gen Generate new funding.
 */
Country::Country(RuleCountry *rules, bool gen) : _rules(rules), _pact(false), _newPact(false), _funding(0), _satisfaction(2)
{
	if (gen)
	{
		_funding.push_back(_rules->generateFunding());
	}
	_activityAlien.push_back(0);
	_activityXcom.push_back(0);
}

/**
 *
 */
Country::~Country()
{
}

/**
 * Loads the country from a YAML file.
 * @param node YAML node.
 */
void Country::load(const YAML::Node &node)
{
	_funding = node["funding"].as< std::vector<int> >(_funding);
	_activityXcom = node["activityXcom"].as< std::vector<int> >(_activityXcom);
	_activityAlien = node["activityAlien"].as< std::vector<int> >(_activityAlien);
	_pact = node["pact"].as<bool>(_pact);
	_newPact = node["newPact"].as<bool>(_newPact);
}

/**
 * Saves the country to a YAML file.
 * @return YAML node.
 */
YAML::Node Country::save() const
{
	YAML::Node node;
	node["type"] = _rules->getType();
	node["funding"] = _funding;
	node["activityXcom"] = _activityXcom;
	node["activityAlien"] = _activityAlien;
	if (_pact)
	{
		node["pact"] = _pact;
	}
	else if (_newPact)
	{
		node["newPact"] = _newPact;
	}
	return node;
}

/**
 * Returns the ruleset for the country's type.
 * @return Pointer to ruleset.
 */
RuleCountry *Country::getRules() const
{
	return _rules;
}

/**
 * Returns the country's current monthly funding.
 * @return Monthly funding.
 */
std::vector<int> &Country::getFunding()
{
	return _funding;
}

/**
 * Changes the country's current monthly funding.
 * @param funding Monthly funding.
 */
void Country::setFunding(int funding)
{
	_funding.back() = funding;
}

/*
 * Keith Richards would be so proud
 * @return satisfaction level, 0 = alien pact, 1 = unhappy, 2 = satisfied, 3 = happy.
 */
int Country::getSatisfaction()
{
	if (_pact)
		return 0;
	return _satisfaction;
}

/**
 * Adds to the country's xcom activity level.
 * @param activity how many points to add.
 */
void Country::addActivityXcom(int activity)
{
	_activityXcom.back() += activity;
}

/**
 * Adds to the country's alien activity level.
 * @param activity how many points to add.
 */
void Country::addActivityAlien(int activity)
{
	_activityAlien.back() += activity;
}

/**
 * Gets the country's xcom activity level.
 * @return activity level.
 */
std::vector<int> &Country::getActivityXcom()
{
	return _activityXcom;
}

/**
 * Gets the country's alien activity level.
 * @return activity level.
 */
std::vector<int> &Country::getActivityAlien()
{
	return _activityAlien;
}

/**
 * reset all the counters,
 * calculate this month's funding,
 * set the change value for the month.
 * @param xcomTotal the council's xcom score
 * @param alienTotal the council's alien score
 */

void Country::newMonth(int xcomTotal, int alienTotal)
{
	_satisfaction = 2;
	int funding = getFunding().back();
	int good = (xcomTotal / 10) + _activityXcom.back();
	int bad = (alienTotal / 20) + _activityAlien.back();
	int oldFunding = _funding.back() / 1000;
	int newFunding = (oldFunding * RNG::generate(5, 20) / 100) * 1000;

	if (bad <= good + 30)
	{
		if (good > bad + 30)
		{
			if (RNG::generate(0, good) > bad)
			{
				// don't go over the cap
				int cap = getRules()->getFundingCap()*1000;
				if (funding + newFunding > cap)
					newFunding = cap - funding;
				if (newFunding)
					_satisfaction = 3;
			}
		}
	}
	else
	{
		if (RNG::generate(0, bad) > good)
		{
			if (newFunding)
			{
				newFunding = -newFunding;
				_satisfaction = 1;
			}
		}
	}

	// about to be in cahoots
	if (_newPact && !_pact)
	{
		_newPact = false;
		_pact = true;
		addActivityAlien(150);
	}



	// set the new funding and reset the activity meters
	if (_pact)
		_funding.push_back(0);
	else if (_satisfaction != 2)
		_funding.push_back(funding + newFunding);
	else
		_funding.push_back(funding);
	
	_activityAlien.push_back(0);
	_activityXcom.push_back(0);
	if (_activityAlien.size() > 12)
		_activityAlien.erase(_activityAlien.begin());
	if (_activityXcom.size() > 12)
		_activityXcom.erase(_activityXcom.begin());
	if (_funding.size() > 12)
		_funding.erase(_funding.begin());
}

/**
 * @return if we will sign a new pact.
 */
bool Country::getNewPact() const
{
	return _newPact;
}

/**
 * sign a new pact at month's end.
 */
void Country::setNewPact()
{
	 _newPact = true;
}

/**
 * no setter for this one, as it gets set automatically
 * at month's end if _newPact is set.
 * @return if we have signed a pact.
 */
bool Country::getPact() const
{
	return _pact;
}

/**
 * sign a new pact.
 */
void Country::setPact()
{
	 _pact = true;
}

}
