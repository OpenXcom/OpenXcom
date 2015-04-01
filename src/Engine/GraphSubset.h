/*
 * Copyright 2010-2015 OpenXcom Developers.
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

#ifndef OPENXCOM_GRAPHSUBSET_H
#define	OPENXCOM_GRAPHSUBSET_H

#include <utility>
#include <algorithm>

namespace OpenXcom
{

	

struct GraphSubset
{

	//define part of surface
	int beg_x, end_x;
	int beg_y, end_y;
	
	GraphSubset(int max_x, int max_y):
			beg_x(0), end_x(max_x),
			beg_y(0), end_y(max_y)
	{

	}
	
	
	GraphSubset(std::pair<int, int> range_x, std::pair<int, int> range_y):
			beg_x(range_x.first), end_x(range_x.second),
			beg_y(range_y.first), end_y(range_y.second)
	{
	
	}
	
	GraphSubset(const GraphSubset& r):
			beg_x(r.beg_x), end_x(r.end_x),
			beg_y(r.beg_y), end_y(r.end_y)
	{

	}
	
	inline GraphSubset offset(int x, int y) const
	{
		GraphSubset ret = *this;
		ret.beg_x += x;
		ret.end_x += x;
		ret.beg_y += y;
		ret.end_y += y;
		return ret;
	}
	
	inline int size_x() const
	{
		return end_x - beg_x;
	}
	
	inline int size_y() const
	{
		return end_y - beg_y;
	}

	
	static inline void intersection_range(int& begin_a, int& end_a, const int& begin_b, const int& end_b)
	{
		if (begin_a >= end_b || begin_b >= end_a)
		{			
			//intersection is empty
			end_a = begin_a;
		}
		else
		{
			begin_a = std::max(begin_a, begin_b); 
			end_a = std::min(end_a, end_b);
		}
	}
	static inline GraphSubset intersection(const GraphSubset& a, const GraphSubset& b)
	{
		GraphSubset ret = a;
		intersection_range(ret.beg_x, ret.end_x, b.beg_x, b.end_x);
		intersection_range(ret.beg_y, ret.end_y, b.beg_y, b.end_y);
		return ret;
	}
	static inline GraphSubset intersection(const GraphSubset& a, const GraphSubset& b,  const GraphSubset& c)
	{
		GraphSubset ret =  intersection(a, b);
		intersection_range(ret.beg_x, ret.end_x, c.beg_x, c.end_x);
		intersection_range(ret.beg_y, ret.end_y, c.beg_y, c.end_y);		
		return ret;
	}
	static inline GraphSubset intersection(const GraphSubset& a, const GraphSubset& b,  const GraphSubset& c, const GraphSubset& d)
	{
		GraphSubset ret =  intersection(a, b, c);
		intersection_range(ret.beg_x, ret.end_x, d.beg_x, d.end_x);
		intersection_range(ret.beg_y, ret.end_y, d.beg_y, d.end_y);		
		return ret;
	}
	
};

}//namespace OpenXcom

#endif	/* OPENXCOM_GRAPHSUBSET_H */

