/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "Tile.h"
#include "Node.h"
#include <SDL.h>
#include "../Ruleset/MapDataSet.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/TileEngine.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Language.h"
#include "../Ruleset/RuleInventory.h"
#include "../Battlescape/PatrolBAIState.h"
#include "../Battlescape/AggroBAIState.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"


namespace OpenXcom
{

/**
 * Initializes a brand new battlescape saved game.
 */
SavedBattleGame::SavedBattleGame() : _width(0), _length(0), _height(0), _tiles(), _selectedUnit(0), _lastSelectedUnit(0), _nodes(), _units(), _items(), _pathfinding(0), _tileEngine(0), _missionType(""), _globalShade(0), _side(FACTION_PLAYER), _turn(1), _debugMode(false), _aborted(false), _itemId(0), _objectiveDestroyed(false)
{
	std::string temp;
	temp = Options::getString("battleScrollButton");
	if ("RMB" == temp) _scrollButton = SDL_BUTTON_RIGHT;
	else if ("MMB" == temp) _scrollButton = SDL_BUTTON_MIDDLE;
	else _scrollButton = -1;
	temp = Options::getString("battleScrollButtonInvertMode");
	_scrollButtonInvertMode = ("Normal" != temp);
	_scrollButtonTimeTolerancy = Options::getInt("battleScrollButtonTimeTolerancy");
	_scrollButtonPixelTolerancy = Options::getInt("battleScrollButtonPixelTolerancy");
}

/**
 * Deletes the game content from memory.
 */
SavedBattleGame::~SavedBattleGame()
{
	for (int i = 0; i < _height * _length * _width; ++i)
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

	node["width"] >> _width;
	node["length"] >> _length;
	node["height"] >> _height;
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

	initMap(_width, _length, _height);

	for (YAML::Iterator i = node["tiles"].begin(); i != node["tiles"].end(); ++i)
	{
		Position pos;
		(*i)["position"][0] >> pos.x;
		(*i)["position"][1] >> pos.y;
		(*i)["position"][2] >> pos.z;
		getTile(pos)->load((*i));
	}
/*	const int recordSize = 19;
	std::string s;
	node["tiles"] >> s;
	std::vector<unsigned char> tileData( s.begin(), s.end() );
	int records = tileData.size() / recordSize;
	for (int i = 0; i < records; ++i)
	{
		int index = tileData.data()[(i * recordSize)] + tileData.data()[(i * recordSize)+1] << 8 + tileData.data()[(i * recordSize)+2] << 16 + tileData.data()[(i * recordSize)+3] << 24;
		_tiles[index]->loadBinary(&tileData.data()[(i * recordSize)]);
	}
*/

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
			b = new BattleUnit(rule->getUnit(type), faction, a, rule->getArmor(armor));
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
	// it does what it says it does.
	updateExposedUnits();
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

}

/**
 * Loads the resources required by the map in the battle save.
 * @param res Pointer to resource pack.
 */
void SavedBattleGame::loadMapResources(ResourcePack *res)
{
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		(*i)->loadData();
	}

	int mdsID, mdID;

	for (int i = 0; i < _height * _length * _width; ++i)
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
	for (std::vector<BattleUnit*>::iterator bu = _units.begin(); bu != _units.end(); ++bu)
	{
		_tileEngine->calculateFOV(*bu);
	}
}

/**
 * Saves the saved battle game to a YAML file.
 * @param out YAML emitter.
 */
void SavedBattleGame::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "width" << YAML::Value << _width;
	out << YAML::Key << "length" << YAML::Value << _length;
	out << YAML::Key << "height" << YAML::Value << _height;
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
	out << YAML::Key << "tiles" << YAML::Value;
	out << YAML::BeginSeq;
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		if (!_tiles[i]->isVoid())
		{
			_tiles[i]->save(out);
		}
	}
	out << YAML::EndSeq;
	/*const int recordSize = 19;
	size_t tileDataSize = recordSize * _height * _length * _width;
	unsigned char* tileData = (unsigned char*) malloc(tileDataSize);
	int ptr = 0;
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		if (!_tiles[i]->isVoid())
		{
			tileData[ptr] = (unsigned int)i;
			_tiles[i]->saveBinary(&tileData[ptr]);
			ptr += recordSize;
		}
		else
		{
			tileDataSize -= recordSize;
		}
	}
	out << YAML::Key << "tiles" << YAML::Value << YAML::Binary(tileData, tileDataSize);
    free(tileData);
    ptr = NULL;
	*/
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
 * @param width
 * @param length
 * @param height
 */
