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

#ifndef ___OXC_UNORDERED_MAP_H
#define ___OXC_UNORDERED_MAP_H


#if !defined(_USE_HASH_MAP) && \
	!defined(_USE_google_sparsehash) && \
	!defined(_USE_boost_unordered_map) && \
	!defined(_USE_TR1) && \
	!defined(_USE_STL_map)


// set a reasonable default; std::tr1::unordered_map is good
#if defined(_MSC_VER) && _MSC_VER >= 1600 // in VS2008, unordered_map has to be installed as some part of the feature pack; there would be whining
#define _USE_TR1

#elif defined(__MINGW32__)
// _michal says his mingw setup has tr1 and that's good enough :)
#define _USE_TR1
#else

// if hash_map doesn't compile, compile with -D_USE_STL_map
// or better yet, get another hash table implementation
#define _USE_HASH_MAP

#endif

#endif


#ifdef _USE_HASH_MAP
#include<hash_map> // a legacy non-standard hash table that's likely actually available
#ifdef _MSC_VER
#define HASH_MAP_NAMESPACE stdext
#elif defined(__GNUC__)
#define HASH_MAP_NAMESPACE __gnu_cxx
#endif

#define unordered_map HASH_MAP_NAMESPACE::hash_map


namespace HASH_MAP_NAMESPACE {
	// from http://gcc.gnu.org/ml/libstdc++/2007-08/msg00057.html
template <>
struct hash<std::string> {
	size_t operator() (const std::string& x) const {
		return hash<const char*>()(x.c_str());
		// hash<const char*> already exists
	}
};
}


#elif defined(_USE_google_sparsehash)
#include <google/dense_hash_map> // this is an excellent hash table but is an addional dependency 
//and also requires an extra call to set_empty_key() after construction which makes it uniquely bothersome to use
#define unordered_map google::dense_hash_map

#elif defined (_USE_boost_unordered_map)
#include <boost/unordered_map.hpp> // this is good but requires libboost; that might be imported with yaml-cpp 0.5.0
#define unordered_map boost::unordered::unordered_map

#elif defined (_USE_TR1)

#if defined(_MSC_VER)
#include <unordered_map> // this was accepted into the standard from libboost and is also good but isn't available in, e.g., VS2003
#else
#include <tr1/unordered_map> 
#endif

#define unordered_map std::tr1::unordered_map

#endif

#if  !defined(unordered_map) || defined(_USE_STL_map)
// use the R-B tree if there's no real hash table available
#define unordered_map std::map

#else


// >> and << operators for unordered_map and yaml-cpp

#include <string>
class YAML::Emitter;
class YAML::Node;

namespace OpenXcom
{
extern YAML::Emitter &operator <<(YAML::Emitter &out, unordered_map<std::string, std::string> &map);
extern void operator >>(const YAML::Node *node, unordered_map<std::string, std::string> &map);
}

#endif


#endif
