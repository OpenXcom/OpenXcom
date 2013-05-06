/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "City.h"
#include <math.h>

namespace OpenXcom
{

/**
 * Initializes a city with certain data.
 * @param name Name of the city.
 * @param lon Longitude of the city.
 * @param lat Latitude of the city.
 */
City::City(const std::string &name, double lon, double lat): _name(name), _lon(lon), _lat(lat)
{
}

/**
 *
 */
City::~City()
{
}

/**
 * Loads the region type from a YAML file.
 * @param node YAML node.
 */
void City::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "name")
		{
			i.second() >> _name;
		}
		else if (key == "lon")
		{
			i.second() >> _lon;
			_lon *= M_PI / 180;
		}
		else if (key == "lat")
		{
			i.second() >> _lat;
			_lat *= M_PI / 180;
		}
	}
}

/**
 * Saves the region type to a YAML file.
 * @param out YAML emitter.
 */
void City::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << _name;
	out << YAML::Key << "lon" << YAML::Value << _lon;
	out << YAML::Key << "lat" << YAML::Value << _lat;
	out << YAML::EndMap;
}

/**
 * Returns the name of the city.
 * @return City name.
 */
std::string City::getName() const
{
	return _name;
}

/**
 * Returns the latitude coordinate of the city.
 * @return Latitude in radian.
 */
double City::getLatitude() const
{
	return _lat;
}

/**
 * Returns the longitude coordinate of the city.
 * @return Longitude in radian.
 */
double City::getLongitude() const
{
	return _lon;
}

}
