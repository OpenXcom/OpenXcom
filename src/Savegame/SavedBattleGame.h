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
#ifndef OPENXCOM_SAVEDBATTLEGAME_H
#define OPENXCOM_SAVEDBATTLEGAME_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "yaml.h"
#include "BattleItem.h"
#include "BattleUnit.h"

namespace OpenXcom
{

class Tile;
class SavedGame;
class MapDataSet;
class RuleUnit;
class MapBlock;
class Node;
class ResourcePack;
class BattleUnit;
class Soldier;
class Position;
class Pathfinding;
class TerrainModifier;
class BattleItem;
class Item;
class RuleInventory;
class Ruleset;

/**
 * Enumator containing all the possible mission types.
 */
enum MissionType { MISS_UFORECOVERY, MISS_UFOASSAULT, MISS_TERROR, MISS_ALIENBASE, MISS_BASEDEFENSE, MISS_CYDONIA };
struct DebriefingStat { DebriefingStat(std::string _item, bool recovery) : item(_item), qty(0), score(0), recovery(recovery) {}; std::string item; int qty; int score; bool recovery; };

/**
 * The battlescape data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like mapdata
 * soldiers, items, etc.
 */
class SavedBattleGame
{
private:
	int _width, _length, _height;
	std::vector<MapDataSet*> _mapDataSets;
	Tile **_tiles; 
	BattleUnit *_selectedUnit;
	std::vector<Node*> _nodes;
	std::vector<BattleUnit*> _units;
	std::vector<BattleItem*> _items;
	Pathfinding *_pathfinding;
	TerrainModifier *_terrainModifier;
	MissionType _missionType;
	int _globalShade;
	UnitFaction _side;
	int _turn;
	bool _debugMode;
	std::vector<DebriefingStat*> _debriefingStats;
	std::vector<DebriefingStat*> _ufoRecoveryStats;
	bool _aborted;
	int _itemId;
	/// Add to debriefing stats.
	void addStat(const std::string &name, int quantity, int score);
public:
	/// Creates a new battle save, based on current generic save.
	SavedBattleGame();
	/// Cleans up the saved game.
	~SavedBattleGame();
	/// Loads a saved battle game from YAML.
	void load(const YAML::Node& node, Ruleset *rule, SavedGame* savedGame);
	/// Saves a saved battle game to YAML.
	void save(YAML::Emitter& out) const;
	/// Set the dimensions of the map and initializes it.
	void initMap(int width, int length, int height);
	/// initiliases pathfinding and terrainmodifier
	void initUtilities(ResourcePack *res);
	/// Gets the game's mapdatafiles.
	std::vector<MapDataSet*> *const getMapDataSets();
	/// Set the mission type.
	void setMissionType(MissionType missionType);
	/// Get the mission type.
	MissionType getMissionType() const;
	/// Set the global shade.
	void setGlobalShade(int shade);
	/// Get the global shade.
	int getGlobalShade() const;
	/// Gets pointer to the tiles, a tile is the smallest component of battlescape.
	Tile **getTiles() const;
	/// Get pointer to the list of nodes.
	std::vector<Node*> *const getNodes();
	/// Get pointer to the list of items.
	std::vector<BattleItem*> *const getItems();
	/// Get pointer to the list of units.
	std::vector<BattleUnit*> *const getUnits();
	/// Gets terrain width.
	int getWidth() const;
	/// Gets terrain length.
	int getLength() const;
	/// Gets terrain height.
	int getHeight() const;
	/// Conversion between coordinates and the tile index.
	int getTileIndex(const Position& pos) const;
	/// Conversion between tile index and coordinates.
	void getTileCoords(int index, int *x, int *y, int *z) const;
	/// Gets the tile at certain position.
	Tile *getTile(const Position& pos) const;
	/// get the currently selected unit
	BattleUnit *getSelectedUnit() const;
	/// set the currently selected unit
	void setSelectedUnit(BattleUnit *unit);
	/// select previous soldier
	BattleUnit *selectPreviousPlayerUnit();
	/// select next soldier
	BattleUnit *selectNextPlayerUnit(bool checkReselect = false);
	/// select unit with position on map
	BattleUnit *selectUnit(const Position& pos);
	/// select unit with position on map
	BattleUnit *selectUnit(Tile *tile);
	/// get the pathfinding object
	Pathfinding *const getPathfinding() const;
	/// get the terrainmodifier object
	TerrainModifier *const getTerrainModifier() const;
	/// get the playing side
	UnitFaction getSide() const;
	/// get the turn number
	int getTurn() const;
	/// end the turn
	void endTurn();
	/// set debug mode
	void setDebugMode();
	/// get debug mode
	bool getDebugMode() const;
	/// load map resources
	void loadMapResources(ResourcePack *res);
	/// resets tiles units are standing on
	void resetUnitTiles();
	/// Removes an item from the game.
	void removeItem(BattleItem *item);
	/// Get a list of debriefing stats.
	std::vector<DebriefingStat*> *getDebriefingStats();
	/// Prepares debriefing.
	void prepareDebriefing(bool aborted);
	/// Whether the mission was aborted.
	bool isAborted();
	/// Gets the current item ID.
	int *getCurrentItemId();

};

}

#endif
