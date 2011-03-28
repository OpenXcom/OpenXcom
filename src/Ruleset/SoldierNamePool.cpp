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
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * Initializes a new pool with blank lists of names.
 */
SoldierNamePool::SoldierNamePool() : _maleFirst(), _femaleFirst(), _maleLast(), _femaleLast()
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
	unsigned int size = 0;

	std::string s = "./DATA/SoldierName/" + filename + ".nam";
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception("Failed to load name pool");
	}
    YAML::Parser parser(fin);
	YAML::Node doc;
    parser.GetNextDocument(doc);

	for(YAML::Iterator it = doc["maleFirst"].begin(); it != doc["maleFirst"].end(); it++)
	{
		std::string name;
		*it >> name;
		_maleFirst.push_back(Language::utf8ToWstr(name));
	}
	for(YAML::Iterator it = doc["femaleFirst"].begin(); it != doc["femaleFirst"].end(); it++)
	{
		std::string name;
		*it >> name;
		_femaleFirst.push_back(Language::utf8ToWstr(name));
	}
	for(YAML::Iterator it = doc["maleLast"].begin(); it != doc["maleLast"].end(); it++)
	{
		std::string name;
		*it >> name;
		_maleLast.push_back(Language::utf8ToWstr(name));
	}
	if (const YAML::Node *pName = doc.FindValue("femaleLast"))
	{
		for(YAML::Iterator it = doc["femaleLast"].begin(); it != doc["femaleLast"].end(); it++)
		{
			std::string name;
			*it >> name;
			_femaleLast.push_back(Language::utf8ToWstr(name));
		}
	}
	else
	{
		_femaleLast = _maleLast;
	}
		
	fin.close();
}

/**
 * Returns a new random name (first + last) from the
 * lists of names contained within.
 * @param gender Returned gender of the name.
 * @return Soldier name.
 */
std::wstring SoldierNamePool::genName(int *gender) const
{
	std::wstringstream name;
	unsigned int first = RNG::generate(1, _maleFirst.size() + _femaleFirst.size());
	if (first <= _maleFirst.size())
	{
		*gender = 0;
		name << _maleFirst[first - 1];
		unsigned int last = RNG::generate(1, _maleLast.size());
		name << " " << _maleLast[last - 1];
	}
	else
	{
		*gender = 1;
		name << _femaleFirst[first - _maleFirst.size() - 1];
		unsigned int last = RNG::generate(1, _femaleLast.size());
		name << " " << _femaleLast[last - 1];
	}
	return name.str();
}

}
