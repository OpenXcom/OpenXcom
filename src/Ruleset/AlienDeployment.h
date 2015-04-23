/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_ALIENDEPLOYMENT_H
#define OPENXCOM_ALIENDEPLOYMENT_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class RuleTerrain;
class Ruleset;

struct ItemSet
{
	std::vector<std::string> items;
};

struct DeploymentData
{
	int alienRank;
	int lowQty, highQty, dQty, extraQty;
	int percentageOutsideUfo;
	std::vector<ItemSet> itemSets;
};
struct BriefingData
{
	int palette, textOffset;
	std::string music, background;
	bool showCraft, showTarget;
	BriefingData() : palette(0), textOffset(0), music("GMDEFEND"), background("BACK16.SCR"), showCraft(true), showTarget(true) { /*Empty by Design*/ };
};
/**
 * Represents a specific type of Alien Deployment.
 * Contains constant info about a Alien Deployment like
 * the number of aliens for each alien type and what items they carry
 * (itemset depends on alien technology advancement level 0, 1 or 2).
 * - deployment type can be a craft's name, but also alien base or cydonia.
 * - alienRank is used to check which nodeRanks can be used to deploy this unit
 *   + to match to a specific unit (=race/rank combination) that should be deployed.
 * @sa Node
 */
class AlienDeployment
{
private:
	std::string _type;
	std::vector<DeploymentData> _data;
	int _width, _length, _height, _civilians;
	std::vector<std::string> _terrains, _music;
	int _shade;
	std::string _nextStage, _race, _script;
	bool _noRetreat, _finalDestination, _finalMission;
	std::string _alert;
	BriefingData _briefingData;
	std::string _markerName;
	int _markerIcon, _durationMin, _durationMax, _minDepth, _maxDepth;
public:
	/// Creates a blank Alien Deployment ruleset.
	AlienDeployment(const std::string &type);
	/// Cleans up the Alien Deployment ruleset.
	~AlienDeployment();
	/// Loads Alien Deployment data from YAML.
	void load(const YAML::Node& node);
	/// Gets the Alien Deployment's type.
	std::string getType() const;
	/// Gets a pointer to the data.
	std::vector<DeploymentData>* getDeploymentData();
	/// Gets dimensions.
	void getDimensions(int *width, int *length, int *height);
	/// Gets civilians.
	int getCivilians() const;
	/// Gets the terrain for battlescape generation.
	std::vector<std::string> getTerrains() const;
	/// Gets the shade level for battlescape generation.
	int getShade() const;
	/// Gets the next stage of the mission.
	std::string getNextStage() const;
	/// Gets the race to use in the next stage.
	std::string getRace() const;
	/// Gets the script to use for this deployment.
	std::string getScript() const;
	/// Checks if aborting this mission will fail the game (all mars and t'leth stages).
	bool isNoRetreat() const;
	/// Checks if this is the destination for the final mission (mars stage 1, t'leth stage 1).
	bool isFinalDestination() const;
	/// Checks if winning this mission will complete the game (mars stage 2, t'leth stage 3).
	bool isFinalMission() const;
	/// Gets the alert message for this mission type.
	std::string getAlertMessage() const;
	/// Gets the briefing data for this mission type.
	BriefingData getBriefingData() const;
	/// Gets the marker name for this mission.
	std::string getMarkerName() const;
	/// Gets the marker icon for this mission.
	int getMarkerIcon() const;
	/// Gets the minimum duration for this mission.
	int getDurationMin() const;
	/// Gets the maximum duration for this mission.
	int getDurationMax() const;
	/// Gets the list of music to pick from.
	std::vector<std::string> &getMusic();
	/// Gets the minimum depth.
	int getMinDepth();
	/// Gets the maximum depth.
	int getMaxDepth();
};

}
#endif
