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
#include "RuleCountry.h"
#include "../Engine/RNG.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of country.
 * @param type String defining the type.
 */
RuleCountry::RuleCountry(const std::string &type) : _type(type), _fundingBase(0), _fundingCap(0), _labelLon(0.0), _labelLat(0.0)
{
}

/**
 *
 */
RuleCountry::~RuleCountry()
{
}

/**
 * Loads the country type from a YAML file.
 * @param node YAML node.
 */
void RuleCountry::load(const YAML::Node &node)
{
	_type = node["type"].as<std::string>(_type);
	_fundingBase = node["fundingBase"].as<int>(_fundingBase);
	_fundingCap = node["fundingCap"].as<int>(_fundingCap);
	if (node["labelLon"])
		_labelLon = Deg2Rad(node["labelLon"].as<double>());
	if (node["labelLat"])
		_labelLat = Deg2Rad(node["labelLat"].as<double>());
	std::vector< std::vector<double> > areas;
	areas = node["areas"].as< std::vector< std::vector<double> > >(areas);
	for (size_t i = 0; i != areas.size(); ++i)
	{
		_lonMin.push_back(Deg2Rad(areas[i][0]));
		_lonMax.push_back(Deg2Rad(areas[i][1]));
		_latMin.push_back(Deg2Rad(areas[i][2]));
		_latMax.push_back(Deg2Rad(areas[i][3]));

		if (_latMin.back() > _latMax.back())
			std::swap(_latMin.back(), _latMax.back());
	}
}

/**
 * Gets the language string that names
 * this country. Each country type
 * has a unique name.
 * @return The country's name.
 */
std::string RuleCountry::getType() const
{
	return _type;
}

/**
 * Generates the random starting funding for the country.
 * @return The monthly funding.
 */
int RuleCountry::generateFunding() const
{
	return RNG::generate(_fundingBase, _fundingBase*2) * 1000;
}

/**
 * Gets the country's funding cap.
 * Country funding can never exceed this.
 * @return The funding cap, in thousands.
 */
int RuleCountry::getFundingCap() const
{
	return _fundingCap;
}

/**
 * Gets the longitude of the country's label on the globe.
 * @return The longitude in radians.
 */
double RuleCountry::getLabelLongitude() const
{
	return _labelLon;
}

/**
 * Gets the latitude of the country's label on the globe.
 * @return The latitude in radians.
 */
double RuleCountry::getLabelLatitude() const
{
	return _labelLat;
}

/**
 * Checks if a point is inside this country.
 * @param lon Longitude in radians.
 * @param lat Latitude in radians.
 * @return True if it's inside, false if it's outside.
 */
bool RuleCountry::insideCountry(double lon, double lat) const
{
	for (size_t i = 0; i < _lonMin.size(); ++i)
	{
		bool inLon, inLat;

		if (_lonMin[i] <= _lonMax[i])
			inLon = (lon >= _lonMin[i] && lon < _lonMax[i]);
		else
			inLon = ((lon >= _lonMin[i] && lon < M_PI*2.0) || (lon >= 0 && lon < _lonMax[i]));

		inLat = (lat >= _latMin[i] && lat < _latMax[i]);

		if (inLon && inLat)
			return true;
	}
	return false;
}

}
