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
#ifndef OPENXCOM_CITY_H
#define OPENXCOM_CITY_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "../Savegame/Target.h"

namespace OpenXcom
{

class Language;

/**
 * Represents a city of the world.
 * Aliens target cities for certain missions.
 */
class City : public Target
{
private:
	std::string _name;
public:
	/// Creates a new city at a certain position.
	City(const std::string &name, double lon, double lat);
	/// Cleans up the city.
	~City();
	/// Gets the city's name.
	std::wstring getName(Language *lang) const;
	/// Gets the city's marker.
	int getMarker() const;
};

}

#endif
