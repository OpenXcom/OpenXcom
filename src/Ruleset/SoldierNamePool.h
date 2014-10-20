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
#ifndef OPENXCOM_SOLDIERNAMEPOOL_H
#define OPENXCOM_SOLDIERNAMEPOOL_H

#include <string>
#include <vector>
#include "../Savegame/Soldier.h"

namespace OpenXcom
{

/**
 * Pool of soldier names to generate random names.
 * Each pool contains a set of first names (male or female) and last names.
 * The first names define the soldier's gender, and are randomly associated
 * with a last name.
 */
class SoldierNamePool
{
private:
	std::vector<std::wstring> _maleFirst, _femaleFirst, _maleLast, _femaleLast;
	std::vector<int> _lookWeights;
	int _totalWeight, _femaleFrequency;
public:
	/// Creates a blank pool.
	SoldierNamePool();
	/// Cleans up the pool.
	~SoldierNamePool();
	/// Loads the pool from YAML.
	void load(const std::string &filename);
	/// Generates a new name from the pool.
	std::wstring genName(SoldierGender *gender, int femaleFrequency) const;
	/// Generates an int representing the index of the soldier's look, when passed the maximum index value.
	size_t genLook(size_t numLooks);
};

}

#endif
