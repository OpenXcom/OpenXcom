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

#ifndef OPENXCOM_SHADERREPEAT_H
#define	OPENXCOM_SHADERREPEAT_H

#include <vector>
#include "ShaderDraw.h"

namespace OpenXcom
{

	
template<typename Pixel>
class ShaderRepeat : public helper::ShaderBase<const Pixel>
{
	int _off_x;
	int _off_y;
	
public:
	typedef helper::ShaderBase<const Pixel> _base;
	friend class helper::controler<ShaderRepeat<Pixel> >;
	
	inline ShaderRepeat(const Surface* s):
		_base(s)
	{
        setOffset(0, 0);
	}
	inline ShaderRepeat(const std::vector<Pixel>& f, int max_x, int max_y):
		_base(f, max_x, max_y)
	{
        setOffset(0, 0);
	}
	
	inline void setOffset(int x, int y)
	{
		_off_x = x;
		_off_y = y;
	}
	inline void addOffset(int x, int y)
	{
		_off_x += x;
		_off_y += y;
	}
};


namespace helper
{

template<typename Pixel>
struct controler<ShaderRepeat<Pixel> >
{
	typedef typename ShaderRepeat<Pixel>::PixelPtr PixelPtr;
	typedef typename ShaderRepeat<Pixel>::PixelRef PixelRef;
	
	const PixelPtr _base;
	
	const GraphSubset _range_domain;
	GraphSubset _range_image;
	
	const int _off_x;
	const int _off_y;
	const int _size_x;
	const int _size_y;
	
	
	int _curr_x;
	int _curr_y;
	
	const int _pitch;
	
	PixelPtr _ptr_curr_x;
	PixelPtr _ptr_curr_y;
	
	controler(const ShaderRepeat<Pixel>& f) : 
		_base(f.ptr()),
		_range_domain(f.getDomain()),
		_range_image(0,0),
		_off_x(f._off_x),
		_off_y(f._off_y),
		_size_x(_range_domain.size_x()),
		_size_y(_range_domain.size_y()),
		_curr_x(0),
		_curr_y(0),
		_pitch(f.pitch()),
		_ptr_curr_x(0),
		_ptr_curr_y(0)
	{
		
	}
	
	//not used
	//inline const GraphSubset& get_range()
	
	inline void mod_range(GraphSubset&)
	{
		//nothing
	}
	inline void set_range(const GraphSubset& g)
	{
		_range_image = g;
	}
	
	inline void mod_y(int&, int&)
	{
		_curr_y = ( _range_image.beg_y - _off_y)%_size_y; 
		if(_curr_y <0)
			_curr_y += _size_y;
		_ptr_curr_y = _base;
	}
	inline void set_y(const int& begin, const int&)
	{
		_curr_y = (_curr_y + begin)%_size_y;
		_ptr_curr_y += (_range_domain.beg_y+_curr_y)*_pitch;
	}
	inline void inc_y()
	{
		++_curr_y;
		_ptr_curr_y += _pitch;
		if(_curr_y == _size_y)
		{
			_curr_y = 0;
			_ptr_curr_y -= _size_y*_pitch;
		}
	}
	
	
	inline void mod_x(int&, int&)
	{
		_curr_x = ( _range_image.beg_x - _off_x)%_size_x; 
		if(_curr_x <0)
			_curr_x += _size_x;
		_ptr_curr_x = _ptr_curr_y;
	}
	inline void set_x(const int& begin, const int&)
	{
		_curr_x = (_curr_x + begin)%_size_x;
		_ptr_curr_x += _range_domain.beg_x +_curr_x;
	}
	inline void inc_x()
	{
		++_curr_x;
		_ptr_curr_x += 1;
		if(_curr_x == _size_x)
		{
			_curr_x = 0;
			_ptr_curr_x -= _size_x;
		}
	}
	
	inline PixelRef get_ref()
	{
		return *_ptr_curr_x;
	}
};
	
}//namespace helper
}//namespace OpenXcom

#endif	/* OPENXCOM_SHADERREPEAT_H */

