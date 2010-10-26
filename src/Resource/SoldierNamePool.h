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
#ifndef OPENXCOM_SOLDIERNAMEPOOL_H
#define OPENXCOM_SOLDIERNAMEPOOL_H

#include <string>
#include <vector>

/**
 * Pool of soldier names to generate random names.
 * Each pool contains a set of first names (male or female) and last names.
 * The first names define the soldier's gender, and are randomly associated
 * with a last name.
 */
class SoldierNamePool
{
private:
	std::vector<std::string> _maleNames, _femaleNames, _lastNames;
public:
	/// Creates a blank pool.
	SoldierNamePool();
	/// Cleans up the pool.
	~SoldierNamePool();
	/// Generates a new name from the pool.
	std::string genName(int *gender);
	/// Adds a male first name.
	void addMaleName(const std::string &name);
	/// Adds a female first name.
	void addFemaleName(const std::string &name);
	/// Adds a last name.
	void addLastName(const std::string &name);
};

#endif
