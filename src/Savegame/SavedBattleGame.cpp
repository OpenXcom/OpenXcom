/*
 * Copyright 2010-2013 OpenXcom Developers.
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
 
 
#include <assert.h>
#include <vector>
#include <deque>
#include <queue>

#include "SavedBattleGame.h"
#include "SavedGame.h"
#include "Tile.h"
#include "Node.h"
#include <SDL.h>
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MCDPatch.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/TileEngine.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Battlescape/BattlescapeGame.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Language.h"
#include "../Engine/Game.h"
#include "../Ruleset/RuleInventory.h"
#include "../Battlescape/PatrolBAIState.h"
#include "../Battlescape/AggroBAIState.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Engine/Logger.h"
#include "SerializationHelper.h"


namespace OpenXcom
{

/**
 * Initializes a brand new battlescape saved game.
 */
SavedBattleGame::SavedBattleGame() : _battleState(0), _mapsize_x(0), _mapsize_y(0),
                                     _mapsize_z(0),   _tiles(), _selectedUnit(0),
                                     _lastSelectedUnit(0), _nodes(), _units(),
                                     _items(), _pathfinding(0), _tileEngine(0),
                                     _missionType(""), _globalShade(0), _side(FACTION_PLAYER),
                                     _turn(1), _debugMode(false), _aborted(false),
                                     _itemId(0), _objectiveDestroyed(false), _fallingUnits(),
                                     _unitsFalling(false), _strafeEnabled(false), _sneaky(false),
                                     _traceAI(false)
{
	_dragButton = Options::getInt("battleScrollDragButton");
	_dragInvert = Options::getBool("battleScrollDragInvert");
	_dragTimeTolerance = Options::getInt("battleScrollDragTimeTolerance");
	_dragPixelTolerance = Options::getInt("battleScrollDragPixelTolerance");
	_strafeEnabled = Options::getBool("strafe");
	_sneaky = Options::getBool("sneakyAI");
	_traceAI = Options::getBool("traceAI");
}

/**
 * Deletes the game content from memory.
 */
SavedBattleGame::~SavedBattleGame()
{
	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		delete _tiles[i];
	}
	delete[] _tiles;

	for (std::vector<Node*>::iterator i = _nodes.begin(); i != _nodes.end(); ++i)
	{
		delete *i;
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		delete *i;
	}

	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete *i;
	}

	delete _pathfinding;
	delete _tileEngine;
}

/**
 * Loads the saved battle game from a YAML file.
 * @param node YAML node.
 */
