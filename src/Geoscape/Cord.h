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
#ifndef OPENXCOM_CORD_H
#define	OPENXCOM_CORD_H

#include <cmath>
#include "../fmath.h"

namespace OpenXcom
{
struct Cord;

struct CordPolar
{
	double lon, lat;

	inline CordPolar(double plon, double plat)
	{
		lon = plon;
		lat = plat;
	}
	inline CordPolar(const CordPolar& pol)
	{
		lon = pol.lon;
		lat = pol.lat;
	}
	inline CordPolar()
	{
		lon = 0;
		lat = 0;
	}
	explicit inline CordPolar(const Cord&);
};

struct Cord
{
	double x, y, z;

	inline Cord(double px, double py, double pz)
	{
		x = px;
		y = py;
		z = pz;
	}
	inline Cord(const Cord& c)
	{
		x = c.x;
		y = c.y;
		z = c.z;
	}
	inline Cord()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	explicit inline Cord(const CordPolar&);

	inline Cord operator +()
	{
		return *this;
	}
	inline Cord operator -()
	{
		return Cord(-x, -y, -z);
	}
	inline Cord& operator *=(double d)
	{
		x *= d;
		y *= d;
		z *= d;
		return *this;
	}
	inline Cord& operator /=(double d)
	{
		double re = 1./d;
		x *= re;
		y *= re;
		z *= re;
		return *this;
	}
	inline Cord& operator +=(const Cord& c)
	{
		x += c.x;
		y += c.y;
		z += c.z;
		return *this;
	}
	inline Cord& operator -=(const Cord& c)
	{
		x -= c.x;
		y -= c.y;
		z -= c.z;
		return *this;
	}
	inline bool operator ==(const Cord& c)
	{
		return AreSame(x, c.x) && AreSame(y, c.y) && AreSame(z, c.z);
	}

	inline double norm() const
	{
		return std::sqrt(x*x + y*y + z*z);
	}
};

inline Cord::Cord(const CordPolar& pol)
{
	x = std::sin(pol.lon) * std::cos(pol.lat);
	y = std::sin(pol.lat);
	z = std::cos(pol.lon) * std::cos(pol.lat);
}

inline CordPolar::CordPolar(const Cord& c)
{
	double inv = 1/c.norm();
	lat = asin(c.y * inv);
	lon = atan2(c.x, c.z);
}

}//namespace OpenXcom
#endif	/* OPENXCOM_CORD_H */

