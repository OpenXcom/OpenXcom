/*
 * Copyright 2010 OpenXcom Developers
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
#include "SavedBattleGame.h"
#include "SavedGame.h"
#include "Craft.h"
#include "Tile.h"
#include "Ufo.h"
#include "Spawnpoint.h"
#include <fstream>
#include <sstream>
#include "../Engine/RNG.h"
#include "../Ruleset/Terrain.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleUfo.h"
#include "../Battlescape/Map.h"

/**
 * Initializes a brand new saved game according to the specified difficulty.
 * @param save
 * @param width = number of tiles on x-axis
 * @param length = number of tiles on y-axis
 * @param height = number of layers (z)
 */
SavedBattleGame::SavedBattleGame(SavedGame *save, int width, int length, int height, Terrain* terrain) : _tiles(),_spawnpoints(),_width(width),_length(length),_height(height),_terrain(terrain)
{
	_viewheight = 1;
}

/** 
 * Deletes the game content from memory.
 */
SavedBattleGame::~SavedBattleGame()
{
	for (int i=0;i<_height*_length*_width;i++)
		delete _tiles[i];
	delete _tiles;

	for (std::vector<Spawnpoint*>::iterator i = _spawnpoints.begin(); i != _spawnpoints.end(); i++)
	{
		delete *i;
	}

}

/** 
 * gets a pointer to the array of tiles
 * @return a pointer to Tile array
 */
Tile** SavedBattleGame::getTiles()
{
	return _tiles;
}

/** 
 * get the currently assigned terrain
 * @return pointer to Terrain
 */
Terrain *SavedBattleGame::getTerrain()
{
	return _terrain;
}

/** 
 * method
 * @param
 * @return
 */
int SavedBattleGame::getWidth()
{
	return _width;
}

/** 
 * method
 * @param
 * @return
 */
int SavedBattleGame::getLength()
{
	return _length;
}

/** 
 * method
 * @param
 * @return
 */
int SavedBattleGame::getHeight()
{
	return _height;
}

/** 
 * Viewheight is the level on the map that is currently in focus
 * @param
 * @return
 */
int SavedBattleGame::getViewHeight()
{
	return _viewheight;
}

/** 
 * moves the viewheigt one up or down
 * @param height relative movement
 * @return
 */
void SavedBattleGame::setRelativeViewHeight(int height)
{
	_viewheight+=height;
	if (_viewheight>_height)
		_viewheight=_height;
	if (_viewheight<1)
		_viewheight=1;
}

/** 
 * this method converts coordinates into a unique index
 * @param
 * @return
 */
int SavedBattleGame::getTileIndex(int x, int y, int z)
{
	return z*_length*_width+y*_width+x;
}
/** 
 * get the Tile on a given position on the map
 * @param
 * @return
 */
Tile *SavedBattleGame::getTile(int x, int y, int z)
{
	if (getTileIndex(x,y,z) > _height*_length*_width)
		throw "Cannot access Tile: index out of bounds";

	return _tiles[getTileIndex(x,y,z)];
}

/**
 * Sets the crafts for a crash site or ufo assault
 **/
void SavedBattleGame::setCrafts(Craft *craft, Ufo* ufo)
{
	_craft = craft;
	_ufo = ufo;
}
/**
 * Get the XCom craft 
 **/
Craft *SavedBattleGame::getCraft()
{
	return _craft;
}
/**
 * Get the UFO
 **/
Ufo *SavedBattleGame::getUfo()
{
	return _ufo;
}

/** 
 * generates a map(set of tiles) for a new battlescape game
 * @param
 * @return
 */
