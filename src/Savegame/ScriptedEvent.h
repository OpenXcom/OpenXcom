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
#ifndef OPENXCOM_SCRIPTED_EVENT_H
#define OPENXCOM_SCRIPTED_EVENT_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "../Ruleset/RuleScriptedEvent.h"

namespace OpenXcom
{
class Ufo;
class Globe;
class Game;
class SavedGame;
class Ruleset;
class Base;
class Craft;
class ScriptedEventLocation;
class Ufo;

/**
 * Represents progress of a scripted event
 * Unlike AlienMissions, scripted events should not be repeatable, so we 
 * leave entries in here to reflect the completed mission.
 * @sa RuleAlienMission
 */
class ScriptedEvent
{
private:
	/// Rules definition for this event
	const RuleScriptedEvent &_rule;
	/// Flag indicating whether event has finished
	bool _isOver;
	/// Scripted events can be of 3 types
	/// - SE_DESTINATION events are simple, and have no geoscape state
	/// - SE_GEOSCAPE events have a static geoscape location, represented by a ScriptedEventTarget
	/// - SE_UFO events spawn and track a UFO, represented by a UFO
	ScriptedEventLocation *_scriptedEventLocation;
	Ufo *_ufo;
	int _ufoDestIndex;

public:
	// Data

	/// Creates a mission of the specified type.
	ScriptedEvent(const RuleScriptedEvent &rule);
	/// Cleans up the mission info.
	~ScriptedEvent();
	/// Loads the mission from YAML.
	void load(const YAML::Node& node, SavedGame &game);
	/// Saves the mission to YAML.
	YAML::Node save() const;
	/// Gets the mission's type.
	const std::string &getType() const;
	/// Returns the Scripted Event Rule
	const RuleScriptedEvent &getRules() const {return _rule;}
	/// Set method for event location
	void setScriptedEventLocation(ScriptedEventLocation *scriptedEventLocation);
	/// Get method for event location
	ScriptedEventLocation *getScriptedEventLocation() const;
	/// Set method for UFO
	void setUfo(Ufo *ufo);
	/// Get method for UFO
	Ufo *getUfo() const;
	/// Method to set up UFO 
	void initUfo();
	/// Method to alter an event UFOs course
	void newUfoDestination();

	// Behaviour

	/// Is this event over?
	bool isOver() const;
	/// Set method for isOver flag
	void setIsOver(bool isOver);
	/// Check whether event requirements are met by this Base/Craft
	bool checkDetectionRestrictions(SavedGame *save, Base *base, Craft *craft);
	/// Check whether event requirements are met by this Base/Craft
	bool checkEngagementRestrictions(SavedGame *save, Base *base, Craft *craft);
	/// Check whether event requirements are met by this Base/Craft
	bool checkRestrictions(EventRestrictions &eventRestrictions, SavedGame *save, Base *base, Craft *craft);

private:

};

}

#endif
