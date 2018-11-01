/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Savegame/Soldier.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Initializes a new pool with blank lists of names.
 */
SoldierNamePool::SoldierNamePool() : _totalWeight(0), _femaleFrequency(-1)
{
}

/**
 *
 */
SoldierNamePool::~SoldierNamePool()
{
}

/**
 * Loads the pool from a YAML file.
 * @param filename YAML file.
 */
void SoldierNamePool::load(const std::string &filename)
{
	YAML::Node doc = YAML::LoadFile(filename);

	for (YAML::const_iterator i = doc["maleFirst"].begin(); i != doc["maleFirst"].end(); ++i)
	{
		std::string name = i->as<std::string>();
		_maleFirst.push_back(name);
	}
	for (YAML::const_iterator i = doc["femaleFirst"].begin(); i != doc["femaleFirst"].end(); ++i)
	{
		std::string name = i->as<std::string>();
		_femaleFirst.push_back(name);
	}
	for (YAML::const_iterator i = doc["maleLast"].begin(); i != doc["maleLast"].end(); ++i)
	{
		std::string name = i->as<std::string>();
		_maleLast.push_back(name);
	}
	for (YAML::const_iterator i = doc["femaleLast"].begin(); i != doc["femaleLast"].end(); ++i)
	{
		std::string name = i->as<std::string>();
		_femaleLast.push_back(name);
	}
	if (_femaleFirst.empty())
	{
		_femaleFirst = _maleFirst;
	}
	if (_femaleLast.empty())
	{
		_femaleLast = _maleLast;
	}
	_lookWeights = doc["lookWeights"].as< std::vector<int> >(_lookWeights);
	_totalWeight = 0;
	for (std::vector<int>::iterator i = _lookWeights.begin(); i != _lookWeights.end(); ++i)
	{
		_totalWeight += (*i);
	}
	_femaleFrequency = doc["femaleFrequency"].as<int>(_femaleFrequency);
}

/**
 * Returns a new random name (first + last) from the
 * lists of names contained within.
 * @param gender Returned gender of the name.
 * @return The soldier's name.
 */
std::string SoldierNamePool::genName(SoldierGender *gender, int femaleFrequency) const
{
	std::ostringstream name;
	bool female;
	if (_femaleFrequency > -1)
	{
		female = RNG::percent(_femaleFrequency);
	}
	else
	{
		female = RNG::percent(femaleFrequency);
	}

	if (!female)
	{
		*gender = GENDER_MALE;
		size_t first = RNG::generate(0, _maleFirst.size() - 1);
		name << _maleFirst[first];
		if (!_maleLast.empty())
		{
			size_t last = RNG::generate(0, _maleLast.size() - 1);
			name << " " << _maleLast[last];
		}
	}
	else
	{
		*gender = GENDER_FEMALE;
		size_t first = RNG::generate(0, _femaleFirst.size() - 1);
		name << _femaleFirst[first];
		if (!_femaleLast.empty())
		{
			size_t last = RNG::generate(0, _femaleLast.size() - 1);
			name << " " << _femaleLast[last];
		}
	}
	return name.str();
}

/**
 * Generates an int representing the index of the soldier's look, when passed the maximum index value.
 * @param numLooks The maximum index.
 * @return The index of the soldier's look.
 */
size_t SoldierNamePool::genLook(size_t numLooks)
{
	int look = 0;
	const int minimumChance = 2;	// minimum chance of a look being selected if it isn't enumerated. This ensures that looks MUST be zeroed to not appear.

	while (_lookWeights.size() < numLooks)
	{
		_lookWeights.push_back(minimumChance);
		_totalWeight += minimumChance;
	}
	while (_lookWeights.size() > numLooks)
	{
		_totalWeight -= _lookWeights.back();
		_lookWeights.pop_back();
	}

	int random = RNG::generate(0, _totalWeight);
	for (std::vector<int>::iterator i = _lookWeights.begin(); i != _lookWeights.end(); ++i)
	{
		if (random <= *i)
		{
			return look;
		}
		random -= *i;
		++look;
	}

	return RNG::generate(0, numLooks - 1);
}

}
