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

#ifndef OPENXCOM_SHADERROTATE_H
#define	OPENXCOM_SHADERROTATE_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "ShaderDraw.h"
#include "FixedFloat.h"
#include "Exception.h"

namespace OpenXcom
{

namespace helper
{
	

typedef FixedFloat<16> FF;
typedef FixedFloatPoint<16> FF_Point;
class rotator
{
	double _rot;
	FF _zoom;
	FF _rot_cos;
	FF _rot_sin;
	FF _rot_cos_inv;
	FF _rot_sin_inv;
	FF_Point _center;
	
	inline void update()
	{
		_rot_cos = std::cos( _rot);
		_rot_sin = std::sin( _rot);
		FF _zoom_inv = FF(1)/_zoom;
		_rot_cos_inv = _rot_cos * _zoom_inv;
		_rot_sin_inv = _rot_sin * _zoom_inv;
		_rot_cos *= _zoom;
		_rot_sin *= _zoom;
	}
	
public:
	inline rotator() : _rot(0), _zoom(1),
		_rot_cos(1), _rot_sin(0),
		_rot_cos_inv(1), _rot_sin_inv(0),
		_center(FF(-1)>>1, FF(-1)>>1)
	{
		
	}
	inline rotator(const rotator& r) : _rot(r._rot), _zoom(r._zoom), 
			_rot_cos(r._rot_cos), _rot_sin(r._rot_sin),
			_rot_cos_inv(r._rot_cos_inv), _rot_sin_inv(r._rot_sin_inv),
			_center(r._center)
	{
		
	}
	
	/// set point where 2d space is rotated
	inline void setPosition(const FF_Point& p)
	{
		// point is offseted by (-0.5, -0.5)
		// this allow treat 2d array postions (x,y) as center of pixel, not left upper corner of that
		_center = p - FF_Point(FF(1)>>1,FF(1)>>1);
	}
	/// move point where 2d space is rotated
	inline void addPosition(const FF_Point& p)
	{
		_center += p;
	}
	
	/// set CW roataion of 2d space (arc degree)
	inline void setRotDeg(int rot)
	{
		_rot = M_PI*rot/180.;
		update();
	}
	/// set CW roataion of 2d space (radians)
	inline void setRotRad(double rot)
	{
		_rot = rot;
		update();
	}
	
	/// increase CW roataion of 2d space (arc degree)
	inline void addRotDeg(int rot)
	{
		_rot += M_PI*rot/180.;
		update();
	}
	/// increase CW roataion of 2d space (radians)
	inline void addRotRad(double rot)
	{
		_rot += rot;
		update();
	}
	
	/// zoom 2d space
	inline void setZoom(FF zoom)
	{
		if(zoom.value() == 0)
			throw new Exception("zoom cant be `0`");
		_zoom = zoom;
		update();
	}
	
	/// get inverse image of mathematical point
	inline FF_Point rotPointInverse(const FF_Point& p) const
	{
		const FF_Point temp = p - _center;
		const FF_Point ret(	temp._x*_rot_cos_inv + temp._y*_rot_sin_inv,
							-temp._x*_rot_sin_inv + temp._y*_rot_cos_inv);
		return ret + _center;
	}
	/// get image of mathematical point
	inline FF_Point rotPoint(const FF_Point& p) const
	{
		const FF_Point temp = p - _center;
		const FF_Point ret( temp._x*_rot_cos - temp._y*_rot_sin,
							temp._x*_rot_sin + temp._y*_rot_cos);
		return ret + _center;
	}
	/// get inverse image of mathematical vector
	inline FF_Point rotVectorInverse(const FF_Point& p) const
	{
		const FF_Point ret(	p._x*_rot_cos_inv + p._y*_rot_sin_inv,
							-p._x*_rot_sin_inv + p._y*_rot_cos_inv);
		return ret;
	}
	/// get image of mathematical vector
	inline FF_Point rotVector(const FF_Point& p) const
	{
		const FF_Point ret( p._x*_rot_cos - p._y*_rot_sin,
							p._x*_rot_sin + p._y*_rot_cos);
		return ret;
	}
};

}//namespace helper

template<typename Pixel>
class ShaderRotate : public helper::ShaderBase<const Pixel>
{
public:
	typedef helper::ShaderBase<const Pixel> _base;
	friend class helper::controler<ShaderRotate<Pixel> >;
	
private:
	typedef helper::FF_Point point;
	typedef helper::FF val;
	
	point _start_pos;
	point _inc_x;
	point _inc_y;
	GraphSubset _range;
	point _move_domain;
	helper::rotator _rotate;
	int _move_x;
	int _move_y;
	
