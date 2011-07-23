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
#include "Tile.h"
#include "Node.h"
#include "SDL.h"
#include "../Ruleset/MapDataSet.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/TerrainModifier.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

/**
 * Initializes a brand new battlescape saved game.
 */
SavedBattleGame::SavedBattleGame() : _tiles(), _nodes(), _units(), _side(FACTION_PLAYER), _turn(1), _debugMode(false)
{

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
	delete _terrainModifier;
}

/**
 * Loads the saved battle game from a YAML file.
 * @param node YAML node.
 */
void SavedBattleGame::load(const YAML::Node &node)
{
	unsigned int size = 0;

	node["width"] >> _width;
	node["length"] >> _length;
	node["height"] >> _height;

	size = node["mapdatafiles"].size();
	for (unsigned int i = 0; i < size; i++)
	{
		std::string name;
		node["mapdatafiles"][i] >> name;
		MapDataSet *mds = new MapDataSet(name);
		_mapDataFiles.push_back(mds);
	}

	initMap(_width, _length, _height);

	int empties = 0;
	int dp = 0;
	
	std::string s;
	std::vector<unsigned char> data;
	node["tiles"] >> s;
	//Base64Decode(s, data);
	for (int i = 0; i < _height * _length * _width; i++)
	{
		if (!empties)
		{
			if (data[dp++] == 0xFF)
			{
				empties = data[dp++];
			}
			else
			{
				for (int part = 0; part < 4; part++)
				{
					_tiles[i]->load(data[dp++], data[dp++], part);
				}
			}
		}
		else
		{
			empties--;
		}
	}

}

/**
 * TODO function header.
 * @param
 * @return
 */
void SavedBattleGame::loadMapResources(ResourcePack *res)
{
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); ++i)
	{
		(*i)->load(res);
	}

	int mdsID, mdID;

	for (int i = 0; i < _height * _length * _width; i++)
	{
		for (int part = 0; part < 4; part++)
		{
			if (mdsID || mdID)
			{
				_tiles[i]->getSaveGameData(&mdsID, &mdID, part);
				_tiles[i]->setMapData(_mapDataFiles[mdsID]->getObjects()->at(mdID), part);
			}
		}
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

	out << YAML::Key << "mapdatafiles" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); ++i)
	{
		out << (*i)->getName();
	}
	out << YAML::EndSeq;

	/* Every tile is 8 bytes, 2 bytes per object(ground,west,north,object) */
	/* 1 byte for the datafile ID, 1 byte for the relative object ID in that file */
	/* Value 0xFF means the next two bytes are the number of empty objects */
	/* The binary data is then base64 encoded to save as a string */
	Uint16 empties = 0;
	std::vector<unsigned char> data;

	for (int i = 0; i < _height * _length * _width; i++)
	{
		if (_tiles[i]->isVoid())
		{
			empties++;
		}
		else
		{
			// if we had empty tiles before
			// write them now
			if (empties)
			{
				data.push_back(0xFF);
				data.push_back((unsigned char)empties);
				empties = 0;
			}
			// now write 4x2 bytes
			for (int part = 0; part < 4; part++)
			{
				if (_tiles[i]->getMapData(part))
				{
					MapDataSet *mds = _tiles[i]->getMapData(part)->getDataset();
					data.push_back((Uint8)(std::find(_mapDataFiles.begin(), _mapDataFiles.end(), mds) - _mapDataFiles.begin()));
					data.push_back((Uint8)(std::find(mds->getObjects()->begin(), mds->getObjects()->end(), _tiles[i]->getMapData(part)) - mds->getObjects()->begin()));
				}
				else
				{
					data.push_back(0);
					data.push_back(0);
				}
			}
		}
	}
	if (empties)
	{
		data.push_back(0xFF);
		data.push_back((unsigned char)empties);
	}
	out << YAML::Key << "tiles" << YAML::Value << YAML::Binary(&data[0], data.size());

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
Tile **SavedBattleGame::getTiles()
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
 * TODO function header.
 * @param
 * @return
 */
void SavedBattleGame::initUtilities(ResourcePack *res)
{
	_pathfinding = new Pathfinding(this);
	_terrainModifier = new TerrainModifier(this, res->getVoxelData());
}

/**
 * Sets the mission type.
 * @param missionType
 */
void SavedBattleGame::setMissionType(MissionType missionType)
{
	_missionType = missionType;
}

/**
 * Gets the mission type.
 * @return missionType
 */
MissionType SavedBattleGame::getMissionType() const
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

	return _tiles[getTileIndex(pos)];
}

/**
 * Gets the currently selected unit
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::getSelectedUnit()
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
 * Select the next player unit TODO move this to BattlescapeState ?
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectNextPlayerUnit()
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bNext = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bNext = true;
	}

	do
	{
		if (bNext && (*i)->getFaction() == _side && !(*i)->isOut())
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
	BattleUnit *bu = 0;

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getPosition() == pos && !(*i)->isOut())
		{
			bu = *i;
			break;
		}
	}

	return bu;
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
Pathfinding *SavedBattleGame::getPathfinding()
{
	return _pathfinding;
}

/**
 * Get the terrain modifier object.
 * @return pointer to the terrain modifier object
 */
TerrainModifier *SavedBattleGame::getTerrainModifier()
{
	return _terrainModifier;
}

/**
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapDataSet*> *SavedBattleGame::getMapDataSets()
{
	return &_mapDataFiles;
}

/**
* get an item from a specific unit and slot
* @return
*/
BattleItem *SavedBattleGame::getItemFromUnit(BattleUnit *unit, InventorySlot slot)
{
	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		if ((*i)->getOwner() == unit && (*i)->getSlot() == slot)
		{
			return *i;
		}
	}
	return 0;
}

/**
 * TODO function header.
 * @param
 * @return
 */
UnitFaction SavedBattleGame::getSide() const
{
	return _side;
}

/**
 * TODO function header.
 * @param
 * @return
 */
int SavedBattleGame::getTurn() const
{
	return _turn;
}

/**
 * TODO function header.
 * @param
 * @return
 */
void SavedBattleGame::endTurn()
{
	if (_side == FACTION_PLAYER)
	{
		_side = FACTION_HOSTILE;
	}
	else if (_side == FACTION_HOSTILE)
	{
		_terrainModifier->prepareNewTurn();
		_turn++;
		_side = FACTION_PLAYER;
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getFaction() == _side)
		{
			(*i)->prepareNewTurn();
		}
	}

	_selectedUnit = 0;
	selectNextPlayerUnit();
}

/**
 * TODO function header.
 * @param
 * @return
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
 * TODO function header.
 * @param
 * @return
 */
bool SavedBattleGame::getDebugMode() const
{
	return _debugMode;
}


}