void SavedBattleGame::load(const YAML::Node &node, Ruleset *rule, SavedGame* savedGame)
{
	int a,b;
	int selectedUnit = 0;

	node["width"] >> _mapsize_x;
	node["length"] >> _mapsize_y;
	node["height"] >> _mapsize_z;
	node["missionType"] >> _missionType;
	node["globalshade"] >> _globalShade;
	node["turn"] >> _turn;
	node["selectedUnit"] >> selectedUnit;

	for (YAML::Iterator i = node["mapdatasets"].begin(); i != node["mapdatasets"].end(); ++i)
	{
		std::string name;
		*i >> name;
		MapDataSet *mds = new MapDataSet(name);
		_mapDataSets.push_back(mds);
	}

	initMap(_mapsize_x, _mapsize_y, _mapsize_z);
	
	if (!node.FindValue("tileTotalBytesPer"))
	{
		// binary tile data not found, load old-style text tiles :(
		for (YAML::Iterator i = node["tiles"].begin(); i != node["tiles"].end(); ++i)
		{
			Position pos;
			(*i)["position"][0] >> pos.x;
			(*i)["position"][1] >> pos.y;
			(*i)["position"][2] >> pos.z;
			getTile(pos)->load((*i));
		}
	} else 
	{
		// load key to how the tile data was saved
		Tile::SerializationKey serKey;
		size_t totalTiles;

        memset(&serKey, 0, sizeof(Tile::SerializationKey));
		node["tileIndexSize"] >> serKey.index;
		node["tileTotalBytesPer"] >> serKey.totalBytes;
		node["tileFireSize"] >> serKey._fire;
		node["tileSmokeSize"] >> serKey._smoke;
		node["tileIDSize"] >> serKey._mapDataID;
		node["tileSetIDSize"] >> serKey._mapDataSetID;
		node["totalTiles"] >> totalTiles;
        if (const YAML::Node *boolFieldsNode = node.FindValue("tileBoolFieldsSize")) 
        {
            *boolFieldsNode >> serKey.boolFields;
        } else
        {
            serKey.boolFields = 1; // boolean flags used to be stored in an unmentioned byte (Uint8) :|
        }

		// load binary tile data! 
		YAML::Binary binTiles;
		node["binTiles"] >> binTiles;

		Uint8 *r = (Uint8*)binTiles.data();
		Uint8 *dataEnd = r + totalTiles * serKey.totalBytes;

		while (r < dataEnd)
		{
			int index = unserializeInt(&r, serKey.index);
			assert (index >= 0 && index < _mapsize_x * _mapsize_z * _mapsize_y);
			_tiles[index]->loadBinary(r, serKey); // loadBinary's privileges to advance *r have been revoked
			r += serKey.totalBytes-serKey.index; // r is now incremented strictly by totalBytes in case there are obsolete fields present in the data
		}		
	}

	for (YAML::Iterator i = node["nodes"].begin(); i != node["nodes"].end(); ++i)
	{
		Node *n = new Node();
		n->load(*i);
		_nodes.push_back(n);
	}

	for (YAML::Iterator i = node["units"].begin(); i != node["units"].end(); ++i)
	{
		UnitFaction faction;

		(*i)["faction"] >> a;
		faction = (UnitFaction)a;

		(*i)["soldierId"] >> a;

		BattleUnit *b;
		if (a < BattleUnit::MAX_SOLDIER_ID) // Unit is linked to a geoscape soldier
		{
			// look up the matching soldier
			b = new BattleUnit(savedGame->getSoldier(a), faction);
		}
		else
		{
			std::string type, armor;
			(*i)["genUnitType"] >> type;
			(*i)["genUnitArmor"] >> armor;
			// create a new Unit.
			b = new BattleUnit(rule->getUnit(type), faction, a, rule->getArmor(armor), savedGame->getDifficulty());
		}
		b->load(*i);
		_units.push_back(b);
		if (faction == FACTION_PLAYER)
		{
			if (b->getId() == selectedUnit)
				_selectedUnit = b;
		}
		else if (b->getStatus() != STATUS_DEAD)
		{
			if (const YAML::Node *ai = (*i).FindValue("AI"))
			{
				std::string state;
				BattleAIState *aiState;
				(*ai)["state"] >> state;
				if (state == "PATROL")
				{
					aiState = new PatrolBAIState(this, b, 0);
				}
				else if (state == "AGGRO")
				{
					aiState = new AggroBAIState(this, b);
				}
				else
				{
					continue;
				}
				aiState->load((*ai));
				b->setAIState(aiState);
			}
		}
	}
	// matches up tiles and units
	resetUnitTiles();

	for (YAML::Iterator i = node["items"].begin(); i != node["items"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		(*i)["id"] >> _itemId;
		if (type != "0")
		{
			BattleItem *item = new BattleItem(rule->getItem(type), &_itemId);
			item->load(*i);
			(*i)["inventoryslot"] >> type;
			if (type != "NULL")
				item->setSlot(rule->getInventory(type));
			(*i)["owner"] >> a;
			(*i)["unit"] >> b;

			// match up items and units
			for (std::vector<BattleUnit*>::iterator bu = _units.begin(); bu != _units.end(); ++bu)
			{
				if ((*bu)->getId() == a)
				{
					item->moveToOwner(*bu);
				}
				if ((*bu)->getId() == b)
				{
					item->setUnit(*bu);
				}
			}

			// match up items and tiles
			if (item->getSlot() && item->getSlot()->getType() == INV_GROUND)
			{
				Position pos;
				(*i)["position"][0] >> pos.x;
				(*i)["position"][1] >> pos.y;
				(*i)["position"][2] >> pos.z;
				if (pos.x != -1)
					getTile(pos)->addItem(item, rule->getInventory("STR_GROUND"));
			}
			_items.push_back(item);
		}
	}

	// tie ammo items to their weapons, running through the items again
	std::vector<BattleItem*>::iterator weaponi = _items.begin();
	for (YAML::Iterator i = node["items"].begin(); i != node["items"].end(); ++i, ++weaponi)
	{
		(*i)["ammoItem"] >> a;
		if (a != -1)
		{
			for (std::vector<BattleItem*>::iterator ammoi = _items.begin(); ammoi != _items.end(); ++ammoi)
			{
				if ((*ammoi)->getId() == a)
				{
					(*weaponi)->setAmmoItem((*ammoi));
					break;
				}
			}
		}
	}
	if (node.FindValue("objectiveDestroyed"))
	{
		node["objectiveDestroyed"] >> _objectiveDestroyed;
	}
}

/**
 * Loads the resources required by the map in the battle save.
 * @param res Pointer to resource pack.
 */
void SavedBattleGame::loadMapResources(Game *game)
{
	ResourcePack *res = game->getResourcePack();
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		(*i)->loadData();
		if (game->getRuleset()->getMCDPatch((*i)->getName()))
		{
			game->getRuleset()->getMCDPatch((*i)->getName())->modifyData(*i);
		}
	}

	int mdsID, mdID;

	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		for (int part = 0; part < 4; part++)
		{
			_tiles[i]->getMapData(&mdID, &mdsID, part);
			if (mdID != -1 && mdsID != -1)
			{
				_tiles[i]->setMapData(_mapDataSets[mdsID]->getObjects()->at(mdID), mdID, mdsID, part);
			}
		}
	}

	initUtilities(res);
	getTileEngine()->calculateSunShading();
	getTileEngine()->calculateTerrainLighting();
	getTileEngine()->calculateUnitLighting();
	getTileEngine()->recalculateFOV();
}

/**
 * Saves the saved battle game to a YAML file.
 * @param out YAML emitter.
 */
void SavedBattleGame::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	if (_objectiveDestroyed)
	{
		out << YAML::Key << "objectiveDestroyed" << YAML::Value << _objectiveDestroyed;
	}
	out << YAML::Key << "width" << YAML::Value << _mapsize_x;
	out << YAML::Key << "length" << YAML::Value << _mapsize_y;
	out << YAML::Key << "height" << YAML::Value << _mapsize_z;
	out << YAML::Key << "missionType" << YAML::Value << _missionType;
	out << YAML::Key << "globalshade" << YAML::Value << _globalShade;
	out << YAML::Key << "turn" << YAML::Value << _turn;
	out << YAML::Key << "selectedUnit" << YAML::Value << (_selectedUnit?_selectedUnit->getId():-1);

	out << YAML::Key << "mapdatasets" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		out << (*i)->getName();
	}
	out << YAML::EndSeq;
#if 0
	out << YAML::Key << "tiles" << YAML::Value;
	out << YAML::BeginSeq;
	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		if (!_tiles[i]->isVoid())
		{
			_tiles[i]->save(out);
		}
	}
	out << YAML::EndSeq;
