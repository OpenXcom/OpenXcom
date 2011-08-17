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
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Alien.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Node.h"
#include "../Savegame/NodeLink.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MapData.h"
#include "../Resource/XcomResourcePack.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "PatrolBAIState.h"

namespace OpenXcom
{

/**
 * Sets up a BattlescapeGenerator.
 * @param game pointer to Game object.
 */
BattlescapeGenerator::BattlescapeGenerator(Game *game) : _game(game)
{
	_save = _game->getSavedGame()->getBattleGame();
	_res = _game->getResourcePack();
	_ufo = 0;
	_craft = 0;
	_craftInventoryTile = 0;
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

	// find out the terrain type
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

	// creates the tile objects
	_save->initMap(_width, _length, _height);
	_save->initUtilities(_res);

	// lets generate the map now and store it inside the tile objects
	generateMap();

	if (_craft != 0)
	{
		// add soldiers that are in the craft
		for (std::vector<Soldier*>::iterator i = _craft->getBase()->getSoldiers()->begin(); i != _craft->getBase()->getSoldiers()->end(); ++i)
		{
			if ((*i)->getCraft() == _craft)
				addSoldier((*i));
		}
		_save->setSelectedUnit(_save->getUnits()->at(0)); // select first soldier

		// add items that are in the craft
		for (std::map<std::string, int>::iterator i = _craft->getItems()->getContents()->begin(); i != _craft->getItems()->getContents()->end(); ++i)
		{
			for (int count=0; count < (*i).second; count++)
				addItem(_game->getRuleset()->getItem((*i).first));
		}
		// test data
		/*addItem(_game->getRuleset()->getItem("STR_RIFLE"));
		addItem(_game->getRuleset()->getItem("STR_RIFLE_CLIP"));
		addItem(_game->getRuleset()->getItem("STR_HEAVY_CANNON"));
		addItem(_game->getRuleset()->getItem("STR_HC_I_AMMO"));
		addItem(_game->getRuleset()->getItem("STR_ROCKET_LAUNCHER"));
		addItem(_game->getRuleset()->getItem("STR_SMALL_ROCKET"));
		addItem(_game->getRuleset()->getItem("STR_GRENADE"));
		addItem(_game->getRuleset()->getItem("STR_SMOKE_GRENADE"));
		addItem(_game->getRuleset()->getItem("STR_PISTOL"));
		addItem(_game->getRuleset()->getItem("STR_PISTOL_CLIP"));
		addItem(_game->getRuleset()->getItem("STR_GRENADE"));
		addItem(_game->getRuleset()->getItem("STR_GRENADE"));
		addItem(_game->getRuleset()->getItem("STR_GRENADE"));*/
	}

	if (_missionType == MISS_UFORECOVERY)
	{
		// add aliens (should depend on mission type & difficulty level)
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), ENGINEER);
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), NAVIGATOR);
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SOLDIER);
	}
	else
	{
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
		addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
	}
	addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
	addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
	addAlien(_game->getRuleset()->getAlien("SECTOID_SOLDIER"), _game->getRuleset()->getArmor("SECTOID_ARMOR0"), SCOUT);
	
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL"));
	addItem(_game->getRuleset()->getItem("STR_PLASMA_PISTOL_CLIP"));

	// set shade (alien bases are a little darker, sites depend on worldshade)
	int worldshades[8] = { 0, 1, 2, 3, 5, 7, 9 , 15 };
	_save->setGlobalShade(worldshades[_worldShade]);

	_save->getTerrainModifier()->calculateSunShading();
	_save->getTerrainModifier()->calculateTerrainLighting();
	_save->getTerrainModifier()->calculateUnitLighting();
}

/**
 * Adds a soldier to the game and place him on a free spawnpoint.
 * @param soldier pointer to the Soldier
 */
