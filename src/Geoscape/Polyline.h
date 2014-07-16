/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_POLYLINE_H
#define OPENXCOM_POLYLINE_H

namespace OpenXcom
{

/**
 * Represents a polyline in the world map.
 * Polylines constitute the textured land portions
 * of the X-Com globe and typically have 3-4 points.
 */
class Polyline
{
private:
	double *_lat, *_lon;
	const int _points;
public:
	/// Creates a polyline with a number of points.
	Polyline(int points);
	/// Cleans up the polyline.
	~Polyline();
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

#endif
