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

	Position operator/(const Position& pos) const { return Position(x / pos.x, y / pos.y, z / pos.z); }
	Position& operator/=(const Position& pos) { x/=pos.x; y/=pos.y; z/=pos.z; return *this; }

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

inline std::ostream& operator<<(std::ostream& out, const Position& pos)
{
    out << "(" << pos.x << "," << pos.y << ","<< pos.z << ")";
    return out;
}


inline std::wostream& operator<<(std::wostream& wout, const Position& pos)
{
    wout << "(" << pos.x << "," << pos.y << ","<< pos.z << ")";
    return wout;
}

typedef Position Vector3i;

}

namespace YAML
{
	template<>
	struct convert<OpenXcom::Position>
	{
		static Node encode(const OpenXcom::Position& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, OpenXcom::Position& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			return true;
		}
	};
}


#endif
