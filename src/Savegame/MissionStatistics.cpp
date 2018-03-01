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
#include "MissionStatistics.h"

#include <yaml-cpp/yaml.h>

namespace OpenXcom
{
	// Load
	void MissionStatistics::load(const YAML::Node &node)
	{
		id = node["id"].as<int>(id);
		markerName = node["markerName"].as<std::string>(markerName);
		markerId = node["markerId"].as<int>(markerId);
		time.load(node["time"]);
		region = node["region"].as<std::string>(region);
		country = node["country"].as<std::string>(country);
		type = node["type"].as<std::string>(type);
		ufo = node["ufo"].as<std::string>(ufo);
		success = node["success"].as<bool>(success);
		score = node["score"].as<int>(score);
		rating = node["rating"].as<std::string>(rating);
		alienRace = node["alienRace"].as<std::string>(alienRace);
		daylight = node["daylight"].as<int>(daylight);
		injuryList = node["injuryList"].as< std::map<int, int> >(injuryList);
		valiantCrux = node["valiantCrux"].as<bool>(valiantCrux);
		lootValue = node["lootValue"].as<int>(lootValue);
	}

	// Save
	YAML::Node MissionStatistics::save() const
	{
		YAML::Node node;
		node["id"] = id;
		if (!markerName.empty())
		{
			node["markerName"] = markerName;
			node["markerId"] = markerId;
		}
		node["time"] = time.save();
		node["region"] = region;
		node["country"] = country;
		node["type"] = type;
		node["ufo"] = ufo;
		node["success"] = success;
		node["score"] = score;
		node["rating"] = rating;
		node["alienRace"] = alienRace;
		node["daylight"] = daylight;
		node["injuryList"] = injuryList;
		if (valiantCrux) node["valiantCrux"] = valiantCrux;
		if (lootValue) node["lootValue"] = lootValue;
		return node;
	}

	std::wstring MissionStatistics::getMissionName(Language *lang) const
	{
		if (!markerName.empty())
		{
			return lang->getString(markerName).arg(markerId);
		}
		else
		{
			return lang->getString(type);
		}
	}

	std::wstring MissionStatistics::getRatingString(Language *lang) const
	{
		std::wostringstream ss;
		if (success)
		{
			ss << lang->getString("STR_VICTORY");
		}
		else
		{
			ss << lang->getString("STR_DEFEAT");
		}
		ss << L" - " << lang->getString(rating);
		return ss.str();
	}

	std::string MissionStatistics::getLocationString() const
	{
		if (country == "STR_UNKNOWN")
		{
			return region;
		}
		else
		{
			return country;
		}
	}

	std::string MissionStatistics::getDaylightString() const
	{
		if (daylight <= 5)
		{
			return "STR_DAY";
		}
		else
		{
			return "STR_NIGHT";
		}
	}

	bool MissionStatistics::isAlienBase() const
	{
		if (type.find("STR_ALIEN_BASE") != std::string::npos || type.find("STR_ALIEN_COLONY") != std::string::npos)
		{
			return true;
		}
		return false;
	}

	bool MissionStatistics::isBaseDefense() const
	{
		if (type == "STR_BASE_DEFENSE")
		{
			return true;
		}
		return false;
	}

	bool MissionStatistics::isUfoMission() const
	{
		if(ufo != "NO_UFO")
		{
			return true;
		}
		return false;
	}

	MissionStatistics::MissionStatistics(const YAML::Node& node) : time(0, 0, 0, 0, 0, 0, 0) { load(node); }
	MissionStatistics::MissionStatistics() : id(0), markerId(0), time(0, 0, 0, 0, 0, 0, 0), region("STR_REGION_UNKNOWN"), country("STR_UNKNOWN"), ufo("NO_UFO"), success(false), score(0), alienRace("STR_UNKNOWN"), daylight(0), valiantCrux(false), lootValue(0) { }
	MissionStatistics::~MissionStatistics() { }
}
