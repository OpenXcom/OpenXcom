/*
 * Copyright 2010 Daniel Albano
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

/**
 * Initializes the polygon with arrays to store each point's coordinates.
 * @param points Number of points.
 */
Polygon::Polygon(int points) : _points(points), _texture(0)
{
	_lat = new double[points];
	_lon = new double[points];
}

/**
 * Deletes the arrays from memory.
 */
Polygon::~Polygon()
{
	delete[] _lat;
	delete[] _lon;
}

/**
 * Returns the latitude (X) of a given point.
 * @param i Point number (0-max).
 * @return Point's latitude.
 */
double Polygon::getLatitude(int i)
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
 * Returns the longitude (Y) of a given point.
 * @param i Point number (0-max).
 * @return Point's longitude.
 */
double Polygon::getLongitude(int i)
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
 * Returns the texture used to draw the polygon
 * (textures are stored in a set).
 * @return Texture sprite number.
 */
int Polygon::getTexture()
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
int Polygon::getPoints()
{
	return _points;
}
