#pragma once
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
#include <map>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents game-specific contents needed
 * for save conversion and ID matching.
 * @sa SaveConverter
 */
class RuleConverter
{
private:
	std::map<std::string, int> _offsets;
	std::vector<std::string> _markers, _countries, _regions, _facilities, _items, _crews, _crafts, _ufos, _craftWeapons, _missions, _armor, _alienRaces, _alienRanks, _research, _manufacture, _ufopaedia;
public:
	/// Creates a blank converter ruleset.
	RuleConverter();
	/// Cleans up the converter ruleset.
	~RuleConverter();
	/// Loads the converter from YAML.
	void load(const YAML::Node& node);
	/// Gets the offset for a specific attribute.
	int getOffset(const std::string &id) { return _offsets[id]; }
	/// Gets the marker ID list.
	const std::vector<std::string> &getMarkers() const { return _markers; }
	/// Gets the country ID list.
	const std::vector<std::string> &getCountries() const { return _countries; }
	/// Gets the region ID list.
	const std::vector<std::string> &getRegions() const { return _regions; }
	/// Gets the facility ID list.
	const std::vector<std::string> &getFacilities() const { return _facilities; }
	/// Gets the item ID list.
	const std::vector<std::string> &getItems() const { return _items; }
	/// Gets the UFO crew ID list.
	const std::vector<std::string> &getCrews() const { return _crews; }
	/// Gets the craft ID list.
	const std::vector<std::string> &getCrafts() const { return _crafts; }
	/// Gets the UFO ID list.
	const std::vector<std::string> &getUfos() const { return _ufos; }
	/// Gets the craft weapon ID list.
	const std::vector<std::string> &getCraftWeapons() const { return _craftWeapons; }
	/// Gets the alien mission ID list.
	const std::vector<std::string> &getMissions() const { return _missions; }
	/// Gets the armor ID list.
	const std::vector<std::string> &getArmor() const { return _armor; }
	/// Gets the alien race ID list.
	const std::vector<std::string> &getAlienRaces() const { return _alienRaces; }
	/// Gets the alien rank ID list.
	const std::vector<std::string> &getAlienRanks() const { return _alienRanks; }
	/// Gets the research ID list.
	const std::vector<std::string> &getResearch() const { return _research; }
	/// Gets the manufacture ID list.
	const std::vector<std::string> &getManufacture() const { return _manufacture; }
	/// Gets the ufopaedia ID list.
	const std::vector<std::string> &getUfopaedia() const { return _ufopaedia; }
};

}
