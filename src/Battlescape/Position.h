/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_POSITION_H
#define OPENXCOM_POSITION_H

namespace OpenXcom
{

/**
 * Easy handling of X-Y-Z coordinates.
 */
class Position
{
public:
	int x, y, z;

	/// Null position constructor.
	Position() : x(0), y(0), z(0) {};
	/// X Y Z position constructor.
	Position(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {};
	/// Copy constructor.
	Position(const Position& pos) : x(pos.x), y(pos.y), z(pos.z) {};

	Position& operator=(const Position& pos) { x = pos.x; y = pos.y; z = pos.z; return *this; }

	Position operator+(const Position& pos) const { return Position(x + pos.x, y + pos.y, z + pos.z); }
	Position& operator+=(const Position& pos) { x+=pos.x; y+=pos.y; z+=pos.z; return *this; }

	Position operator-(const Position& pos) const { return Position(x - pos.x, y - pos.y, z - pos.z); }
	Position& operator-=(const Position& pos) { x-=pos.x; y-=pos.y; z-=pos.z; return *this; }

	Position operator*(const Position& pos) const { return Position(x * pos.x, y * pos.y, z * pos.z); }
	Position& operator*=(const Position& pos) { x*=pos.x; y*=pos.y; z*=pos.z; return *this; }
	Position operator*(const int v) const { return Position(x * v, y * v, z * v); }
	Position& operator*=(const int v) { x*=v; y*=v; z*=v; return *this; }

	/// == operator
    bool operator== (const Position& pos) const
	{
		return x == pos.x && y == pos.y && z == pos.z;
	}
	/// != operator
    bool operator!= (const Position& pos) const
	{
		return x != pos.x || y != pos.y || z != pos.z;
	}

};

}

#endif
