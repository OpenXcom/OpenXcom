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
#ifndef OPENXCOM_SCRIPTBIND_H
#define	OPENXCOM_SCRIPTBIND_H

namespace OpenXcom
{
namespace helper
{
namespace
{

/**
 * Helper struct used for crating one time geter to private fields
 */
template<typename T>
struct Bind
{
	typedef T Base;
	typedef T Work;
	static const Work* func(const Base* t) { return t; }
};

/**
 * Helper struct used for crating one time geter to private fields
 */
template<typename B, typename T, T B::Work::* P>
struct BindMem
{
	typedef typename B::Base Base;
	typedef T Work;
	static const Work* func(const Base* t) { if(B::func(t)) return &(B::func(t)->*P); return 0; }
};

/**
 * Helper struct used for crating one time geter to private fields
 */
template<typename B, typename T, T* B::Work::* P>
struct BindPtr
{
	typedef typename B::Base Base;
	typedef T Work;
	static const Work* func(const Base* t) { if(B::func(t)) return B::func(t)->*P; return 0; }
};

/**
 * Helper struct used for crating one time geter to private fields
 */
template<typename B, typename T, const T* (B::Work::* P)() const>
struct BindFun
{
	typedef typename B::Base Base;
	typedef T Work;
	static const Work* func(const Base* t) { if(B::func(t)) return (B::func(t)->*P)(); return 0; }
};

template<typename B, int B::Work::* P>
int geter(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P); return 0;  }

template<typename B, int (B::Work::* P)() const>
int geter(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(); return 0;  }

template<typename B, int (B::Work::* P)(int) const, int I>
int geter(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(I); return 0;  }

template<typename B, typename Arg, int (B::Work::* P)(Arg) const, Arg I>
int geter(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(I); return 0;  }

template<typename B, typename Enum, Enum B::Work::* P>
int geter_cast(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P); return 0;  }

template<typename B, typename Enum, Enum (B::Work::* P)() const>
int geter_cast(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(); return 0;  }

template<typename B, typename Enum, Enum (B::Work::* P)(int) const, int I>
int geter_cast(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(I); return 0;  }

template<typename B, typename Enum, typename Arg, Enum (B::Work::* P)(Arg) const, Arg I>
int geter_cast(const typename B::Base* t) { if(B::func(t)) return (B::func(t)->*P)(I); return 0;  }

} //namespace
} //namespace helper
} //namespace OpenXcom

#endif	/* OPENXCOM_SCRIPTBIND_H */

