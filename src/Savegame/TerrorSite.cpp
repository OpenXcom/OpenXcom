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
#include "TerrorSite.h"
#include <sstream>
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a terror site.
 */
TerrorSite::TerrorSite() : Target(), _id(0), _secondsRemaining(0), _inBattlescape(false)
{
}

/**
 *
 */
TerrorSite::~TerrorSite()
{
}

/**
 * Loads the terror site from a YAML file.
 * @param node YAML node.
 */
void TerrorSite::load(const YAML::Node &node)
{
	Target::load(node);
	_id = node["id"].as<int>(_id);
	_secondsRemaining = node["secondsRemaining"].as<size_t>(_secondsRemaining);
	_race = node["race"].as<std::string>(_race);
	_inBattlescape = node["inBattlescape"].as<bool>(_inBattlescape);
}

/**
 * Saves the terror site to a YAML file.
 * @return YAML node.
 */
YAML::Node TerrorSite::save() const
{
	YAML::Node node = Target::save();
	node["id"] = _id;
	if (_secondsRemaining)
		node["secondsRemaining"] = _secondsRemaining;
	node["race"] = _race;
	if (_inBattlescape)
		node["inBattlescape"] = _inBattlescape;
	return node;
}

/**
 * Saves the terror site's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node TerrorSite::saveId() const
{
	YAML::Node node = Target::saveId();
	node["type"] = "STR_TERROR_SITE";
	node["id"] = _id;
	return node;
}

/**
 * Returns the terror site's unique ID.
 * @return Unique ID.
 */
int TerrorSite::getId() const
{
	return _id;
}

/**
 * Changes the terror site's unique ID.
 * @param id Unique ID.
 */
void TerrorSite::setId(int id)
{
	_id = id;
}

/**
 * Returns the terror site's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring TerrorSite::getName(Language *lang) const
{
	return lang->getString("STR_TERROR_SITE").arg(_id);
}

/**
 * Returns the globe marker for the terror site.
 * @return Marker sprite, -1 if none.
 */
int TerrorSite::getMarker() const
{
	return 5;
}

/**
 * Returns the number of seconds remaining before the terror site expires.
 * @return Amount of seconds.
 */
size_t TerrorSite::getSecondsRemaining() const
{
	return _secondsRemaining;
}

/**
 * Changes the number of seconds before the terror site expires.
 * @param seconds Amount of seconds.
 */
void TerrorSite::setSecondsRemaining(size_t seconds)
{
	_secondsRemaining = seconds;
}

/**
 * Returns the alien race currently residing in the terror site.
 * @return Alien race.
 */
std::string TerrorSite::getAlienRace() const
{
	return _race;
}

/**
 * Changes the alien race currently residing in the terror site.
 * @param race Alien race.
 */
void TerrorSite::setAlienRace(const std::string &race)
{
	_race = race;
}

/**
 * Gets the terror site's battlescape status.
 * @return bool
 */
bool TerrorSite::isInBattlescape() const
{
	return _inBattlescape;
}

/**
 * Sets the terror site's battlescape status.
 * @param inbattle .
 */
void TerrorSite::setInBattlescape(bool inbattle)
{
	_inBattlescape = inbattle;
}

}
