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
#include <SDL_types.h>
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

/**
 * Represents a polygon in the world map.
 * Polygons constitute the textured land portions
 * of the X-Com globe and typically have 3-4 points.
 */
class Polygon
{
private:
	double *_lat, *_lon;
	Sint16 *_x, *_y;
	int _points, _texture;
public:
	/// Creates a polygon with a number of points.
	Polygon(int points);
	/// Creates a new polygon from an existing one.
	Polygon(const Polygon& other);
	/// Cleans up the polygon.
	~Polygon();
	/// Loads the polygon from YAML.
	void load(const YAML::Node& node);
	/// Gets the latitude of a point.
	double getLatitude(int i) const;
	/// Sets the latitude of a point.
	void setLatitude(int i, double lat);
	/// Gets the longitude of a point.
	double getLongitude(int i) const;
	/// Sets the longitude of a point.
	void setLongitude(int i, double lon);
	/// Gets the X coordinate of a point.
	Sint16 getX(int i) const;
	/// Sets the X coordinate of a point.
	void setX(int i, Sint16 x);
	/// Gets the Y coordinate of a point.
	Sint16 getY(int i) const;
	/// Sets the Y coordinate of a point.
	void setY(int i, Sint16 y);
	/// Gets the texture of the polygon.
	int getTexture() const;
	/// Sets the texture of the polygon.
	void setTexture(int tex);
	/// Gets the number of points of the polygon.
	int getPoints() const;
};

}
