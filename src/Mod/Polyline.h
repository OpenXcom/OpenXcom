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
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents a polyline in the world map.
 * Polylines constitute the detail portions of the
 * X-Com globe and typically represent borders and rivers.
 */
class Polyline
{
private:
	double *_lat, *_lon;
	int _points;
public:
	/// Creates a polyline with a number of points.
	Polyline(int points);
	/// Cleans up the polyline.
	~Polyline();
	/// Loads the polyline from YAML.
	void load(const YAML::Node& node);
	/// Gets the latitude of a point.
	double getLatitude(int i) const;
	/// Sets the latitude of a point.
	void setLatitude(int i, double lat);
	/// Gets the longitude of a point.
	double getLongitude(int i) const;
	/// Sets the longitude of a point.
	void setLongitude(int i, double lon);
	/// Gets the number of points of the polyline.
	int getPoints() const;
};

}
