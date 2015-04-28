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
#ifndef OPENXCOM_TARGET_H
#define OPENXCOM_TARGET_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class Language;

/**
 * Base class for targets on the globe
 * with a set of radian coordinates.
 */
class Target
{
protected:
	double _lon, _lat;
	int _depth;
	std::vector<Target*> _followers;
	/// Creates a target.
	Target();
public:
	/// Cleans up the target.
	virtual ~Target();
	/// Loads the moving target from YAML.
	void load(const YAML::Node& node);
	/// Saves the target to YAML.
	virtual YAML::Node save() const;
	/// Saves the target's ID to YAML.
	virtual YAML::Node saveId() const;
	/// Gets the target's longitude.
	double getLongitude() const;
	/// Sets the target's longitude.
	void setLongitude(double lon);
	/// Gets the target's latitude.
	double getLatitude() const;
	/// Sets the target's latitude.
	void setLatitude(double lat);
	/// Gets the target's name.
	virtual std::wstring getName(Language *lang) const = 0;
	/// Gets the target's marker.
	virtual int getMarker() const = 0;
	/// Gets the target's followers.
	std::vector<Target*> *getFollowers();
	/// Gets the distance to another target.
	double getDistance(const Target *target) const;
	/// Gets the depth of the target.
	int getSiteDepth();
	/// Sets the depth of the target.
	void setSiteDepth(int depth);
};

}

#endif
