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

#include <vector>

class Tile;
class SavedGame;
class RuleTerrain;
class RuleUnitSprite;
class MapBlock;
class Craft;
class Ufo;
class Node;
class ResourcePack;
class BattleSoldier;
class BattleUnit;
class Soldier;
class Position;
class Pathfinding;

/**
 * The battlescape data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like terrain
 * soldiers, items, etc.
 */
class SavedBattleGame
{
private:
	int _width, _length, _height; // dimensions of the battlemap, origin is topright edge

	// using a oldskool array here, because the size is static and a little faster than vector
	Tile **_tiles; 

	Craft *_craft;
	Ufo *_ufo;
	BattleSoldier *_selectedSoldier;

	std::vector<Node*> _nodes;
	std::vector<BattleSoldier*> _soldiers;
	//std::vector<BattleItem*> _items;

	RuleTerrain *_terrain;
	Pathfinding *_pathfinding;
public:
	/// Creates a new battle save, based on current generic save.
	SavedBattleGame(SavedGame *save, int width, int length, int height, RuleTerrain *terrain);
	/// Cleans up the saved game.
	~SavedBattleGame();
	/// Gets pointer to the tiles, a tile is the smallest component of battlescape.
	Tile **getTiles();
	/// Gets pointer to the terrain.
	RuleTerrain *getTerrain();
	/// Gets terrain width.
	int getWidth();
	/// Gets terrain length.
	int getLength();
	/// Gets terrain height.
	int getHeight();
	/// Conversion between coordinates and the tile index.
	int getTileIndex(const Position& pos);
	/// Conversion between tile index and coordinates.
	void getTileCoords(int index, int *x, int *y, int *z);
	/// Gets the tile at certain position.
	Tile *getTile(const Position& pos);
	/// Sets the xcom craft and ufo for this battle game.
	void setCrafts(Craft *craft, Ufo* ufo);
	/// Get the xcom craft in the battle game.
	Craft *getCraft();
	/// Gets the ufo in the battle game (returns NULL when no ufo).
	Ufo *getUfo();
	/// Generate a new battlescape map.
	void generateMap(ResourcePack *res);
	/// links tiles with terrainobjects, for easier/faster lookup
	void linkTilesWithTerrainObjects(ResourcePack *res);
	/// Add a soldier to the game
	void addSoldier(Soldier *soldier, RuleUnitSprite *rules);
	/// get the currently selected soldier
	BattleSoldier *getSelectedSoldier();
	/// set the currently selected soldier
	void setSelectedSoldier(BattleSoldier *soldier);
	/// select next soldier
	BattleSoldier *selectNextSoldier();
	/// select unit with position on map
	BattleUnit *selectUnit(const Position& pos);
	/// select unit with position on map
	BattleUnit *selectUnit(Tile *tile);
	/// get pointer to the list of nodes
	std::vector<Node*> *getNodes();
	/// get the pathfinding object
	Pathfinding *getPathfinding();

};

#endif
