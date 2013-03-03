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

#ifndef OPENXCOM_SHADERDRAW_H
#define	OPENXCOM_SHADERDRAW_H

#include "ShaderDrawHelper.h"
	
namespace OpenXcom
{

/**
 * Universal blit function
 * @tparam ColorFunc class that contains static function `func` that get 5 arguments
 * function is used to modify these arguments.
 * @param dest_frame destination surface modified by function.
 * @param src0_frame surface or scalar
 * @param src1_frame surface or scalar
 * @param src2_frame surface or scalar
 * @param src3_frame surface or scalar
 */
template<typename ColorFunc, typename DestType, typename Src0Type, typename Src1Type, typename Src2Type, typename Src3Type>
static inline void ShaderDraw(const DestType& dest_frame, const Src0Type& src0_frame, const Src1Type& src1_frame, const Src2Type& src2_frame, const Src3Type& src3_frame)
{
	//creating helper objects
	helper::controler<DestType> dest(dest_frame);
	helper::controler<Src0Type> src0(src0_frame);
	helper::controler<Src1Type> src1(src1_frame);
	helper::controler<Src2Type> src2(src2_frame);
	helper::controler<Src3Type> src3(src3_frame);

	//get basic draw range in 2d space
	GraphSubset end_temp = dest.get_range();
	
	//intersections with src ranges
	src0.mod_range(end_temp);
	src1.mod_range(end_temp);
	src2.mod_range(end_temp);
	src3.mod_range(end_temp);
	
	const GraphSubset end = end_temp;
	if(end.size_x() == 0 || end.size_y() == 0)
		return;
	//set final draw range in 2d space
	dest.set_range(end);
	src0.set_range(end);
	src1.set_range(end);
	src2.set_range(end);
	src3.set_range(end);


	int begin_y = 0, end_y = end.size_y();
	//determining iteration range in y-axis
	dest.mod_y(begin_y, end_y);
	src0.mod_y(begin_y, end_y);
	src1.mod_y(begin_y, end_y);
	src2.mod_y(begin_y, end_y);
	src3.mod_y(begin_y, end_y);
	if(begin_y>=end_y)
		return;
	//set final iteration range
	dest.set_y(begin_y, end_y);
	src0.set_y(begin_y, end_y);
	src1.set_y(begin_y, end_y);
	src2.set_y(begin_y, end_y);
	src3.set_y(begin_y, end_y);

	//iteration on y-axis
	for(int y = end_y-begin_y; y>0; --y, dest.inc_y(), src0.inc_y(), src1.inc_y(), src2.inc_y(), src3.inc_y())
	{
		int begin_x = 0, end_x = end.size_x();
		//determining iteration range in x-axis
		dest.mod_x(begin_x, end_x);
		src0.mod_x(begin_x, end_x);
		src1.mod_x(begin_x, end_x);
		src2.mod_x(begin_x, end_x);
		src3.mod_x(begin_x, end_x);
		if(begin_x>=end_x)
			continue;
		//set final iteration range
		dest.set_x(begin_x, end_x);
		src0.set_x(begin_x, end_x);
		src1.set_x(begin_x, end_x);
		src2.set_x(begin_x, end_x);
		src3.set_x(begin_x, end_x);
		
		//iteration on x-axis
		for(int x = end_x-begin_x; x>0; --x, dest.inc_x(), src0.inc_x(), src1.inc_x(), src2.inc_x(), src3.inc_x())
		{
			ColorFunc::func(dest.get_ref(), src0.get_ref(), src1.get_ref(), src2.get_ref(), src3.get_ref());				
		}
	}

};
	
template<typename ColorFunc, typename DestType, typename Src0Type, typename Src1Type, typename Src2Type>
static inline void ShaderDraw(const DestType& dest_frame, const Src0Type& src0_frame, const Src1Type& src1_frame, const Src2Type& src2_frame)
{
	ShaderDraw<ColorFunc>(dest_frame, src0_frame, src1_frame, src2_frame, helper::Nothing());
}
template<typename ColorFunc, typename DestType, typename Src0Type, typename Src1Type>
static inline void ShaderDraw(const DestType& dest_frame, const Src0Type& src0_frame, const Src1Type& src1_frame)
{
	ShaderDraw<ColorFunc>(dest_frame, src0_frame, src1_frame, helper::Nothing(), helper::Nothing());
}
template<typename ColorFunc, typename DestType, typename Src0Type>
static inline void ShaderDraw(const DestType& dest_frame, const Src0Type& src0_frame)
{
	ShaderDraw<ColorFunc>(dest_frame, src0_frame, helper::Nothing(), helper::Nothing(), helper::Nothing());
}
template<typename ColorFunc, typename DestType>
static inline void ShaderDraw(const DestType& dest_frame)
{
	ShaderDraw<ColorFunc>(dest_frame, helper::Nothing(), helper::Nothing(), helper::Nothing(), helper::Nothing());
}

template<typename T>
static inline helper::Scalar<T> ShaderScalar(T& t)
{
	return helper::Scalar<T>(t);
}
template<typename T>
static inline helper::Scalar<const T> ShaderScalar(const T& t)
{
	return helper::Scalar<const T>(t);
}
	
namespace helper
{
	
const Uint8 ColorGroup = 15<<4;
const Uint8 ColorShade = 15;
const Uint8 ColorShadeMax = 15;
const Uint8 BLACK = 15;

}//namespace helper

}//namespace OpenXcom


#endif	/* OPENXCOM_SHADERDRAW_H */

