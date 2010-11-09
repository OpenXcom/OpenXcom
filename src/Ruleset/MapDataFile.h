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
#ifndef OPENXCOM_MAPDATAFILE_H
#define OPENXCOM_MAPDATAFILE_H

#include <string>

/**
 * Represents a Terrain Map Datafile.
 * It contains constant info about this mapblock, like it's name and size
 * Map datafiles are stored in RuleSet
 * @sa MapBlock
 */
class MapDataFile
{
private:
	std::string _name;
	int _size;
public:
	MapDataFile(std::string name, int size);
	~MapDataFile();
	/// Get the datafile's name (used for MAP generation).
	std::string getName();
	/// Get the datafile's size.
	int getSize();
};

#endif
