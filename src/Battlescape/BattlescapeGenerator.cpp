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
#include <fstream>
#include <sstream>
#include "BattlescapeGenerator.h"
#include "TerrainModifier.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Craft.h"
#include "../Engine/RNG.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/MapDataFile.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/Ruleset.h"
#include "../Resource/TerrainObjectSet.h"
#include "../Resource/TerrainObject.h"
#include "../Resource/XcomResourcePack.h"
#include "../Engine/Game.h"


namespace OpenXcom
{

/**
 * Sets up a BattlescapeGenerator.
 * @param game pointer to Game object.
 */
BattlescapeGenerator::BattlescapeGenerator(Game *game) : _game(game)
{
	_save = _game->getSavedGame()->getBattleGame();
	_ufo = 0;
	_craft = 0;
}

/**
 * Deletes the BattlescapeGenerator.
 */
BattlescapeGenerator::~BattlescapeGenerator()
{

}

/**
 * Sets the x-com craft involved in the battle.
 * @param craft Pointer to XCom craft.
 */
void BattlescapeGenerator::setCraft(Craft *craft)
{
	_craft = craft;
	_craft->setInBattlescape(true);
}

/**
 * Sets the ufo involved in the battle.
 * @param ufo Pointer to UFO.
 */
void BattlescapeGenerator::setUfo(Ufo *ufo)
{
	_ufo = ufo;
	_ufo->setInBattlescape(true);
}

/**
 * Sets the world texture where a ufo crashed. This is used to determine the terrain.
 * @param texture Texture id of the polygon on the globe.
 */
void BattlescapeGenerator::setWorldTexture(int texture)
{
	_worldTexture = texture;
}

/**
 * Sets the world shade where a ufo crashed. This is used to determine the battlescape light level.
 * @param shade Shade of the polygon on the globe.
 */
void BattlescapeGenerator::setWorldShade(int shade)
{
	_worldShade = shade;
}

/**
 * Sets the mission type. This is used to determine the various elements of the battle.
 * @param missionType MissionType.
 */
void BattlescapeGenerator::setMissionType(MissionType missionType)
{
	_missionType = missionType;
	_save->setMissionType(missionType);
}

/**
 * This will start the generator: it will fill up the battlescapesavegame with data.
 */
void BattlescapeGenerator::run()
{
	_width = 50;
	_length = 50;
	_height = 4;
	_unitCount = 0;

	if (_missionType == MISS_TERROR)
	{
		_terrain = _game->getRuleset()->getTerrain("URBAN");
	}
	else
	{
		switch (_worldTexture)
		{
		case 0:	
		case 6:
		case 10:
		case 11:	
			{
				if (_ufo != 0)
				{
					if (_ufo->getLatitude() < 0)
					{ // northern hemisphere
						_terrain = _game->getRuleset()->getTerrain("FOREST");
					}else
					{ // southern hemisphere
						_terrain = _game->getRuleset()->getTerrain("JUNGLE");
					}
				}
				else
				{
					_terrain = _game->getRuleset()->getTerrain("FOREST");
				}
				break;
			}
		case 1:
		case 2:
		case 3:
		case 4:
			{
				_terrain = _game->getRuleset()->getTerrain("CULTA");
				break;
			}
		case 5:
			{
				_terrain = _game->getRuleset()->getTerrain("MOUNT");
				break;
			}
		case 7:
		case 8:
			{
				_terrain = _game->getRuleset()->getTerrain("DESERT");
				break;
			}
		case 9:
		case 12:
			{
				_terrain = _game->getRuleset()->getTerrain("POLAR");
				break;
			}
		}	
	}

	_save->initMap(_width, _length, _height);

	// lets generate the map now and store it inside the tiles
	generateMap(_game->getResourcePack());
	if (_craft != 0)
	{
		// add soldiers that are in the craft
		for (std::vector<Soldier*>::iterator i = _craft->getBase()->getSoldiers()->begin(); i != _craft->getBase()->getSoldiers()->end(); i++)
		{
			if ((*i)->getCraft() == _craft)
				addSoldier((*i), _game->getRuleset()->getUnitSprites("XCOM_0"));
		}

		// add items that are in the craft
		for (std::map<std::string, Item*>::iterator i = _craft->getItems()->begin(); i != _craft->getItems()->end(); i++)
		{
				addItem((*i).second);
		}
	}

	if (_missionType == MISS_UFORECOVERY)
	{
		// add aliens (should depend on mission type & difficulty level)
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), ENGINEER, "Sectoid Engineer");
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), NAVIGATOR, "Sectoid Navigator");
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), SOLDIER, "Sectoid Soldier");
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), SCOUT, "Sectoid Soldier");
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), SCOUT, "Sectoid Soldier");
		addAlien(_game->getRuleset()->getUnitSprites("SECTOID"), SCOUT, "Sectoid Soldier");
	}

	// set shade (alien bases are a little darker, sites depend on worldshade)
	int worldshades[8] = { 0, 1, 2, 3, 5, 7, 9 , 15 };
	_save->setGlobalShade(worldshades[_worldShade]);

	_save->getTerrainModifier()->calculateSunShading();
	_save->getTerrainModifier()->calculateLighting();
}

/**
 * Adds a soldier to the game and place him on a free spawnpoint.
 * @param soldier pointer to the Soldier
 * @param rules pointer to the RuleUnitSprite which holds info about unit drawing.
 */
