/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "AlienDeployment.h"

namespace YAML
{
	template<>
	struct convert<OpenXcom::ItemSet>
	{
		static Node encode(const OpenXcom::ItemSet& rhs)
		{
			Node node;
			node = rhs.items;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::ItemSet& rhs)
		{
			if (!node.IsSequence())
				return false;

			rhs.items = node.as< std::vector<std::string> >(rhs.items);
			return true;
		}
	};
	template<>
	struct convert<OpenXcom::DeploymentData>
	{
		static Node encode(const OpenXcom::DeploymentData& rhs)
		{
			Node node;
			node["alienRank"] = rhs.alienRank;
			node["lowQty"] = rhs.lowQty;
			node["highQty"] = rhs.highQty;
			node["dQty"] = rhs.dQty;
			node["extraQty"] = rhs.extraQty;
			node["percentageOutsideUfo"] = rhs.percentageOutsideUfo;
			node["itemSets"] = rhs.itemSets;
			return node;
		}

		static bool decode(const Node& node, OpenXcom::DeploymentData& rhs)
		{
			if (!node.IsMap())
				return false;

			rhs.alienRank = node["alienRank"].as<int>(rhs.alienRank);
			rhs.lowQty = node["lowQty"].as<int>(rhs.lowQty);
			rhs.highQty = node["highQty"].as<int>(rhs.highQty);
			rhs.dQty = node["dQty"].as<int>(rhs.dQty);
			rhs.extraQty = node["extraQty"].as<int>(0); // give this a default, as it's not 100% needed, unlike the others.
			rhs.percentageOutsideUfo = node["percentageOutsideUfo"].as<int>(rhs.percentageOutsideUfo);
			rhs.itemSets = node["itemSets"].as< std::vector<OpenXcom::ItemSet> >(rhs.itemSets);
			return true;
		}
	};
	template<>
	struct convert<OpenXcom::BriefingData>
	{
		static Node encode(const OpenXcom::BriefingData& rhs)
		{
			Node node;
			node["palette"] = rhs.palette;
			node["textOffset"] = rhs.textOffset;
			node["title"] = rhs.title;
			node["desc"] = rhs.desc;
			node["music"] = rhs.music;
			node["cutscene"] = rhs.cutscene;
			node["background"] = rhs.background;
			node["showCraft"] = rhs.showCraft;
			node["showTarget"] = rhs.showTarget;
			return node;
		}
		static bool decode(const Node& node, OpenXcom::BriefingData& rhs)
		{
			if (!node.IsMap())
				return false;
			rhs.palette = node["palette"].as<int>(rhs.palette);
			rhs.textOffset = node["textOffset"].as<int>(rhs.textOffset);
			rhs.title = node["title"].as<std::string>(rhs.title);
			rhs.desc = node["desc"].as<std::string>(rhs.desc);
			rhs.music = node["music"].as<std::string>(rhs.music);
			rhs.cutscene = node["cutscene"].as<std::string>(rhs.cutscene);
			rhs.background = node["background"].as<std::string>(rhs.background);
			rhs.showCraft = node["showCraft"].as<bool>(rhs.showCraft);
			rhs.showTarget = node["showTarget"].as<bool>(rhs.showTarget);
			return true;
		}
	};
}

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of deployment data.
 * @param type String defining the type.
 */
AlienDeployment::AlienDeployment(const std::string &type) : _type(type), _width(0), _length(0), _height(0), _civilians(0), _shade(-1), _finalDestination(false), _isAlienBase(false), _alert("STR_ALIENS_TERRORISE"),
	_alertBackground("BACK03.SCR"), _markerName("STR_TERROR_SITE"), _markerIcon(-1), _durationMin(0), _durationMax(0), _minDepth(0), _maxDepth(0), _minSiteDepth(0), _maxSiteDepth(0), _genMissionFrequency(0),
	_objectiveType(-1), _objectivesRequired(0), _objectiveCompleteScore(0), _objectiveFailedScore(0), _despawnPenalty(0), _points(0), _turnLimit(0), _cheatTurn(20), _chronoTrigger(FORCE_LOSE)
{
}

/**
 *
 */
AlienDeployment::~AlienDeployment()
{
}

/**
 * Loads the Deployment from a YAML file.
 * @param node YAML node.
 */