	void update_range()
	{
		_inc_x = _rotate.rotVectorInverse(point(1,0));
		_inc_y = _rotate.rotVectorInverse(point(0,1));
		
		_start_pos = _rotate.rotPointInverse(point(0,0));
		
		point tab[] =
		{ 
			point(_base::_range_domain.beg_x-1, _base::_range_domain.beg_y-1)+_move_domain,
			point(_base::_range_domain.beg_x-1, _base::_range_domain.end_y+1)+_move_domain,
			point(_base::_range_domain.end_x+1, _base::_range_domain.beg_y-1)+_move_domain,
			point(_base::_range_domain.end_x+1, _base::_range_domain.end_y+1)+_move_domain,
		};
		for(int i=0; i<4; ++i)
		{
			tab[i] = _rotate.rotPoint(tab[i]);
		}
		val beg_x = tab[0]._x;
		val end_x = tab[0]._x;
		val beg_y = tab[0]._y;
		val end_y = tab[0]._y;
		for(int i=1; i<4; ++i)
		{
			if(beg_x> tab[i]._x)
				beg_x = tab[i]._x;
			else if(end_x< tab[i]._x)
				end_x = tab[i]._x;
			
			if(beg_y> tab[i]._y)
				beg_y = tab[i]._y;
			else if(end_y< tab[i]._y)
				end_y = tab[i]._y;
				
		}
		_range = GraphSubset(std::make_pair((int)beg_x, (int)end_x+1),  std::make_pair((int)beg_y, (int)end_y+1));
	}
	
	inline point get_point_start() const
	{
		return _start_pos - _move_domain;
	}
	
public:
	inline ShaderRotate(const Surface* s):
		_base(s),
		_start_pos(0, 0),
		_inc_x(1, 0),
		_inc_y(0,1),
		_range(_base::_range_domain),
		_move_domain(),
		_rotate(),
		_move_x( s->getX()),
		_move_y( s->getY())
	{
		setRotationDeg(0, 0, 0);
	}
	inline ShaderRotate(const Surface* s, int rot, int rot_x, int rot_y):
		_base(s),
		_start_pos(0,0),
		_inc_x(1, 0),
		_inc_y(0,1),
		_range(_base::_range_domain),
		_move_domain(),
		_rotate(),
		_move_x( s->getX()),
		_move_y( s->getY())
	{
		setRotationDeg(rot, rot_x, rot_y);
	}
	inline ShaderRotate(const Surface* s, int rot):
		_base(s),
		_start_pos(0,0),
		_inc_x(1, 0),
		_inc_y(0,1),
		_range(_base::_range_domain),
		_move_domain(),
		_rotate(),
		_move_x( s->getX()),
		_move_y( s->getY())
	{
		setRotationDeg(rot, val(_base::_range_domain.size_x())/2, val(_base::_range_domain.size_y())/2);
	}
	
	inline ShaderRotate(const ShaderRotate& f): 
		_base(f),
		_start_pos(f._start_pos),
		_inc_x(f._inc_x),
		_inc_y(f._inc_y),
		_range(f._range),
		_move_domain(f._move_domain),
		_rotate(f._rotate),
		_move_x(f._move_x),
		_move_y(f._move_y)
	{

	}
	
	inline ShaderRotate(const std::vector<Pixel>& f, int max_x, int max_y): 
		_base(f, max_x, max_y),
		_start_pos(),
		_inc_x(1, 0),
		_inc_y(0,1),
		_range(_base::_range_domain),
		_move_domain(),
		_rotate(),
		_move_x(),
		_move_y()
	{
		setRotationDeg(0, 0, 0);
	}
	
	
	inline void setRotationDeg(int rot, val rot_x, val rot_y)
	{
		_rotate.setRotDeg(rot);
		_rotate.setPosition(point(rot_x, rot_y));
		update_range();
	}
	inline void setRotationRad(double rot, val rot_x, val rot_y)
	{
		_rotate.setRotRad(rot);
		_rotate.setPosition(point(rot_x, rot_y));
		update_range();
	}
	inline void setDeg(int rot)
	{
		_rotate.setRotDeg(rot);
		update_range();
	}
	inline void setRad(double rot)
	{
		_rotate.setRotRad(rot);
		update_range();
	}
	inline void addDeg(int rot)
	{
		_rotate.addRotDeg(rot);
		update_range();
	}
	inline void addRad(double rot)
	{
		_rotate.addRotRad(rot);
		update_range();
	}
	
	inline void setPos(val rot_x, val rot_y)
	{
		_rotate.setPosition(point(rot_x, rot_y));
		update_range();
	}
	inline void addPos(val rot_x, val rot_y)
	{
		_rotate.addPosition(point(rot_x, rot_y));
		update_range();
	}
	
	inline void setZoom(val zoom)
	{
		_rotate.setZoom(zoom);
		update_range();
	}
	
	
	inline GraphSubset getImage() const
	{
		return _range.offset(_move_x, _move_y);
	}
	
	inline void setDomain(const GraphSubset& g) const
	{
		_base::setDomain(g);
		update_range();
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
	inline void setMoveDomain(val x, val y)
	{
		_move_domain = point(x, y);
		update_range();
	}
	inline void addMoveDomain(val x, val y)
	{
		_move_domain += point(x, y);
		update_range();
	}
};

namespace helper
{

}//namespace helper
}//namespace OpenXcom
#endif	/* SHADERROTATE_H */

