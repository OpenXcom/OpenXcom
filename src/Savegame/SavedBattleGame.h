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
class Game;
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
	BattlescapeState *_battleState;
	int _mapsize_x, _mapsize_y, _mapsize_z;
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
	Uint8 _dragButton;  // this is a cache for Options::getString("battleScrollDragButton")
	bool _dragInvert;  // this is a cache for Options::getString("battleScrollDragInvert")
	int _dragTimeTolerance;  // this is a cache for Options::getInt("battleScrollDragTimeTolerance")
	int _dragPixelTolerance;  // this is a cache for Options::getInt("battleScrollDragPixelTolerance")
	bool _objectiveDestroyed;
	std::vector<BattleUnit*> _exposedUnits;
	std::list<BattleUnit*> _fallingUnits;
	bool _unitsFalling, _strafeEnabled, _sneaky, _traceAI;
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
	void initMap(int mapsize_x, int mapsize_y, int mapsize_z);
	/// initialises pathfinding and tileengine
	void initUtilities(ResourcePack *res);
	/// Gets the game's mapdatafiles.
	std::vector<MapDataSet*> *getMapDataSets();
	/// Set the mission type.
	void setMissionType(const std::string &missionType);
	/// Get the mission type.
	std::string getMissionType() const;
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
	/// Gets terrain size x.
	int getMapSizeX() const;
	/// Gets terrain size y.
	int getMapSizeY() const;
	/// Gets terrain size z.
	int getMapSizeZ() const;
	/// Gets terrain x*y*z
	int getMapSizeXYZ() const;


	/// Conversion between coordinates and the tile index.
	//  int getTileIndex(const Position& pos) const;
	/**
	 * This method converts coordinates into a unique index.
	 * getTile() calls this every time, so should be inlined along with it.
	 * @param pos position
	 * @return Unique index.
	 */
	inline int getTileIndex(const Position& pos) const
	{
		return pos.z * _mapsize_y * _mapsize_x + pos.y * _mapsize_x + pos.x;
	}

	/// Conversion between tile index and coordinates.
	void getTileCoords(int index, int *x, int *y, int *z) const;
	/// Gets the tile at certain position.
	//  Tile *getTile(const Position& pos) const;
	/**
	 * Gets the Tile on a given position on the map.
	 * This method is called over 50mil+ times per turn so it seems useful
	 * to inline it.
	 * @param pos position
	 * @return Pointer to tile.
	 */
	inline Tile *getTile(const Position& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.z < 0
			|| pos.x >= _mapsize_x || pos.y >= _mapsize_y || pos.z >= _mapsize_z)
			return 0;

		return _tiles[getTileIndex(pos)];
	}
	/// get the currently selected unit
	BattleUnit *getSelectedUnit() const;
	/// set the currently selected unit
	void setSelectedUnit(BattleUnit *unit);
	/// select previous soldier
	BattleUnit *selectPreviousPlayerUnit(bool checkReselect = false);
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
	void loadMapResources(Game *game);
	/// resets tiles units are standing on
	void resetUnitTiles();
	/// Removes an item from the game.
	void removeItem(BattleItem *item);
	/// Whether the mission was aborted.
	void setAborted(bool flag);
	/// Whether the mission was aborted.
	bool isAborted() const;
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
	/// get DragButton
	Uint8 getDragButton() const;
	/// get DragInverted
	bool isDragInverted() const;
	/// get DragTimeTolerance
	int getDragTimeTolerance() const;
	/// get DragPixelTolerance
	int getDragPixelTolerance() const;
	/// get the number of units that can see this unit
	int getSpottingUnits(BattleUnit* unit) const;
	/// add this unit to the vector of falling units
	bool addFallingUnit(BattleUnit* unit);
	/// get the vector of falling units
	std::list<BattleUnit*> *getFallingUnits();
	/// toggle the switch that says "there are units falling, start the fall state"
	void setUnitsFalling(bool fall);
	/// check the status of the switch that says "there are units falling"
	bool getUnitsFalling() const;
	/// check the strafe setting
	bool getStrafeSetting() const;
	/// check the sneaky ai setting
	bool getSneakySetting() const;
	/// get the traceAI setting
	bool getTraceSetting() const;
	/// get a pointer to the BattlescapeState
	BattlescapeState *getBattleState();
	/// set the pointer to the BattlescapeState
	void setBattleState(BattlescapeState *bs);
	/// return a pointer to the highest ranked, living XCOM unit
	BattleUnit* getHighestRankedXCom();
	/// get the morale modifier for XCOM based on the highest ranked, living XCOM unit, or the modifier for the unit passed to this function.
	int getMoraleModifier(BattleUnit* unit = 0);
	/// check whether a particular faction has eyes on *unit (whether any unit on that faction sees *unit)
	bool eyesOnTarget(UnitFaction faction, BattleUnit* unit);
	/// attempt to place unit on or near entryPoint
	bool placeUnitNearPosition(BattleUnit *unit, Position entryPoint);
	/// resets the turn counter.
	void resetTurnCounter();
	/// resets visibility of all tiles on the map.
	void resetTiles();
};

}

#endif
