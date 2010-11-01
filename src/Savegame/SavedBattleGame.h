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
#ifndef OPENXCOM__SAVEDBATTLEGAME_H
#define OPENXCOM__SAVEDBATTLEGAME_H

#include <vector>

class Tile;
class SavedGame;
class Terrain;
class MapBlock;
class Craft;
class Ufo;
class Spawnpoint;

/**
 * The game data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like terrain
 * soldiers, items,...
 */
class SavedBattleGame
{
private:
	Terrain* _terrain;
	int _width, _length, _height; // dimensions of the battlemap, origin is topright edge

	int _viewheight; // the layer we are on

	// using a oldskool array here, because the size is static and a little faster than vector
	Tile** _tiles; 

	Craft* _craft;
	Ufo* _ufo;
	std::vector<Spawnpoint*> _spawnpoints;
	//std::vector<BattleSoldier*> _soldiers;
	//std::vector<BattleItem*> _items;


public:
	/// Creates a new battle save, based on current generic save.
	SavedBattleGame(SavedGame *save, int width, int length, int height, Terrain* terrain);
	/// Cleans up the saved game.
	~SavedBattleGame();
	/// Gets pointer to the tiles, a tile is the smallest component of battlescape.
	Tile** getTiles();
	/// Gets pointer to the terrain.
	Terrain *getTerrain();
	/// Gets terrain width.
	int getWidth();
	/// Gets terrain length.
	int getLength();
	/// Gets terrain height.
	int getHeight();
	/// Gets the height player is currently viewing.
	int getViewHeight();
	/// Gets relative viewheight.
	void setRelativeViewHeight(int height);
	/// Conversion between coordinates and the tile index.
	int getTileIndex(int x, int y, int z);
	/// Gets the tile at certain coordinates.
	Tile *getTile(int x, int y, int z);
	/// Sets the xcom craft and ufo for this battle game.
	void setCrafts(Craft *craft, Ufo* ufo);
	/// Get the xcom craft in the battle game.
	Craft *getCraft();
	/// Gets the ufo in the battle game (returns NULL when no ufo).
	Ufo *getUfo();
	/// Generate a new battlescape map.
	void generateMap();

};

#endif
