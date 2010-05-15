/*
 * Copyright 2010 Daniel Albano
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

SoldierNamePool::SoldierNamePool() : _maleNames(), _femaleNames(), _lastNames()
{
}

SoldierNamePool::~SoldierNamePool()
{
}

string SoldierNamePool::genName(int *gender)
{
	stringstream name;
	unsigned int first = SavedGame::genRandom(1, _maleNames.size() + _femaleNames.size());
	unsigned int last = SavedGame::genRandom(1, _lastNames.size());
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

vector<string> *SoldierNamePool::getMaleNames()
{
	return &_maleNames;
}

vector<string> *SoldierNamePool::getFemaleNames()
{
	return &_femaleNames;
}

vector<string> *SoldierNamePool::getLastNames()
{
	return &_lastNames;
}