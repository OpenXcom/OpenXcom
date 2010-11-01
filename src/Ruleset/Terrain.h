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
#ifndef OPENXCOM_TERRAIN_H
#define OPENXCOM_TERRAIN_H

#include <map>
#include <vector>
#include <string>

class Surface;
class Tile;
class ResourcePack;
class SavedBattleGame;
class MapBlock;

class Terrain
{
private:
	std::vector<std::string*> _sheetnames;
	std::vector<MapBlock*> _mapblocks;
	std::string _name;
public:
	Terrain(const std::string &name);
	~Terrain();
	/// gets the list of sheetnames, without their extensions
	std::vector<std::string*> *getSheetnames();
	/// gets a surface given their absolute index and frame number
	Surface* getSurface(int i, int frame);
	/// get the terrain name (used for MAP generation)
	std::string getName();
	/// gets the list of mapblocks
	std::vector<MapBlock*> *getMapBlocks();
	/// get a random mapblock
	MapBlock* getRandomMapBlock(int maxsize, bool landingzone);
};

#endif
