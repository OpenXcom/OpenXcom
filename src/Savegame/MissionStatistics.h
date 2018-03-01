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
#include "GameTime.h"
#include "../Engine/Language.h"

#include <map>
#include <string>
#include <sstream>

/*
* Instead of pulling in yaml-cpp, just pre-declare the require Node
* we require in member function definitions.
*/
namespace YAML
{
class Node;
}

namespace OpenXcom
{

/**
* Container for mission statistics.
*/
struct MissionStatistics
{
	/// Variables
	int id;
	std::string markerName;
	int markerId;
	GameTime time;
	std::string region, country, type, ufo;
	bool success;
	std::string rating;
	int score;
	std::string alienRace;
	int daylight;
	std::map<int, int> injuryList;
	bool valiantCrux;
	int lootValue;

	// Load
	void load(const YAML::Node &node);

	// Save
	YAML::Node save() const;

	std::wstring getMissionName(Language *lang) const;

	std::wstring getRatingString(Language *lang) const;

	std::string getLocationString() const;

	std::string getDaylightString() const;

	bool isAlienBase() const;

	bool isBaseDefense() const;

	bool isUfoMission() const;

	MissionStatistics(const YAML::Node& node);
	MissionStatistics();
	~MissionStatistics();
};

}
