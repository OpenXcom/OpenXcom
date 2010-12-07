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
#include "SavedBattleGame.h"
#include "SavedGame.h"
#include "Craft.h"
#include "Tile.h"
#include "Ufo.h"
#include "BattleSoldier.h"
#include "BattleItem.h"
#include <fstream>
#include <sstream>
#include "../Engine/RNG.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleUnitSprite.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/XcomRuleset.h"
#include "../Battlescape/Map.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/Position.h"
#include "../Savegame/Node.h"
#include "../Resource/TerrainObject.h"
#include "../Resource/TerrainObjectSet.h"
#include "../Resource/ResourcePack.h"
#include "../Resource/XcomResourcePack.h"

/**
 * Initializes a brand new battlescape saved game.
 * @param save Pointer to saved game.
 * @param width Number of tiles on x-axis.
 * @param length Number of tiles on y-axis.
 * @param height Number of layers (z).
 * @param terrain Pointer to terrain set to use.
 */
SavedBattleGame::SavedBattleGame(SavedGame *save, int width, int length, int height, RuleTerrain *terrain) : _width(width), _length(length), _height(height), _tiles(), _craft(0), _ufo(0), _nodes(), _soldiers(), _terrain(terrain)
{
	_pathfinding = new Pathfinding(this);
}

/** 
 * Deletes the game content from memory.
 */
SavedBattleGame::~SavedBattleGame()
{
	for (int i = 0; i < _height * _length * _width; i++)
	{
		delete _tiles[i];
	}
	delete _tiles;

	for (std::vector<Node*>::iterator i = _nodes.begin(); i != _nodes.end(); i++)
	{
		delete *i;
	}

	for (std::vector<BattleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); i++)
	{
		delete *i;
	}

	for (std::vector<BattleItem*>::iterator i = _inventory.begin(); i != _inventory.end(); i++)
	{
		delete *i;
	}

	delete _pathfinding;
}

/** 
 * Gets a pointer to the array of tiles.
 * @return A pointer to Tile array.
 */
Tile **SavedBattleGame::getTiles()
{
	return _tiles;
}

/** 
 * Gets the currently assigned terrain.
 * @return Pointer to Terrain.
 */
RuleTerrain *SavedBattleGame::getTerrain()
{
	return _terrain;
}

/** 
 * Gets the map width.
 * @return Width in tiles.
 */
int SavedBattleGame::getWidth()
{
	return _width;
}

/** 
 * Gets the map length.
 * @return Length in tiles.
 */
int SavedBattleGame::getLength()
{
	return _length;
}

/** 
 * Gets the map height.
 * @return Height in layers.
 */
int SavedBattleGame::getHeight()
{
	return _height;
}

/** 
 * This method converts coordinates into a unique index.
 * @param pos position
 * @return Unique index.
 */
int SavedBattleGame::getTileIndex(const Position& pos)
{
	return pos.z * _length * _width + pos.y * _width + pos.x;
}

/** 
 * This method converts an index to coords.
 * @param index tileindex
 * @param x pointer to X coordinate.
 * @param y pointer to Y coordinate.
 * @param z pointer to Z coordinate.
 * @return Unique index.
 */
void SavedBattleGame::getTileCoords(int index, int *x, int *y, int *z)
{
	*z = index / (_length * _width);
	*y = (index % (_length * _width)) / _width;
	*x = (index % (_length * _width)) % _width;
}

/** 
 * Gets the Tile on a given position on the map.
 * @param pos position
 * @return Pointer to tile.
 */
Tile *SavedBattleGame::getTile(const Position& pos)
{
	if (pos.x < 0 || pos.y < 0 || pos.z < 0
		|| pos.x >= _width || pos.y >= _length || pos.z >= _height)
		return 0;

	if (getTileIndex(pos) > _height * _length * _width)
		return 0;

	return _tiles[getTileIndex(pos)];
}

/**
 * Sets the crafts for a crash site or UFO assault.
 * @param craft Pointer to XCom craft.
 * @param ufo Pointer to UFO.
 */
void SavedBattleGame::setCrafts(Craft *craft, Ufo *ufo)
{
	_craft = craft;
	_ufo = ufo;
}

/**
 * Gets the XCom craft on the map.
 * @return Pointer to craft.
 */
Craft *SavedBattleGame::getCraft()
{
	return _craft;
}

/**
 * Gets the UFO on the map.
 * @return Pointer to UFO.
 */
Ufo *SavedBattleGame::getUfo()
{
	return _ufo;
}

/**
 * Adds a soldier to the game and place him on a free spawnpoint.
 * @param soldier pointer to the Soldier
 * @param rules pointer to the RuleUnitSprite which holds info about unit drawing.
 */
