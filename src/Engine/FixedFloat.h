/*
 * Copyright 2010-2013 OpenXcom Developers.
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

#ifndef OPENXCOM_FIXEDFLOAT_H
#define	OPENXCOM_FIXEDFLOAT_H


namespace OpenXcom
{


///fixed point float number based on long int
template<int OFF>
class FixedFloat
{
	long _val;
public:
	static const int one = (1<<OFF);
	static const int mask_float = one - 1;
	static const int mask_int = -1 << OFF;

	inline FixedFloat() : _val() { }

	inline FixedFloat(int i, int f) : _val((i<<OFF) + f) { }

	inline FixedFloat(const int& i) : _val(i<<OFF) { }
	inline FixedFloat(double f) : _val(one*f) { }

	inline FixedFloat(const FixedFloat<OFF>& f) : _val(f._val) { }

	inline FixedFloat<OFF>& operator =(const FixedFloat<OFF>& f)
	{
		_val = f._val;
		return *this;
	}
	inline FixedFloat<OFF>& operator =(const int& i)
	{
		_val = i<<OFF;
		return *this;
	}
	inline FixedFloat<OFF>& operator =(const double& f)
	{
		_val = one*f;
		return *this;
	}

	inline FixedFloat<OFF>& operator +=(const FixedFloat<OFF>& f)
	{
		_val += f._val;
		return *this;
	}
	inline FixedFloat<OFF>& operator +=(const int& i)
	{
		_val += i<<OFF;
		return *this;
	}
	inline FixedFloat<OFF>& operator *=(const FixedFloat<OFF>& f)
	{
		_val = ((long long)_val*f._val)>>OFF;
		return *this;
	}
	inline FixedFloat<OFF>& operator *=(const int& i)
	{
		_val *= i;
		return *this;
	}

	inline FixedFloat<OFF> operator-() const
	{
		return FixedFloat<OFF>(0, -_val);
	}


	inline FixedFloat<OFF> cut() const
	{
		return FixedFloat<OFF>(0, rest());
	}

	inline operator int() const
	{
		return _val>>OFF;
	}
	
	inline int rest() const
	{
		return  _val & mask_float;
	}

	inline int value() const
	{
		return  _val;
	}


	inline FixedFloat<OFF> operator/(int i) const
	{
		return FixedFloat<OFF>(0, _val/i);
	}
	inline FixedFloat<OFF> operator/(const FixedFloat<OFF>& f) const
	{
		return FixedFloat<OFF>(0,((long long)_val<<OFF)/f._val);
	}
	inline FixedFloat<OFF> operator*(int i) const
	{
		return FixedFloat<OFF>(0, _val*i);
	}
	inline FixedFloat<OFF> operator*(const FixedFloat<OFF>& f) const
	{
		return FixedFloat<OFF>(0, ((long long)_val*f._val)>>OFF);
	}

	inline FixedFloat<OFF> operator+(int i) const
	{
		return FixedFloat<OFF>(i, _val);
	}
	inline FixedFloat<OFF> operator-(int i) const
	{
		return FixedFloat<OFF>(-i, _val);
	}
	inline FixedFloat<OFF> operator+(const FixedFloat<OFF>& f) const
	{
		return FixedFloat<OFF>(0, _val + f._val);
	}
	inline FixedFloat<OFF> operator-(const FixedFloat<OFF>& f) const
	{
		return FixedFloat<OFF>(0, _val - f._val);
	}
	
	
	inline FixedFloat<OFF> operator<<(int i)
	{
		return FixedFloat<OFF>(0, _val << i);
	}
	inline FixedFloat<OFF>& operator<<=(int i)
	{
		_val <<= i;
		return *this;
	}
	inline FixedFloat<OFF> operator>>(int i)
	{
		return FixedFloat<OFF>(0, _val >> i);
	}
	inline FixedFloat<OFF>& operator>>=(int i)
	{
		_val >>= i;
		return *this;
	}
	
	
	inline bool operator<(const FixedFloat<OFF>& f) const
	{
		return _val < f._val;
	}
	inline bool operator>(const FixedFloat<OFF>& f) const
	{
		return _val > f._val;
	}
	inline bool operator<=(const FixedFloat<OFF>& f) const
	{
		return _val <= f._val;
	}
	inline bool operator>=(const FixedFloat<OFF>& f) const
	{
		return _val >= f._val;
	}
	inline bool operator==(const FixedFloat<OFF>& f) const
	{
		return _val == f._val;
	}
	inline bool operator!=(const FixedFloat<OFF>& f) const
	{
		return _val != f._val;
	}
};
template<int OFF>
inline FixedFloat<OFF> operator*(int i, FixedFloat<OFF> f)
{
	return f*i;
}
template<int OFF>
inline FixedFloat<OFF> operator/(int i, FixedFloat<OFF> f)
{
	return FixedFloat<OFF>(i)/f;
}
template<int OFF>
inline FixedFloat<OFF> operator+(int i, FixedFloat<OFF> f)
{
	return f+i;
}
template<int OFF>
inline FixedFloat<OFF> operator-(int i, FixedFloat<OFF> f)
{
	return f-i;
}


/// 2d point of fixed point floats
template<int OFF>
struct FixedFloatPoint
{
	FixedFloat<OFF> _x;
	FixedFloat<OFF> _y;

		
	inline FixedFloatPoint() : _x(), _y()
	{
		
	}
	inline FixedFloatPoint(int x, int y) : _x(x), _y(y)
	{
		
	}
	inline FixedFloatPoint(FixedFloat<OFF> x, FixedFloat<OFF> y) : _x(x), _y(y)
	{
		
	}
	
	inline FixedFloatPoint<OFF>& operator+=(const FixedFloatPoint<OFF>& b)
	{
		_x += b._x;
		_y += b._y;
		return *this;
	}
	inline FixedFloatPoint<OFF>& operator-=(const FixedFloatPoint<OFF>& b)
	{
		_x -= b._x;
		_y -= b._y;
		return *this;
	}
	inline FixedFloatPoint<OFF>& operator*=(const FixedFloat<OFF>& b)
	{
		_x *= b;
		_y *= b;
		return *this;
	}
	inline FixedFloatPoint<OFF> operator+(const FixedFloatPoint<OFF>& b) const
	{
		return FixedFloatPoint<OFF>(_x+b._x, _y+b._y);
	}
	inline FixedFloatPoint<OFF> operator-(const FixedFloatPoint<OFF>& b) const
	{
		return FixedFloatPoint<OFF>(_x-b._x, _y-b._y);
	}
	inline FixedFloatPoint<OFF> operator*(const FixedFloat<OFF>& b) const
	{
		return FixedFloatPoint<OFF>(_x*b, _y*b);
	}
	inline FixedFloatPoint<OFF> operator*(int b) const
	{
		return FixedFloatPoint<OFF>(_x*b, _y*b);
	}
	

	
	inline void cutInt()
	{
		_x = _x.cut();
		_y = _y.cut();
	}
	
};
	
template<int OFF>
inline FixedFloatPoint<OFF> operator*(int i, FixedFloatPoint<OFF> f)
{
	return f*i;
}
template<int OFF>
inline FixedFloatPoint<OFF> operator*(const FixedFloat<OFF>& b, FixedFloatPoint<OFF> f)
{
	return f*b;
}
	
	
}//namespace OpenXcom

#endif	/* OPENXCOM_FIXEDFLOAT_H */