void BattlescapeGenerator::addSoldier(Soldier *soldier, RuleUnitSprite *rules)
{
	BattleUnit *unit = new BattleUnit(rules, FACTION_PLAYER);
	unit->setSoldier(soldier);
	unit->setId(_unitCount++);

	Position pos;
	int x, y, z;

	for (int i = 0; i < _height * _length * _width; i++)
	{
		// to spawn an xcom soldier, there has to be a tile, with a floor, with the starting point attribute and no object in the way
		if (_save->getTiles()[i] && _save->getTiles()[i]->getTerrainObject(O_FLOOR) && _save->getTiles()[i]->getTerrainObject(O_FLOOR)->getSpecialType() == START_POINT && !_save->getTiles()[i]->getTerrainObject(O_OBJECT))
		{
			_save->getTileCoords(i, &x, &y, &z);
			pos = Position(x, y, z);
			if (_save->selectUnit(pos) == 0)
			{
				unit->setPosition(pos);
				_save->getTiles()[i]->setUnit(unit);
				break;
			}
		}
	}

	_save->getUnits()->push_back(unit);
	_save->setSelectedUnit(unit);
}

/**
 * Adds an alien to the game and place him on a free spawnpoint.
 * @param rules pointer to the RuleUnitSprite which holds info about unit drawing.
 * @param rank The rank of the alien.
 * @param name The name of the alien.
 */
void BattlescapeGenerator::addAlien(RuleUnitSprite *rules, NodeRank rank, const std::string &name)
{
	BattleUnit *unit = new BattleUnit(rules, FACTION_HOSTILE);
	Node *node;
	bool bFound = false;
	unit->setId(_unitCount++);


	// find a place to spawn, going from highest priority to lowest
	// some randomness is added
	for (int priority=10; priority > 0 && !bFound; priority--)
	{
		for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end() && !bFound; i++)
		{
			node = *i;
			if (node->getRank() == rank
				&& node->getPriority() == priority
				&& _save->selectUnit(node->getPosition()) == 0
				&& (RNG::generate(0,2) == 1))
			{
				unit->setPosition(node->getPosition());
				_save->getTile(node->getPosition())->setUnit(unit);
				bFound = true;
			}
		}
	}

	// second try in case we still haven't found a place to spawn
	// this time without randomness
	for (int priority=10; priority > 0 && !bFound; priority--)
	{
		for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end() && !bFound; i++)
		{
			node = *i;
			if (node->getRank() == rank
				&& node->getPriority() == priority
				&& _save->selectUnit(node->getPosition()) == 0
				)
			{
				unit->setPosition(node->getPosition());
				_save->getTile(node->getPosition())->setUnit(unit);
				bFound = true;
				break;
			}
		}
	}

	unit->setDirection(RNG::generate(0,7));
	unit->setName(name);

	_save->getUnits()->push_back(unit);
}
/**
 * Adds an item to the game and assign it to a soldier?.
 * @param item pointer to the Item
 */
void BattlescapeGenerator::addItem(Item *item)
{
	BattleItem *bi = new BattleItem(item);

	// todo: define position
	_save->getItems()->push_back(bi);
}

/** 
 * Generates a map (set of tiles) for a new battlescape game.
 * @param res pointer to ResourcePack.
 */
void BattlescapeGenerator::generateMap(ResourcePack *res)
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

	/* create tile objects */
	for (int i = 0; i < _height * _length * _width; i++)
	{
		Position pos;
		_save->getTileCoords(i, &pos.x, &pos.y, &pos.z);
		_save->getTiles()[i] = new Tile(pos);
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
				((XcomResourcePack*)res)->loadMAP(blocks[itX][itY], itX * 10, itY * 10, _save, _terrain);
				if (!landingzone[itX][itY])
				{
					((XcomResourcePack*)res)->loadRMP(blocks[itX][itY], itX * 10, itY * 10, _save);
				}
			}
		}
	}

	if (_ufo != 0)
	{
		((XcomResourcePack*)res)->loadMAP(ufoMap, ufoX * 10, ufoY * 10, _save, _ufo->getRules()->getBattlescapeTerrainData());
		((XcomResourcePack*)res)->loadRMP(ufoMap, ufoX * 10, ufoY * 10, _save);
	}

	if (_craft != 0)
	{
		((XcomResourcePack*)res)->loadMAP(craftMap, craftX * 10, craftY * 10, _save, _craft->getRules()->getBattlescapeTerrainData(), true);
		((XcomResourcePack*)res)->loadRMP(craftMap, craftX * 10, craftY * 10, _save);
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
void BattlescapeGenerator::linkTilesWithTerrainObjects(ResourcePack *res)
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
				tile = _save->getTile(Position(itX, itY, itZ));
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

	// now also link the alt&die terrainobjects to real objects
	MapDataFile* mdf = 0;
	TerrainObject* tob = 0;
	TerrainObject* altObject = 0;
	for (std::vector<MapDataFile*>::iterator i = _terrain->getMapDataFiles()->begin(); i != _terrain->getMapDataFiles()->end(); i++)
	{
		mdf = *i;
		for (std::vector<TerrainObject*>::iterator j = res->getTerrainObjectSet(mdf->getName())->getTerrainObjects()->begin(); j != res->getTerrainObjectSet(mdf->getName())->getTerrainObjects()->end(); j++)
		{
			tob = *j;
			if (tob->getAltMCD(0) == 0 && tob->getObjectType() == O_FLOOR)
			{
				altObject = res->getTerrainObjectSet("BLANKS.MCD")->getTerrainObjects()->at(1);
				tob->setAltObject(altObject, 0);
			}
			else if (tob->getAltMCD(0))
			{
				altObject = res->getTerrainObjectSet(mdf->getName())->getTerrainObjects()->at(tob->getAltMCD(0));
				tob->setAltObject(altObject, 0);
			}
			if (tob->getAltMCD(1))
			{
				altObject = res->getTerrainObjectSet(mdf->getName())->getTerrainObjects()->at(tob->getAltMCD(1));
				tob->setAltObject(altObject, 1);
			}
		}
	}
}


}
