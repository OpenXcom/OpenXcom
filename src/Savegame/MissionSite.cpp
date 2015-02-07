/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "MissionSite.h"
#include <sstream>
#include "../Engine/Language.h"
#include "../Ruleset/RuleAlienMission.h"

namespace OpenXcom
{

/**
 * Initializes a mission site.
 */
MissionSite::MissionSite(const RuleAlienMission *rules) : Target(), _rules(rules), _id(0), _secondsRemaining(0), _inBattlescape(false)
{
}

/**
 *
 */
MissionSite::~MissionSite()
{
}

/**
 * Loads the mission site from a YAML file.
 * @param node YAML node.
 */
void MissionSite::load(const YAML::Node &node)
{
	Target::load(node);
	_id = node["id"].as<int>(_id);
	_secondsRemaining = node["secondsRemaining"].as<size_t>(_secondsRemaining);
	_race = node["race"].as<std::string>(_race);
	_inBattlescape = node["inBattlescape"].as<bool>(_inBattlescape);
}

/**
 * Saves the mission site to a YAML file.
 * @return YAML node.
 */
YAML::Node MissionSite::save() const
{
	YAML::Node node = Target::save();
	node["type"] = _rules->getType();
	node["id"] = _id;
	if (_secondsRemaining)
		node["secondsRemaining"] = _secondsRemaining;
	node["race"] = _race;
	if (_inBattlescape)
		node["inBattlescape"] = _inBattlescape;
	return node;
}

/**
 * Saves the mission site's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node MissionSite::saveId() const
{
	YAML::Node node = Target::saveId();
	node["type"] = _rules->getType();
	node["id"] = _id;
	return node;
}

/**
 * Returns the ruleset for the mission's type.
 * @return Pointer to ruleset.
 */
const RuleAlienMission *MissionSite::getRules() const
{
	return _rules;
}

/**
 * Returns the mission site's unique ID.
 * @return Unique ID.
 */
int MissionSite::getId() const
{
	return _id;
}

/**
 * Changes the mission site's unique ID.
 * @param id Unique ID.
 */
void MissionSite::setId(int id)
{
	_id = id;
}

/**
 * Returns the mission site's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring MissionSite::getName(Language *lang) const
{
	return lang->getString(_rules->getMarkerName()).arg(_id);
}

/**
 * Returns the globe marker for the mission site.
 * @return Marker sprite, -1 if none.
 */
int MissionSite::getMarker() const
{
	if (_rules->getMarkerIcon() == -1)
		return 5;
	return _rules->getMarkerIcon();
}

/**
 * Returns the number of seconds remaining before the mission site expires.
 * @return Amount of seconds.
 */
size_t MissionSite::getSecondsRemaining() const
{
	return _secondsRemaining;
}

/**
 * Changes the number of seconds before the mission site expires.
 * @param seconds Amount of seconds.
 */
void MissionSite::setSecondsRemaining(size_t seconds)
{
	_secondsRemaining = seconds;
}

/**
 * Returns the alien race currently residing in the mission site.
 * @return Alien race.
 */
std::string MissionSite::getAlienRace() const
{
	return _race;
}

/**
 * Changes the alien race currently residing in the mission site.
 * @param race Alien race.
 */
void MissionSite::setAlienRace(const std::string &race)
{
	_race = race;
}

/**
 * Gets the mission site's battlescape status.
 * @return bool
 */
bool MissionSite::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Sets the mission site's battlescape status.
 * @param inbattle .
 */
void MissionSite::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

}
