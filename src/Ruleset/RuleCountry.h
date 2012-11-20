/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_RULECOUNTRY_H
#define OPENXCOM_RULECOUNTRY_H

#include <string>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents a specific funding country.
 * Contains constant info like its location in the
 * world and starting funding range.
 */
class RuleCountry
{
private:
	std::string _type;
	int _fundingMin, _fundingMax;
	double _labelLon, _labelLat;
	std::vector<double> _lonMin, _lonMax, _latMin, _latMax;
public:
	/// Creates a blank country ruleset.
	RuleCountry(const std::string &type);
	/// Cleans up the country ruleset.
	~RuleCountry();
	/// Loads the country from YAML.
	void load(const YAML::Node& node);
	/// Saves the country to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the country's type.
	std::string getType() const;
	/// Gets the country's minimum funding.
	int getMinFunding() const;
	/// Gets the country's maximum funding.
	int getMaxFunding() const;
	/// Gets the country's label X position.
	double getLabelLongitude() const;
	/// Gets the country's label Y position.
	double getLabelLatitude() const;
	/// Checks if a point is inside the country.
	bool insideCountry(double lon, double lat) const;
};

}

#endif
