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

namespace OpenXcom
{
	
	
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
	inline double norm() const
	{
		return sqrt(x*x+ y*y + z*z);
	}
};

}//namespace OpenXcom
#endif	/* OPENXCOM_CORD_H */

