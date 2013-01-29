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
#ifndef OPENXCOM_SAVEDBATTLEGAME_H
#define OPENXCOM_SAVEDBATTLEGAME_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
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
class TileEngine;
class BattleItem;
class Item;
class RuleInventory;
class Ruleset;

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
	BattleUnit *_selectedUnit, *_lastSelectedUnit;
	std::vector<Node*> _nodes;
	std::vector<BattleUnit*> _units;
	std::vector<BattleItem*> _items;
	Pathfinding *_pathfinding;
	TileEngine *_tileEngine;
	std::string _missionType;
	int _globalShade;
	UnitFaction _side;
	int _turn;
	bool _debugMode;
	bool _aborted;
	int _itemId;
	Uint8 _scrollButton;  // this is a cache for Options::getString("battleScrollButton")
	bool _scrollButtonInvertMode;  // this is a cache for Options::getString("battleScrollButtonInvertMode")
	int _scrollButtonTimeTolerancy;  // this is a cache for Options::getInt("battleScrollButtonTimeTolerancy")
	int _scrollButtonPixelTolerancy;  // this is a cache for Options::getInt("battleScrollButtonPixelTolerancy")
	bool _objectiveDestroyed;
	std::vector<BattleUnit*> _exposedUnits;
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
	/// initialises pathfinding and tileengine
	void initUtilities(ResourcePack *res);
	/// Gets the game's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Set the mission type.
	void setMissionType(const std::string &missionType);
	/// Get the mission type.
	std::string getMissionType() const;
	/// Get the next mission type.
	std::string getNextStage() const;
	/// Set the global shade.
	void setGlobalShade(int shade);
	/// Get the global shade.
	int getGlobalShade() const;
	/// Gets pointer to the tiles, a tile is the smallest component of battlescape.
	Tile **getTiles() const;
	/// Get pointer to the list of nodes.
	std::vector<Node*> *getNodes();
	/// Get pointer to the list of items.
	std::vector<BattleItem*> *getItems();
	/// Get pointer to the list of units.
	std::vector<BattleUnit*> *getUnits();
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
	BattleUnit *selectNextPlayerUnit(bool checkReselect = false, bool setReselect = false);
	/// select unit with position on map
	BattleUnit *selectUnit(const Position& pos);
	/// select unit with position on map
	BattleUnit *selectUnit(Tile *tile);
	/// get the pathfinding object
	Pathfinding *getPathfinding() const;
	/// get a pointer to the tileengine
	TileEngine *getTileEngine() const;
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
	/// Whether the mission was aborted.
	void setAborted(bool flag);
	/// Whether the mission was aborted.
	bool isAborted();
	/// Whether the objective is destroyed.
	void setObjectiveDestroyed(bool flag);
	/// Whether the objective is detroyed.
	bool isObjectiveDestroyed();
	/// Gets the current item ID.
	int *getCurrentItemId();
	/// Gets a spawn node.
	Node *getSpawnNode(int nodeRank, BattleUnit *unit);
	/// Gets a patrol node.
	Node *getPatrolNode(bool scout, BattleUnit *unit, Node *fromNode);
	/// New turn preparations.
	void prepareNewTurn();
	/// Revive unconscious units (healthcheck).
	void reviveUnconsciousUnits();
	/// Remove the body item that corresponds to the unit
	void removeUnconsciousBodyItem(BattleUnit *bu);
	/// Set or try to set a unit of a certain size on a certain position of the map.
	bool setUnitPosition(BattleUnit *bu, const Position &position, bool testOnly = false);
	/// get ScrollButton
	Uint8 getScrollButton() const;
	/// get ScrollButtonInvertMode
	bool getScrollButtonInvertMode() const;
	/// get ScrollButtonTimeTolerancy
	int getScrollButtonTimeTolerancy() const;
	/// get ScrollButtonPixelTolerancy
	int getScrollButtonPixelTolerancy() const;
	void updateExposedUnits();
	std::vector<BattleUnit*> *getExposedUnits();
	int getSpottingUnits(BattleUnit* unit) const;

};

}

#endif