void AlienDeployment::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_data = node["data"].as< std::vector<DeploymentData> >(_data);
	_width = node["width"].as<int>(_width);
	_length = node["length"].as<int>(_length);
	_height = node["height"].as<int>(_height);
	_civilians = node["civilians"].as<int>(_civilians);
	_terrains = node["terrains"].as<std::vector<std::string> >(_terrains);
	_shade = node["shade"].as<int>(_shade);
	_nextStage = node["nextStage"].as<std::string>(_nextStage);
	_race = node["race"].as<std::string>(_race);
	_finalDestination = node["finalDestination"].as<bool>(_finalDestination);
	_winCutscene = node["winCutscene"].as<std::string>(_winCutscene);
	_loseCutscene = node["loseCutscene"].as<std::string>(_loseCutscene);
	_script = node["script"].as<std::string>(_script);
	_alert = node["alert"].as<std::string>(_alert);
	_alertBackground = node["alertBackground"].as<std::string>(_alertBackground);
	_briefingData = node["briefing"].as<BriefingData>(_briefingData);
	_markerName = node["markerName"].as<std::string>(_markerName);
	_markerIcon = node["markerIcon"].as<int>(_markerIcon);
	if (node["depth"])
	{
		_minDepth = node["depth"][0].as<int>(_minDepth);
		_maxDepth = node["depth"][1].as<int>(_maxDepth);
	}
	if (node["siteDepth"])
	{
		_minSiteDepth = node["siteDepth"][0].as<int>(_minSiteDepth);
		_maxSiteDepth = node["siteDepth"][1].as<int>(_maxSiteDepth);
	}
	if (node["duration"])
	{
		_durationMin = node["duration"][0].as<int>(_durationMin);
		_durationMax = node["duration"][1].as<int>(_durationMax);
	}
	_music = node["music"].as< std::vector<std::string> >(_music);
	_objectiveType = node["objectiveType"].as<int>(_objectiveType);
	_objectivesRequired = node["objectivesRequired"].as<int>(_objectivesRequired);
	_objectivePopup = node["objectivePopup"].as<std::string>(_objectivePopup);

	if (node["objectiveComplete"])
	{
		_objectiveCompleteText = node["objectiveComplete"][0].as<std::string>(_objectiveCompleteText);
		_objectiveCompleteScore = node["objectiveComplete"][1].as<int>(_objectiveCompleteScore);
	}
	if (node["objectiveFailed"])
	{
		_objectiveFailedText = node["objectiveFailed"][0].as<std::string>(_objectiveFailedText);
		_objectiveFailedScore = node["objectiveFailed"][1].as<int>(_objectiveFailedScore);
	}
	_despawnPenalty = node["despawnPenalty"].as<int>(_despawnPenalty);
	_points = node["points"].as<int>(_points);
	_cheatTurn = node["cheatTurn"].as<int>(_cheatTurn);
	_turnLimit = node["turnLimit"].as<int>(_turnLimit);
	_chronoTrigger = ChronoTrigger(node["chronoTrigger"].as<int>(_chronoTrigger));
	_isAlienBase = node["alienBase"].as<bool>(_isAlienBase);
	_genMissionType = node["genMissionType"].as<std::string>(_genMissionType);
	_genMissionFrequency = node["genMissionFreq"].as<int>(_genMissionFrequency);
}

/**
 * Returns the language string that names
 * this deployment. Each deployment type has a unique name.
 * @return Deployment name.
 */
std::string AlienDeployment::getType() const
{
	return _type;
}

/**
 * Gets a pointer to the data.
 * @return Pointer to the data.
 */
std::vector<DeploymentData>* AlienDeployment::getDeploymentData()
{
	return &_data;
}

/**
 * Gets dimensions.
 * @param width Width.
 * @param length Length.
 * @param height Height.
 */
void AlienDeployment::getDimensions(int *width, int *length, int *height) const
{
	*width = _width;
	*length = _length;
	*height = _height;
}

/**
 * Gets the number of civilians.
 * @return The number of civilians.
 */
int AlienDeployment::getCivilians() const
{
	return _civilians;
}

/**
 * Gets the terrain for battlescape generation.
 * @return The terrain.
 */
std::vector<std::string> AlienDeployment::getTerrains() const
{
	return _terrains;
}

/**
 * Gets the shade level for battlescape generation.
 * @return The shade level.
 */
int AlienDeployment::getShade() const
{
	return _shade;
}

/**
 * Gets the next stage of the mission.
 * @return The next stage of the mission.
 */
std::string AlienDeployment::getNextStage() const
{
	return _nextStage;
}

/**
 * Gets the race to use on the next stage of the mission.
 * @return The race for the next stage of the mission.
 */
std::string AlienDeployment::getRace() const
{
	return _race;
}

/**
 * Gets the script to use to generate a mission of this type.
 * @return The script to use to generate a mission of this type.
 */
std::string AlienDeployment::getScript() const
{
	return _script;
}

/**
 * Gets if winning this mission completes the game.
 * @return if winning this mission completes the game.
 */
bool AlienDeployment::isFinalDestination() const
{
	return _finalDestination;
}

/**
 * Gets the cutscene to play when the mission is won.
 * @return the cutscene to play when the mission is won.
 */
std::string AlienDeployment::getWinCutscene() const
{
	return _winCutscene;
}

/**
 * Gets the cutscene to play when the mission is lost.
 * @return the cutscene to play when the mission is lost.
 */
std::string AlienDeployment::getLoseCutscene() const
{
	return _loseCutscene;
}

