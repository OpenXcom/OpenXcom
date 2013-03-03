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
#include "RuleCountry.h"
#include "../Engine/RNG.h"

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
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "fundingBase")
		{
			i.second() >> _fundingBase;
		}
		else if (key == "fundingCap")
		{
			i.second() >> _fundingCap;
		}
		else if (key == "labelLon")
		{
			i.second() >> _labelLon;
		}
		else if (key == "labelLat")
		{
			i.second() >> _labelLat;
		}
		else if (key == "lonMin")
		{
			i.second() >> _lonMin;
		}
		else if (key == "lonMax")
		{
			i.second() >> _lonMax;
		}
		else if (key == "latMin")
		{
			i.second() >> _latMin;
		}
		else if (key == "latMax")
		{
			i.second() >> _latMax;
		}
	}
}

/**
 * Saves the country type to a YAML file.
 * @param out YAML emitter.
 */
void RuleCountry::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "fundingBase" << YAML::Value << _fundingBase;
	out << YAML::Key << "fundingCap" << YAML::Value << _fundingCap;
	out << YAML::Key << "labelLon" << YAML::Value << _labelLon;
	out << YAML::Key << "labelLat" << YAML::Value << _labelLat;
	out << YAML::Key << "lonMin" << YAML::Value << _lonMin;
	out << YAML::Key << "lonMax" << YAML::Value << _lonMax;
	out << YAML::Key << "latMin" << YAML::Value << _latMin;
	out << YAML::Key << "latMax" << YAML::Value << _latMax;
	out << YAML::EndMap;
}

/**
 * Returns the language string that names
 * this country. Each country type
 * has a unique name.
 * @return Country name.
 */
std::string RuleCountry::getType() const
{
	return _type;
}

/**
 * Generates the random starting funding for the country.
 * @return Monthly funding.
 */
int RuleCountry::generateFunding() const
{
	return RNG::generate(_fundingBase, _fundingBase*2) * 1000;
}

/**
 * Returns the country's funding cap.
 * Country funding can never exceed this.
 * @return Funding cap, in thousands.
 */
int RuleCountry::getFundingCap() const
{
	return _fundingCap;
}

/**
 * Returns the longitude of the country's label on the globe.
 * @return Longitude in radians.
 */
double RuleCountry::getLabelLongitude() const
{
	return _labelLon;
}

/**
 * Returns the latitude of the country's label on the globe.
 * @return Latitude in radians.
 */
double RuleCountry::getLabelLatitude() const
{
	return _labelLat;
}

/**
 * Checks if a point is inside this country.
 * @param lon Longitude in radians.
 * @param lat Latitude in radians.
 * @return True if it's inside, False if it's outside.
 */
bool RuleCountry::insideCountry(double lon, double lat) const
{
	for (unsigned int i = 0; i < _lonMin.size(); ++i)
	{
		bool inLon, inLat;

		if (_lonMin[i] <= _lonMax[i])
			inLon = (lon >= _lonMin[i] && lon < _lonMax[i]);
		else
			inLon = ((lon >= _lonMin[i] && lon < 6.283) || (lon >= 0 && lon < _lonMax[i]));

		inLat = (lat >= _latMin[i] && lat < _latMax[i]);

		if (inLon && inLat)
			return true;
	}
	return false;
}
}
