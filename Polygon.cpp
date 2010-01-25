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

Polygon::Polygon(int points) : _points(points), _texture(0)
{
	_lat = new double[points];
	_lon = new double[points];
}

Polygon::~Polygon()
{
	delete [] _lat;
	delete [] _lon;
}

double Polygon::getLatitude(int i)
{
	return _lat[i];
}

void Polygon::setLatitude(int i, double lat)
{
	_lat[i] = lat;
}

double Polygon::getLongitude(int i)
{
	return _lon[i];
}

void Polygon::setLongitude(int i, double lon)
{
	_lon[i] = lon;
}

int Polygon::getTexture()
{
	return _texture;
}

void Polygon::setTexture(int tex)
{
	_texture = tex;
}

int Polygon::getPoints()
{
	return _points;
}