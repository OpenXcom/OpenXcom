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
#include "Polyline.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Initializes the polyline with arrays to store each point's coordinates.
 * @param points Number of points.
 */
Polyline::Polyline(int points) : _points(points)
{
	_lat = new double[points];
	_lon = new double[points];
}

/**
 * Deletes the arrays from memory.
 */
Polyline::~Polyline()
{
	delete[] _lat;
	delete[] _lon;
}

/**
 * Loads the polyline from a YAML file.
 * @param node YAML node.
 */
void Polyline::load(const YAML::Node &node)
{
	delete[] _lat;
	delete[] _lon;

	std::vector<double> coords = node.as< std::vector<double> >();
	_points = coords.size() / 2;
	_lat = new double[_points];
	_lon = new double[_points];

	for (size_t i = 0; i < coords.size(); i += 2)
	{
		size_t j = i / 2;
		_lon[j] = Deg2Rad(coords[i]);
		_lat[j] = Deg2Rad(coords[i+1]);
	}
}

/**
 * Returns the latitude (X) of a given point.
 * @param i Point number (0-max).
 * @return Point's latitude.
 */
double Polyline::getLatitude(int i) const
{
	return _lat[i];
}

/**
 * Changes the latitude of a given point.
 * @param i Point number (0-max).
 * @param lat Point's latitude.
 */
void Polyline::setLatitude(int i, double lat)
{
	_lat[i] = lat;
}

/**
 * Returns the longitude (Y) of a given point.
 * @param i Point number (0-max).
 * @return Point's longitude.
 */
double Polyline::getLongitude(int i) const
{
	return _lon[i];
}

/**
 * Changes the latitude of a given point.
 * @param i Point number (0-max).
 * @param lon Point's longitude.
 */
void Polyline::setLongitude(int i, double lon)
{
	_lon[i] = lon;
}

/**
 * Returns the number of points (vertexes) that make up the polyline.
 * @return Number of points.
 */
int Polyline::getPoints() const
{
	return _points;
}

}
