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
#include "BattleUnit.h"
#include "BattleItem.h"
#include "Node.h"
#include "SDL.h"
#include "../Ruleset/MapDataSet.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/TerrainModifier.h"
#include "../Battlescape/Position.h"

namespace OpenXcom
{

/**
 * Initializes a brand new battlescape saved game.
 */
SavedBattleGame::SavedBattleGame() : _tiles(), _nodes(), _units()
{

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
	delete[] _tiles;

	for (std::vector<Node*>::iterator i = _nodes.begin(); i != _nodes.end(); i++)
	{
		delete *i;
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); i++)
	{
		delete *i;
	}

	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); i++)
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
	//TODO: Daiky
}

/**
 * Saves the saved battle game to a YAML file.
 * @param out YAML emitter.
 */
void SavedBattleGame::save(YAML::Emitter &out) const
{
/** 
* under construction
*/
	out << YAML::BeginMap;

	out << YAML::Key << "width" << YAML::Value << _width;
	out << YAML::Key << "length" << YAML::Value << _length;
	out << YAML::Key << "height" << YAML::Value << _height;

	out << YAML::Key << "mapdatafiles" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataFiles.begin(); i != _mapDataFiles.end(); i++)
	{
		out << (*i)->getName();
	}
	out << YAML::EndSeq;

	/* Every tile is 8 bytes, 2 bytes per object(ground,west,north,object) */
	/* 1 byte for the datafile ID, 1 byte for the relative object ID in that file */
	/* Value 0xFF means the next two bytes are the number of empty objects */
	/* The binary data is then base64 encoded to save as a string */
	/*Uint8 tileData[8];
	Uint16 empties = 0;
	std::string tilesData;
	for (int i = 0; i < _height * _length * _width; i++)
	{
		if (_tiles[i]->isVoid())
		{
			empties++;
		}
		else
		{
			if (empties)
			{
				tilesData += 0xFF;
				tilesData += empties;
				empties = 0;
			}
		}
	}
	if (empties)
	{
		tilesData += 0xFF;
		tilesData += empties;
	}
	//out << YAML::Key << "tiles" << YAML::WriteBinary(tilesData, tilesData.length());
	*/
	out << YAML::Key << "units" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BattleUnit*>::const_iterator i = _units.begin(); i != _units.end(); i++)
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
	_pathfinding = new Pathfinding(this);
	_terrainModifier = new TerrainModifier(this);
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
 * Select the next player unit TODO move this to BattlescapeState !!
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectNextPlayerUnit()
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bNext = false;

	do
	{
		if (bNext && (*i)->getFaction() == FACTION_PLAYER)
		{
			break;
		}
		if ((*i) == _selectedUnit)
		{
			bNext = true;
		}
		i++;
		if (i == _units.end())
		{
			i = _units.begin();
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

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); i++)
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
	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		if ((*i)->getOwner() == unit && (*i)->getSlot() == slot)
		{
			return *i;
		}
	}
	return 0;
}

/** under construction
*
*/
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}



}
