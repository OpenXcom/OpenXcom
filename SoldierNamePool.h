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
#ifndef OPENXCOM__SOLDIERNAMEPOOL_H
#define OPENXCOM__SOLDIERNAMEPOOL_H

#include <string>
#include <sstream>
#include <vector>
#include "SavedGame.h"

using namespace std;

class SoldierNamePool
{
private:
	vector<string> _maleNames, _femaleNames, _lastNames;
public:
	SoldierNamePool();
	~SoldierNamePool();
	string genName(int *gender);
	vector<string> *getMaleNames();
	vector<string> *getFemaleNames();
	vector<string> *getLastNames();
};

#endif