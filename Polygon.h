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
#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <string>

using namespace std;

class Polygon
{
private:
	double *_lat;
	double *_lon;
	const int _points;
	int _texture;
public:
	Polygon(int points);
	~Polygon();
	double getLatitude(int i);
	void setLatitude(int i, double lat);
	double getLongitude(int i);
	void setLongitude(int i, double lon);
	int getTexture();
	void setTexture(int tex);
	int getPoints();
};

#endif