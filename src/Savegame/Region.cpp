/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "Region.h"
#include "../Ruleset/RuleRegion.h"

namespace OpenXcom
{

/**
 * Initializes a region of the specified type.
 * @param rules Pointer to ruleset.
 */
Region::Region(RuleRegion *rules): _rules(rules)
{
	_activityAlien.push_back(0);
	_activityXcom.push_back(0);
}

/**
 *
 */
Region::~Region()
{
}

/**
 * Loads the region from a YAML file.
 * @param node YAML node.
 */
void Region::load(const YAML::Node &node)
{
	node["activityXcom"] >> _activityXcom;
	node["activityAlien"] >> _activityAlien;
}

/**
 * Saves the region to a YAML file.
 * @param out YAML emitter.
 */
void Region::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _rules->getType();
	out << YAML::Key << "activityXcom" << YAML::Value << _activityXcom;
	out << YAML::Key << "activityAlien" << YAML::Value << _activityAlien;
	out << YAML::EndMap;
}

/**
 * Returns the ruleset for the region's type.
 * @return Pointer to ruleset.
 */
RuleRegion *Region::getRules() const
{
	return _rules;
}

/**
 * Adds to the region's xcom activity level.
 */
void Region::addActivityXcom(int activity)
{
	_activityXcom.back() += activity;
}

/**
 * Adds to the region's alien activity level.
 */
void Region::addActivityAlien(int activity)
{
	_activityAlien.back() += activity;
}

/**
 * Gets the region's xcom activity level.
 * @return activity level.
 */
const std::vector<int> &Region::getActivityXcom() const
{
	return _activityXcom;
}

/**
 * Gets the region's alien activity level.
 * @return activity level.
 */
const std::vector<int> &Region::getActivityAlien() const
{
	return _activityAlien;
}

void Region::newMonth()
{
	_activityAlien.push_back(0);
	_activityXcom.push_back(0);
	if(_activityAlien.size() > 12)
		_activityAlien.erase(_activityAlien.begin());
	if(_activityXcom.size() > 12)
		_activityXcom.erase(_activityXcom.begin());
}
}
