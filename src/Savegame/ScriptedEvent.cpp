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
#define _USE_MATH_DEFINES

#include "ScriptedEvent.h"
#include "../Ruleset/RuleScriptedEvent.h"
#include "../Engine/RNG.h"
#include <assert.h>
#include <algorithm>
#include <functional>
#include <math.h>
#include "Base.h"
#include "Craft.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleUfo.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Waypoint.h"
#include "ScriptedEventLocation.h"


namespace OpenXcom
{

ScriptedEvent::ScriptedEvent(const RuleScriptedEvent &rule) : _rule(rule), _isOver(false), _scriptedEventLocation(0), _ufo(0)
{
	// Empty by design.
}

ScriptedEvent::~ScriptedEvent()
{
}

/**
 * @param node The YAML node containing the data.
 * @param game The game data, required to locate the alien base.
 */
void ScriptedEvent::load(const YAML::Node& node, SavedGame &game)
{
	_isOver = node["isOver"].as<bool>(_isOver);
	_ufoDestIndex = node["ufoDestIndex"].as<int>(_ufoDestIndex);
}

YAML::Node ScriptedEvent::save() const
{
	YAML::Node node;
	node["type"] = _rule.getType();
	node["isOver"] = _isOver;
	if (_ufo) node["ufoDestIndex"] = _ufoDestIndex;

	return node;
}

/**
 * The type ID of the scripted event
 * @return The type ID of the scripted event
 */
const std::string &ScriptedEvent::getType() const
{
	return _rule.getType();
}

/**
 * Check if a mission is over and can be safely removed from the game.
 * A mission is over if it will spawn no more UFOs and it has no UFOs still in
 * the game.
 * @return If the mission can be safely removed from game.
 */
bool ScriptedEvent::isOver() const
{
	return _isOver;
}

/**
 * Set method for the isOver flag
 * @param isOver whether the event has been completed
 */
void ScriptedEvent::setIsOver(bool isOver)
{
	_isOver = isOver;
}

/** 
 * Checks if the restrictions for this event are met by this base/craft
 * @param save Save game object
 * @param base Xcom base to check against restrictions
 * @param craft Xcom craft to check against restrictions, or 0 if craft not applicable 
 * @return flag
 */
bool ScriptedEvent::checkDetectionRestrictions(SavedGame *save, Base *base, Craft *craft)
{
	// Look up the ruleset for this event
	return checkRestrictions(_rule.getDetectionRestrictions(), save, base, craft);
}

/** 
 * Checks if the restrictions for this event are met by this base/craft
 * @param save Save game object
 * @param base Xcom base to check against restrictions
 * @param craft Xcom craft to check against restrictions, or 0 if craft not applicable 
 * @return flag
 */
bool ScriptedEvent::checkEngagementRestrictions(SavedGame *save, Base *base, Craft *craft)
{
	// Look up the ruleset for this event
	return checkRestrictions(_rule.getEngagementRestrictions(), save, base, craft);
}


/** 
 * Checks if the restrictions for this event are met by this base/craft
 * @param save Save game object
 * @param base Xcom base to check against restrictions
 * @param craft Xcom craft to check against restrictions, or 0 if craft not applicable 
 * @return flag
 */
bool ScriptedEvent::checkRestrictions(EventRestrictions &eventRestrictions, SavedGame *save, Base *base, Craft *craft)
{
	bool hasTech, hasFacility, hasCraft = false;

	// Check the technology requirement
	hasTech = (eventRestrictions.research_id) == "" ? true :save->isResearched(eventRestrictions.research_id);

	// Check the facility requirement
	hasFacility = (eventRestrictions.facility_id) == "" ? true : (base->countAvailableFacilities(eventRestrictions.facility_id) > 0);

	// Check the craft requirement.  If no craft pointer was passed 
	if (eventRestrictions.craft_ids.size() == 0) hasCraft = true;
	for (std::vector<std::string>::const_iterator j = eventRestrictions.craft_ids.begin(); j != eventRestrictions.craft_ids.end(); j++)
	{
		if (craft == 0 || craft->getRules()->getType() == *j) hasCraft = true;
	}

	return hasTech && hasFacility && hasCraft;
}

/**
 * Get method for the event's geoscape location
 * @return Target object for the event's location on the geoscape
 */
ScriptedEventLocation *ScriptedEvent::getScriptedEventLocation() const
{
	return _scriptedEventLocation;
}

/**
 * Set method for the event location
 * @param scriptedEventLocation Target object describing the events geoscape location
 */
void ScriptedEvent::setScriptedEventLocation(ScriptedEventLocation *scriptedEventLocation)
{
	_scriptedEventLocation = scriptedEventLocation;
}

/**
 * Get method for the event's UFO on the geoscape
 * @return Target object for the event's UFO on the geoscape
 */
Ufo *ScriptedEvent::getUfo() const
{
	return _ufo;
}

/**
 * Set method for the event UFO
 * @param ufo The event's UFO
 */
void ScriptedEvent::setUfo(Ufo *ufo)
{
	_ufo = ufo;

}

/*
 * Set up Ufo for the first time.
 */
void ScriptedEvent::initUfo()
{
	assert(_ufo);

	// This is the first time we have set up this UFO.  Give it a new
	// location on the geoscape
	_ufoDestIndex = RNG::generate(0, _rule.getUfoEvent().locations.size() - 1);
	Location loc = _rule.getUfoEvent().locations.at(_ufoDestIndex);

	_ufo->setLatitude(loc.lat);
	_ufo->setLongitude(loc.lon);
	_ufo->setAltitude("STR_HIGH_UC");
}

/**
 * For UFO events, choose a new destination for the Ufo object.
 * 
 * An event UFO is more restricted than a mission UFO - we do not want it to
 * leave the geoscape, land, or to fly above water, so we keep tighter control of it.
 *
 * This method simply chooses a new destination at random from those in the rules and
 * sets a random intermediate speed.  It's assumed that the locations specify a 
 * convex landmass on the geoscape, i.e. that heading from one to any other in a 
 * straight line will not cross water.
 */
void ScriptedEvent::newUfoDestination()
{
	assert(_ufo);

	// Select a new destination and ensure that it's different from the
	// current location.  We assume that at least 2 locations were provided
	int locId = RNG::generate(0, _rule.getUfoEvent().locations.size() - 2);
	if (locId >= _ufoDestIndex) ++locId;
	_ufoDestIndex = locId;

	Location loc = _rule.getUfoEvent().locations.at(_ufoDestIndex);

	_ufo->setSpeed(RNG::generate(25, 100) * _ufo->getRules()->getMaxSpeed() / 100);
	Waypoint *wp = new Waypoint();
	wp->setLatitude(loc.lat);
	wp->setLongitude(loc.lon);

	_ufo->setDestination(wp);
}

}