void SavedBattleGame::initMap(int width, int length, int height)
{
	if (!_nodes.empty())
	{
		for (int i = 0; i < _height * _length * _width; ++i)
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
	_width = width;
	_length = length;
	_height = height;
	_tiles = new Tile*[_height * _length * _width];
	/* create tile objects */
	for (int i = 0; i < _height * _length * _width; ++i)
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
 * Gets the next mission type.
 * @return missionType
 */
std::string SavedBattleGame::getNextStage() const
{
	if (_missionType == "STR_MARS_CYDONIA_LANDING")
	{
		return "STR_MARS_THE_FINAL_ASSAULT";
	}
	else
		return "";
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
 * @return Width in tiles.
 */
int SavedBattleGame::getWidth() const
{
	return _width;
}

/**
 * Gets the map length.
 * @return Length in tiles.
 */
int SavedBattleGame::getLength() const
{
	return _length;
}

/**
 * Gets the map height.
 * @return Height in layers.
 */
int SavedBattleGame::getHeight() const
{
	return _height;
}

/**
 * This method converts coordinates into a unique index.
 * @param pos position
 * @return Unique index.
 */
int SavedBattleGame::getTileIndex(const Position& pos) const
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
void SavedBattleGame::getTileCoords(int index, int *x, int *y, int *z) const
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
Tile *SavedBattleGame::getTile(const Position& pos) const
{
	if (pos.x < 0 || pos.y < 0 || pos.z < 0
		|| pos.x >= _width || pos.y >= _length || pos.z >= _height)
		return 0;

	return _tiles[getTileIndex(pos)];
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
BattleUnit *SavedBattleGame::selectPreviousPlayerUnit()
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
 * @param checkReselect Don't reselect unit.
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectNextPlayerUnit(bool checkReselect)
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bNext = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bNext = true;
	}
	else
	if (checkReselect)
	{
		_selectedUnit->dontReselect();
	}

	do
	{
		if (bNext && (*i)->getFaction() == _side && !(*i)->isOut() && (*i)->reselectAllowed())
		{
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
				selectPreviousPlayerUnit();
			while (_selectedUnit && _selectedUnit->getFaction() != FACTION_PLAYER)
				selectPreviousPlayerUnit();
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
			selectPreviousPlayerUnit();
		while (_selectedUnit && _selectedUnit->getFaction() != FACTION_PLAYER)
			selectPreviousPlayerUnit();
	}
	
	// hide all aliens (VOF calculations below will turn them visible again)
	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getTurnsExposed() && _side == FACTION_PLAYER)
		{
			(*i)->setTurnsExposed((*i)->getTurnsExposed() - 1);
			updateExposedUnits();
		}
		if (_side == FACTION_PLAYER && _turn == 20 && (*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			(*i)->setTurnsExposed(-1);
			updateExposedUnits();
		}
		if ((*i)->getFaction() != FACTION_PLAYER)
		{
			(*i)->setVisible(false);
		}
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getFaction() == _side)
		{
			(*i)->prepareNewTurn();
		}
		_tileEngine->calculateFOV(*i);
	}

	if (_side != FACTION_PLAYER)
		selectNextPlayerUnit();
}

/**
 * Turns on debug mode.
 */
void SavedBattleGame::setDebugMode()
{
	for (int i = 0; i < _height * _length * _width; ++i)
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
				(*i)->getTile()->setUnit(0);
			}
			int size = (*i)->getArmor()->getSize() - 1;
			for (int x = size; x >= 0; x--)
			{
				for (int y = size; y >= 0; y--)
				{
					Tile *t = getTile((*i)->getPosition() + Position(x,y,0));
					t->setUnit((*i));
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
	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		if (*i == item)
		{
			_items.erase(i);
			break;
		}
	}
	// due to strange design, the item has to be removed from the tile it is on too (if it is on a tile)
	for (int i = 0; i < getWidth() * getLength() * getHeight(); ++i)
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
bool SavedBattleGame::isAborted()
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
			&& setUnitPosition(unit, (*i)->getPosition(), true)		// check if not already occupied
			&& (*i)->getPosition().x > 0 && (*i)->getPosition().y > 0 )
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
	std::vector<Node*> compliantNodes;	

	for (std::vector<int>::iterator i = fromNode->getNodeLinks()->begin(); i != fromNode->getNodeLinks()->end(); ++i )
	{
		if (*i > 0)
		{
			Node *n = getNodes()->at(*i);
			if ((n->getRank() > 0 || scout)										// for no scouts we find a node with a rank above 0
				&& (!(n->getType() & Node::TYPE_SMALL) 
					|| unit->getArmor()->getSize() == 1)				// the small unit bit is not set or the unit is small
				&& (!(n->getType() & Node::TYPE_FLYING) 
					|| unit->getArmor()->getMovementType() == MT_FLY)// the flying unit bit is not set or the unit can fly
				&& !n->isAllocated() // check if not allocated
				&& setUnitPosition(unit, n->getPosition(), true)	// check if not already occupied
				&& n->getPosition().x > 0 && n->getPosition().y > 0)
			{
				compliantNodes.push_back(n);
			}
		}
	}

	if (compliantNodes.empty()) return 0;

	return compliantNodes[RNG::generate(0, compliantNodes.size() - 1)];
}