#else
	// first, write out the field sizes we're going to use to write the tile data
	out << YAML::Key << "tileIndexSize" << YAML::Value << Tile::serializationKey.index;
	out << YAML::Key << "tileTotalBytesPer" << YAML::Value << Tile::serializationKey.totalBytes;
	out << YAML::Key << "tileFireSize" << YAML::Value << Tile::serializationKey._fire;
	out << YAML::Key << "tileSmokeSize" << YAML::Value << Tile::serializationKey._smoke;
	out << YAML::Key << "tileIDSize" << YAML::Value << Tile::serializationKey._mapDataID;
	out << YAML::Key << "tileSetIDSize" << YAML::Value << Tile::serializationKey._mapDataSetID;
    out << YAML::Key << "tileBoolFieldsSize" << YAML::Value << Tile::serializationKey.boolFields;

	size_t tileDataSize = Tile::serializationKey.totalBytes * _mapsize_z * _mapsize_y * _mapsize_x;
	Uint8* tileData = (Uint8*) calloc(tileDataSize, 1);
	Uint8* w = tileData;

	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		if (!_tiles[i]->isVoid())
		{
			serializeInt(&w, Tile::serializationKey.index, i);
			_tiles[i]->saveBinary(&w);
		}
		else
		{
			tileDataSize -= Tile::serializationKey.totalBytes;
		}
	}
	out << YAML::Key << "totalTiles" << YAML::Value << tileDataSize / Tile::serializationKey.totalBytes; // not strictly necessary, just convenient
	out << YAML::Key << "binTiles" << YAML::Value << YAML::Binary(tileData, tileDataSize);
    free(tileData);


#endif

	out << YAML::Key << "nodes" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Node*>::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "units" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BattleUnit*>::const_iterator i = _units.begin(); i != _units.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "items" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BattleItem*>::const_iterator i = _items.begin(); i != _items.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}

/**
 * Gets a pointer to the array of tiles.
 * @return A pointer to Tile array.
 */
Tile **SavedBattleGame::getTiles() const
{
	return _tiles;
}

/**
 * Initializes the array of tiles + creates a pathfinding object.
 * @param mapsize_x
 * @param mapsize_y
 * @param mapsize_z
 */
void SavedBattleGame::initMap(int mapsize_x, int mapsize_y, int mapsize_z)
{
	if (!_nodes.empty())
	{
		for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
		{
			delete _tiles[i];
		}
		delete[] _tiles;

		for (std::vector<Node*>::iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			delete *i;
		}

		_nodes.clear();
		_mapDataSets.clear();
	}
	_mapsize_x = mapsize_x;
	_mapsize_y = mapsize_y;
	_mapsize_z = mapsize_z;
	_tiles = new Tile*[_mapsize_z * _mapsize_y * _mapsize_x];
	/* create tile objects */
	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		Position pos;
		getTileCoords(i, &pos.x, &pos.y, &pos.z);
		_tiles[i] = new Tile(pos);
	}

}

/**
 * Initializes the map utilities.
 * @param res Pointer to resource pack.
 */
void SavedBattleGame::initUtilities(ResourcePack *res)
{
	_pathfinding = new Pathfinding(this);
	_tileEngine = new TileEngine(this, res->getVoxelData());
}

/**
 * Sets the mission type.
 * @param missionType
 */
void SavedBattleGame::setMissionType(const std::string &missionType)
{
	_missionType = missionType;
}

/**
 * Gets the mission type.
 * @return missionType
 */
std::string SavedBattleGame::getMissionType() const
{
	return _missionType;
}

/**
 * Sets the global shade.
 * @param shade
 */
void SavedBattleGame::setGlobalShade(int shade)
{
	_globalShade = shade;
}

/**
 * Gets the global shade.
 * @return int
 */
int SavedBattleGame::getGlobalShade() const
{
	return _globalShade;
}

/**
 * Gets the map width.
 * @return Size X in tiles.
 */
int SavedBattleGame::getMapSizeX() const
{
	return _mapsize_x;
}

/**
 * Gets the map length.
 * @return Size Y in tiles.
 */
int SavedBattleGame::getMapSizeY() const
{
	return _mapsize_y;
}

/**
 * Gets the map height.
 * @return Size Z in layers.
 */
int SavedBattleGame::getMapSizeZ() const
{
	return _mapsize_z;
}

/**
 * Gets the map size in tiles.
 * @return map size.
 */
int SavedBattleGame::getMapSizeXYZ() const
{
	return _mapsize_x * _mapsize_y * _mapsize_z;
}

/**
 * This method converts an index to coords.
 * @param index tileindex
 * @param x pointer to X coordinate.
 * @param y pointer to Y coordinate.
 * @param z pointer to Z coordinate.
 * @return Unique index.
 */
void SavedBattleGame::getTileCoords(int index, int *x, int *y, int *z) const
{
	*z = index / (_mapsize_y * _mapsize_x);
	*y = (index % (_mapsize_y * _mapsize_x)) / _mapsize_x;
	*x = (index % (_mapsize_y * _mapsize_x)) % _mapsize_x;
}


/**
 * Gets the currently selected unit
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::getSelectedUnit() const
{
	return _selectedUnit;
}

/**
 * Sets the currently selected unit.
 * @param unit pointer to BattleUnit.
 */
void SavedBattleGame::setSelectedUnit(BattleUnit *unit)
{
	_selectedUnit = unit;
}

