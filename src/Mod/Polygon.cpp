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
#include "Polygon.h"
#include "../fmath.h"

namespace OpenXcom
{

/**
 * Initializes the polygon with arrays to store each point's coordinates.
 * @param points Number of points.
 */
Polygon::Polygon(int points) : _points(points), _texture(0)
{
	_lat = new double[_points];
	_lon = new double[_points];
	_x = new Sint16[_points];
	_y = new Sint16[_points];
	for (int i = 0; i < _points; ++i)
	{
		_lat[i] = 0.0;
		_lon[i] = 0.0;
		_x[i] = 0;
		_y[i] = 0;
	}
}

/**
 * Performs a deep copy of an existing polygon.
 * @param other Polygon to copy from.
 */
Polygon::Polygon(const Polygon& other)
{
	_points = other._points;
	_lat = new double[_points];
	_lon = new double[_points];
	_x = new Sint16[_points];
	_y = new Sint16[_points];
	for (int i = 0; i < _points; ++i)
	{
		_lat[i] = other._lat[i];
		_lon[i] = other._lon[i];
		_x[i] = other._x[i];
		_y[i] = other._y[i];
	}
	_texture = other._texture;
}

/**
 * Deletes the arrays from memory.
 */
Polygon::~Polygon()
{
	delete[] _lat;
	delete[] _lon;
	delete[] _x;
	delete[] _y;
}

/**
 * Loads the polygon from a YAML file.
 * @param node YAML node.
 */
void Polygon::load(const YAML::Node &node)
{
	delete[] _lat;
	delete[] _lon;
	delete[] _x;
	delete[] _y;

	std::vector<double> coords = node.as< std::vector<double> >();
	_points = (coords.size() - 1) / 2;
	_lat = new double[_points];
	_lon = new double[_points];
	_x = new Sint16[_points];
	_y = new Sint16[_points];

	_texture = coords[0];
	for (size_t i = 1; i < coords.size(); i += 2)
	{
		size_t j = (i - 1) / 2;
		_lon[j] = Deg2Rad(coords[i]);
		_lat[j] = Deg2Rad(coords[i+1]);
		_x[j] = 0;
		_y[j] = 0;
	}
}

/**
 * Returns the latitude of a given point.
 * @param i Point number (0-max).
 * @return Point's latitude.
 */
double Polygon::getLatitude(int i) const
{
	return _lat[i];
}

/**
 * Changes the latitude of a given point.
 * @param i Point number (0-max).
 * @param lat Point's latitude.
 */
void Polygon::setLatitude(int i, double lat)
{
	_lat[i] = lat;
}

/**
 * Returns the longitude of a given point.
 * @param i Point number (0-max).
 * @return Point's longitude.
 */
double Polygon::getLongitude(int i) const
{
	return _lon[i];
}

/**
 * Changes the latitude of a given point.
 * @param i Point number (0-max).
 * @param lon Point's longitude.
 */
void Polygon::setLongitude(int i, double lon)
{
	_lon[i] = lon;
}

/**
 * Returns the X coordinate of a given point.
 * @param i Point number (0-max).
 * @return Point's X coordinate.
 */
Sint16 Polygon::getX(int i) const
{
	return _x[i];
}

/**
 * Changes the X coordinate of a given point.
 * @param i Point number (0-max).
 * @param x Point's X coordinate.
 */
void Polygon::setX(int i, Sint16 x)
{
	_x[i] = x;
}

/**
 * Returns the Y coordinate of a given point.
 * @param i Point number (0-max).
 * @return Point's Y coordinate.
 */
Sint16 Polygon::getY(int i) const
{
	return _y[i];
}

/**
 * Changes the Y coordinate of a given point.
 * @param i Point number (0-max).
 * @param y Point's Y coordinate.
 */
void Polygon::setY(int i, Sint16 y)
{
	_y[i] = y;
}

/**
 * Returns the texture used to draw the polygon
 * (textures are stored in a set).
 * @return Texture sprite number.
 */
int Polygon::getTexture() const
{
	return _texture;
}

/**
 * Changes the texture used to draw the polygon.
 * @param tex Texture sprite number.
 */
void Polygon::setTexture(int tex)
{
	_texture = tex;
}

/**
 * Returns the number of points (vertexes) that make up the polygon.
 * @return Number of points.
 */
int Polygon::getPoints() const
{
	return _points;
}

}