void BattlescapeGenerator::addSoldier(Soldier *soldier)
{
	BattleUnit *unit = new BattleUnit(soldier, FACTION_PLAYER);
	unit->setId(_unitCount++);

	Position pos;
	int x, y, z;

	for (int i = _height * _length * _width - 1; i >= 0; i--)
	{
		// to spawn an xcom soldier, there has to be a tile, with a floor, with the starting point attribute and no object in the way
		if (_save->getTiles()[i] && _save->getTiles()[i]->getMapData(MapData::O_FLOOR) && _save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT && !_save->getTiles()[i]->getMapData(MapData::O_OBJECT))
		{
			_save->getTileCoords(i, &x, &y, &z);
			pos = Position(x, y, z);
			if (_save->selectUnit(pos) == 0)
			{
				unit->setPosition(pos);
				_save->getTiles()[i]->setUnit(unit); // maybe we should assign all units to the first tile of the skyranger before the inventory pre-equip and then reassign them to their correct tile afterwards?
				if (_craftInventoryTile == 0)
					_craftInventoryTile = _save->getTiles()[i];
				break;
			}
		}
	}

	_save->getUnits()->push_back(unit);
}

/**
 * Adds an alien to the game and place him on a free spawnpoint.
 * @param rules pointer to the RuleAlien which holds info about alien .
 * @param armor The armor of the alien.
 * @param rank The rank of the alien, used for spawn point search.
 */
void BattlescapeGenerator::addAlien(RuleAlien *rules, RuleArmor *armor, NodeRank rank)
{
	BattleUnit *unit = new BattleUnit(new Alien(rules, armor, _game->getLanguage()), FACTION_HOSTILE);
	Node *node;
	bool bFound = false;
	unit->setId(_unitCount++);
	int lastSegment = -1;

	// find a place to spawn, going from lowest priority to heighest
	// some randomness is added
	for (int priority = 1; priority <= 10 && !bFound; priority++)
	{
		for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end() && !bFound; ++i)
		{
			node = *i;
			if (node->getRank() == rank
				&& node->getPriority() == priority
				&& _save->selectUnit(node->getPosition()) == 0
				&& (RNG::generate(0,2) == 1)
				&& lastSegment != node->getSegment())
			{
				lastSegment = node->getSegment();
				unit->setPosition(node->getPosition());
				_save->getTile(node->getPosition())->setUnit(unit);
				unit->setAIState(new PatrolBAIState(_game->getSavedGame()->getBattleGame(), unit, node));
				bFound = true;
			}
		}
	}

	// second try in case we still haven't found a place to spawn
	// this time without randomness
	for (int priority = 1; priority <= 10 && !bFound; priority++)
	{
		for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end() && !bFound; ++i)
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
				unit->setAIState(new PatrolBAIState(_game->getSavedGame()->getBattleGame(), unit, node));
				break;
			}
		}
	}

	unit->setDirection(RNG::generate(0,7));

	_save->getUnits()->push_back(unit);
}

/**
 * Adds an item to the game and assign it to a soldier?.
 * @param item pointer to the Item
 */
void BattlescapeGenerator::addItem(RuleItem *item)
{
	BattleItem *bi = new BattleItem(item);
	bool placed = false;

	switch (item->getBattleType())
	{
	case BT_AMMO:
		// find equipped weapons that can be loaded with this ammo
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			BattleItem *weapon = _save->getItemFromUnit((*i), "STR_RIGHT_HAND");
			if (weapon && weapon->getAmmoItem() == 0)
			{
				if (weapon->setAmmoItem(bi) == 0)
				{
					placed = true;
					break;
				}
			}
		}
		break;
	case BT_GRENADE:
	case BT_PROXIMITYGRENADE:
		// find the first soldier with a free belt slot to equip grenades
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if (!_save->getItemFromUnit((*i), "STR_BELT"))
			{
				bi->moveToOwner((*i));
				bi->setSlot(_game->getRuleset()->getInventory("STR_BELT"));
				placed = true;
				break;
			}
		}
		break;
	case BT_FIREARM:
	case BT_MELEE:
		// find the first soldier with a free right hand to equip weapons
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if (!_save->getItemFromUnit((*i), "STR_RIGHT_HAND"))
			{
				bi->moveToOwner((*i));
				bi->setSlot(_game->getRuleset()->getInventory("STR_RIGHT_HAND"));
				placed = true;
				break;
			}
		}
		// maybe we find ammo on the ground to load it with
		if (placed)
		{
			for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end(); ++i)
			{
				if (bi->setAmmoItem((*i)) == 0)
				{
					break;
				}
			}
		}
		break;
	case BT_MEDIKIT:
	case BT_SCANNER:
		// find the first soldier with a free backpack
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if (!_save->getItemFromUnit((*i), "STR_BACKPACK"))
			{
				bi->moveToOwner((*i));
				bi->setSlot(_game->getRuleset()->getInventory("STR_BACKPACK"));
				placed = true;
				break;
			}
		}
		break;
	case BT_NONE:
		break;
	}

	_save->getItems()->push_back(bi);

	// if we did not auto equip the item, place it on the ground
	if (!placed)
	{
		_craftInventoryTile->addItem(bi);
	}
}

