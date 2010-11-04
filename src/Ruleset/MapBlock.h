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
#ifndef OPENXCOM_MAPBLOCK_H
#define OPENXCOM_MAPBLOCK_H

#include <string>

class SavedBattleGame;
class RuleTerrain;
class TerrainObjectSet;

/**
 * Represents a Terrain Map Block.
 * It contains constant info about this mapblock, like it's name, dimensions, attributes...
 * Map blocks are stored in RuleTerrain objects
 * @sa MapBlock
 */
class MapBlock
{
private:
	RuleTerrain *_terrain;
	std::string _name;
	int _width, _length;
	bool _landingZone;
public:
	MapBlock(RuleTerrain *terrain, std::string name, int width, int length, bool landingZone);
	~MapBlock();
	/// get the mapblock name (used for MAP generation)
	std::string getName();
	/// get width
	int getWidth();
	/// get width
	int getLength();
	/// is landingzoneable?
	bool isLandingZone();
	/// loads an XCOM MAP file
	int loadMAP(int xoff, int yoff, SavedBattleGame *save, RuleTerrain *terrain);
	/// loads an XCOM RMP file
	void loadRMP(int xoff, int yoff, SavedBattleGame *save);
};

#endif