/**
 * Gets the alert message displayed when this mission spawns.
 * @return String ID for the message.
 */
std::string AlienDeployment::getAlertMessage() const
{
	return _alert;
}

/**
* Gets the alert background displayed when this mission spawns.
* @return Sprite ID for the background.
*/
std::string AlienDeployment::getAlertBackground() const
{
	return _alertBackground;
}

/**
 * Gets the briefing data for this mission type.
 * @return data for the briefing window to use.
 */
BriefingData AlienDeployment::getBriefingData() const
{
	return _briefingData;
}

/**
 * Returns the globe marker name for this mission.
 * @return String ID for marker name.
 */
std::string AlienDeployment::getMarkerName() const
{
	return _markerName;
}

/**
 * Returns the globe marker icon for this mission.
 * @return Marker sprite, -1 if none.
 */
int AlienDeployment::getMarkerIcon() const
{
	return _markerIcon;
}

/**
 * Returns the minimum duration for this mission type.
 * @return Duration in hours.
 */
int AlienDeployment::getDurationMin() const
{
	return _durationMin;
}

/**
 * Returns the maximum duration for this mission type.
 * @return Duration in hours.
 */
int AlienDeployment::getDurationMax() const
{
	return _durationMax;
}

/**
 * Gets The list of musics this deployment has to choose from.
 * @return The list of track names.
 */
std::vector<std::string> &AlienDeployment::getMusic()
{
	return _music;
}

/**
 * Gets The minimum depth for this deployment.
 * @return The minimum depth.
 */
int AlienDeployment::getMinDepth() const
{
	return _minDepth;
}

/**
 * Gets The maximum depth for this deployment.
 * @return The maximum depth.
 */
int AlienDeployment::getMaxDepth() const
{
	return _maxDepth;
}

/**
 * Gets The minimum depth for this deployment's mission site.
 * @return The minimum depth.
 */
int AlienDeployment::getMinSiteDepth() const
{
	return _minSiteDepth;
}

/**
 * Gets The maximum depth for this deployment's mission site.
 * @return The maximum depth.
 */
int AlienDeployment::getMaxSiteDepth() const
{
	return _maxSiteDepth;
}

/**
 * Gets the target type for this mission (ie: alien control consoles and synonium devices).
 * @return the target type for this mission.
 */
int AlienDeployment::getObjectiveType() const
{
	return _objectiveType;
}

/**
 * Gets the number of objectives required by this mission.
 * @return the number of objectives required.
 */
int AlienDeployment::getObjectivesRequired() const
{
	return _objectivesRequired;
}

/**
 * Gets the string name for the popup to splash when the objective conditions are met.
 * @return the string to pop up.
 */
std::string AlienDeployment::getObjectivePopup() const
{
	return _objectivePopup;
}

/**
 * Fills out the variables associated with mission success, and returns if those variables actually contain anything.
 * @param &text a reference to the text we wish to alter.
 * @param &score a reference to the score we wish to alter.
 * @return if there is anything worthwhile processing.
 */
bool AlienDeployment::getObjectiveCompleteInfo(std::string &text, int &score) const
{
	text = _objectiveCompleteText;
	score = _objectiveCompleteScore;
	return !text.empty();
}

/**
 * Fills out the variables associated with mission failure, and returns if those variables actually contain anything.
 * @param &text a reference to the text we wish to alter.
 * @param &score a reference to the score we wish to alter.
 * @return if there is anything worthwhile processing.
 */
bool AlienDeployment::getObjectiveFailedInfo(std::string &text, int &score) const
{
	text = _objectiveFailedText;
	score = _objectiveFailedScore;
	return !text.empty();
}

/**
 * Gets the score penalty XCom receives for letting this mission despawn.
 * @return the score for letting this site despawn.
 */
int AlienDeployment::getDespawnPenalty() const
{
	return _despawnPenalty;
}

/**
 * Gets the score penalty against XCom for this site existing.
 * This penalty is applied half-hourly for sites and daily for bases.
 * @return the number of points the aliens get per half hour.
 */
int AlienDeployment::getPoints() const
{
	return _points;
}

/**
 * Gets the maximum number of turns we have before this mission ends.
 * @return the turn limit.
 */
int AlienDeployment::getTurnLimit() const
{
	return _turnLimit;
}

/**
 * Gets the action type to perform when the timer expires.
 * @return the action type to perform.
 */
ChronoTrigger AlienDeployment::getChronoTrigger() const
{
	return _chronoTrigger;
}

/**
 * Gets the turn at which the players become exposed to the AI.
 * @return the turn to start cheating.
 */
int AlienDeployment::getCheatTurn() const
{
	return _cheatTurn;
}

bool AlienDeployment::isAlienBase() const
{
	return _isAlienBase;
}

std::string AlienDeployment::getGenMissionType() const
{
	return _genMissionType;
}

int AlienDeployment::getGenMissionFrequency() const
{
	return _genMissionFrequency;
}

}
