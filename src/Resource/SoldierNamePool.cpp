/*
 * Copyright 2010 OpenXcom Developers.
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
#include "SoldierNamePool.h"
#include <sstream>
#include "../Engine/RNG.h"

/**
 * Initializes a new pool with blank lists of names.
 */
SoldierNamePool::SoldierNamePool() : _maleNames(), _femaleNames(), _lastNames()
{
}

/**
 *
 */
SoldierNamePool::~SoldierNamePool()
{
}

/**
 * Returns a new random name (first + last) from the
 * lists of names contained within.
 * @param gender Returned gender of the name.
 * @return Soldier name.
 */
std::string SoldierNamePool::genName(int *gender) const
{
	std::stringstream name;
	unsigned int first = RNG::generate(1, _maleNames.size() + _femaleNames.size());
	unsigned int last = RNG::generate(1, _lastNames.size());
	if (first <= _maleNames.size())
	{
		*gender = 0;
		name << _maleNames[first - 1];
	}
	else
	{
		*gender = 1;
		name << _femaleNames[first - _maleNames.size() - 1];
	}
	name << " " << _lastNames[last - 1];
	return name.str();
}

/**
 * Adds a new male first name to the pool.
 * @param name New name.
 */
void SoldierNamePool::addMaleName(const std::string &name)
{
	_maleNames.push_back(name);
}

/**
 * Adds a new female first name to the pool.
 * @param name New name.
 */
void SoldierNamePool::addFemaleName(const std::string &name)
{
	_femaleNames.push_back(name);
}

/**
 * Adds a new last name to the pool.
 * @param name New name.
 */
void SoldierNamePool::addLastName(const std::string &name)
{
	_lastNames.push_back(name);
}