void SavedBattleGame::generateMap()
{
	int x=0,y=0;
	int blocksToDo=0;
	MapBlock* blocks[10][10];
	bool landingzone[10][10];
	MapBlock* dummy = new MapBlock("dummy",0,0,false);
	MapBlock* craftMap = NULL;
	MapBlock* ufoMap = NULL;
	int craftX=0,craftY=0;
	int ufoX=0,ufoY=0;
	bool placed=false;

	for (int i=0;i<10;i++)
		for (int j=0;j<10;j++)
			blocks[i][j] = NULL;
	for (int i=0;i<10;i++)
		for (int j=0;j<10;j++)
			landingzone[i][j] = false;

	blocksToDo = (_width/10)*(_length/10);

	/* allocate the array and the objects in it */
	_tiles = new Tile*[_height*_length*_width];
	for (int i=0;i<_height*_length*_width;i++)
		_tiles[i] = new Tile();

	/* Determine UFO landingzone (do this first because ufo is generally bigger) */
	if (_ufo != NULL)
	{
		// crafts always consist of 1 mapblock, but can have all sorts of sizes
		ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getMapBlocks()->at(0);
		ufoX=RNG::generate(0,(_length/10)- ufoMap->getWidth()/10);
		ufoY=RNG::generate(0,(_width/10)- ufoMap->getLength()/10);
		for (int i=0;i<ufoMap->getWidth()/10;i++)
			for (int j=0;j<ufoMap->getLength()/10;j++)
				landingzone[ufoX+i][ufoY+j] = true;
	}
	/* Determine Craft landingzone */
	if (_craft != NULL)
	{
		// crafts always consist of 1 mapblock, but can have all sorts of sizes
		craftMap = _craft->getRules()->getBattlescapeTerrainData()->getMapBlocks()->at(0);
		while (!placed)
		{
			craftX=RNG::generate(0,(_length/10)- craftMap->getWidth()/10);
			craftY=RNG::generate(0,(_width/10)- craftMap->getLength()/10);
			placed=true;
			// check if this place is ok
			for (int i=0;i<craftMap->getWidth()/10;i++)
			{
				for (int j=0;j<craftMap->getLength()/10;j++)
				{
					if (landingzone[craftX+i][craftY+j])
					{
						placed=false; // whoops the ufo is already here, try again
					}
				}
			}
			// if ok, allocate it
			if (placed)
			{
				for (int i=0;i<craftMap->getWidth()/10;i++)
					for (int j=0;j<craftMap->getLength()/10;j++)
						landingzone[craftX+i][craftY+j] = true;
			}
		}
	}

	/* Random map generation for crash/landing sites */
	while (blocksToDo)
	{
		if (blocks[x][y] == NULL)
		{
			// last block of this row or column or next block is not free or big block would block landingzone
			if (x == ((_width/10)-1) || y == ((_length/10)-1) || blocks[x+1][y] == dummy 
				|| landingzone[x+1][y] || landingzone[x+1][y+1] || landingzone[x][y+1] || blocksToDo==1)
			{
				// only small block will fit
				blocks[x][y] = _terrain->getRandomMapBlock(10,landingzone[x][y]);
				blocksToDo--;
				x++;
			}else
			{
				blocks[x][y] = _terrain->getRandomMapBlock(20,landingzone[x][y]);
				blocksToDo--;
				if (blocks[x][y]->getWidth() == 20) // big block
				{
					// mark mapblocks as used
					blocks[x+1][y] = dummy;
					blocksToDo--;
					blocks[x+1][y+1] = dummy;
					blocksToDo--;
					blocks[x][y+1] = dummy;
					blocksToDo--;
					x++;
				}
				x++;
			}
		}else
		{
			x++;
		}

		if (x >= (_width/10)) // reach the end
		{
			x=0; y++;
		}
	}

	/* now load them up */
	for (int itY=0;itY<10;itY++)
		for (int itX=0;itX<10;itX++)
			if (blocks[itX][itY] != NULL && blocks[itX][itY] != dummy)
				blocks[itX][itY]->loadMAP(itX*10,itY*10,this, 0);

	if (_ufo != NULL)
	{
		// the offset is minus 2 for craft mapblocks
		ufoMap->loadMAP(ufoX*10,ufoY*10,this, Map::ufoPos-2);
	}

	if (_craft != NULL)
	{
		// the offset is minus 2 for craft mapblocks
		craftMap->loadMAP(craftX*10,craftY*10,this, Map::craftPos-2);
	}

	/* TODO: map generation for terror sites */

	/* TODO: map generation for base defense mission */

	/* TODO: map generation for alien base assault */

	delete dummy;
}

