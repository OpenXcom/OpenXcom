/*
 * Copyright 2010 OpenXcom Developers.
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

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of country.
 * @param type String defining the type.
 */
RuleCountry::RuleCountry(const std::string &type) : _type(type), _fundingMin(0), _fundingMax(0), _labelLon(0.0), _labelLat(0.0)
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
		else if (key == "fundingMin")
		{
			i.second() >> _fundingMin;
		}
		else if (key == "fundingMax")
		{
			i.second() >> _fundingMax;
		}
		else if (key == "labelLon")
		{
			i.second() >> _labelLon;
		}
		else if (key == "labelLat")
		{
			i.second() >> _labelLat;
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
	out << YAML::Key << "fundingMin" << YAML::Value << _fundingMin;
	out << YAML::Key << "fundingMax" << YAML::Value << _fundingMax;
	out << YAML::Key << "labelLon" << YAML::Value << _labelLon;
	out << YAML::Key << "labelLat" << YAML::Value << _labelLat;
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
 * Returns the country's minimum starting funding,
 * in thousands.
 * @return Monthly funding.
 */
int RuleCountry::getMinFunding() const
{
	return _fundingMin;
}

/**
 * Changes the country's minimum starting funding,
 * in thousands.
 * @param funding Monthly funding.
 */
void RuleCountry::setMinFunding(int funding)
{
	_fundingMin = funding;
}

/**
 * Returns the country's maximum starting funding.
 * @return Monthly funding.
 */
int RuleCountry::getMaxFunding() const
{
	return _fundingMax;
}

/**
 * Changes the country's maximum starting funding.
 * @param funding Monthly funding.
 */
void RuleCountry::setMaxFunding(int funding)
{
	_fundingMax = funding;
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
 * Changes the longitude of the country's label on the globe.
 * @param lon Longitude in radians.
 */
void RuleCountry::setLabelLongitude(double lon)
{
	_labelLon = lon;
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
 * Changes the latitude of the country's label on the globe.
 * @param lat Latitude in radians.
 */
void RuleCountry::setLabelLatitude(double lat)
{
	_labelLat = lat;
}

}