/**
 * Select the previous player unit TODO move this to BattlescapeState ?
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectPreviousPlayerUnit(bool checkReselect)
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bPrev = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bPrev = true;
	}

	do
	{
		if (bPrev && (*i)->getFaction() == _side && !(*i)->isOut())
		{
			if ( !checkReselect || ((*i)->reselectAllowed()))
				break;
		}
		if ((*i) == _selectedUnit)
		{
			bPrev = true;
		}
		if (i == _units.begin())
		{
			i = _units.end();
			wraps++;
		}
		--i;
		// back to where we started... no more units found
		if (wraps == 3)
		{
			_selectedUnit = 0;
			return _selectedUnit;
		}
	}
	while (true);

	_selectedUnit = (*i);

	return _selectedUnit;
}

/**
 * Select the next player unit.
 * TODO move this to BattlescapeState ?
 * @param checkReselect should we check if we should reselect unit?
 * @param setReselect Don't reselect unit.
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectNextPlayerUnit(bool checkReselect, bool setReselect)
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bNext = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bNext = true;
	}
	else
	if (setReselect)
	{
		_selectedUnit->dontReselect();
	}

	do
	{
		if (bNext && (*i)->getFaction() == _side && !(*i)->isOut())
		{
			if ( !checkReselect || ((*i)->reselectAllowed()))
				break;
		}
		if ((*i) == _selectedUnit)
		{
			bNext = true;
		}
		++i;
		if (i == _units.end())
		{
			i = _units.begin();
			wraps++;
		}
		// back to where we started... no more units found
		if (wraps == 2)
		{
			_selectedUnit = 0;
			return _selectedUnit;
		}
	}
	while (true);

	_selectedUnit = (*i);

	return _selectedUnit;
}

/**
 * Select unit with position on map.
 * @param pos Position
 * @return pointer to BattleUnit - 0 when nothing found
 */
BattleUnit *SavedBattleGame::selectUnit(const Position& pos)
{
	BattleUnit *bu = getTile(pos)->getUnit();

	if (bu && bu->isOut())
	{
		return 0;
	}
	else
	{
		return bu;
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
 * Gets the list of units.
 * @return pointer to the list of units
 */
std::vector<BattleUnit*> *SavedBattleGame::getUnits()
{
	return &_units;
}

/**
 * Gets the list of items.
 * @return pointer to the list of items
 */
std::vector<BattleItem*> *SavedBattleGame::getItems()
{
	return &_items;
}

/**
 * Get the pathfinding object.
 * @return pointer to the pathfinding object
 */
Pathfinding *SavedBattleGame::getPathfinding() const
{
	return _pathfinding;
}

/**
 * Get the terrain modifier object.
 * @return pointer to the terrain modifier object
 */
TileEngine *SavedBattleGame::getTileEngine() const
{
	return _tileEngine;
}

/**
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapDataSet*> *SavedBattleGame::getMapDataSets()
{
	return &_mapDataSets;
}

/**
 * Gets the side currently playing.
 * @return Unit faction.
 */
UnitFaction SavedBattleGame::getSide() const
{
	return _side;
}

/**
 * Gets the current turn number.
 * @return Turn.
 */
int SavedBattleGame::getTurn() const
{
	return _turn;
}

/**
 * Ends the current turn and progresses to the next one.
 */
void SavedBattleGame::endTurn()
{
	if (_side == FACTION_PLAYER)
	{
		if (_selectedUnit && _selectedUnit->getOriginalFaction() == FACTION_PLAYER)
			_lastSelectedUnit = _selectedUnit;
		_side = FACTION_HOSTILE;
	}
	else if (_side == FACTION_HOSTILE)
	{
		_side = FACTION_NEUTRAL;
		// if there is no neutral team, we skip this and instantly prepare the new turn for the player
		if (selectNextPlayerUnit() == 0)
		{
			prepareNewTurn();
			_turn++;
			_side = FACTION_PLAYER;
			if (_lastSelectedUnit && !_lastSelectedUnit->isOut())
				_selectedUnit = _lastSelectedUnit;
			else
				selectNextPlayerUnit();
			while (_selectedUnit && _selectedUnit->getFaction() != FACTION_PLAYER)
				selectNextPlayerUnit();
		}

	}
	else if (_side == FACTION_NEUTRAL)
	{
		prepareNewTurn();
		_turn++;
		_side = FACTION_PLAYER;
		if (_lastSelectedUnit && !_lastSelectedUnit->isOut())
			_selectedUnit = _lastSelectedUnit;
		else
			selectNextPlayerUnit();
		while (_selectedUnit && _selectedUnit->getFaction() != FACTION_PLAYER)
			selectNextPlayerUnit();
	}

	if (_side == FACTION_PLAYER)
	{
		int liveSoldiers, liveAliens;

		_battleState->getBattleGame()->tallyUnits(liveAliens, liveSoldiers, false);

		// update the "number of turns since last spotted"
		for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
		{
			if ((*i)->getTurnsExposed() < 255 && _side == FACTION_PLAYER)
			{
				(*i)->setTurnsExposed((*i)->getTurnsExposed() +	1);
			}
			if (_side == FACTION_PLAYER && (*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut()
				&& (_turn >= 20 || liveAliens < 2))
			{
				(*i)->setTurnsExposed(0);
			}
		}
	}

	// hide all aliens (VOF calculations below will turn them visible again)
	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getFaction() == _side)
		{
			(*i)->prepareNewTurn();
		}
		if ((*i)->getFaction() != FACTION_PLAYER)
		{
			(*i)->setVisible(false);
		}
	}
	
	// re-run calculateFOV() *after* all aliens have been set not-visible
	_tileEngine->recalculateFOV();

	if (_side != FACTION_PLAYER)
		selectNextPlayerUnit();
}

/**
 * Turns on debug mode.
 */
void SavedBattleGame::setDebugMode()
{
	for (int i = 0; i < _mapsize_z * _mapsize_y * _mapsize_x; ++i)
	{
		_tiles[i]->setDiscovered(true, 2);
	}

	_debugMode = true;
}

/**
 * Gets the current debug mode.
 * @return Debug mode.
 */
bool SavedBattleGame::getDebugMode() const
{
	return _debugMode;
}

BattlescapeState *SavedBattleGame::getBattleState()
{
	return _battleState;
}

void SavedBattleGame::setBattleState(BattlescapeState *bs)
{
	_battleState = bs;
}


/**
 * Resets all the units to their current standing tile(s).
 */
void SavedBattleGame::resetUnitTiles()
{
	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if (!(*i)->isOut())
		{
			if ((*i)->getTile() && (*i)->getTile()->getUnit() == (*i))
			{
				(*i)->getTile()->setUnit(0); // XXX XXX XXX doesn't this fail to clear 3 out of 4 tiles for 2x2 units?
			}
			int size = (*i)->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					Tile *t = getTile((*i)->getPosition() + Position(x,y,0));
					t->setUnit((*i), getTile(t->getPosition() + Position(0,0,-1)));
				}
			}

		}
		if ((*i)->getFaction() == FACTION_PLAYER)
		{
			(*i)->setVisible(true);
		}
	}
}

