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
	std::vector<Target*> _followers;
public:
	/// Creates a target.
	Target();
	/// Cleans up the target.
	virtual ~Target();
	/// Loads the target from YAML.
	virtual void load(const YAML::Node& node);
	/// Saves the target to YAML.
	virtual void save(YAML::Emitter& out) const;
	/// Saves the target's ID to YAML.
	virtual void saveId(YAML::Emitter& out) const;
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
	/// Gets the target's followers.
	std::vector<Target*> *getFollowers();
	/// Gets the distance to another target.
	double getDistance(const Target *target) const;
};

}

#endif
