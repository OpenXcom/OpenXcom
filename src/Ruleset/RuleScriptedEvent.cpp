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
#include "RuleScriptedEvent.h"
#include <math.h>

namespace YAML
{
	template<>
	struct convert<OpenXcom::EventRestrictions>
	{
		static Node encode(const OpenXcom::EventRestrictions& rhs)
		{
			Node node;
			node["research"] = rhs.research_id;
			node["craft"] = rhs.craft_ids;
			node["facility"] = rhs.facility_id;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::EventRestrictions& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.research_id = node["research"].as<std::string>(rhs.research_id);
			rhs.craft_ids = node["craft"].as<std::vector<std::string>>(rhs.craft_ids);
			rhs.facility_id = node["facility"].as<std::string>(rhs.facility_id);

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::Location>
	{
		static Node encode(const OpenXcom::Location& rhs)
		{
			Node node;
			node["lon"] = rhs.lon;
			node["lat"] = rhs.lat;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::Location& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.lon = node["lon"].as<double>(rhs.lon) * M_PI / 180;
			rhs.lat = node["lat"].as<double>(rhs.lat) * M_PI / 180;

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::DestinationEvent>
	{
		static Node encode(const OpenXcom::DestinationEvent& rhs)
		{
			Node node;
			node["buttonText"] = rhs.buttonText_id;
			node["buttonX"] = rhs.buttonX;
			node["buttonY"] = rhs.buttonX;
			node["buttonWidth"] = rhs.buttonWidth;
			node["buttonHeight"] = rhs.buttonHeight;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::DestinationEvent& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.buttonText_id = node["buttonText"].as<std::string>(rhs.buttonText_id);
			rhs.buttonX = node["buttonX"].as<int>(rhs.buttonX);
			rhs.buttonY = node["buttonY"].as<int>(rhs.buttonY);
			rhs.buttonWidth = node["buttonWidth"].as<int>(rhs.buttonWidth);
			rhs.buttonHeight = node["buttonHeight"].as<int>(rhs.buttonHeight);

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::LocationEvent>
	{
		static Node encode(const OpenXcom::LocationEvent& rhs)
		{
			Node node;
			node["locations"] = rhs.locations;
			node["icon"] = rhs.icon;
			node["color"] = rhs.color;
			node["name"] = rhs.name_id;
			node["automaticDetection"] = rhs.automaticDetection;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::LocationEvent& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.locations = node["locations"].as<std::vector<OpenXcom::Location>>(rhs.locations);
			rhs.name_id = node["name"].as<std::string>(rhs.name_id);
			rhs.automaticDetection = node["automaticDetection"].as<bool>(rhs.automaticDetection);

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::UfoEvent>
	{
		static Node encode(const OpenXcom::UfoEvent& rhs)
		{
			Node node;
			node["ufoType"] = rhs.ufoType_id;
			node["race"] = rhs.race_id;
			node["locations"] = rhs.locations;
			node["name"] = rhs.name_id;
			node["automaticDetection"] = rhs.automaticDetection;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::UfoEvent& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.ufoType_id = node["ufoType"].as<std::string>(rhs.ufoType_id);
			rhs.race_id = node["race"].as<std::string>(rhs.race_id);
			rhs.locations = node["locations"].as<std::vector<OpenXcom::Location>>(rhs.locations);
			rhs.color = node["color"].as<int>(rhs.color);
			rhs.name_id = node["name"].as<std::string>(rhs.name_id);
			rhs.automaticDetection = node["automaticDetection"].as<bool>(rhs.automaticDetection);

			return true;
		}
	};

	template<>
	struct convert<OpenXcom::MissionScreenData>
	{
		static Node encode(const OpenXcom::MissionScreenData& rhs)
		{
			Node node;
			node["background"] = rhs.background_id;
			node["title"] = rhs.title_id;
			node["music"] = rhs.music_id;
			node["palette"] = rhs.palette;
			node["mainText"] = rhs.mainText_id;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::MissionScreenData& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.background_id = node["background"].as<std::string>(rhs.background_id);
			rhs.title_id = node["title"].as<std::string>(rhs.title_id);
			rhs.palette = node["palette"].as<int>(rhs.palette);
			rhs.music_id = node["music"].as<std::string>(rhs.music_id);
			rhs.mainText_id = node["mainText"].as<std::string>(rhs.mainText_id);
			return true;
		}
	};
}

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of mission screen
 * @param type String defining the type.
 */
RuleScriptedEvent::RuleScriptedEvent(const std::string &type) : _type(type), _scriptedEventType(SE_NONE), _successScoreText_id(""), _successScore(0), _failScoreText_id(""), _failScore(0), _rewardResearch_id(""), _winOnSuccess(false), _loseOnFail(false)
{
}

/**
 *
 */
RuleScriptedEvent::~RuleScriptedEvent()
{
}

/**
 * Loads the Scripted Event from a YAML file.
 * @param node YAML node.
 * @param ruleset Ruleset for the Scripted Event.
 */
void RuleScriptedEvent::load(const YAML::Node &node, Ruleset *ruleset)
{
	_type = node["type"].as<std::string>(_type);

	_spawnRestrictions = node["spawnRestrictions"].as<EventRestrictions>(_spawnRestrictions);
	_detectionRestrictions = node["detectionRestrictions"].as<EventRestrictions>(_detectionRestrictions);
	_engagementRestrictions = node["engagementRestrictions"].as<EventRestrictions>(_engagementRestrictions);

	_scriptedEventType = (SCRIPTED_EVENT_TYPE) node["scriptedEventType"].as<int>(_scriptedEventType);
	_destinationEvent = node["destinationEvent"].as<DestinationEvent>(_destinationEvent);
	_locationEvent = node["locationEvent"].as<LocationEvent>(_locationEvent);
	_ufoEvent = node["ufoEvent"].as<UfoEvent>(_ufoEvent);

	_successScoreText_id = node["successScoreText"].as<std::string>(_successScoreText_id);
	_successScore = node["successScore"].as<int>(_successScore);

	_failScoreText_id = node["failScoreText"].as<std::string>(_failScoreText_id);
	_failScore = node["failScore"].as<int>(_failScore);

	_rewardItems = node["rewardItems"].as< std::map<std::string, int> >(_rewardItems);
	_rewardResearch_id = node["rewardResearch"].as<std::string>(_rewardResearch_id);

	_winOnSuccess = node["winOnSuccess"].as<bool>(_winOnSuccess);
	_loseOnFail = node["loseOnFail"].as<bool>(_loseOnFail);

	_confirmScreen = node["confirmScreen"].as<MissionScreenData>(_confirmScreen);
	_detectedScreen = node["detectedScreen"].as<MissionScreenData>(_detectedScreen);
	_failedToEngageScreen = node["failedToEngageScreen"].as<MissionScreenData>(_failedToEngageScreen);

	_successDebriefingScreen = node["successDebriefingScreen"].as<MissionScreenData>(_successDebriefingScreen);
	_failureDebriefingScreen = node["failureDebriefingScreen"].as<MissionScreenData>(_failureDebriefingScreen);
}


/**
 * Gets the language string that names
 * this Scripted Event. Each Scripted Event has a unique name.
 * @return The Scripted Event name.
 */
const std::string &RuleScriptedEvent::getType() const
{
	return _type;
}

/**
 * Gets the code representing the type of this event (geoscape, ufo 
 * @return The Scripted Event name.
 */
SCRIPTED_EVENT_TYPE RuleScriptedEvent::getScriptedEventType() const
{
	return _scriptedEventType;
}

/**
 * Gets the rules for determining how the event is created
 * @return EventRestrictions struct
 */
EventRestrictions RuleScriptedEvent::getSpawnRestrictions() const
{
	return _spawnRestrictions;
}

/**
 * Gets the requirements for detecting this event on the geoscape
 * @return EventRestrictions struct
 */
EventRestrictions RuleScriptedEvent::getDetectionRestrictions() const
{
	return _detectionRestrictions;
}

/**
 * Gets the requirements for engaging this event on the geoscape
 * @return EventRestrictions struct
 */
EventRestrictions RuleScriptedEvent::getEngagementRestrictions() const
{
	return _engagementRestrictions;
}

/**
 * For a Destination Event (e.g. Cydonia), get the event data
 * @return DestinationEvent struct
 */
DestinationEvent RuleScriptedEvent::getDestinationEvent() const
{
	return _destinationEvent;
}

/**
 * For a Geoscape Event (e.g. a unique base mission), get the event data
 * @return EventRestrictions struct
 */
LocationEvent RuleScriptedEvent::getLocationEvent() const
{
	return _locationEvent;
}

/**
 * For a UFO Event (e.g. a unique UFO), get the event data
 * @return EventRestrictions struct
 */
UfoEvent RuleScriptedEvent::getUfoEvent() const
{
	return _ufoEvent;
}

/**
 * Details of line entry on debriefing screen
 * @return Text string ID for scoring line
 */
std::string RuleScriptedEvent::getSuccessScoreText() const
{
	return _successScoreText_id;
}

/**
 * Details of line entry on debriefing screen
 * @return Text score value
 */
int RuleScriptedEvent::getSuccessScore() const
{
	return _successScore;
}

/**
 * Details of line entry on debriefing screen
 * @return Text string ID for scoring line
 */
std::string RuleScriptedEvent::getFailScoreText() const
{
	return _failScoreText_id;
}

/**
 * Details of line entry on debriefing screen
 * @return Text score value
 */
int RuleScriptedEvent::getFailScore() const
{
	return _failScore;
}

/**
 * Gets the list of items rewarded on event completion.
 * @return A list of item IDs and quanitites.
 */
const  std::map<std::string, int> & RuleScriptedEvent::getRewardItems() const
{
	return _rewardItems;
}

/**
 * Technology reward for completing mission
 * @return Text string ID for research 
 */
const std::string RuleScriptedEvent::getRewardResearch() const
{
	return _rewardResearch_id;
}

/**
 * Whether or not to win the entire game if the mission succeeds.
 * @return flag
 */
const bool RuleScriptedEvent::getWinOnSuccess() const
{
	return _winOnSuccess;
}

/**
 * Whether or not to lose the entire game if the mission fails.
 * @return flag
 */
const bool RuleScriptedEvent::getLoseOnFail() const
{
	return _loseOnFail;
}

/**
 * Screen data for geoscape confirmation window
 * @return screen data
 */
const MissionScreenData &RuleScriptedEvent::getConfirmScreen() const
{
	return _confirmScreen;
}

/**
 * Screen data for geoscape detection window
 * @return screen data
 */
const MissionScreenData &RuleScriptedEvent::getDetectedScreen() const
{
	return _detectedScreen;
}

/**
 * Screen data if craft/base inadequate to engage event
 * @return screen data
 */
const MissionScreenData &RuleScriptedEvent::getFailedToEngageScreen() const
{
	return _failedToEngageScreen;
}

/**
 * Screen data for debriefing screen
 * @return screen data
 */
const MissionScreenData &RuleScriptedEvent::getSuccessDebriefingScreen() const
{
	return _successDebriefingScreen;
}

/**
 * Screen data for debriefing screen
 * @return screen data
 */
const MissionScreenData &RuleScriptedEvent::getFailureDebriefingScreen() const
{
	return _failureDebriefingScreen;
}

}

