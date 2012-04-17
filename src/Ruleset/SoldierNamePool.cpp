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
#include "SoldierNamePool.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Exception.h"
#include "../Engine/CrossPlatform.h"

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
	std::string s = CrossPlatform::getDataFile("SoldierName/" + filename + ".nam");
	std::ifstream fin(s.c_str());
	if (!fin)
	{
		throw Exception("Failed to load name pool");
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
	size_t first = RNG::generate(1, _maleFirst.size() + _femaleFirst.size());
	if (first <= _maleFirst.size())
	{
		*gender = 0;
		name << _maleFirst[first - 1];
		size_t last = RNG::generate(1, _maleLast.size());
		name << " " << _maleLast[last - 1];
	}
	else
	{
		*gender = 1;
		name << _femaleFirst[first - _maleFirst.size() - 1];
		size_t last = RNG::generate(1, _femaleLast.size());
		name << " " << _femaleLast[last - 1];
	}
	return name.str();
}

}