/**
 * Removes an item from the game. Eg. when ammo item is depleted.
 * @param item The Item to remove.
 */
void SavedBattleGame::removeItem(BattleItem *item)
{
	// due to strange design, the item has to be removed from the tile it is on too (if it is on a tile)
	Tile *t = item->getTile();
	BattleUnit *b = item->getOwner();
	if (t)
	{
		for (std::vector<BattleItem*>::iterator it = t->getInventory()->begin(); it != t->getInventory()->end(); ++it)
		{
			if ((*it) == item)
			{
				t->getInventory()->erase(it);
				break;
			}
		}
	}
	else if (b)
	{
		for (std::vector<BattleItem*>::iterator it = b->getInventory()->begin(); it != b->getInventory()->end(); ++it)
		{
			if ((*it) == item)
			{
				b->getInventory()->erase(it);
				break;
			}
		}
	}

	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		if (*i == item)
		{
			_items.erase(i);
			break;
		}
	}

	/*
	for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; ++i)
	{
		for (std::vector<BattleItem*>::iterator it = _tiles[i]->getInventory()->begin(); it != _tiles[i]->getInventory()->end(); )
		{
			if ((*it) == item)
			{
				it = _tiles[i]->getInventory()->erase(it);
				return;
			}
			++it;
		}
	}
	*/

}

/**
 * Is the mission aborted or successful.
 * @param bool.
 */
void SavedBattleGame::setAborted(bool flag)
{
	_aborted = flag;
}

/**
 * Is the mission aborted or successful.
 * @return bool.
 */
bool SavedBattleGame::isAborted() const
{
	return _aborted;
}

/**
 * Is the mission aborted or successful.
 * @param bool.
 */
void SavedBattleGame::setObjectiveDestroyed(bool flag)
{
	_objectiveDestroyed = flag;
	if (flag && Options::getBool("battleAutoEnd"))
	{
		_battleState->getBattleGame()->statePushBack(0);
	}
}

/**
 * Is the mission aborted or successful.
 * @return bool.
 */
bool SavedBattleGame::isObjectiveDestroyed()
{
	return _objectiveDestroyed;
}

/**
 * Gets the current item ID.
 * @return Current item ID pointer.
 */
int *SavedBattleGame::getCurrentItemId()
{
	return &_itemId;
}

/**
 * Finds a fitting node where a unit can spawn.
 * @param nodeRank Rank of the node (is not rank of the alien!).
 * @param unit Pointer to the unit (to get its position)
 * @return pointer to the chosen node.
 */
Node *SavedBattleGame::getSpawnNode(int nodeRank, BattleUnit *unit)
{
	int highestPriority = -1;
	std::vector<Node*> compliantNodes;	

	for (std::vector<Node*>::iterator i = getNodes()->begin(); i != getNodes()->end(); ++i)
	{
		if ((*i)->getRank() == nodeRank										// ranks must match
			&& (!((*i)->getType() & Node::TYPE_SMALL) 
				|| unit->getArmor()->getSize() == 1)				// the small unit bit is not set or the unit is small
			&& (!((*i)->getType() & Node::TYPE_FLYING) 
				|| unit->getArmor()->getMovementType() == MT_FLY)// the flying unit bit is not set or the unit can fly
			&& (*i)->getPriority() > 0										// priority 0 is no spawnplace
			&& setUnitPosition(unit, (*i)->getPosition(), true))		// check if not already occupied
		{
			if ((*i)->getPriority() > highestPriority)
			{
				highestPriority = (*i)->getPriority();
				compliantNodes.clear(); // drop the last nodes, as we found a higher priority now
			}
			if ((*i)->getPriority() == highestPriority)
			{
				compliantNodes.push_back((*i));
			}
		}
	}
	
	if (compliantNodes.empty()) return 0;

	int n = RNG::generate(0, compliantNodes.size() - 1);

	return compliantNodes[n];
}

/**
 * Finds a fitting node where a unit can patrol to.
 * @param nodeRank Rank of the node (is not rank of the alien!).
 * @param unit Pointer to the unit (to get its position)
 * @return pointer to the choosen node.
 */