void SavedBattleGame::addSoldier(Soldier *soldier, RuleUnitSprite *rules)
{
	BattleSoldier *bs = new BattleSoldier(soldier, rules);

	Position pos;
	int x, y, z;

	for (int i = 0; i < _height * _length * _width; i++)
	{
		// to spawn an xcom soldier, there has to be a tile, with a floor, with the starting point attribute and no object in the way
		if (_tiles[i] && _tiles[i]->getTerrainObject(0) && _tiles[i]->getTerrainObject(0)->getSpecialType() == START_POINT && !_tiles[i]->getTerrainObject(3))
		{
			getTileCoords(i, &x, &y, &z);
			pos = Position(x, y, z);
			if (selectUnit(pos) == 0)
			{
				bs->setPosition(pos);
				break;
			}
		}
	}

	_soldiers.push_back(bs);
	_selectedSoldier = bs;
}

/**
 * Adds an item to the game and assign it to a soldier?.
 * @param item pointer to the Item
 */
void SavedBattleGame::addItem(Item *item)
{
	BattleItem *bi = new BattleItem(item);

	_inventory.push_back(bi);
}

/**
 * Gets the currently selected soldier
 * @return pointer to BattleSoldier.
 */
BattleSoldier *SavedBattleGame::getSelectedSoldier()
{
	return _selectedSoldier;
}

/**
 * Sets the currently selected soldier.
 * @param soldier pointer to BattleSoldier.
 */
void SavedBattleGame::setSelectedSoldier(BattleSoldier *soldier)
{
	_selectedSoldier = soldier;
}

/**
 * Select the next soldier
 * @return pointer to BattleSoldier.
 */
BattleSoldier *SavedBattleGame::selectNextSoldier()
{

	for (std::vector<BattleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); i++)
	{
		if ((*i) == _selectedSoldier)
		{
			i++;
			if (i == _soldiers.end())
			{
				_selectedSoldier = (*_soldiers.begin());
				break;
			}
			else
			{
				_selectedSoldier =  *i;
				break;
			}
		}
	}

	return _selectedSoldier;
}

/**
 * Select unit with position on map.
 * @param pos Position
 * @return pointer to BattleUnit - 0 when nothing found
 */
BattleUnit *SavedBattleGame::selectUnit(const Position& pos)
{
	BattleUnit *bu = 0;

	for (std::vector<BattleSoldier*>::iterator i = _soldiers.begin(); i != _soldiers.end(); i++)
	{
		if ((*i)->getPosition() == pos)
		{
			bu = *i;
			break;
		}
	}

	return bu;
}

/** 
 * Generates a map (set of tiles) for a new battlescape game.
 * @param res pointer to ResourcePack.
 */
