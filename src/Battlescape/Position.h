/*
 * Copyright 2010-2012 OpenXcom Developers.
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

#include <yaml-cpp/yaml.h>

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

    Position operator/(const int v) const { return Position(x / v, y / v, z / v); }

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

typedef Position Vector3i; 

inline void operator>> (const YAML::Node& node, Position& pos)
{
	node[0] >> pos.x;
	node[1] >> pos.y;
	node[2] >> pos.z;
}

inline YAML::Emitter& operator<< (YAML::Emitter& out, const Position& pos)
{
	out << YAML::Flow << YAML::BeginSeq << pos.x << pos.y << pos.z << YAML::EndSeq;
    return out;
}

}


#endif