Node *SavedBattleGame::getPatrolNode(bool scout, BattleUnit *unit, Node *fromNode)
{
	std::vector<Node *> compliantNodes;	
	Node *preferred = 0;
	
	if (fromNode == 0)
	{
		if (Options::getBool("traceAI")) { Log(LOG_INFO) << "This alien got lost. :("; }
		fromNode = getNodes()->at(RNG::generate(0, getNodes()->size() - 1));
	}

	// scouts roam all over while all others shuffle around to adjacent nodes at most:
	const int end = scout ? getNodes()->size() : fromNode->getNodeLinks()->size();
	
	for (int i = 0; i < end; ++i)
	{
			if (!scout && fromNode->getNodeLinks()->at(i) < 1) continue;
			Node *n = getNodes()->at(scout ? i : fromNode->getNodeLinks()->at(i));
			if ((n->getFlags() > 0 || n->getRank() > 0 || scout)										// for non-scouts we find a node with a desirability above 0
			    && (!(n->getType() & Node::TYPE_SMALL) 
					|| unit->getArmor()->getSize() == 1)				// the small unit bit is not set or the unit is small
				&& (!(n->getType() & Node::TYPE_FLYING) 
					|| unit->getArmor()->getMovementType() == MT_FLY)// the flying unit bit is not set or the unit can fly
				&& !n->isAllocated() // check if not allocated
				&& !(n->getType() & Node::TYPE_DANGEROUS)   // don't go there if an alien got shot there; stupid behavior like that 
				&& setUnitPosition(unit, n->getPosition(), true)	// check if not already occupied
				&& getTile(n->getPosition()) && !getTile(n->getPosition())->getFire() // you are not a firefighter; do not patrol into fire
				&& (!scout || n != fromNode)	// scouts push forward
				&& n->getPosition().x > 0 && n->getPosition().y > 0)
			{
				if (!preferred 
					|| (preferred->getRank() == Node::nodeRank[unit->getRankInt()][0] && preferred->getFlags() < n->getFlags())
					|| preferred->getFlags() < n->getFlags()) preferred = n;
				compliantNodes.push_back(n);
			}
	}

	if (compliantNodes.empty())
	{ 
		if (Options::getBool("traceAI")) { Log(LOG_INFO) << (scout ? "Scout " : "Guard ") << "found no patrol node! XXX XXX XXX"; }
		if (unit->getArmor()->getSize() > 1 && !scout) 
		{
			return getPatrolNode(true, unit, fromNode); // move dammit
		} else return 0; 
	}
	
	if (scout)
	{
		// scout picks a random destination:
		return compliantNodes[RNG::generate(0, compliantNodes.size() - 1)];
	} else
	{
		if (!preferred) return 0;
		// non-scout patrols to highest value unoccupied node that's not fromNode
		if (Options::getBool("traceAI")) { Log(LOG_INFO) << "Choosing node flagged " << preferred->getFlags(); }
		return preferred;
	}
}

/**
 * New turn preparations. Like fire and smoke spreading.
 * @return True when objective destroyed by fire
 */
void SavedBattleGame::prepareNewTurn()
{
	std::vector<Tile*> tilesOnFire;
	std::vector<Tile*> tilesOnSmoke;

	// prepare a list of tiles on fire
	for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; ++i)
	{
		if (getTiles()[i]->getFire() > 0)
		{
			tilesOnFire.push_back(getTiles()[i]);
		}
	}

	// first: fires spread
	for (std::vector<Tile*>::iterator i = tilesOnFire.begin(); i != tilesOnFire.end(); ++i)
	{
		// if we haven't added fire here this turn
		if ((*i)->getOverlaps() == 0)
		{
			// reduce the fire timer
			(*i)->setFire((*i)->getFire() -1);

			// if we're still burning
			if ((*i)->getFire())
			{
				// propegate in four cardinal directions (0, 2, 4, 6)
				for (int dir = 0; dir <= 6; dir += 2)
				{
					Position pos;
					Pathfinding::directionToVector(dir, &pos);
					Tile *t = getTile((*i)->getPosition() + pos);
					// if there's no wall blocking the path of the flames...
					if (t && getTileEngine()->horizontalBlockage((*i), t, DT_IN) == 0)
					{
						// attempt to set this tile on fire
						t->ignite((*i)->getSmoke());
					}
				}
			}
			// fire has burnt out
			else
			{
				(*i)->setSmoke(0);
				// burn this tile, and any object in it, if it's not fireproof/indestructible.
				if ((*i)->getMapData(MapData::O_OBJECT))
				{
					if ((*i)->getMapData(MapData::O_OBJECT)->getFlammable() != 255 && (*i)->getMapData(MapData::O_OBJECT)->getArmor() != 255)
					{
						if ((*i)->destroy(MapData::O_OBJECT))
						{
							_objectiveDestroyed = true;
						}
						if ((*i)->destroy(MapData::O_FLOOR))
						{
							_objectiveDestroyed = true;
						}
					}
				}
				else if ((*i)->getMapData(MapData::O_FLOOR))
				{
					if ((*i)->getMapData(MapData::O_FLOOR)->getFlammable() != 255 && (*i)->getMapData(MapData::O_FLOOR)->getArmor() != 255)
					{
						if ((*i)->destroy(MapData::O_FLOOR))
						{
							_objectiveDestroyed = true;
						}
					}
				}
			}
		}
	}

	// prepare a list of tiles on fire/with smoke in them (smoke acts as fire intensity)
	for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; ++i)
	{
		if (getTiles()[i]->getSmoke() > 0)
		{
			tilesOnSmoke.push_back(getTiles()[i]);
		}
	}

	// now make the smoke spread.
	for (std::vector<Tile*>::iterator i = tilesOnSmoke.begin(); i != tilesOnSmoke.end(); ++i)
	{
		// smoke and fire follow slightly different rules.
		if ((*i)->getFire() == 0)
		{
			// reduce the smoke counter
			(*i)->setSmoke((*i)->getSmoke() - 1);
			// if we're still smoking
			if ((*i)->getSmoke())
			{
				// spread in four cardinal directions
				for (int dir = 0; dir <= 6; dir += 2)
				{
					Position pos;
					Pathfinding::directionToVector(dir, &pos);
					Tile *t = getTile((*i)->getPosition() + pos);
					// as long as there are no walls blocking us
					if (t && getTileEngine()->horizontalBlockage((*i), t, DT_SMOKE) == 0)
					{
						// only add smoke to empty tiles, or tiles with no fire, and smoke that was added this turn
						if (t->getSmoke() == 0 || (t->getFire() == 0 && t->getOverlaps() != 0))
						{
							t->addSmoke((*i)->getSmoke());
						}
					}
				}
			}
		}
		else
		{
			// smoke from fire spreads upwards one level if there's no floor blocking it.
			Position pos = Position(0,0,1);
			Tile *t = getTile((*i)->getPosition() + pos);
			if (t && t->hasNoFloor(*i))
			{
				// only add smoke equal to half the intensity of the fire
				t->addSmoke((*i)->getSmoke()/2);
			}
			// then it spreads in the four cardinal directions.
			for (int dir = 0; dir <= 6; dir += 2)
			{
				Pathfinding::directionToVector(dir, &pos);
				t = getTile((*i)->getPosition() + pos);
				if (t && getTileEngine()->horizontalBlockage((*i), t, DT_SMOKE) == 0)
				{
					t->addSmoke((*i)->getSmoke()/2);
				}
			}
		}
	}

	if (!tilesOnFire.empty() || !tilesOnSmoke.empty())
	{
		// do damage to units, average out the smoke, etc.
		for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; ++i)
		{
			if (getTiles()[i]->getSmoke() != 0)
				getTiles()[i]->prepareNewTurn();
		}
		// fires could have been started, stopped or smoke could reveal/conceal units.
		getTileEngine()->calculateTerrainLighting();
	}

	reviveUnconsciousUnits();

}