void SavedBattleGame::generateMap(ResourcePack *res)
{
	int x = 0, y = 0;
	int blocksToDo = 0;
	MapBlock* blocks[10][10];
	bool landingzone[10][10];
	int craftX = 0, craftY = 0;
	int ufoX = 0, ufoY = 0;
	bool placed = false;

	MapBlock* dummy = new MapBlock(_terrain, "dummy", 0, 0, false);
	MapBlock* craftMap = 0;
	MapBlock* ufoMap = 0;


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			blocks[i][j] = 0;
			landingzone[i][j] = false;
		}
	}

	blocksToDo = (_width / 10) * (_length / 10);

	/* allocate the array and the objects in it */
	_tiles = new Tile*[_height * _length * _width];
	for (int i = 0; i < _height * _length * _width; i++)
	{
		Position pos;
		getTileCoords(i, &pos.x, &pos.y, &pos.z);
		_tiles[i] = new Tile(pos);
	}

	/* Determine UFO landingzone (do this first because ufo is generally bigger) */
	if (_ufo != 0)
	{
		// crafts always consist of 1 mapblock, but can have all sorts of sizes
		ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getMapBlocks()->at(0);

		ufoX = RNG::generate(0, (_length / 10) - ufoMap->getWidth() / 10);
		ufoY = RNG::generate(0, (_width / 10) - ufoMap->getLength() / 10);

		for (int i = 0; i < ufoMap->getWidth() / 10; i++)
		{
			for (int j = 0; j < ufoMap->getLength() / 10; j++)
			{
				landingzone[ufoX + i][ufoY + j] = true;
			}
		}
	}

	/* Determine Craft landingzone */
	if (_craft != 0)
	{
		// crafts always consist of 1 mapblock, but can have all sorts of sizes
		craftMap = _craft->getRules()->getBattlescapeTerrainData()->getMapBlocks()->at(0);
		while (!placed)
		{
			craftX = RNG::generate(0, (_length/10)- craftMap->getWidth() / 10);
			craftY = RNG::generate(0, (_width/10)- craftMap->getLength() / 10);
			placed = true;
			// check if this place is ok
			for (int i = 0; i < craftMap->getWidth() / 10; i++)
			{
				for (int j = 0; j < craftMap->getLength() / 10; j++)
				{
					if (landingzone[craftX + i][craftY + j])
					{
						placed = false; // whoops the ufo is already here, try again
					}
				}
			}
			// if ok, allocate it
			if (placed)
			{
				for (int i = 0; i < craftMap->getWidth() / 10; i++)
					for (int j = 0; j < craftMap->getLength() / 10; j++)
						landingzone[craftX + i][craftY + j] = true;
			}
		}
	}

	/* Random map generation for crash/landing sites */
	while (blocksToDo)
	{
		if (blocks[x][y] == 0)
		{
			// last block of this row or column or next block is not free or big block would block landingzone
			if (x == ((_width / 10) - 1) || y == ((_length / 10) - 1) || blocks[x + 1][y] == dummy 
				|| landingzone[x + 1][y] || landingzone[x + 1][y + 1] || landingzone[x][y + 1] || blocksToDo == 1)
			{
				// only small block will fit
				blocks[x][y] = _terrain->getRandomMapBlock(10, landingzone[x][y]);
				blocksToDo--;
				x++;
			}
			else
			{
				blocks[x][y] = _terrain->getRandomMapBlock(20, landingzone[x][y]);
				blocksToDo--;
				if (blocks[x][y]->getWidth() == 20) // big block
				{
					// mark mapblocks as used
					blocks[x + 1][y] = dummy;
					blocksToDo--;
					blocks[x + 1][y + 1] = dummy;
					blocksToDo--;
					blocks[x][y + 1] = dummy;
					blocksToDo--;
					x++;
				}
				x++;
			}
		}
		else
		{
			x++;
		}

		if (x >= (_width / 10)) // reach the end
		{
			x = 0;
			y++;
		}
	}

	/* now load them up */
	for (int itY = 0; itY < 10; itY++)
	{
		for (int itX = 0; itX < 10; itX++)
		{
			if (blocks[itX][itY] != 0 && blocks[itX][itY] != dummy)
			{
				((XcomResourcePack*)res)->loadMAP(blocks[itX][itY], itX * 10, itY * 10, this, _terrain);
				if (!landingzone[itX][itY])
				{
					((XcomResourcePack*)res)->loadRMP(blocks[itX][itY], itX * 10, itY * 10, this);
				}
			}
		}
	}

	if (_ufo != 0)
	{
		((XcomResourcePack*)res)->loadMAP(ufoMap, ufoX * 10, ufoY * 10, this, _ufo->getRules()->getBattlescapeTerrainData());
		((XcomResourcePack*)res)->loadRMP(ufoMap, ufoX * 10, ufoY * 10, this);
	}

	if (_craft != 0)
	{
		((XcomResourcePack*)res)->loadMAP(craftMap, craftX * 10, craftY * 10, this, _craft->getRules()->getBattlescapeTerrainData());
		((XcomResourcePack*)res)->loadRMP(craftMap, craftX * 10, craftY * 10, this);
	}

	/* TODO: map generation for terror sites */

	/* TODO: map generation for base defense mission */

	/* TODO: map generation for alien base assault */

	delete dummy;

	linkTilesWithTerrainObjects(res);
}

/**
 * This function will link the tiles with the actual objects in the resourcepack
 * using their object names.
 * @param res pointer to resourcepack
 */
void SavedBattleGame::linkTilesWithTerrainObjects(ResourcePack *res)
{
	Tile *tile;
	std::string dataFilename;
	int objectID;
	int beginX = 0, endX = _width - 1;
    int beginY = 0, endY = _length - 1;
    int beginZ = 0, endZ = _height - 1;

    for (int itZ = beginZ; itZ <= endZ; itZ++) 
	{
        for (int itX = beginX; itX <= endX; itX++) 
		{
            for (int itY = endY; itY >= beginY; itY--) 
			{
				tile = getTile(Position(itX, itY, itZ));
				if (tile)
				{
					for (int part = 0; part < 4; part++)
					{
						if (!tile->getName(part).empty())
						{
							_terrain->parseTerrainObjectName(tile->getName(part),&dataFilename,&objectID);
							tile->setTerrainObject(res->getTerrainObjectSet(dataFilename)->getTerrainObjects()->at(objectID), part);
						}
					}
				}
			}
		}
	}
}

/**
 * Gets the list of nodes.
 * @return pointer to the list of nodes
 */
std::vector<Node*> *SavedBattleGame::getNodes()
{
	return &_nodes;
}

/**
 * Get the pathfinding object.
 * @return pointer to the pathfinding object
 */
Pathfinding *SavedBattleGame::getPathfinding()
{
	return _pathfinding;
}
