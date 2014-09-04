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
#ifndef OPENXCOM_SCRIPTEDEVENTLOCATION_H
#define OPENXCOM_SCRIPTEDEVENTLOCATION_H

#include "Target.h"
#include <string>
#include <yaml-cpp/yaml.h>
#include "ScriptedEvent.h"

namespace OpenXcom
{

/**
 * Represents a scripted event occurence on the geoscape
 */
class ScriptedEventLocation : public Target
{
private:
	/// The scripted event that requires this location
	ScriptedEvent *_scriptedEvent;
	/// Whether or not the event has been detected
	bool _detected;
	/// Unique target identifier for the event
	int _id;
public:
	/// Creates a new scripted event on the geoscape
	ScriptedEventLocation(ScriptedEvent *scriptedEvent);
	/// Cleans up the scripted event 
	~ScriptedEventLocation();
	/// Loads the scripted event from YAML.
	void load(const YAML::Node &node, SavedGame &game);
	/// Saves the scripted event to YAML.
	YAML::Node save() const; 
	/// Saves the scripted event's ID to YAML.
	YAML::Node saveId() const;
	/// Gets the scripted event parent
	ScriptedEvent *getScriptedEvent() const;
	/// Gets the scripted event location's name.
	std::wstring getName(Language *lang) const;
	/// Get method for detected flag
	bool getDetected();
	/// Set method for detected flag
	void setDetected(bool detected);
	/// Get method for the unique identifier
	int getId() const;
    /// Set method for the unique identifier
	void setId(int id);
	/// Gets the event location's geoscape marker
	int getMarker() const;
};

}

#endif
