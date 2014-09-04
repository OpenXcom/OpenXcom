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
#include "ScriptedEventLocation.h"
#include <sstream>
#include "../Engine/Language.h"
#include "ScriptedEvent.h"
#include "SavedGame.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * Initializes a new geoscape event location
 */
ScriptedEventLocation::ScriptedEventLocation(ScriptedEvent *scriptedEvent) : Target(), _scriptedEvent(scriptedEvent), _detected(false), _id(0)
{
}

/**
 *
 */
ScriptedEventLocation::~ScriptedEventLocation()
{
}

/**
 * Loads the scripted event location from a YAML file.
 * @param node YAML node.
 */
void ScriptedEventLocation::load(const YAML::Node &node, SavedGame &game)
{
	Target::load(node);
	_detected = node["detected"].as<bool>(_detected);
	_id = node["id"].as<int>(_id);

	std::string eventID = node["scriptedEvent"].as<std::string>();
	ScriptedEvent *found = game.getScriptedEvent(eventID);
	if (!found)
	{
		// Corrupt save file.
		throw Exception("Unknown event " + eventID + ", save file is corrupt.");
	}
	_scriptedEvent = found;
	_scriptedEvent->setScriptedEventLocation(this);
}

/**
 * Saves scripted event location to a YAML file.
 * @return YAML node.
 */
YAML::Node ScriptedEventLocation::save() const
{
	YAML::Node node = Target::save();
	node["scriptedEvent"] = _scriptedEvent->getRules().getType();
	node["detected"] = _detected;
	node["id"] = _id;

	return node;
}

/**
 * Saves the scripted event location's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node ScriptedEventLocation::saveId() const
{
	YAML::Node node = Target::saveId();
	node["type"] = "STR_SCRIPTED_EVENT_LOCATION";
	node["id"] = _id;

	return node;
}

/**
 * Returns the scripted event's unique identifying name.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring ScriptedEventLocation::getName(Language *lang) const
{
	return lang->getString(_scriptedEvent->getRules().getLocationEvent().name_id);
}

/**
 * Get method for detected flag.
 * @return detected flag
 */
bool ScriptedEventLocation::getDetected()
{
	return _detected;
}

/**
 * Set method for detected flag.
 * @param detected detected flag
 */
void ScriptedEventLocation::setDetected(bool detected)
{
	_detected = detected;
}

/**
 * Get method for the scripted event parent
 * @return pointer to parent object
 */
ScriptedEvent *ScriptedEventLocation::getScriptedEvent() const
{
	return _scriptedEvent;
}

/**
 * Returns the event location's unique ID. 
 * @return Unique ID.
 */
int ScriptedEventLocation::getId() const
{
	return _id;
}

/**
 * Changes the event locations unique ID.
 * @param id Unique ID.
 */
void ScriptedEventLocation::setId(int id)
{
	_id = id;
}

/**
 * Returns the globe marker for the event location
 * @return Marker sprite, -1 if none.
 */
int ScriptedEventLocation::getMarker() const
{
	return 5;
}

}