/**
 * New turn preparations. Like fire and smoke spreading.
 * @return True when objective destroyed by fire
 */
void SavedBattleGame::prepareNewTurn()
{
	std::vector<Tile*> tilesOnFire;
	std::vector<Tile*> tilesOnSmoke;

	// prepare a list of tiles on fire/smoke
	for (int i = 0; i < getWidth() * getLength() * getHeight(); ++i)
	{
		if (getTiles()[i]->getFire() > 0)
		{
			tilesOnFire.push_back(getTiles()[i]);
		}
		if (getTiles()[i]->getSmoke() > 0)
		{
			tilesOnSmoke.push_back(getTiles()[i]);
		}
	}

	// smoke spreads in 1 random direction, but the direction is same for all smoke
	int spreadX = RNG::generate(-1, +1);
	int spreadY = RNG::generate(-1, +1);
	for (std::vector<Tile*>::iterator i = tilesOnSmoke.begin(); i != tilesOnSmoke.end(); ++i)
	{
		int x = (*i)->getPosition().x;
		int y = (*i)->getPosition().y;
		int z = (*i)->getPosition().z;

		if ((*i)->getUnit() && !(*i)->getUnit()->isOut())
		{
			// units in smoke suffer stun
			(*i)->getUnit()->damage(Position(), ((*i)->getSmoke()/5)+1, DT_SMOKE, true);
		}

		Tile *t = getTile(Position(x+spreadX, y+spreadY, z));
		if (t && !t->getSmoke() && getTileEngine()->horizontalBlockage((*i), t, DT_SMOKE) == 0)
		{
			t->addSmoke((*i)->getSmoke()/2);
		}
		Tile *t2 = getTile(Position(x+spreadX+spreadX, y+spreadY+spreadY, z));
		if (t && t2 && !t2->getSmoke() && getTileEngine()->horizontalBlockage(t, t2, DT_SMOKE) == 0)
		{
			t2->addSmoke((*i)->getSmoke()/4);
		}

		// smoke also spreads upwards
		t = getTile(Position(x, y, z+1));
		if (t && !t->getSmoke() && getTileEngine()->verticalBlockage((*i), t, DT_SMOKE) == 0)
		{
			t->addSmoke((*i)->getSmoke()/2);
		}

		(*i)->prepareNewTurn();
	}

	for (std::vector<Tile*>::iterator i = tilesOnFire.begin(); i != tilesOnFire.end(); ++i)
	{
		if ((*i)->getUnit())
		{
			// units on a flaming tile suffer damage
			(*i)->getUnit()->damage(Position(0,0,0), RNG::generate(1,12), DT_IN, true);
			// units on a flaming tile can catch fire 33% chance
			if (RNG::generate(0,2) == 1)
			{
				(*i)->getUnit()->setFire(RNG::generate(1,5));
			}
		}

		int z = (*i)->getPosition().z;
		for (int x = (*i)->getPosition().x-1; x <= (*i)->getPosition().x+1; ++x)
		{
			for (int y = (*i)->getPosition().y-1; y <= (*i)->getPosition().y+1; ++y)
			{
				Tile *t = getTile(Position(x, y, z));
				if (t && t->getFire() == 0)
				{
					// check adjacent tiles - if they have a flammability of < 255, there is a chance...
					if (getTileEngine()->horizontalBlockage((*i), t, DT_IN) == 0)
					{
						int flam = t->getFlammability();
						if (flam < 255)
						{
							double base = RNG::boxMuller(0,126);
							if (base < 0) base *= -1;

							if (flam < base)
							{
								if (RNG::generate(0, flam) < 2)
								{
									t->ignite();
								}
							}
						}
					}
				}
			}
		}
		if (!_objectiveDestroyed)
			_objectiveDestroyed = (*i)->prepareNewTurn();
	}

	if (!tilesOnFire.empty())
	{
		getTileEngine()->calculateTerrainLighting(); // fires could have been stopped
	}

	reviveUnconsciousUnits();

}