/**
 * Units that are unconscious but shouldn't are revived, they need a tile to stand on. The unit's current position could be occupied.
 * We will search in all directions for a free tile, if not found, the unit stays unconscious...
 */
void SavedBattleGame::reviveUnconsciousUnits()
{
	for (std::vector<BattleUnit*>::iterator i = getUnits()->begin(); i != getUnits()->end(); ++i)
	{
		if ((*i)->getArmor()->getSize() == 1)
		{
			Position originalPosition = (*i)->getPosition();
			if (originalPosition == Position(-1, -1, -1))
			{
				for (std::vector<BattleItem*>::iterator j = _items.begin(); j != _items.end(); ++j)
				{
					if ((*j)->getUnit() && (*j)->getUnit() == *i && (*j)->getOwner())
					{
						originalPosition = (*j)->getOwner()->getPosition();
					}
				}
			}
			if((*i)->getStatus() == STATUS_UNCONSCIOUS && (*i)->getStunlevel() < (*i)->getHealth() && (*i)->getHealth() > 0)
			{
				if (placeUnitNearPosition((*i), originalPosition))
				{
					// recover from unconscious
					(*i)->turn(false); // makes the unit stand up again
					(*i)->setCache(0);
					getTileEngine()->calculateFOV((*i));
					getTileEngine()->calculateUnitLighting();
					removeUnconsciousBodyItem((*i));
					break;
				}
			}
		}
	}
}

/**
  *   Remove the body item that corresponds to the unit
  */
void SavedBattleGame::removeUnconsciousBodyItem(BattleUnit *bu)
{
	// remove the unconscious body item corresponding to this unit
	for (std::vector<BattleItem*>::iterator it = getItems()->begin(); it != getItems()->end(); )
	{
		if ((*it)->getUnit() == bu)
		{
			removeItem((*it));
			break;
		}
		++it;
	}
}
/**
 * Function handles the placement of units on the map. This handles large units that are placed on multiple tiles.
 * @return Whether the unit could be successfully placed or not.
 */
bool SavedBattleGame::setUnitPosition(BattleUnit *bu, const Position &position, bool testOnly)
{
	int size = bu->getArmor()->getSize() - 1;

	// first check if the tiles are occupied
	for (int x = size; x >= 0; x--)
	{
		for (int y = size; y >= 0; y--)
		{
			Tile *t = getTile(position + Position(x,y,0));
			if (t == 0 || (t->getUnit() != 0 && t->getUnit() != bu) || t->getTUCost(MapData::O_OBJECT, bu->getArmor()->getMovementType()) == 255)
			{
				return false;
			}
		}
	}

	if (size > 0)
	{
		getPathfinding()->setUnit(bu);
		for (int dir = 2; dir <= 4; ++dir)
		{
			if (getPathfinding()->isBlocked(getTile(position), 0, dir, 0))
				return false;
		}
	}

	if (testOnly) return true;

	for (int x = size; x >= 0; x--)
	{
		for (int y = size; y >= 0; y--)
		{
			if (x==0 && y==0)
			{
				bu->setPosition(position + Position(x,y,0));
			}
			getTile(position + Position(x,y,0))->setUnit(bu, getTile(position + Position(x,y,-1)));
		}
	}

	return true;
}

/**
 * Gets the scroll drag button. (which mouse button is the scroll-button)
 * @return ScrollButton.
 */
Uint8 SavedBattleGame::getDragButton() const
{
	return _dragButton;
}

/**
 * Gets if the scroll drag is inverted.
 * @return true drags away from the cursor, false drags towards (like a grab).
 */
bool SavedBattleGame::isDragInverted() const
{
	return _dragInvert;
}

