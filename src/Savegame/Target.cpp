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
#include "Target.h"
#include "Craft.h"
#include "SerializationHelper.h"
#include "../fmath.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a target with blank coordinates.
 */
Target::Target() : _lon(0.0), _lat(0.0), _depth(0)
{
}

/**
 * Make sure no crafts are chasing this target.
 */
Target::~Target()
{
	for (size_t i = 0; i < _followers.size(); ++i)
	{
		Craft *craft = dynamic_cast<Craft*>(_followers[i]);
		if (craft)
		{
			craft->returnToBase();
		}
	}
}

/**
 * Loads the target from a YAML file.
 * @param node YAML node.
 */
void Target::load(const YAML::Node &node)
{
	_lon = node["lon"].as<double>(_lon);
	_lat = node["lat"].as<double>(_lat);
	if (const YAML::Node &name = node["name"])
	{
		_name = Language::utf8ToWstr(name.as<std::string>());
	}
	_depth = node["depth"].as<int>(_depth);
}

/**
 * Saves the target to a YAML file.
 * @returns YAML node.
 */
YAML::Node Target::save() const
{
	YAML::Node node;
	node["lon"] = serializeDouble(_lon);
	node["lat"] = serializeDouble(_lat);
	if (!_name.empty())
		node["name"] = Language::wstrToUtf8(_name);
	if (_depth)
		node["depth"] = _depth;
	return node;
}

/**
 * Saves the target's unique identifiers to a YAML file.
 * @return YAML node.
 */
YAML::Node Target::saveId() const
{
	YAML::Node node;
	node["lon"] = serializeDouble(_lon);
	node["lat"] = serializeDouble(_lat);
	return node;
}

/**
 * Returns the longitude coordinate of the target.
 * @return Longitude in radian.
 */
double Target::getLongitude() const
{
	return _lon;
}

/**
 * Changes the longitude coordinate of the target.
 * @param lon Longitude in radian.
 */
void Target::setLongitude(double lon)
{
	_lon = lon;

	// Keep between 0 and 2xPI
	while (_lon < 0)
		_lon += 2 * M_PI;
	while (_lon >= 2 * M_PI)
		_lon -= 2 * M_PI;
}

/**
 * Returns the latitude coordinate of the target.
 * @return Latitude in radian.
 */
double Target::getLatitude() const
{
	return _lat;
}

/**
 * Changes the latitude coordinate of the target.
 * @param lat Latitude in radian.
 */
void Target::setLatitude(double lat)
{
	_lat = lat;
	// If you travel past a pole, continue on the other side of the globe.
	if (_lat < -M_PI/2)
	{
		_lat = -M_PI - _lat;
		setLongitude(_lon + M_PI);
	}
	else if (_lat > M_PI/2)
	{
		_lat = M_PI - _lat;
		setLongitude(_lon - M_PI);
	}
}

/**
 * Returns the target's unique identifying name.
 * If there's no custom name, the language default is used.
 * @param lang Language to get strings from.
 * @return Full name.
 */
std::wstring Target::getName(Language *lang) const
{
	if (_name.empty())
		return getDefaultName(lang);
	return _name;
}

/**
 * Changes the target's custom name.
 * @param newName New custom name. If set to blank, the language default is used.
 */
void Target::setName(const std::wstring &newName)
{
	_name = newName;
}

/**
 * Returns the list of crafts currently
 * following this target.
 * @return Pointer to list of crafts.
 */
std::vector<Target*> *Target::getFollowers()
{
	return &_followers;
}

/**
 * Returns the great circle distance to another
 * target on the globe.
 * @param target Pointer to other target.
 * @returns Distance in radian.
 */
double Target::getDistance(const Target *target) const
{
	return acos(cos(_lat) * cos(target->getLatitude()) * cos(target->getLongitude() - _lon) + sin(_lat) * sin(target->getLatitude()));
}

/**
 * Gets the mission site's depth.
 * @return the depth of the site.
 */
int Target::getSiteDepth() const
{
	return _depth;
}

/**
 * Sets the mission site's depth.
 * @param depth the depth we want.
 */
void Target::setSiteDepth(int depth)
{
	_depth = depth;
}

}