/**
 * Generates a map (set of tiles) for a new battlescape game.
 */
void BattlescapeGenerator::generateMap()
{
	int x = 0, y = 0;
	int blocksToDo = 0;
	MapBlock* blocks[10][10];
	bool landingzone[10][10];
	int segments[10][10];
	int craftX = 0, craftY = 0;
	int ufoX = 0, ufoY = 0;
	bool placed = false;

	MapBlock* dummy = new MapBlock(_terrain, "dummy", 0, 0, false);
	MapBlock* craftMap = 0;
	MapBlock* ufoMap = 0;

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			blocks[i][j] = 0;
			landingzone[i][j] = false;
		}
	}

	blocksToDo = (_width / 10) * (_length / 10);

	/* Determine UFO landingzone (do this first because ufo is generally bigger) */
	if (_ufo != 0)
	{
		// crafts always consist of 1 mapblock, but can have all sorts of sizes
		ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getMapBlocks()->at(0);

		ufoX = RNG::generate(0, (_length / 10) - ufoMap->getWidth() / 10);
		ufoY = RNG::generate(0, (_width / 10) - ufoMap->getLength() / 10);

		for (int i = 0; i < ufoMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getLength() / 10; ++j)
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
			for (int i = 0; i < craftMap->getWidth() / 10; ++i)
			{
				for (int j = 0; j < craftMap->getLength() / 10; ++j)
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
				for (int i = 0; i < craftMap->getWidth() / 10; ++i)
					for (int j = 0; j < craftMap->getLength() / 10; ++j)
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

	for (std::vector<MapDataSet*>::iterator i = _terrain->getMapDataSets()->begin(); i != _terrain->getMapDataSets()->end(); ++i)
	{
		(*i)->load(_res);
		_save->getMapDataSets()->push_back(*i);
	}

	/* now load them up */
	int segment = 0;
	for (int itY = 0; itY < 10; itY++)
	{
		for (int itX = 0; itX < 10; itX++)
		{
			segments[itX][itY] = segment;
			if (blocks[itX][itY] != 0 && blocks[itX][itY] != dummy)
			{
				loadMAP(blocks[itX][itY], itX * 10, itY * 10, _terrain);
				if (!landingzone[itX][itY])
				{
					loadRMP(blocks[itX][itY], itX * 10, itY * 10, segment++);
				}
			}
		}
	}

	if (_ufo != 0)
	{
		for (std::vector<MapDataSet*>::iterator i = _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->load(_res);
			_save->getMapDataSets()->push_back(*i);
		}
		loadMAP(ufoMap, ufoX * 10, ufoY * 10, _ufo->getRules()->getBattlescapeTerrainData());
		loadRMP(ufoMap, ufoX * 10, ufoY * 10, Node::UFOSEGMENT);
		for (int i = 0; i < ufoMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getLength() / 10; j++)
			{
				segments[ufoX + i][ufoY + j] = Node::UFOSEGMENT;
			}
		}
	}

	if (_craft != 0)
	{
		for (std::vector<MapDataSet*>::iterator i = _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->load(_res);
			_save->getMapDataSets()->push_back(*i);
		}
		loadMAP(craftMap, craftX * 10, craftY * 10, _craft->getRules()->getBattlescapeTerrainData(), true);
		loadRMP(craftMap, craftX * 10, craftY * 10, Node::CRAFTSEGMENT);
		for (int i = 0; i < craftMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < craftMap->getLength() / 10; j++)
			{
				segments[craftX + i][craftY + j] = Node::CRAFTSEGMENT;
			}
		}
	}

	/* attach nodelinks to each other */
	for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
	{
		Node *node = (*i);
		int segmentX = node->getPosition().x / 10;
		int segmentY = node->getPosition().y / 10;
		int neighbourSegments[4] = {segments[segmentX+1][segmentY], segments[segmentX][segmentY+1], segments[segmentX-1][segmentY], segments[segmentX][segmentY-1] };
		int neighbourDirections[4] = { -2, -3, -4, -5 };
		int neighbourDirectionsInverted[4] = { -4, -5, -2, -3 };
		for (int j = 0; j < 5; j++)
		{
			for (int n = 0; n < 4; n++)
			{
				if (node->getNodeLink(j)->getConnectedNodeID() == neighbourDirections[n])
				{
					for (std::vector<Node*>::iterator k = _save->getNodes()->begin(); k != _save->getNodes()->end(); ++k)
					{
						if ((*k)->getSegment() == neighbourSegments[n])
						{
							for (int l = 0; l < 5; l++)
							{
								if ((*k)->getNodeLink(l)->getConnectedNodeID() == neighbourDirectionsInverted[n])
								{
									(*k)->getNodeLink(l)->setConnectedNodeID(node->getID());
									node->getNodeLink(j)->setConnectedNodeID((*k)->getID());
								}
							}
						}
					}
				}
			}
		}
	}

	/* TODO: map generation for terror sites */

	/* TODO: map generation for base defense mission */

	/* TODO: map generation for alien base assault */

	delete dummy;
}