/**
 * Units that are unconscious but shouldn't are revived, they need a tile to stand on. The unit's current position could be occupied.
 * We will search in all directions for a free tile, if not found, the unit stays unconscious...
 */
void SavedBattleGame::reviveUnconsciousUnits()
{
	int xd[9] = {0, 0, 1, 1, 1, 0, -1, -1, -1};
	int yd[9] = {0, -1, -1, 0, 1, 1, 1, 0, -1};

	for (std::vector<BattleUnit*>::iterator i = getUnits()->begin(); i != getUnits()->end(); ++i)
	{
		if ((*i)->getArmor()->getSize() == 1)
		{
			Position originalPosition = (*i)->getPosition();
			for (int dir = 0; dir < 9 && (*i)->getStatus() == STATUS_UNCONSCIOUS && (*i)->getStunlevel() < (*i)->getHealth() && (*i)->getHealth() > 0; dir++)
			{
				Tile *t = getTile(originalPosition + Position(xd[dir],yd[dir],0));
				if (t && t->getUnit() == 0 && !t->hasNoFloor())
				{
					// recover from unconscious
					(*i)->setPosition(originalPosition + Position(xd[dir],yd[dir],0));
					getTile(originalPosition + Position(xd[dir],yd[dir],0))->setUnit(*i);
					(*i)->turn(false); // makes the unit stand up again
					(*i)->setCache(0);
					getTileEngine()->calculateFOV((*i));
					getTileEngine()->calculateUnitLighting();
					removeUnconsciousBodyItem((*i));
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
			if (t == 0 || (t->getUnit() != 0 && t->getUnit() != bu))
			{
				return false;
			}
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
			getTile(position + Position(x,y,0))->setUnit(bu);
		}
	}

	return true;
}

/**
 * Gets the ScrollButton type. (which mouse button is the scroll-button)
 * @return ScrollButton type.
 */
Uint8 SavedBattleGame::getScrollButton() const
{
	return _scrollButton;
}

/**
 * Gets the ScrollButton InvertMode.
 * @return ScrollButton InvertMode.
 */
bool SavedBattleGame::getScrollButtonInvertMode() const
{
	return _scrollButtonInvertMode;
}

/**
 * Gets the ScrollButton TimeTolerancy.
 * @return ScrollButton TimeTolerancy.
 */
int SavedBattleGame::getScrollButtonTimeTolerancy() const
{
	return _scrollButtonTimeTolerancy;
}

/**
 * Gets the ScrollButton PixelTolerancy.
 * @return ScrollButton PixelTolerancy.
 */
int SavedBattleGame::getScrollButtonPixelTolerancy() const
{
	return _scrollButtonPixelTolerancy;
}

void SavedBattleGame::updateExposedUnits()
{
	_exposedUnits.clear();
	for (std::vector<BattleUnit*>::const_iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getTurnsExposed() && (*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
			_exposedUnits.push_back(*i);
	}
}

std::vector<BattleUnit*> *SavedBattleGame::getExposedUnits()
{
	return &_exposedUnits;
}

/**
 * Gets the Number of units that can see this a given unit.
 * @param unit The unit to check for visibility.
 * @return number of spotting units.
 */
int SavedBattleGame::getSpottingUnits(BattleUnit* unit) const
{
	int spotting = 0;
	for (std::vector<BattleUnit*>::const_iterator i = _units.begin(); i != _units.end(); ++i)
	{
		std::vector<BattleUnit*>::iterator find = std::find((*i)->getVisibleUnits()->begin(), (*i)->getVisibleUnits()->end(), unit);
		if (find != (*i)->getVisibleUnits()->end())
			++spotting;
	}
	return spotting;
}

}
