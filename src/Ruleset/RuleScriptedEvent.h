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
#ifndef OPENXCOM_RULESCRIPTEDEVENT_H
#define OPENXCOM_RULESCRIPTEDEVENT_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Ruleset;

// Identify the different categories of events that can be defined.  
enum SCRIPTED_EVENT_TYPE {SE_NONE, SE_DESTINATION, SE_LOCATION, SE_UFO};

// Identify the different types of screen data that can be set up for a scripted event.
enum SCRIPTED_EVENT_SCREEN {SS_NONE, SS_CONFIRM, SS_DETECTED, SS_FAILED_ENGAGE, SS_DEBRIEF_SUCCESS, SS_DEBRIEF_FAILED};

struct EventRestrictions
{
	std::string research_id;
	std::vector<std::string> craft_ids;
	std::string facility_id;
};

struct Location
{
	double lon;
	double lat;
};

struct DestinationEvent
{
	std::string buttonText_id;
	int buttonX;
	int buttonY;
	int buttonWidth;
	int buttonHeight;
};

struct LocationEvent
{
	std::vector<Location> locations;
	int icon;
	int color;
	std::string name_id;
	bool automaticDetection;
};

struct UfoEvent
{
	std::string ufoType_id;
	std::string race_id;
	std::vector<Location> locations;
	int color;
	std::string name_id;
	bool automaticDetection;
};

struct MissionScreenData
{
	int palette;
	std::string background_id;
	std::string music_id;
	std::string title_id;
	std::string mainText_id;
};

/**
 * Represents a specific type of UFO.
 * Contains constant info about a UFO like
 * speed, weapons, scores, etc.
 * @sa Ufo
 */
class RuleScriptedEvent
{
private:
	std::string _type;
	SCRIPTED_EVENT_TYPE _scriptedEventType;
	EventRestrictions _spawnRestrictions;
	EventRestrictions _detectionRestrictions;
	EventRestrictions _engagementRestrictions;
	DestinationEvent _destinationEvent;
	LocationEvent _locationEvent;
	UfoEvent _ufoEvent;
	std::string _successScoreText_id;
	int _successScore;
	std::string _failScoreText_id;
	int _failScore;
	std::map<std::string, int> _rewardItems;
	std::string _rewardResearch_id;
	bool _winOnSuccess;
	bool _loseOnFail;
	MissionScreenData _confirmScreen;
	MissionScreenData _detectedScreen;
	MissionScreenData _failedToEngageScreen;
	MissionScreenData _successDebriefingScreen;
	MissionScreenData _failureDebriefingScreen;

public:
	/// Creates a blank Scripted Event ruleset.
	RuleScriptedEvent(const std::string &type);
	/// Cleans up the Scripted Event ruleset.
	~RuleScriptedEvent();
	/// Loads Scripted Event data from YAML.
	void load(const YAML::Node& node, Ruleset *ruleset);
	/// Gets the ID for the name text
	const std::string &getType() const;
	/// Gets the Scripted Event type 
	SCRIPTED_EVENT_TYPE RuleScriptedEvent::getScriptedEventType() const;
	/// Get a particular set of restrictions (an event may have any/all of these)
	EventRestrictions getSpawnRestrictions() const;
	EventRestrictions getDetectionRestrictions() const;
	EventRestrictions getEngagementRestrictions() const;
	/// Get a particular Event (an event may have one of these)
	DestinationEvent getDestinationEvent() const;
	LocationEvent getLocationEvent() const;
	UfoEvent getUfoEvent() const;
	/// Get the additional scoring data for the debriefing screen
	std::string getSuccessScoreText() const;
	int getSuccessScore() const;
	std::string getFailScoreText() const;
	int getFailScore() const;
	/// Get the list of reward items on mission success
	const std::map<std::string, int> & getRewardItems() const;
	/// Get the reward research item on mission success
	const std::string getRewardResearch() const;
	/// Get the game critical flags - does this event trigger an entire game win/loss
	const bool getWinOnSuccess() const;
	const bool getLoseOnFail() const;
	/// Get the display data for the screens to confirm and detect the event
	const MissionScreenData &getConfirmScreen() const;
	const MissionScreenData &getDetectedScreen() const;
	const MissionScreenData &getFailedToEngageScreen() const;
	const MissionScreenData &getSuccessDebriefingScreen() const;
	const MissionScreenData &getFailureDebriefingScreen() const;
};

}

#endif