/**
 * Loads a X-Com format MAP file into the tiles of the battlegame.
 * @param mapblock Pointer to MapBlock.
 * @param xoff Mapblock offset in X direction.
 * @param yoff Mapblock offset in Y direction.
 * @param save Pointer to the current SavedBattleGame.
 * @param terrain Pointer to the Terrain rule.
 * @param discovered Whether or not this mapblock is discovered (eg. landingsite of the x-com plane)
 * @return int Height of the loaded mapblock (this is needed for spawpoint calculation...)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * @note Y-axis is in reverse order
 */
int BattlescapeGenerator::loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, bool discovered)
{
	int width, length, height;
	int x = xoff, y = yoff, z = 0;
	char size[3];
	unsigned char value[4];
	std::stringstream filename;
	filename << "MAPS/" << mapblock->getName() << ".MAP";
	std::string mapDataFileName;
	int terrainObjectID;

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load MAP");
	}

	mapFile.read((char*)&size, sizeof(size));
	length = (int)size[0];
	width = (int)size[1];
	height = (int)size[2];
	z += height - 1;
	y += length - 1;
	mapblock->setHeight(height);

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; part++)
		{
			terrainObjectID = (int)((unsigned char)value[part]);
			if (terrainObjectID>0)
			{
				_save->getTile(Position(x, y, z))->setMapData(terrain->getMapData(terrainObjectID),part);
			}
			// if the part is empty and it's not a floor, remove it
			// it prevents growing grass in UFOs
			if (terrainObjectID == 0 && part > 0)
			{
				_save->getTile(Position(x, y, z))->setMapData(0,part);
			}
		}
		_save->getTile(Position(x, y, z))->setDiscovered(discovered, 2);

		x++;

		if (x == (width + xoff))
		{
			x = xoff;
			y--;
		}
		if (y == yoff - 1)
		{
			y = length - 1 + yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();

	return height;
}

/**
 * Loads a X-Com format RMP file into the spawnpoints of the battlegame.
 * @param mapblock pointer to MapBlock.
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param segment mapblock segment
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
void BattlescapeGenerator::loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment)
{
	int id = 0;
	char value[24];
	std::stringstream filename;
	filename << "ROUTES/" << mapblock->getName() << ".RMP";

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load RMP");
	}

	int nodeOffset = _save->getNodes()->size();

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		Node *node = new Node(nodeOffset + id, Position(xoff + (int)value[1], yoff + (mapblock->getLength() - 1 - (int)value[0]), mapblock->getHeight() - 1 - (int)value[2]), segment, (int)value[19], (int)value[20], (int)value[21], (int)value[22], (int)value[23]);
		for (int j=0;j<5;++j)
		{
			int connectID = (int)((signed char)value[4 + j*3]);
			if (connectID > -1)
			{
				connectID += nodeOffset;
			}
			node->assignNodeLink(new NodeLink(connectID, (int)value[5 + j*3], (int)value[6 + j*3]), j);
		}
		_save->getNodes()->push_back(node);
		id++;
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();
}

}
