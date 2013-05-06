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

#ifndef OPENXCOM_SHADERMOVE_H
#define	OPENXCOM_SHADERMOVE_H

#include "ShaderDraw.h"

namespace OpenXcom
{


template<typename Pixel>
class ShaderMove : public helper::ShaderBase<Pixel>
{
	int _move_x;
	int _move_y;
	
public:
	typedef helper::ShaderBase<Pixel> _base;
	friend class helper::controler<ShaderMove<Pixel> >;
	
	inline ShaderMove(Surface* s):
		_base(s),
		_move_x(s->getX()), _move_y(s->getY())
	{
		
	}
	
	inline ShaderMove(Surface* s, int move_x, int move_y): 
		_base(s),
		_move_x(move_x), _move_y(move_y)
	{
		
	}
	
	inline ShaderMove(const ShaderMove& f):
		_base(f),
		_move_x(f._move_x), _move_y(f._move_y)
	{
		
	}
	
	inline ShaderMove(std::vector<Pixel>& f, int max_x, int max_y): 
		_base(f, max_x, max_y),
		_move_x(), _move_y()
	{
		
	}
	
	inline ShaderMove(std::vector<Pixel>& f, int max_x, int max_y, int move_x, int move_y): 
		_base(f, max_x, max_y),
		_move_x(move_x), _move_y(move_y)
	{
		
	}
	
	inline GraphSubset getImage() const
	{
		return _base::_range_domain.offset(_move_x, _move_y);
	}
	
	inline void setMove(int x, int y)
	{
		_move_x = x;
		_move_y = y;
	}
	inline void addMove(int x, int y)
	{
		_move_x += x;
		_move_y += y;
	}
};

	

namespace helper
{

template<typename Pixel>
struct controler<ShaderMove<Pixel> > : public controler_base<typename ShaderMove<Pixel>::PixelPtr, typename ShaderMove<Pixel>::PixelRef>
{
	typedef typename ShaderMove<Pixel>::PixelPtr PixelPtr;
	typedef typename ShaderMove<Pixel>::PixelRef PixelRef;
	
	typedef controler_base<PixelPtr, PixelRef> base_type;
		
	controler(const ShaderMove<Pixel>& f) : base_type(f.ptr(), f.getDomain(), f.getImage(), std::make_pair(1, f.pitch()))
	{
		
	}
	
};

}//namespace helper

inline ShaderMove<Uint8> ShaderSurface(Surface* s)
{
	return ShaderMove<Uint8>(s);
}
inline ShaderMove<Uint8> ShaderSurface(Surface* s, int x, int y)
{
	return ShaderMove<Uint8>(s, x, y);
}

}//namespace OpenXcom

#endif	/* OPENXCOM_SHADERMOVE_H */

