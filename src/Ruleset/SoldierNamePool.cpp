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
#include "SoldierNamePool.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Savegame/Soldier.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Exception.h"
#include "../Engine/CrossPlatform.h"

namespace OpenXcom
{

/**
 * Initializes a new pool with blank lists of names.
 */
SoldierNamePool::SoldierNamePool() : _maleFirst(), _femaleFirst(), _maleLast(), _femaleLast(), _totalWeight(0)
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
	std::string s = CrossPlatform::getDataFile("SoldierName/" + filename + ".nam");
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception(filename + " not found");
	}
	YAML::Parser parser(fin);
	YAML::Node doc;
	parser.GetNextDocument(doc);

	for (YAML::Iterator i = doc["maleFirst"].begin(); i != doc["maleFirst"].end(); ++i)
	{
		std::string name;
		*i >> name;
		_maleFirst.push_back(Language::utf8ToWstr(name));
	}
	for (YAML::Iterator i = doc["femaleFirst"].begin(); i != doc["femaleFirst"].end(); ++i)
	{
		std::string name;
		*i >> name;
		_femaleFirst.push_back(Language::utf8ToWstr(name));
	}
	for (YAML::Iterator i = doc["maleLast"].begin(); i != doc["maleLast"].end(); ++i)
	{
		std::string name;
		*i >> name;
		_maleLast.push_back(Language::utf8ToWstr(name));
	}
	if (const YAML::Node *pName = doc.FindValue("femaleLast"))
	{
		for (YAML::Iterator i = pName->begin(); i != pName->end(); ++i)
		{
			std::string name;
			*i >> name;
			_femaleLast.push_back(Language::utf8ToWstr(name));
		}
	}
	else
	{
		_femaleLast = _maleLast;
	}
	if (const YAML::Node *pName = doc.FindValue("lookWeights"))
	{
		for (YAML::Iterator i = pName->begin(); i != pName->end(); ++i)
		{
			int a;
			*i >> a;
			_totalWeight += a;
			_lookWeights.push_back(a);
		}
	}
	fin.close();
}

/**
 * Returns a new random name (first + last) from the
 * lists of names contained within.
 * @param gender Returned gender of the name.
 * @return Soldier name.
 */
std::wstring SoldierNamePool::genName(SoldierGender *gender) const
{
	std::wstringstream name;
	int gen = RNG::generate(1, 10);
	if (gen <= 5)
	{
		*gender = GENDER_MALE;
		size_t first = RNG::generate(0, _maleFirst.size() - 1);
		name << _maleFirst[first];
		size_t last = RNG::generate(0, _maleLast.size() - 1);
		name << " " << _maleLast[last];
	}
	else
	{
		*gender = GENDER_FEMALE;
		size_t first = RNG::generate(0, _femaleFirst.size() - 1);
		name << _femaleFirst[first];
		size_t last = RNG::generate(0, _femaleLast.size() - 1);
		name << " " << _femaleLast[last];
	}
	return name.str();
}

int SoldierNamePool::genLook(size_t numLooks)
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