/**
 * Gets the amount of time the button must be pushed
 * to start a drag scroll.
 * @return Time in miliseconds.
 */
int SavedBattleGame::getDragTimeTolerance() const
{
	return _dragTimeTolerance;
}

/**
 * Gets the amount of pixels the mouse must move
 * to start a drag scroll.
 * @return Number of pixels.
 */
int SavedBattleGame::getDragPixelTolerance() const
{
	return _dragPixelTolerance;
}

/**
 * Gets the Number of units that can see this a given unit.
 * @param unit The unit to check for visibility.
 * @return number of spotting units.
 */
int SavedBattleGame::getSpottingUnits(BattleUnit* unit) const
{
	int spotting = 0;
	for (std::vector<BattleUnit*>::const_iterator i = unit->getVisibleUnits()->begin(); i != unit->getVisibleUnits()->end(); ++i) // cheating! perhaps only consider visible units here
	{																															// k
		std::vector<BattleUnit*>::iterator find = std::find((*i)->getVisibleUnits()->begin(), (*i)->getVisibleUnits()->end(), unit);
		if (find != (*i)->getVisibleUnits()->end())
			++spotting;
	}
	return spotting;
}


/**
 * @brief Check whether anyone on a particular faction is looking at *unit
 * 
 * Similar to getSpottingUnits() but returns a bool and stops searching if one positive hit is found.
 * 
 * @param faction A faction, of course
 * @param unit Whom to spot
 * @return true when the unit can be seen
 */
bool SavedBattleGame::eyesOnTarget(UnitFaction faction, BattleUnit* unit)
{
	for (std::vector<BattleUnit*>::iterator i = getUnits()->begin(); i != getUnits()->end(); ++i)
	{
		if ((*i)->getFaction() != faction) continue;
		
		std::vector<BattleUnit*> *vis = (*i)->getVisibleUnits();
		if (std::find(vis->begin(), vis->end(), unit) != vis->end()) return true;
		// aliens know the location of all XCom agents sighted by all other aliens due to sharing locations over their space-walkie-talkies				
	}

	return false;
}


bool SavedBattleGame::addFallingUnit(BattleUnit* unit)
{
	bool add = true;
	for (std::list<BattleUnit*>::iterator i = _fallingUnits.begin(); i != _fallingUnits.end(); ++i)
	{
		if (unit == *i)
		{
			add = false;
		}
	}
	if (add)
	{
		_fallingUnits.push_front(unit);
		_unitsFalling = true;
	}
	return add;
}

std::list<BattleUnit*> *SavedBattleGame::getFallingUnits()
{
	return &_fallingUnits;
}

void SavedBattleGame::setUnitsFalling(bool fall)
{
	_unitsFalling = fall;
}

bool SavedBattleGame::getUnitsFalling() const
{
	return _unitsFalling;
}
bool SavedBattleGame::getStrafeSetting() const
{
	return _strafeEnabled;
}
bool SavedBattleGame::getSneakySetting() const
{
	return _sneaky;
}
bool SavedBattleGame::getTraceSetting() const
{
	return _traceAI;
}

BattleUnit* SavedBattleGame::getHighestRankedXCom()
{
	BattleUnit* highest = 0;
	for (std::vector<BattleUnit*>::iterator j = _units.begin(); j != _units.end(); ++j)
	{
		if ((*j)->getOriginalFaction() == FACTION_PLAYER && !(*j)->isOut())
		{
			if (highest == 0 || (*j)->getRankInt() > highest->getRankInt())
			{
				highest = *j;
			}
		}
	}
	return highest;
}

int SavedBattleGame::getMoraleModifier(BattleUnit* unit)
{
	int result = 100;

	if (unit == 0)
	{
		BattleUnit *leader = getHighestRankedXCom();
		if (leader)
		{
			switch (leader->getRankInt())
			{
			case 5:
				result += 25;
			case 4:
				result += 10;
			case 3:
				result += 5;
			case 2:
				result += 10;
			default:
				break;
			}
		}
	}
	else if (unit->getFaction() == FACTION_PLAYER)
	{
		switch (unit->getRankInt())
		{
		case 5:
			result += 25;
		case 4:
			result += 20;
		case 3:
			result += 10;
		case 2:
			result += 20;
		default:
			break;
		}
	}
	return result;
}

/*
 * place a unit on or near a position
 * @param unit the unit to place
 * @param entryPoint the position around which to attempt to place the unit
 * @return if we were successful
 */

bool SavedBattleGame::placeUnitNearPosition(BattleUnit *unit, Position entryPoint)
{
	if (setUnitPosition(unit, entryPoint))
	{
		return true;
	}

	for (int dir = 0; dir <= 7; ++dir)
	{
		Position offset;
		getPathfinding()->directionToVector(dir, &offset);
		Tile *t = getTile(entryPoint + offset);
		if (t && !getPathfinding()->isBlocked(getTile(entryPoint), t, dir, 0)
			&& setUnitPosition(unit, entryPoint + offset))
		{
			return true;
		}
	}
	
	if (unit->getArmor()->getMovementType() == MT_FLY)
	{
		Tile *t = getTile(entryPoint + Position(0, 0, 1));
		if (t && t->hasNoFloor(getTile(entryPoint)) && setUnitPosition(unit, entryPoint + Position(0, 0, 1)))
		{
			return true;
		}
	}
	return false;
}

void SavedBattleGame::resetTurnCounter()
{
	_turn = 1;
}

/**
 * resets visibility of all tiles on the map.
 */
void SavedBattleGame::resetTiles()
{
	for (int i = 0; i != getMapSizeXYZ(); ++i)
	{
		_tiles[i]->setDiscovered(false, 2);
	}
}
}
